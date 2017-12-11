#include <mbed.h>

#define max(a,b) ({ __typeof__ (a) _a = (a);  __typeof__ (b) _b = (b);  _a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__ (a) _a = (a);  __typeof__ (b) _b = (b);  _a < _b ? _a : _b; })

Serial pc(USBTX, USBRX); // tx, rx

PwmOut servo(p22);
DigitalOut ground(p20);
DigitalOut Vcc(p18);
AnalogIn throttle(p19);

int main() {
    ground = 0;
    Vcc = 1;
    servo.period_us(4000);          // servo requires a 20ms period

    while(1){
        float throttle_us = min(max(125.0f + 125.0f * throttle.read(),125),250);
        pc.printf("%.6f\r", throttle_us);
        servo.pulsewidth_us(throttle_us); // servo position determined by a pulsewidth between 1-2ms

    }
}
