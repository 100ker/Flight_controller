#include "controller.hpp"
#include <iostream>

void Controller::initialize(dataStruct *data, controllerConfigStruct *controllerConfig)
{
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
    controllerMode = STABILIZE;
    updateParameters();
}

void Controller::update(void)
{
    if (!(*dataPtr).armMotor || ((*dataPtr).remote.throttle<=10 ))
    {
        escController[0].update(0);
        escController[1].update(0);
        escController[2].update(0);
        escController[3].update(0);
        return;
    }

    if ((*dataPtr).acroMode && (controllerMode == STABILIZE))
    {
        controllerMode = ACRO;
        updateParameters();
    }

    if (!(*dataPtr).acroMode && (controllerMode == ACRO))
    {
        controllerMode = STABILIZE;
        updateParameters();
    }

    if (controllerMode == STABILIZE)
    {
        rollError = (*dataPtr).remote.roll/20 - (*dataPtr).imu.roll;
        pitchError = (*dataPtr).remote.pitch/20 - (*dataPtr).imu.pitch;
    }

    if (controllerMode == ACRO)
    {
        rollError = (*dataPtr).remote.roll/20 - (*dataPtr).imu.rollVelocity;
        pitchError = (*dataPtr).remote.pitch/20 - (*dataPtr).imu.pitchVelocity;
    }

    yawError = (*dataPtr).remote.yaw - (*dataPtr).imu.yawVelocity;

    for (int i = 0; i <= 3; i++)
    {
        rollControlValue = (*controllerConfigPtr).signs[i][0] * Kp[0] * rollError;
        pitchControlValue = (*controllerConfigPtr).signs[i][1] * Kp[1] * pitchError;
        yawControlValue = (*controllerConfigPtr).signs[i][2] * Kp[2] * yawError;
        setpoint[i] = (*dataPtr).remote.throttle / 1024.0 * 125.0 + rollControlValue + pitchControlValue + yawControlValue;
        escController[i].update(setpoint[i]);
    }
}

void Controller::updateParameters(void)
{
    switch (controllerMode)
    {
    case ACRO:
        for (int i = 0; i <= 2; i++)
        {
            Kp[i] = (*controllerConfigPtr).acroModeConfig.Kp[i];
        }
        break;

    case STABILIZE:
        for (int i = 0; i <= 3; i++)
        {
            Kp[i] = (*controllerConfigPtr).stabilizingModeConfig.Kp[i];
        }
        break;
    }
}
