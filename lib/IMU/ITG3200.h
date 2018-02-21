#ifndef ITG3200_H
#define ITG3200_H

#include "mbed.h"
#include "config.hpp"

class ITG3200{
    private:
        static const int  ITG3200_ADDRESS = (0x68<<1);
        static const int  REG_SAMPLE_DIVIDER = 0x15;
        static const int  REG_CONFIG = 0x16;
        static const int  REG_DATA_TEMP = 0x1B;
        static const int  REG_DATA_X = 0x1D;
        static const int  REG_DATA_Y = 0x1F;
        static const int  REG_DATA_Z = 0x21;


        static const int  SAMPLE_RATE_8KHZ_256 = 0x00;
        static const int  SAMPLE_RATE_1KHZ_188 = 0x01;
        static const int  SAMPLE_RATE_1KHZ_98 = 0x02;
        static const int  SAMPLE_RATE_1KHZ_42 = 0x03;
        static const int  SAMPLE_RATE_1KHZ_20 = 0x04;
        static const int  SAMPLE_RATE_1KHZ_10 = 0x05;
        static const int  SAMPLE_RATE_1KHZ_5 = 0x06;

        static const int  FS_OFFSET = 0x03;
        static const int  FS_SELECT = 0x03;

        static const float scaleFactor = 14.375;

        char buffer[8];
        float x,y,z;
        float a, b, c;

        I2C i2c;

    public:
        int initialize(ITG3200ConfigStruct config);
            ITG3200(): i2c(p28,p27){}
        int read(float * temp, float * fx, float * fy, float * fz);
};

#endif
