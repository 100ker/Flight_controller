#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>

struct controllerStruct{
    uint16_t throttle;
    uint16_t roll;
    uint16_t pitch;
    uint16_t yaw;

    controllerStruct() : throttle(0),
                         roll(0),
                         pitch(0),
                         yaw(0)
                         {}
};

struct imuStruct {
    uint16_t roll;
    uint16_t pitch;
    uint16_t yaw;
    int16_t rollVelocity;
    int16_t pitchVelocity;
    int16_t yawVelocity;
};

struct dataStruct{
    controllerStruct controller;
    imuStruct imu;
    uint16_t batteryLevel;
    bool acroMode;
    bool armMotor;

    dataStruct() :  acroMode(false),
                    armMotor(false)
                    {
                    }
    } ;

struct configStruct{
    uint8_t channel;
    uint64_t txAddress;
    uint64_t rxAddress;
    uint8_t transferSize;
    float tickerPeriod;

    configStruct() :    channel(101),
                        txAddress(0x00F0F0F0F0),
                        rxAddress(0x00F0F0F0F0),
                        transferSize(8),
                        tickerPeriod(1.0/500.0)
                        {
                        }

    };
#endif
