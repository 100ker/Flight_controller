#include <mbed.h>
#include "Watchdog.hpp"
#include <config.hpp>
#include "serialHandler.hpp"
#include "transceiver.h"
#include "IMU.h"
#include "controller.hpp"
#include "helpers.hpp"

Serial pc(USBTX,USBRX);
SerialHandler serial;
DigitalOut led(LED1), led2(LED2), led3(LED3), led4(LED4), radioPower(p30);
AnalogIn battery(p20);
dataStruct data;
configStruct config;
Transceiver radio(p5, p6, p7, p9, p14);
InterruptIn radioInterrupt(p10);
Ticker controllerInterrupt, gyroInterrupt, ledTicker, angleInterrupt, batteryTicker;
Controller controller(p22, p24, p23, p21); // p21 = fl, p22 = fr, p23 = rl, p24 = rr
IMU imu;
LocalFileSystem local("local");

uint8_t status;

void batteryLevelUpdate(void){
        data.batteryLevel.f = battery.read_u16();
}

void ledUpdate(void){
    led4 = 1-led4;
}

int main(void)
{
    loadConfig();

    serial.initialize(); 

    radioPower=1;
    wait(0.5);
    status = radio.initialize();

    if (status)
    {
        ledTicker.attach(&ledUpdate, 0.5);
        while (status)
        {
            radioPower=0;
            wait(0.5);
            radioPower=1;
            wait(0.5);
            status = radio.initialize();
        }
        ledTicker.detach();
    }

    radioInterrupt.fall(callback(&radio, &Transceiver::interruptHandler));

    status = imu.initialize();
    if (status)
    {
        led2 = 1;
        return 0;
    }

    ledTicker.attach(&ledUpdate, 0.5);
    batteryTicker.attach(&batteryLevelUpdate, 0.1);

    led4=1;
    data.newPacket = false;
    while(!data.newPacket){
        data.batteryLevel.f = battery.read_u16();
    }

    ledTicker.detach();
    led4 = 0;

    controller.initialize();

    controllerInterrupt.attach(callback(&controller, &Controller::update), 1.0/config.flightTickerFrequency);
    gyroInterrupt.attach(callback(&imu, &IMU::updateGyro),1.0/config.gyroTickerFrequency);
    angleInterrupt.attach(callback(&imu, &IMU::updateAngles),1.0/config.angleTickerFrequency);

    // while(1){      
        // pc.printf("Tesc: %.6f\t", ((float)data.remote.throttle)/65536.0f);
        // pc.printf("Resc: %.6f\t", ((float)data.remote.roll)/32768.0f);
        // pc.printf("Pesc: %.6f\t", ((float)data.remote.pitch)/32768.0f);
        // pc.printf("Yesc: %.6f\n", ((float)data.remote.yaw)/32768.0f);
        // pc.printf("Rimu: %.6f\t", data.imu.rollVelocity);
        // pc.printf("Pimu: %.6f\t", data.imu.pitchVelocity);
        // pc.printf("Yimu: %.6f\t", data.imu.yawVelocity);        
        // pc.printf("Rimu: %.6f\t", data.imu.roll);
        // pc.printf("Pimu: %.6f\t", data.imu.pitch);
        // pc.printf("Yimu: %.6f\n", data.imu.yaw);
    //     pc.printf("armed: %d\t", data.armMotor);
    //     pc.printf("acro: %d\n", data.acroMode);        
    // }
}
