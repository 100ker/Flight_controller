#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>

struct remoteStruct
{
    volatile float throttle;
    volatile float roll;
    volatile float pitch;
    volatile float yaw;
    volatile bool signalLost;
    volatile uint16_t missedPackets;

    remoteStruct() : throttle(0),
                     roll(0),
                     pitch(0),
                     yaw(0),
                     signalLost(false)
    {
    }
};

struct imuStruct
{
    float roll;
    float pitch;
    float yaw;
    float rollVelocity;
    float pitchVelocity;
    float yawVelocity;
};

typedef union _batteryLevelUnion {
    uint16_t f;
    uint8_t u[2];
} _batteryLevel;

struct dataStruct
{
    remoteStruct remote;
    imuStruct imu;
    _batteryLevel batteryLevel;
    volatile bool acroMode;
    volatile bool armMotor;
    volatile bool newPacket;

    dataStruct() : acroMode(false),
                   armMotor(false),
                   newPacket(false)
    {
    }
};

struct radioConfigStruct
{
    uint8_t channel;
    uint64_t txAddress;
    uint64_t rxAddress;
    uint8_t transferSize;
};

struct rateControllerStruct
{
    float KpRoll, KiRoll, KdRoll;
    float KpPitch, KiPitch, KdPitch;
    float KpYaw, KiYaw, KdYaw;
};

struct angleControllerStruct
{
    float KpRoll, KiRoll, KdRoll;
    float KpPitch, KiPitch, KdPitch;
};

struct controllerConfigStruct
{
    rateControllerStruct rateController;
    angleControllerStruct angleController;
    int8_t signs[4][3];
    float anglePrescalerRoll;
    float anglePrescalerPitch;
    float ratePrescalerRoll;
    float ratePrescalerPitch;
    float ratePrescalerYaw;
    float angleImuPrescalerRoll;
    float angleImuPrescalerPitch;
    float rateImuPrescalerRoll;
    float rateImuPrescalerPitch;
    float rateImuPrescalerYaw;
};

struct ITG3200ConfigStruct
{
    float a;
    float b;
    float c;
};

struct HMC5883LConfigStruct
{
    float a;
    float b;
    float c;
};

struct ADXL345ConfigStruct
{
    float a;
    float b;
    float c;
};

struct imuConfigStruct
{
    ADXL345ConfigStruct adxl345;

    HMC5883LConfigStruct hmc5883l;

    ITG3200ConfigStruct itg3200;
};

struct configStruct
{
    radioConfigStruct radioConfig;
    controllerConfigStruct controllerConfig;
    imuConfigStruct imuconfig;
    uint16_t flightTickerFrequency;
    uint16_t gyroTickerFrequency;
    uint16_t angleTickerFrequency;
};

extern dataStruct data;
extern configStruct config;

#endif
