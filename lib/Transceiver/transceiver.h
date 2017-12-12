#include <nRF24L01P.h>
#include <config.h>

class Transceiver{
    private:
        nRF24L01P radio;
        uint8_t transferSize;
        char * rxData;
        dataStruct * dataPtr;

    public:
        Transceiver(PinName mosi, PinName miso, PinName sck, PinName csn, PinName ce, PinName irq): radio(mosi,miso,sck,csn,ce,irq){};
        uint8_t initialize(configStruct config, dataStruct * data);
        bool messageAvailable();
        void send();
        void receive(int pipe,char* buffer,uint8_t length);
        void update();
        void setAcknowledgePayload(int pipe);
    };
