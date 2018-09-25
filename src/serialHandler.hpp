#include <mbed.h>
#include "config.hpp"
#include <iniparser.h>

class SerialHandler
{
  public:
    SerialHandler() : _connection(USBTX, USBRX){};
    void initialize(void)
    {
        _connection.baud(9600);
        _connection.attach(callback(this, &SerialHandler::rxInterruptHandler), Serial::RxIrq);
        //_connection.putc('a');
        // _connection.attach(callback(this, &SerialHandler::txInterruptHandler), Serial::TxIrq);
    };

  private:
    Serial _connection;
    char rxBuffer[256], txBuffer[256];
    uint8_t rxBufferPosition, txBufferPosition, interpreterPosition;

    void rxInterruptHandler(void)
    {   
        DigitalOut led(LED1);
        led = 1;
        rxBuffer[rxBufferPosition] = _connection.getc();
        _connection.putc(rxBuffer[rxBufferPosition]);
        if (rxBuffer[rxBufferPosition] == '\n')
        {
            interpretData();
        }
        else
        {
            rxBufferPosition++;
        }
    };
    void txInterruptHandler(void){
        // _connection.putc(txBuffer[txBufferPosition]);
        // txBufferPosition++;
    };

    void interpretData(void)
    {
        while (interpreterPosition <= rxBufferPosition)
        {
            switch (rxBuffer[interpreterPosition])
            {
            case '0':{
                _connection.putc('a');
                _connection.printf("Wilbur\n");
                rxBufferPosition = 0;
                interpreterPosition = 0;
                return;
            }
            case '1':{
                _connection.printf("%u\n", config.radioConfig.channel);
                _connection.printf("%lx\n", config.radioConfig.txAddress);
                _connection.printf("%u\n", config.flightTickerFrequency);
                _connection.printf("%u\n", config.gyroTickerFrequency);
                rxBufferPosition = 0;
                interpreterPosition = 0;
                return;
            }
            case '2':
                {
                LocalFileSystem local("local");
                dictionary *dir = iniparser_load("/local/config.ini");
                FILE *file = fopen("/local/config.ini", "w");
                
                char formattedString[15];
                interpreterPosition++;
                sprintf(formattedString, "%c%c%c", rxBuffer[interpreterPosition++], rxBuffer[interpreterPosition++], rxBuffer[interpreterPosition++]);
                iniparser_set(dir, "radio:channel", formattedString);
                sprintf(formattedString, "0x00%c%c%c%c%c%c%c%c", rxBuffer[interpreterPosition++], rxBuffer[interpreterPosition++], rxBuffer[interpreterPosition++], rxBuffer[interpreterPosition++], rxBuffer[interpreterPosition++], rxBuffer[interpreterPosition++], rxBuffer[interpreterPosition++], rxBuffer[interpreterPosition++]);
                iniparser_set(dir, "radio:txaddress", formattedString);
                iniparser_set(dir, "radio:rxaddress", formattedString);
                sprintf(formattedString, "%c%c%c%c", rxBuffer[interpreterPosition++], rxBuffer[interpreterPosition++], rxBuffer[interpreterPosition++], rxBuffer[interpreterPosition++]);
                iniparser_set(dir, "misc:tickerfrequency", formattedString);
                iniparser_dump_ini(dir, file);
                fclose(file);
                
                rxBufferPosition = 0;
                interpreterPosition = 0;
                return;
                }
            default:
                interpreterPosition = 0;
                rxBufferPosition = 0;
                return;
            }
            interpreterPosition++;
        }
    }
};