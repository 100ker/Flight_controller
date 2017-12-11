#ifndef ADXL345_H
#define ADXL345_H

#include <mbed.h>

class ADXL345{
    private:
        static const int ADXL345_ADDRESS = (0x53<<1);
        static const int REG_BW_RATE = 0x2C;
        static const int REG_POWER_CTRL = 0x2D;
        static const int REG_DATA_FORMAT = 0x31;
        static const int REG_DATA_X = 0x32;
        static const int REG_DATA_Y = 0x34;
        static const int REG_DATA_Z = 0x36;

        static const int CMD_MEASUREMENT_MODE = 0x08;
        static const int CMD_DATA_RATE_200HZ = 0x0B;
        static const int CMD_DATA_FORMAT_FULL_RES_16G = 0x0B;
        static const int CMD_SELF_TEST = 0x80;

        char buffer[6];
        uint16_t gRange;
        float x, y, z;
        float Fx, Fy, Fz;
        float alphaX, alphaY, alphaZ;

        I2C i2c;

        uint8_t convertGRange(uint16_t gRange);

    public:
        int initialize(uint16_t gRange, float xFilter, float yFilter, float zFilter);
            ADXL345(): i2c(p28,p27){}
        int read(float * fx, float * fy, float * fz);
        int selfTest(void);
        int whoAmI(void);
};

#endif
