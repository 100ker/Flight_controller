#include "transceiver.h"
#include "mbed.h"

uint8_t Transceiver::initialize(uint8_t channel, unsigned long long txAddress, unsigned long long rxAddress, uint8_t transferSize){
    Serial pc(USBTX, USBRX);
    radio.powerUp();
    radio.setRfFrequency (2400 + channel);
    radio.setTransferSize(2*transferSize);
    radio.setCrcWidth(16);
    radio.enableAutoAcknowledge(NRF24L01P_PIPE_P0);
    radio.setRxAddress(rxAddress);
    radio.setTxAddress(txAddress);
    uint8_t statRegister = radio.getStatusRegister();
    // check if status register is correct. If not, a complete reboot is needed
    if ((statRegister != 0x08) && (statRegister != 0x0e)){
        pc.printf("%02x\r\n",statRegister);
        return 1;
    }
    radio.setReceiveMode();
    radio.enable();
    
    this->transferSize = transferSize;
    rxData = new char[transferSize];
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
    
void Transceiver::update(dataStruct * data){
    Serial pc(USBTX, USBRX); // tx, rx
    
    receive(NRF24L01P_PIPE_P0, rxData, transferSize);
    (*data).throttle = (uint16_t)rxData[1] << 8 |rxData[0];
    (*data).roll = (uint16_t)rxData[3] << 8 |rxData[2];
    (*data).pitch = (uint16_t)rxData[5] << 8 |rxData[4];
    (*data).yaw = (uint16_t)rxData[7] << 8 |rxData[6];
    }