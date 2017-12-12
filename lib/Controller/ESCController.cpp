#include "ESCController.h"

#define max(a,b) ({ __typeof__ (a) _a = (a);  __typeof__ (b) _b = (b);  _a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__ (a) _a = (a);  __typeof__ (b) _b = (b);  _a < _b ? _a : _b; })

void ESCController::initialize(void){
    pwm.period_us(4000);
    pwm.pulsewidth_us(125); // 125 us idle
}

void ESCController::update(uint8_t value){
    pwm.pulsewidth_us(min(max(value+125,125),250));
}
