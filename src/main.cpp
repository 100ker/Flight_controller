#include "mbed.h"
#include "controller.h"
#include "transceiver.h"
#include "IMU.h"
#include "config.h"
#include <iostream>
#include <bitset>

DigitalOut led(LED1), led2(LED2), led3(LED3);
AnalogIn battery(p20);
dataStruct data;
configStruct config;
Transceiver radio(p5, p6, p7, p8, p9, p10);
Ticker ticker;
Controller controller(p21, p22, p23, p24);
IMU imu;

uint8_t status;

void tick(void){
    radio.update(&data);
    controller.update(&data);
    data.batteryLevel = battery.read_u16();
    radio.setAcknowledgePayload(0,&data);
}

int main() {
    // wait(5);
    led=0;
    status = radio.initialize(config.channel,config.rxAddress, config.txAddress,config.transferSize);
    if (status) {
        led2 = 1;
        return 0;
    }

    imu.initialize();

    led=1;

    while (!data.controller.throttle){
        radio.update(&data);
        std::cout << (int)data.controller.throttle << std::endl;
    }

    led2=1;

    while (data.controller.throttle < 1000){
        radio.update(&data);
        std::cout << (int)data.controller.throttle << std::endl;
    }
    led2=0;
    led3=1;

    controller.initialize();

    ticker.attach(&tick,config.tickerPeriod);
}
