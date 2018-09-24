#include <mbed.h>
#include <transceiver.h>
#include <iostream>

uint8_t Transceiver::initialize(void)
{
    packetReceived = false;

    // Check is status register is correct, if not, a full reboot is needed :(
    uint8_t statRegister = _radio.getStatusRegister();
    if ((statRegister != 0x08) && (statRegister != 0x0e) && (statRegister != 0x0f))
    {
        return 1;
    }
    _radio.setRfFrequency(2400 + config.radioConfig.channel);
    _radio.setTransferSize(config.radioConfig.transferSize);
    _radio.setCrcWidth(16);
    _radio.enableAutoAcknowledge(NRF24L01P_PIPE_P0);
    _radio.setRxAddress(config.radioConfig.rxAddress);
    _radio.setTxAddress(config.radioConfig.txAddress);
    _radio.setAirDataRate(NRF24L01P_DATARATE_250_KBPS);
    _radio.setReceiveMode();
    _radio.powerUp();
    _radio.enable();
    transferSize = config.radioConfig.transferSize;
    rxBuffer = new char[transferSize];

    return 0;
}

void Transceiver::interruptHandler(void){
    status = _radio.getStatusRegister();

    if (status == 0)
    { //data not ready?
        while (status == 0)
        {
            status = _radio.getStatusRegister();
        }
    }
    if (status & 1)
    { // TX FIFO full
        _radio.disable();
        _radio.flushTX();
    }
    if (status & 16)
    { // max TX retransmits
        _radio.disable();
        _radio.flushTX();
        _radio.setRegister(0x07, 16);
        signalStrength = movingAvg(signalStrengthArray, &sum, pos, sizeof(signalStrengthArray), 0);
        pos++;
        if (pos >= sizeof(signalStrengthArray) - 1){
            pos = 0;
        }
    }
    if (status & 32)
    { // TX sent (ACK package available if autoAck is enabled)
        _radio.disable();
        _radio.flushTX();
        _radio.setRegister(0x07, 32);
        signalStrength = movingAvg(signalStrengthArray, &sum, pos, sizeof(signalStrengthArray), 100);
        pos++;
        if (pos >= sizeof(signalStrengthArray) - 1){
            pos = 0;
        }
    }
    if (status & 64)
    { // RX received
        data.newPacket = true;
        _radio.read(0, rxBuffer, transferSize);
        _radio.setRegister(0x07, 64);
        send(0,(char * )data.batteryLevel.u,2);
        for (int i = 0; i<=15;i++){
            rxData[i/4].c[i & 3] = rxBuffer[i];
        }
        data.remote.throttle = rxData[0].f;
        data.remote.roll = rxData[1].f;
        data.remote.pitch = rxData[2].f;
        data.remote.yaw = rxData[3].f;
        data.armMotor = (bool)((rxBuffer[16] >> 1) & 0x01);
        data.acroMode = 1-(bool)(rxBuffer[16] & 0x01);
        _radio.flushRX();
    }
    
}

void Transceiver::send(uint8_t pipe, char * buffer, uint8_t length){
    _radio.write(pipe, buffer, length);
}

uint8_t Transceiver::movingAvg(uint8_t *ptrArrNumbers, uint16_t *ptrSum, uint8_t pos, uint16_t len, uint8_t nextNum)
{
  //Subtract the oldest number from the prev sum, add the new number
  *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
  //Assign the nextNum to the position in the array
  ptrArrNumbers[pos] = nextNum;
  //return the average
  return *ptrSum / len;
}
