#include "ITG3200.h"

int ITG3200::initialize(ITG3200ConfigStruct config)
{
    i2c.frequency(400000);

    // set sample rate and filter frequency
    buffer[0] = REG_CONFIG;
    buffer[1] = (FS_SELECT << FS_OFFSET) | SAMPLE_RATE_1KHZ_188;

    i2c.write(ITG3200_ADDRESS, buffer, 2);

    // set sample divider
    buffer[0] = REG_SAMPLE_DIVIDER;
    buffer[1] = 0x00;

    i2c.write(ITG3200_ADDRESS, buffer, 2);

    buffer[0] = REG_POWER_MANAGEMENT;
    buffer[1] = PLL_GYRO_X << CLOCK_SELECT_OFFSET;
    a = config.a;
    b = config.b;
    c = config.c;

    return 0;
}

int ITG3200::read(float *temp, float *roll, float *pitch, float *yaw)
{
    // write register address to device
    buffer[0] = REG_DATA_X;
    i2c.write(ITG3200_ADDRESS, buffer, 1, true);

    // request read from device
    i2c.read(ITG3200_ADDRESS, buffer, 6);

    // calculate data from raw readings
    // *temp = (float)(35 + ((int16_t)((buffer[0]<<8) + buffer[1])+13200.0)/280.0);
    *roll = (1 - a) * *roll + a * ((float)((int16_t)((buffer[0] << 8) + buffer[1])) / scaleFactor - rollOffset);
    *pitch = (1 - b) * *pitch + b * ((float)((int16_t)((buffer[2] << 8) + buffer[3])) / scaleFactor - pitchOffset);
    *yaw = (1 - c) * *yaw + c * ((float)((int16_t)((buffer[4] << 8) + buffer[5])) / scaleFactor - yawOffset);

    return 0;
}
