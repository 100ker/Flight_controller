#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <mbed.h>
#include "ESCController.h"
#include "config.h"

class Controller{
    private:
        ESCController escController[4];
        dataStruct * dataPtr;
        controllerConfigStruct * controllerConfigPtr;
        uint8_t setpoint[4];

    public:
        Controller(PinName pin1, PinName pin2, PinName pin3, PinName pin4): 
        escController{ESCController(pin1), ESCController(pin2), ESCController(pin3), ESCController(pin4)} {}; // delegation of constructors
        void initialize(dataStruct * data, controllerConfigStruct * controllerConfig);
        void update(void);
};

#endif