#include <transceiver.h>
#include <mbed.h>
#include <iostream>
#include <bitset>

uint8_t Transceiver::initialize(configStruct config, dataStruct * data){
    radio.powerUp();

    // Check is status register is correct, if not, a full reboot is needed :(
    uint8_t statRegister = radio.getStatusRegister();
    if ((statRegister != 0x08) && (statRegister != 0x0e)){
        std::cout << bitset<8>(statRegister) << std::endl;
        return 1;
      }
    radio.setRfFrequency (2400 + config.radioConfig.channel);
    radio.setTransferSize(config.radioConfig.transferSize);
    radio.setCrcWidth(16);
    radio.enableAutoAcknowledge(NRF24L01P_PIPE_P0);
    radio.setRxAddress(config.radioConfig.rxAddress);
    radio.setTxAddress(config.radioConfig.txAddress);
    radio.setReceiveMode();
    
    radio.enable();

    transferSize = config.radioConfig.transferSize;
    rxData = new char[transferSize];
    dataPtr = data;
    return 0;
    }

bool Transceiver::messageAvailable(){
    int status = 0;
    DigitalOut led1(LED1),led2(LED2),led3(LED3),led4(LED4);
    status = this->radio.readable(NRF24L01P_PIPE_P0);
    if (status){led1=1;return status;}
    status = this->radio.readable(NRF24L01P_PIPE_P1);
    if (status){led2=1;return status;}
    status = this->radio.readable(NRF24L01P_PIPE_P2);
    if (status){led3=1;return status;}
    status = this->radio.readable(NRF24L01P_PIPE_P3);
    if (status){led4=1;return status;}
    status = this->radio.readable(NRF24L01P_PIPE_P4);
    if (status){led1=1;led2=1;return status;}
    return 0;
    }

void Transceiver::send(){




    }

void Transceiver::receive(int pipe,char* buffer,uint8_t length){
    if ( radio.readable() )
        radio.read( pipe, buffer, length );
    }

void Transceiver::update(void){
    receive(NRF24L01P_PIPE_P0, rxData, transferSize);
    (*dataPtr).remote.throttle = (uint16_t)rxData[1] << 8 |rxData[0];
    (*dataPtr).remote.roll = (int16_t)rxData[3] << 8 |rxData[2];
    (*dataPtr).remote.pitch = (int16_t)rxData[5] << 8 |rxData[4];
    (*dataPtr).remote.yaw = (int16_t)rxData[7] << 8 |rxData[6];
    (*dataPtr).acroMode = ((bool)rxData[8] >> 1) & 0x01;
    (*dataPtr).armMotor = (bool)rxData[8] & 0x01;
    }

void Transceiver::setAcknowledgePayload(int pipe){
        uint8_t package[4];
        package[0] = (*dataPtr).batteryLevel & 0xFF;
        package[1] = (*dataPtr).batteryLevel >> 8;
        radio.writeAcknowledgePayload(pipe, &package[0], 2);
    }