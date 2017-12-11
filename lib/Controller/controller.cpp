#include "controller.h"

void Controller::initialize(void){
    this->escController1.initialize();
    this->escController2.initialize();
    this->escController3.initialize();
    this->escController4.initialize();
}

void Controller::update(dataStruct * data){
    // if ((*data).armMotor){
    uint8_t value = (uint8_t)(125.0 + (*data).controller.throttle/1023.0*125.0);// - (*pitch)/90.0*45.0;
    this->escController1.update(value);
//    this->escController4.update(value);
    // }
}
