#include "mbed.h"
#ifndef PID_H
#define PID_H


class PID{
    public:
        void initialize(float Kp, float Ki, float Kd, float dt, float max, float min);
        float calculate(float setpoint, float currentValue);
        void reset(void);

    private:
        float _Kp, _Ki, _Kd, _dt, _max, _min;
        float _error, _errorIntegral, _errorIntegrated, _errorDerivative, _errorDerivativeArray[32];
        float _output;
        uint8_t pos;

};
#endif