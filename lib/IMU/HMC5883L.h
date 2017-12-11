#ifndef HMC5883L_H
#define HMC5883L_H

#include <mbed.h>

class HMC5883L{
    private:
    // address and register address values
        static const int HMC5883L_ADDRESS = (0x1E<<1);
        static const int REG_CONFIG_A = 0x00;
        static const int REG_CONFIG_B = 0x01;
        static const int REG_MODE = 0x02;
        static const int REG_DATA_X = 0x03;
        static const int REG_DATA_Y = 0x05;
        static const int REG_DATA_Z = 0x07;
        static const int REG_STATUS = 0x09;
        static const int REG_ID = 0x0A;

    // registry default values
        static const int CONFIG_A_DEFAULT = 0x10; // corresponds to 15 Hz
        static const int CONFIG_A_15_8 = 0x70; // 15 Hz, average of 8 samples
        static const int CONFIG_B_DEFAULT = 0x20; // +-1.3 Ga, corresponds with 1090 LSb/Gauss
        static const int MODE_DEFAULT = 0x01; // single measurement mode
        static const int MODE_CONTINUOUS = 0x00; // continuous measurement mode

    // error registry value
        static const int MEASUREMENT_OVERFLOW = -4096; // this value is in data registers if ADC overflow occurs

        char buffer[6];
        float x, y, z;

        I2C i2c;

    public:
        int initialize();
            HMC5883L(): i2c(p28,p27){}
        int read(float * x, float * y, float * z);
};

#endif
