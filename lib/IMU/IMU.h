#ifndef IMU_H
#define IMU_H

#include "ADXL345.h"
#include "ITG3200.h"
#include "HMC5883L.h"
#include "config.h"

class IMU{
    private:
        ITG3200 itg3200;
        ADXL345 adxl345;
        HMC5883L hmc5883l;
        dataStruct * dataPtr;

        float accelerations[3], velocities[3], heading[3], rotationMatrix[3][3];
        float temp;
        float qw, qx, qy, qz;
        float estimated_roll, estimated_pitch, k;

        void getReadings(void);
        void calculateQuaternions(void);
    public:
        int initialize(dataStruct * data);
        void calibrate(void);
        void update(void);
        void estimator(float * roll, float * pitch);

};

#endif
