#include "IMU.h"

/*
Initializes the devices on the IMU. Returns an error code if an error occurs.
It starts with the GYRO, which needs no parameters.
Secondly, it initializes the accelerometer. This need a maximum range and 3
filter values.
Finally, the magnetometer is initialized.

@param          Pointer to the data struct
@return         status. 0 if success, error code otherwise

*/
int IMU::initialize(dataStruct * data){
    int status;
    // Initialize the GYRO.
    status = itg3200.initialize();
    if (status)
        return status;

    // Initialize the accelerometer.
    status = adxl345.initialize(16,1,1,1);
    if (status)
        return status|0x10;

    // Initialize the magnetometer.
    status = hmc5883l.initialize();
    if (status)
        return status|0x20;

    dataPtr = data;

    return 0;
}

/*
Calibrates the devices on the IMU.

@param          none
@return         none

*/
void IMU::calibrate(void){



}

/*
Starts the transfers for the different devices on the IMU. It stores these values
in the class variables.

@param          none
@return         none

*/
void IMU::getReadings(void){
    // Read from accelerometer.
    adxl345.read(&accelerations[0],&accelerations[1],&accelerations[2]);

    // Read from GRYO.
    itg3200.read(&temp,&velocities[0],&velocities[1],&velocities[2]);

    // Read from magnetometer.
    hmc5883l.read(&heading[0],&heading[1],&heading[2]);
}

/*
Calculates the quaternions using the raw values.
It first generates the rotation matrix from the raw values.
Finally, it calculates the quaternions and stores them in the class variables.
the class variables.

@param          none
@return         none

*/
void IMU::calculateQuaternions(void){
    // The "Down" vector is gained directly from the accelerometer and is then
    // normalized for calculations.
//    Serial pc(USBTX, USBRX); // tx, rx
    rotationMatrix[2][0] = accelerations[0];
    rotationMatrix[2][1] = accelerations[1];
    rotationMatrix[2][2] = accelerations[2];
//    pc.printf("%.4f %.4f %.4f\r\n",rotationMatrix[2][0],rotationMatrix[2][1],rotationMatrix[2][2]);
    float total = sqrt(rotationMatrix[2][0]*rotationMatrix[2][0] + rotationMatrix[2][1]*rotationMatrix[2][1] + rotationMatrix[2][2]*rotationMatrix[2][2]);
    rotationMatrix[2][0] = rotationMatrix[2][0]/total;
    rotationMatrix[2][1] = rotationMatrix[2][1]/total;
    rotationMatrix[2][2] = rotationMatrix[2][2]/total;

    // The "East" vector is gained by taking the cross product of the "Down" vector
    // and the heading. Afterwards, it's normalized.
    rotationMatrix[1][0] = accelerations[1]* heading[2] - accelerations[2]*heading[1];
    rotationMatrix[1][1] = accelerations[2]* heading[0] - accelerations[0]*heading[2];
    rotationMatrix[1][2] = accelerations[0]* heading[1] - accelerations[1]*heading[0];

    total = sqrt(rotationMatrix[1][0]*rotationMatrix[1][0] + rotationMatrix[1][1]*rotationMatrix[1][1] + rotationMatrix[1][2]*rotationMatrix[1][2]);
    rotationMatrix[1][0] = rotationMatrix[1][0]/total;
    rotationMatrix[1][1] = rotationMatrix[1][1]/total;
    rotationMatrix[1][2] = rotationMatrix[1][2]/total;

    // Finally, the "North" vector is gained by taking the cross product of the
    // "Down" vector and the "East" vector. Afterwards, it's normalized.
    rotationMatrix[0][0] = rotationMatrix[1][1]*accelerations[2] - rotationMatrix[1][2]*accelerations[1];
    rotationMatrix[0][1] = rotationMatrix[1][2]*accelerations[0] - rotationMatrix[1][0]*accelerations[2];
    rotationMatrix[0][2] = rotationMatrix[1][0]*accelerations[1] - rotationMatrix[1][1]*accelerations[0];

    total = sqrt(rotationMatrix[0][0]*rotationMatrix[0][0] + rotationMatrix[0][1]*rotationMatrix[0][1] + rotationMatrix[0][2]*rotationMatrix[0][2]);
    rotationMatrix[0][0] = rotationMatrix[0][0]/total;
    rotationMatrix[0][1] = rotationMatrix[0][1]/total;
    rotationMatrix[0][2] = rotationMatrix[0][2]/total;

    // Calculation quaternions from the rotation matrix. First, it is checked if
    // the trace of the matrix is positive, since not doing  will result
    // in errors in the calculation (dividing by 0 or imaginary roots).
    float trace = rotationMatrix[0][0] + rotationMatrix[1][1] + rotationMatrix[2][2];
    float s;
    if (trace>0){
        s = 0.5/ sqrt(trace+1.0);
        qw = 0.25/s;
        qx = (rotationMatrix[2][1] - rotationMatrix[1][2])*s;
        qy = (rotationMatrix[0][2] - rotationMatrix[2][0])*s;
        qz = (rotationMatrix[1][0] - rotationMatrix[0][1])*s;
    } else if ((rotationMatrix[0][0]>rotationMatrix[1][1])&&(rotationMatrix[0][0]>rotationMatrix[2][2])){
        s = 2.0 * sqrt(1+rotationMatrix[0][0] - rotationMatrix[1][1] - rotationMatrix[2][2]);
        qw = (rotationMatrix[2][1] - rotationMatrix[1][2])/s;
        qx = 0.25*s;
        qy = (rotationMatrix[0][1] + rotationMatrix[1][0])/s;
        qz = (rotationMatrix[0][2] + rotationMatrix[2][0])/s;
    } else if (rotationMatrix[1][1]>rotationMatrix[2][2]){
        s = 2.0 * sqrt(1+rotationMatrix[1][1] - rotationMatrix[0][0] - rotationMatrix[2][2]);
        qw = (rotationMatrix[0][2] - rotationMatrix[2][0])/s;
        qx = (rotationMatrix[0][1] + rotationMatrix[1][0])/s;
        qy = 0.25*s;
        qz = (rotationMatrix[1][2] + rotationMatrix[2][1])/s;
    } else {
        s = 2.0 * sqrt(1+rotationMatrix[2][2] - rotationMatrix[0][0] - rotationMatrix[0][0]);
        qw = (rotationMatrix[1][0] - rotationMatrix[0][1])/s;
        qx = (rotationMatrix[0][2] + rotationMatrix[2][0])/s;
        qy = (rotationMatrix[1][2] + rotationMatrix[2][1])/s;
        qz = 0.25*s;
    }
}

