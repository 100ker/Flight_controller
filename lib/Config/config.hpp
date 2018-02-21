#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>

struct remoteStruct
{
    uint16_t throttle;
    float roll;
    float pitch;
    float yaw;
    uint16_t missedPackets;

    remoteStruct() : throttle(0),
                     roll(0),
                     pitch(0),
                     yaw(0)
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

struct dataStruct
{
    remoteStruct remote;
    imuStruct imu;
    uint16_t batteryLevel;
    bool acroMode;
    bool armMotor;

    dataStruct() : acroMode(true),
                   armMotor(false)
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

struct acroModeStruct
{
    float Kp[3], Ki[3], Kd[3];
};

struct stabilizingModeStruct
{
    float Kp[3], Ki[3], Kd[3];
};

struct controllerConfigStruct
{
    acroModeStruct acroModeConfig;
    stabilizingModeStruct stabilizingModeConfig;
    int8_t signs[4][3];
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
    float tickerPeriod;
};

#endif
