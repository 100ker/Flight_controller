#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <mbed.h>
#include "ESCController.hpp"
#include "config.hpp"

class Controller
{
  private:
    ESCController escController[4];
    dataStruct *dataPtr;
    controllerConfigStruct *controllerConfigPtr;
    uint8_t setpoint[4];
    float rollError, pitchError, yawError;
    float rollControlValue, pitchControlValue, yawControlValue;
    enum controllerModes {ACRO, STABILIZE};
    controllerModes controllerMode;
    float Kp[3], Ki[3], Kd[3];
    void updateParameters(void);

  public:
    Controller(PinName pin1, PinName pin2, PinName pin3, PinName pin4) : escController{ESCController(pin1), ESCController(pin2), ESCController(pin3), ESCController(pin4)} {}; // delegation of constructors
    void initialize(dataStruct *data, controllerConfigStruct *controllerConfig);
    void update(void);
};

#endif