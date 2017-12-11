#include "IMU.h"

/*
Initializes the devices on the IMU. Returns an error code if an error occurs.
It starts with the GYRO, which needs no parameters.
Secondly, it initializes the accelerometer. This need a maximum range and 3
filter values.
Finally, the magnetometer is initialized.

@param          none
@return         status. 0 if success, error code otherwise

*/
int IMU::initialize(void){
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
    this->adxl345.read(&this->accelerations[0],&this->accelerations[1],&this->accelerations[2]);

    // Read from GRYO.
    this->itg3200.read(&this->temp,&this->velocities[0],&this->velocities[1],&this->velocities[2]);

    // Read from magnetometer.
    this->hmc5883l.read(&this->heading[0],&this->heading[1],&this->heading[2]);
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
    this->rotationMatrix[2][0] = this->accelerations[0];
    this->rotationMatrix[2][1] = this->accelerations[1];
    this->rotationMatrix[2][2] = this->accelerations[2];
//    pc.printf("%.4f %.4f %.4f\r\n",this->rotationMatrix[2][0],this->rotationMatrix[2][1],this->rotationMatrix[2][2]);
    float total = sqrt(this->rotationMatrix[2][0]*this->rotationMatrix[2][0] + this->rotationMatrix[2][1]*this->rotationMatrix[2][1] + this->rotationMatrix[2][2]*this->rotationMatrix[2][2]);
    this->rotationMatrix[2][0] = this->rotationMatrix[2][0]/total;
    this->rotationMatrix[2][1] = this->rotationMatrix[2][1]/total;
    this->rotationMatrix[2][2] = this->rotationMatrix[2][2]/total;

    // The "East" vector is gained by taking the cross product of the "Down" vector
    // and the heading. Afterwards, it's normalized.
    this->rotationMatrix[1][0] = this->accelerations[1]* this->heading[2] - this->accelerations[2]*this->heading[1];
    this->rotationMatrix[1][1] = this->accelerations[2]* this->heading[0] - this->accelerations[0]*this->heading[2];
    this->rotationMatrix[1][2] = this->accelerations[0]* this->heading[1] - this->accelerations[1]*this->heading[0];

    total = sqrt(this->rotationMatrix[1][0]*this->rotationMatrix[1][0] + this->rotationMatrix[1][1]*this->rotationMatrix[1][1] + this->rotationMatrix[1][2]*this->rotationMatrix[1][2]);
    this->rotationMatrix[1][0] = this->rotationMatrix[1][0]/total;
    this->rotationMatrix[1][1] = this->rotationMatrix[1][1]/total;
    this->rotationMatrix[1][2] = this->rotationMatrix[1][2]/total;

    // Finally, the "North" vector is gained by taking the cross product of the
    // "Down" vector and the "East" vector. Afterwards, it's normalized.
    this->rotationMatrix[0][0] = this->rotationMatrix[1][1]*this->accelerations[2] - this->rotationMatrix[1][2]*this->accelerations[1];
    this->rotationMatrix[0][1] = this->rotationMatrix[1][2]*this->accelerations[0] - this->rotationMatrix[1][0]*this->accelerations[2];
    this->rotationMatrix[0][2] = this->rotationMatrix[1][0]*this->accelerations[1] - this->rotationMatrix[1][1]*this->accelerations[0];

    total = sqrt(this->rotationMatrix[0][0]*this->rotationMatrix[0][0] + this->rotationMatrix[0][1]*this->rotationMatrix[0][1] + this->rotationMatrix[0][2]*this->rotationMatrix[0][2]);
    this->rotationMatrix[0][0] = this->rotationMatrix[0][0]/total;
    this->rotationMatrix[0][1] = this->rotationMatrix[0][1]/total;
    this->rotationMatrix[0][2] = this->rotationMatrix[0][2]/total;

    // Calculation quaternions from the rotation matrix. First, it is checked if
    // the trace of the matrix is positive, since not doing this will result
    // in errors in the calculation (dividing by 0 or imaginary roots).
    float trace = this->rotationMatrix[0][0] + this->rotationMatrix[1][1] + this->rotationMatrix[2][2];
    float s;
    if (trace>0){
        s = 0.5/ sqrt(trace+1.0);
        this->qw = 0.25/s;
        this->qx = (this->rotationMatrix[2][1] - this->rotationMatrix[1][2])*s;
        this->qy = (this->rotationMatrix[0][2] - this->rotationMatrix[2][0])*s;
        this->qz = (this->rotationMatrix[1][0] - this->rotationMatrix[0][1])*s;
    } else if ((this->rotationMatrix[0][0]>this->rotationMatrix[1][1])&&(this->rotationMatrix[0][0]>this->rotationMatrix[2][2])){
        s = 2.0 * sqrt(1+this->rotationMatrix[0][0] - this->rotationMatrix[1][1] - this->rotationMatrix[2][2]);
        this->qw = (this->rotationMatrix[2][1] - this->rotationMatrix[1][2])/s;
        this->qx = 0.25*s;
        this->qy = (this->rotationMatrix[0][1] + this->rotationMatrix[1][0])/s;
        this->qz = (this->rotationMatrix[0][2] + this->rotationMatrix[2][0])/s;
    } else if (this->rotationMatrix[1][1]>this->rotationMatrix[2][2]){
        s = 2.0 * sqrt(1+this->rotationMatrix[1][1] - this->rotationMatrix[0][0] - this->rotationMatrix[2][2]);
        this->qw = (this->rotationMatrix[0][2] - this->rotationMatrix[2][0])/s;
        this->qx = (this->rotationMatrix[0][1] + this->rotationMatrix[1][0])/s;
        this->qy = 0.25*s;
        this->qz = (this->rotationMatrix[1][2] + this->rotationMatrix[2][1])/s;
    } else {
        s = 2.0 * sqrt(1+this->rotationMatrix[2][2] - this->rotationMatrix[0][0] - this->rotationMatrix[0][0]);
        this->qw = (this->rotationMatrix[1][0] - this->rotationMatrix[0][1])/s;
        this->qx = (this->rotationMatrix[0][2] + this->rotationMatrix[2][0])/s;
        this->qy = (this->rotationMatrix[1][2] + this->rotationMatrix[2][1])/s;
        this->qz = 0.25*s;
    }
}

