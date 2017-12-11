#include "HMC5883L.h"

int HMC5883L::initialize(){
    this->i2c.frequency(400000);

    // set config registry A
    this->buffer[0] = this->REG_CONFIG_A;
    this->buffer[1] = this->CONFIG_A_15_8;

    this->i2c.write(this->HMC5883L_ADDRESS,this->buffer,2);

    // set config registry B
    this->buffer[0] = this->REG_CONFIG_B;
    this->buffer[1] = this->CONFIG_B_DEFAULT;

    this->i2c.write(this->HMC5883L_ADDRESS,this->buffer,2);

    // set mode registry
    this->buffer[0] = this->REG_MODE;
    this->buffer[1] = this->MODE_CONTINUOUS;

    this->i2c.write(this->HMC5883L_ADDRESS,this->buffer,2);

    // check if device is ready
    // TODO



    return 0;
}

int HMC5883L::read(float * x, float * y, float * z){
    // write register address to device
    this->buffer[0] = this->REG_DATA_X;
    this->i2c.write(this->HMC5883L_ADDRESS,this->buffer,1,true);

    // request read from device
    this->i2c.read(this->HMC5883L_ADDRESS,this->buffer,6);

    // calculate data from raw readings
    *x = (float)(int16_t)((this->buffer[0]<<8) + this->buffer[1])*0.92;
    *y = (float)(int16_t)((this->buffer[2]<<8) + this->buffer[3])*0.92;
    *z = (float)(int16_t)((this->buffer[4]<<8) + this->buffer[5])*0.92;

    //*x = atan2(*y,*x)/3.14159265359*180;


    return 0;
}    