/*
Updates the dataPtr. The function takes pointers to floats and updates the values
there.
It starts with updating the raw values. Then calculates the quaternions from
that dataPtr and finally calculates Euler angles from the quaterions.
Still has to be seen if we need Euler angles or if we can control using
quaternions.

@param          none
@return         none

*/
void IMU::update(void){
    // Get raw values from devices.
    getReadings();

    if (!(*dataPtr).acroMode){
        // Calculate quaternions from the raw values.
        calculateQuaternions();

        // Calculate Euler angles from quaternions.
        (*dataPtr).imu.roll = atan2(2*(qw*qx+qy*qz),1-2*(qx*qx+qy*qy))/3.14159265359*180;
        (*dataPtr).imu.pitch = asin(2*(qw*qy-qz*qx))/3.14159265359*180;
        (*dataPtr).imu.yaw = atan2(2*(qw*qz+qx*qy),1-2*(qy*qy+qz*qz))/3.14159265359*180;
    }
    else {
        (*dataPtr).imu.rollVelocity = velocities[0];
        (*dataPtr).imu.pitchVelocity = velocities[1];
        (*dataPtr).imu.yawVelocity = velocities[2];
    }
}

void IMU::estimator(float * roll, float * pitch){
    getReadings();
//    Serial pc(USBTX, USBRX); // tx, rx
    k = 1;
    float angle = -atan2(accelerations[1],accelerations[2])*180/3.1417;
    float error = estimated_roll - angle;
    estimated_roll += (-velocities[0] - error*k)*0.05;
    *roll = estimated_roll;


    angle = -atan2(accelerations[0],accelerations[2])*180/3.1417;
    error = estimated_pitch - angle;
    estimated_pitch += (-velocities[1] - error*k*4)*0.05;
    *pitch = estimated_pitch;

//    pc.printf("%.4f\t%.4f\r\n",estimated_roll,estimated_pitch);
}