/*
Updates the data. The function takes pointers to floats and updates the values
there.
It starts with updating the raw values. Then calculates the quaternions from
that data and finally calculates Euler angles from the quaterions.
Still has to be seen if we need Euler angles or if we can control using
quaternions.

@param          roll, pitch and yaw float pointers
@return         none

*/
void IMU::update(dataStruct * data){
    // Get raw values from devices.
    this->getReadings();

    if (!(*data).acroMode){
        // Calculate quaternions from the raw values.
        this->calculateQuaternions();

        // Calculate Euler angles from quaternions.
        (*data).imu.roll = atan2(2*(this->qw*this->qx+this->qy*this->qz),1-2*(this->qx*this->qx+this->qy*this->qy))/3.14159265359*180;
        (*data).imu.pitch = asin(2*(this->qw*this->qy-this->qz*this->qx))/3.14159265359*180;
        (*data).imu.yaw = atan2(2*(this->qw*this->qz+this->qx*this->qy),1-2*(this->qy*this->qy+this->qz*this->qz))/3.14159265359*180;
    }
    else {
        (*data).imu.rollVelocity = velocities[0];
        (*data).imu.pitchVelocity = velocities[1];
        (*data).imu.yawVelocity = velocities[2];
    }
}

void IMU::estimator(float * roll, float * pitch){
    this->getReadings();
//    Serial pc(USBTX, USBRX); // tx, rx
    this->k = 1;
    float angle = -atan2(this->accelerations[1],this->accelerations[2])*180/3.1417;
    float error = this->estimated_roll - angle;
    this->estimated_roll += (-this->velocities[0] - error*this->k)*0.05;
    *roll = this->estimated_roll;


    angle = -atan2(this->accelerations[0],this->accelerations[2])*180/3.1417;
    error = this->estimated_pitch - angle;
    this->estimated_pitch += (-this->velocities[1] - error*this->k*4)*0.05;
    *pitch = this->estimated_pitch;

//    pc.printf("%.4f\t%.4f\r\n",this->estimated_roll,this->estimated_pitch);
}
