#include "nRF24L01P.h"
#include "config.h"

class Transceiver{
    private:
        nRF24L01P radio;
        uint8_t transferSize;
        char * rxData;
    
    public:
        Transceiver(PinName mosi, PinName miso, PinName sck, PinName csn, PinName ce, PinName irq): radio(mosi,miso,sck,csn,ce,irq){};
        uint8_t initialize(uint8_t channel, unsigned long long txAddress, unsigned long long rxAddress, uint8_t transferSize);
        bool messageAvailable();
        void send();
        void receive(int pipe,char* buffer,uint8_t length);
        void update(dataStruct * data);    
    };