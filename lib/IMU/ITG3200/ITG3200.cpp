#include "ITG3200.h"

int ITG3200::initialize(){
    this->i2c.frequency(400000);

    // set sample rate and filter frequency
    this->buffer[0] = this->REG_CONFIG;
    this->buffer[1] = (this->FS_SELECT << this->FS_OFFSET) + this->SAMPLE_RATE_8KHZ_256;
    
    this->i2c.write(this->ITG3200_ADDRESS,this->buffer,2);
    
    // set sample divider
    this->buffer[0] = this->REG_SAMPLE_DIVIDER;
    this->buffer[1] = 0x00;
    
    this->i2c.write(this->ITG3200_ADDRESS,this->buffer,2);
    
    return 0;    
}

int ITG3200::read(float* temp, float * fx, float * fy, float * fz){
    // write register address to device
    this->buffer[0] = this->REG_DATA_TEMP;
    this->i2c.write(this->ITG3200_ADDRESS,this->buffer,1,true);
    
    // request read from device
    this->i2c.read(this->ITG3200_ADDRESS,this->buffer,8);
    
    // calculate data from raw readings
    *temp = (float)(35 + ((int16_t)((this->buffer[0]<<8) + this->buffer[1])+13200.0)/280.0);
    *fx = (float)((int16_t)((this->buffer[2]<<8) + this->buffer[3]))/this->scaleFactor;
    *fy = (float)((int16_t)((this->buffer[4]<<8) + this->buffer[5]))/this->scaleFactor;
    *fz = (float)((int16_t)((this->buffer[6]<<8) + this->buffer[7]))/this->scaleFactor;

    return 0;
}    