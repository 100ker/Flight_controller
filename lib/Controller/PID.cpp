#include "PID.hpp"

void PID::initialize(float Kp, float Ki, float Kd, float dt, float max, float min){
    _Kp = Kp;
    _Ki = Ki;
    _Kd = Kd;
    _dt = dt;
    _max = max;
    _min = min;
    pos = 0;
    reset();
}

float PID::calculate(float setpoint, float currentValue){
    _error = setpoint - currentValue;

    _errorIntegral = _error * _dt + _errorIntegrated;

    _errorDerivative = (_error - _errorDerivativeArray[pos])/_dt;

    _errorDerivativeArray[pos] = _error;

    pos = (pos + 1) & 0x1F;

    _output = _error * _Kp + _errorIntegral * _Ki + _errorDerivative * _Kd;

    if (_output > _max){
        _output = _max;
    }
    else if (_output < _min){
        _output = _min;
    }
    else {
        _errorIntegrated = _errorIntegral;
    }

    return _output;
}

void PID::reset(void){
    _errorIntegrated = 0;
}