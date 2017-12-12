#include "mbed.h"
#include "config.h"
#include "transceiver.h"
#include "IMU.h"
#include "controller.h"
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
    radio.update();
    imu.update();
    controller.update();
    data.batteryLevel = battery.read_u16();
    radio.setAcknowledgePayload(0);
}

int main() {
    // wait(5);
    led=0;
    status = radio.initialize(config, &data);
    if (status) {
        led2 = 1;
        return 0;
    }

    imu.initialize(&data);

    led=1;

    while (!data.remote.throttle){
        radio.update();
        std::cout << (int)data.remote.throttle << std::endl;
    }

    led2=1;

    while (data.remote.throttle < 1000){
        radio.update();
        std::cout << (int)data.remote.throttle << std::endl;
    }
    led2=0;
    led3=1;

    controller.initialize(&data, &config.controllerConfig);

    ticker.attach(&tick,config.tickerPeriod);
}
