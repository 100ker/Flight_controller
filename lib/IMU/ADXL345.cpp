#include "ADXL345.h"
#include <iostream>

int ADXL345::initialize(uint16_t gRangeInput, float xFilter, float yFilter, float zFilter){
    alphaX = xFilter;
    alphaY = yFilter;
    alphaZ = zFilter;
    Fx = 0.0;
    Fy = 0.0;
    Fz = 0.0;

    i2c.frequency(400000);
    gRange = gRangeInput;
    buffer[0] = REG_DATA_FORMAT;
    buffer[1] = convertGRange();

    int status = i2c.write(ADXL345_ADDRESS,buffer,2);
    if (status != 0){
//        pc.printf("%02x%02x\r\n",buffer[0],buffer[1]);
        return 1;
    }

    buffer[0] = REG_BW_RATE;
    buffer[1] = CMD_DATA_RATE_200HZ;
    status = i2c.write(ADXL345_ADDRESS,buffer,2);
    if (status != 0){
        return 2;
    }

    buffer[0] = REG_POWER_CTRL;
    buffer[1] = CMD_MEASUREMENT_MODE;

    status = i2c.write(ADXL345_ADDRESS,buffer,2);
    if (status != 0){
        return 3;
    }

//    status = selfTest();
//    if (status != 0){
//        return status;
//    }

    return 0;
}

uint8_t ADXL345::convertGRange(){
    uint8_t value = 0x00;
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
    // buffer[0] = REG_DATA_FORMAT;
    // buffer[1] = CMD_DATA_FORMAT_FULL_RES_16G;

    // int status = i2c.write(ADXL345_ADDRESS,buffer,2);
    // if (status != 0){
    //     return 4;
    // }

    // char bufferOff[20][6];
    // for (int i=0;i<20;i++){
    //     i2c.read(ADXL345_ADDRESS,bufferOff[i],6);
    // }

    // buffer[0] = REG_DATA_FORMAT;
    // buffer[1] = CMD_DATA_FORMAT_FULL_RES_16G + CMD_SELF_TEST;

    // status = i2c.write(ADXL345_ADDRESS,buffer,2);
    // if (status != 0){
    //     return 5;
    // }

    // wait(0.2);

    // char bufferOn[20][6];
    // for (int i=0;i<20;i++){
    //     i2c.read(ADXL345_ADDRESS,bufferOn[i],6);
    // }

    // float totalOff[3];
    // float averageOff[3];
    // for (int i=0;i<20;i++){
    //     totalOff[0] += ((float)(bufferOff[i][0])*3.9/1000.0);
    //     totalOff[1] += ((float)(bufferOff[i][2])*3.9/1000.0);
    //     totalOff[2] += ((float)(bufferOff[i][4])*3.9/1000.0);
    // }

    // averageOff[0] = totalOff[0]/20;
    // averageOff[1] = totalOff[1]/20;
    // averageOff[2] = totalOff[2]/20;

    // float totalOn[3];
    // float averageOn[3];
    // for (int i=0;i<20;i++){
    //     totalOn[0] += ((float)((int16_t)(bufferOn[i][0]))*32.0/1024.0);
    //     totalOn[1] += ((float)((int16_t)(bufferOn[i][2]))*32.0/1024.0);
    //     totalOn[2] += ((float)((int16_t)(bufferOn[i][4]))*32.0/1024.0);
    // }
    // averageOn[0] = totalOn[0]/20;
    // averageOn[1] = totalOn[1]/20;
    // averageOn[2] = totalOn[2]/20;
    // Serial pc(USBTX, USBRX); // tx, rx
    // if (averageOn[0]-averageOff[0] < 6.0 ){
    //     pc.printf("X: %.2f\r\n",averageOn[0]-averageOff[0]);
    //     return 6;
    // }
    // if (averageOn[1]-averageOff[1] > -6.0 ){
    //     pc.printf("Y: %.2f\r\n",averageOn[1]-averageOff[1]);
    //     return 7;
    // }
    // if (averageOn[2]-averageOff[2] < 10.0 ){
    //     pc.printf("Z: %.2f\r\n",averageOn[2]-averageOff[2]);
    //     return 8;
    // }

    // buffer[0] = REG_DATA_FORMAT;
    // buffer[1] = CMD_DATA_FORMAT_FULL_RES_16G;

    // status = i2c.write(ADXL345_ADDRESS,buffer,2);
    // if (status != 0){
    //     return 9;
    // }

    return 0;
}

int ADXL345::read(float * fx, float * fy, float * fz){
    // write register address to device
    buffer[0] = REG_DATA_X;
    i2c.write(ADXL345_ADDRESS,buffer,1,true);

//    Serial pc(USBTX, USBRX); // tx, rx

    // request data from device
    i2c.read(ADXL345_ADDRESS,buffer,6);
//    pc.printf("%02x%02x%02x%02x\r\n",buffer[0],buffer[1],buffer[2],buffer[3]);
    // calculate data from raw readings
    x = ((float)((int16_t)(buffer[1]<<8)+buffer[0]))*gRange/(1023.0);
    y = ((float)((int16_t)(buffer[3]<<8)+buffer[2]))*gRange/(1023.0);
    z = ((float)((int16_t)(buffer[5]<<8)+buffer[4]))*gRange/(1023.0);
    
    // filter data
    *fx = x * alphaX + Fx * (1.0-alphaX);
    *fy = y * alphaY + Fy * (1.0-alphaY);
    *fz = z * alphaZ + Fz * (1.0-alphaZ);

    // store filtered data for next calculation
    Fx = *fx;
    Fy = *fy;
    Fz = *fz;

    return 0;
}

int ADXL345::whoAmI(void){
    buffer[0] = 0x00;
//    buffer[1] = convertGRange(gRange);

    i2c.write(ADXL345_ADDRESS,buffer,1,true);
    i2c.read(ADXL345_ADDRESS,buffer,1);
    return buffer[0];

}
