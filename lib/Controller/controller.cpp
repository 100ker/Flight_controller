#include "controller.h"

void Controller::initialize(dataStruct * data, controllerConfigStruct * controllerConfig){
    escController[0].initialize();
    escController[1].initialize();
    escController[2].initialize();
    escController[3].initialize();
    dataPtr = data;
    controllerConfigPtr = controllerConfig;

    setpoint[0] = 0;
    setpoint[1] = 0;
    setpoint[2] = 0;
    setpoint[3] = 0;
}

void Controller::update(void){
    if ((*dataPtr).acroMode){
        for (int i=0;i<=3;i++){
            float rollVelocityError = (*dataPtr).remote.roll - (*dataPtr).imu.rollVelocity;
            float rollVelocityControlValue = (*controllerConfigPtr).acroModeConfig.Kp[i][0] * rollVelocityError;
            float pitchVelocityError = (*dataPtr).remote.pitch - (*dataPtr).imu.pitchVelocity;
            float pitchVelocityControlValue = (*controllerConfigPtr).acroModeConfig.Kp[i][1] * pitchVelocityError;
            float yawVelocityError = (*dataPtr).remote.yaw - (*dataPtr).imu.yawVelocity;
            float yawVelocityControlValue = (*controllerConfigPtr).acroModeConfig.Kp[i][2] * yawVelocityError;
            setpoint[i] = (*dataPtr).remote.throttle/1023.0*125.0 + \
                            (*controllerConfigPtr).signs[i][0] * rollVelocityControlValue + \
                            (*controllerConfigPtr).signs[i][1] * pitchVelocityControlValue + \
                            (*controllerConfigPtr).signs[i][2] * yawVelocityControlValue;
        }
    }

    for (int i = 0; i<=3; i++){
        escController[i].update(setpoint[i]);
    }

    // uint8_t value = (uint8_t)((*dataPtr).remote.throttle/1023.0*125.0);// - (*pitch)/90.0*45.0;
    // escController[0].update(value);
//    escController4.update(value);
    // }
}
