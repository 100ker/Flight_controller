#include "ADXL345.h"

int ADXL345::initialize(uint16_t gRange, float xFilter, float yFilter, float zFilter){
    this->alphaX = xFilter;
    this->alphaY = yFilter;
    this->alphaZ = zFilter;
    this->Fx = 0.0;
    this->Fy = 0.0;
    this->Fz = 0.0;
    
    
    this->gRange = 2*gRange;
    
    this->i2c.frequency(400000);
    
    this->buffer[0] = this->REG_DATA_FORMAT;
    this->buffer[1] = this->convertGRange(gRange);

    int status = this->i2c.write(this->ADXL345_ADDRESS,this->buffer,2);
    if (status != 0){
//        pc.printf("%02x%02x\r\n",this->buffer[0],this->buffer[1]);
        return 1;
    }
    
    this->buffer[0] = this->REG_BW_RATE;
    this->buffer[1] = this->CMD_DATA_RATE_200HZ;
    status = this->i2c.write(this->ADXL345_ADDRESS,this->buffer,2);
    if (status != 0){
        return 2;
    }
    
    this->buffer[0] = this->REG_POWER_CTRL;
    this->buffer[1] = this->CMD_MEASUREMENT_MODE;
    
    status = this->i2c.write(this->ADXL345_ADDRESS,this->buffer,2);
    if (status != 0){
        return 3;
    }
    
//    status = this->selfTest();
//    if (status != 0){
//        return status;
//    }
    
    return 0;
}

uint8_t ADXL345::convertGRange(uint16_t gRange){
    uint8_t value;
    switch (gRange) {
        case 2:
            value = 0x00;
            break;
        case 4:
            value = 0x01;
            break;
        case 8:
            value = 0x02;
            break;
        case 16:
            value = 0x03;
            break;
        }
        
    return value;
}

int ADXL345::selfTest(void){
    this->buffer[0] = this->REG_DATA_FORMAT;
    this->buffer[1] = this->CMD_DATA_FORMAT_FULL_RES_16G;
    
    int status = this->i2c.write(this->ADXL345_ADDRESS,this->buffer,2);
    if (status != 0){
        return 4;
    }
    
    char bufferOff[20][6];
    for (int i=0;i<20;i++){
        this->i2c.read(this->ADXL345_ADDRESS,bufferOff[i],6);
    }
    
    this->buffer[0] = this->REG_DATA_FORMAT;
    this->buffer[1] = this->CMD_DATA_FORMAT_FULL_RES_16G + CMD_SELF_TEST;
    
    status = this->i2c.write(this->ADXL345_ADDRESS,this->buffer,2);
    if (status != 0){
        return 5;
    }
    
    wait(0.2);
    
    char bufferOn[20][6];
    for (int i=0;i<20;i++){
        this->i2c.read(this->ADXL345_ADDRESS,bufferOn[i],6);
    }
    
    float totalOff[3];
    float averageOff[3];
    for (int i=0;i<20;i++){
        totalOff[0] += ((float)(bufferOff[i][0])*3.9/1000.0);
        totalOff[1] += ((float)(bufferOff[i][2])*3.9/1000.0);
        totalOff[2] += ((float)(bufferOff[i][4])*3.9/1000.0);
    }
    
    averageOff[0] = totalOff[0]/20;
    averageOff[1] = totalOff[1]/20;
    averageOff[2] = totalOff[2]/20;
    
    float totalOn[3];
    float averageOn[3];
    for (int i=0;i<20;i++){
        totalOn[0] += ((float)((int16_t)(bufferOn[i][0]))*32.0/1024.0);
        totalOn[1] += ((float)((int16_t)(bufferOn[i][2]))*32.0/1024.0);
        totalOn[2] += ((float)((int16_t)(bufferOn[i][4]))*32.0/1024.0);
    }
    averageOn[0] = totalOn[0]/20;
    averageOn[1] = totalOn[1]/20;
    averageOn[2] = totalOn[2]/20;
    Serial pc(USBTX, USBRX); // tx, rx
    if (averageOn[0]-averageOff[0] < 6.0 ){
        pc.printf("X: %.2f\r\n",averageOn[0]-averageOff[0]);
        return 6;
    }
    if (averageOn[1]-averageOff[1] > -6.0 ){
        pc.printf("Y: %.2f\r\n",averageOn[1]-averageOff[1]);
        return 7;
    }
    if (averageOn[2]-averageOff[2] < 10.0 ){
        pc.printf("Z: %.2f\r\n",averageOn[2]-averageOff[2]);
        return 8;
    }
    
    this->buffer[0] = this->REG_DATA_FORMAT;
    this->buffer[1] = this->CMD_DATA_FORMAT_FULL_RES_16G;
    
    status = this->i2c.write(this->ADXL345_ADDRESS,this->buffer,2);
    if (status != 0){
        return 9;
    }
    
    return 0;
}

int ADXL345::read(float * fx, float * fy, float * fz){
    // write register address to device
    this->buffer[0] = this->REG_DATA_X;
    this->i2c.write(this->ADXL345_ADDRESS,buffer,1,true);
    
//    Serial pc(USBTX, USBRX); // tx, rx
    
    // request data from device
    this->i2c.read(this->ADXL345_ADDRESS,buffer,6);
//    pc.printf("%02x%02x%02x%02x\r\n",this->buffer[0],this->buffer[1],this->buffer[2],this->buffer[3]);
    // calculate data from raw readings
    this->x = ((float)((int16_t)(buffer[1]<<8)+buffer[0]))*this->gRange/(1024.0);
    this->y = ((float)((int16_t)(buffer[3]<<8)+buffer[2]))*this->gRange/(1024.0);
    this->z = ((float)((int16_t)(buffer[5]<<8)+buffer[4]))*this->gRange/(1024.0);
    
    // filter data
    *fx = x * this->alphaX + this->Fx * (1.0-this->alphaX);
    *fy = y * this->alphaY + this->Fy * (1.0-this->alphaY);
    *fz = z * this->alphaZ + this->Fz * (1.0-this->alphaZ);
    
    // store filtered data for next calculation
    this->Fx = *fx;
    this->Fy = *fy;
    this->Fz = *fz;
    
    return 0;
}

int ADXL345::whoAmI(void){
    this->buffer[0] = 0x00;
//    this->buffer[1] = this->convertGRange(gRange);

    int status = this->i2c.write(this->ADXL345_ADDRESS,this->buffer,1,true);
    this->i2c.read(this->ADXL345_ADDRESS,this->buffer,1);
    return buffer[0];

}