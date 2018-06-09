#include <transceiver.h>
#include <mbed.h>
#include <iostream>
#include <bitset>
#include "../../src/main.h"


uint8_t Transceiver::initialize(configStruct config, dataStruct *data)
{
    _radio.powerUp();

    // Check is status register is correct, if not, a full reboot is needed :(
    uint8_t statRegister = _radio.getStatusRegister();
    if ((statRegister != 0x08) && (statRegister != 0x0e) && (statRegister != 0x0f))
    {
        // _radio.disable();
        // _radio.powerDown();

        // _radio.setRegister(,0x07);
        std::cout << std::hex << bitset<8>(statRegister) << std::endl;
        return 1;
    }
    _radio.setRfFrequency(2400 + config.radioConfig.channel);
    _radio.setTransferSize(config.radioConfig.transferSize);
    _radio.setCrcWidth(16);
    _radio.enableAutoAcknowledge(NRF24L01P_PIPE_P0);
    _radio.setRxAddress(config.radioConfig.rxAddress);
    _radio.setTxAddress(config.radioConfig.txAddress);
    _radio.setReceiveMode();

    _radio.enable();
    transferSize = config.radioConfig.transferSize;
    rxData = new char[transferSize];
    dataPtr = data;
    return 0;
}

void Transceiver::powerDown(){
    _radio.disable();
    _radio.powerDown();
}

bool Transceiver::messageAvailable()
{
    int status = 0;
    status = _radio.readable(NRF24L01P_PIPE_P0);
    return status;
}

void Transceiver::receive(int pipe, char *buffer, uint8_t length)
{
    if (_radio.readable()){
        _radio.read(pipe, buffer, length);
        serialConnection.printf("boem\n");
    }
    else serialConnection.printf("biem\n");
}

void Transceiver::update(void)
{
    if (messageAvailable())
    {
        receive(NRF24L01P_PIPE_P0, rxData, transferSize);
        (*dataPtr).remote.throttle = (uint16_t)rxData[1] << 8 | rxData[0];
        (*dataPtr).remote.roll = (float)((int16_t)(rxData[3] << 8 | rxData[2]));
        (*dataPtr).remote.pitch = (float)((int16_t)(rxData[5] << 8 | rxData[4]));
        (*dataPtr).remote.yaw = (float)((int16_t)(rxData[7] << 8 | rxData[6]));
        (*dataPtr).acroMode = 0;//((bool)rxData[9] >> 1) & 0x01;
        (*dataPtr).armMotor = 1;//(bool)rxData[9] & 0x01;
        (*dataPtr).remote.missedPackets = 0;
    }
    else
    {
        (*dataPtr).remote.missedPackets++;
    }
}

void Transceiver::setAcknowledgePayload(int pipe)
{
    uint8_t package[4];
    package[0] = (*dataPtr).batteryLevel & 0xFF;
    package[1] = (*dataPtr).batteryLevel >> 8;
    _radio.writeAcknowledgePayload(pipe, &package[0], 2);
}