#include "mbed.h"

class ESCController{
    private:
    PwmOut pwm;

    public:
    ESCController(PinName pin): pwm(pin) {};
    void initialize();
    void update(uint8_t);
    
};