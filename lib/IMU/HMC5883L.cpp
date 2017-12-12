#include "HMC5883L.h"

int HMC5883L::initialize(){
    i2c.frequency(400000);

    // set config registry A
    buffer[0] = REG_CONFIG_A;
    buffer[1] = CONFIG_A_15_8;

    i2c.write(HMC5883L_ADDRESS,buffer,2);

    // set config registry B
    buffer[0] = REG_CONFIG_B;
    buffer[1] = CONFIG_B_DEFAULT;

    i2c.write(HMC5883L_ADDRESS,buffer,2);

    // set mode registry
    buffer[0] = REG_MODE;
    buffer[1] = MODE_CONTINUOUS;

    i2c.write(HMC5883L_ADDRESS,buffer,2);

    // check if device is ready
    // TODO



    return 0;
}

int HMC5883L::read(float * x, float * y, float * z){
    // write register address to device
    buffer[0] = REG_DATA_X;
    i2c.write(HMC5883L_ADDRESS,buffer,1,true);

    // request read from device
    i2c.read(HMC5883L_ADDRESS,buffer,6);

    // calculate data from raw readings
    *x = (float)(int16_t)((buffer[0]<<8) + buffer[1])*0.92;
    *y = (float)(int16_t)((buffer[2]<<8) + buffer[3])*0.92;
    *z = (float)(int16_t)((buffer[4]<<8) + buffer[5])*0.92;

    //*x = atan2(*y,*x)/3.14159265359*180;


    return 0;
}    
