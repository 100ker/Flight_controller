#ifndef ESCCONTROLLER_H
#define ESCCONTROLLER_H

#include "mbed.h"

class ESCController{
    private:
    PwmOut pwm;

    public:
    ESCController(PinName pin): pwm(pin) {};
    void initialize();
    void update(uint8_t);

};

#endif