#ifndef ESCCONTROLLER_H
#define ESCCONTROLLER_H

#include "mbed.h"
#include "FastPWM.h"

class ESCController{
    private:
    FastPWM pwm;

    public:
    ESCController(PinName pin): pwm(pin) {};
    void initialize();
    void update(float);
    void fire(void);

};

#endif