
#include "mbed.h"
#include "nRF24L01P.h"
#include "controller.h"
#include "transceiver.h"
#include "config.h"
 
Serial pc(USBTX, USBRX); // tx, rx
DigitalOut led(LED1), led2(LED2), led3(LED3);
dataStruct data;
configStruct config;
Transceiver radio(p5, p6, p7, p8, p9, p10);
Ticker ticker;
Controller controller(p21, p22, p23, p24);

uint8_t status;
 
 void tick(void){
        radio.update(&data);             
        controller.update(&data);
//        pc.printf("Throttle = %u Roll = %u Pitch = %u Yaw = %u\n\r", data.throttle, data.roll, data.pitch, data.yaw);
     }
 
int main() {
    status = radio.initialize(config.channel,config.rxAddress, config.txAddress,config.transferSize);
    if (status) {
        //pc.printf("%02x\r\n",status);
        led2 = 1;
        return 0;
        }

    radio.update(&data);
    led=0;
    while (data.throttle < 1000){radio.update(&data);}
    led=1;
    while (data.throttle != 0) {radio.update(&data);}
    controller.initialize();
    ticker.attach(&tick,config.tickerPeriod);
}

/*#include <mbed.h>

#define max(a,b) ({ __typeof__ (a) _a = (a);  __typeof__ (b) _b = (b);  _a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__ (a) _a = (a);  __typeof__ (b) _b = (b);  _a < _b ? _a : _b; })

Serial pc(USBTX, USBRX); // tx, rx

PwmOut servo(p22);
DigitalOut ground(p20);
DigitalOut Vcc(p18);
AnalogIn throttle(p19);

int main() {
    ground = 0;
    Vcc = 1;
    servo.period_us(4000);          // servo requires a 20ms period

    while(1){
        float throttle_us = min(max(125.0f + 125.0f * throttle.read(),125),250);
        pc.printf("%.6f\r", throttle_us);
        servo.pulsewidth_us(throttle_us); // servo position determined by a pulsewidth between 1-2ms

    }
}
*/