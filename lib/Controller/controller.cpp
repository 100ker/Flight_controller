#include "controller.hpp"
#include <iostream>

void Controller::initialize(void)
{
    escController[0].initialize();
    escController[1].initialize();
    escController[2].initialize();
    escController[3].initialize();

    setpoint[0] = 0;
    setpoint[1] = 0;
    setpoint[2] = 0;
    setpoint[3] = 0;

    pidRoll.initialize(config.controllerConfig.angleController.KpRoll, config.controllerConfig.angleController.KiRoll, config.controllerConfig.angleController.KdRoll, 1.0/config.flightTickerFrequency, 50.0, -50.0);
    pidRollVelocity.initialize(config.controllerConfig.rateController.KpRoll, config.controllerConfig.rateController.KiRoll, config.controllerConfig.rateController.KdRoll, 1.0/config.flightTickerFrequency, 50.0, -50.0);
    pidPitch.initialize(config.controllerConfig.angleController.KpPitch, config.controllerConfig.angleController.KiPitch, config.controllerConfig.angleController.KdPitch, 1.0/config.flightTickerFrequency, 50.0, -50.0);
    pidPitchVelocity.initialize(config.controllerConfig.rateController.KpPitch, config.controllerConfig.rateController.KiPitch, config.controllerConfig.rateController.KdPitch, 1.0/config.flightTickerFrequency, 50.0, -50.0);
    pidYawVelocity.initialize(config.controllerConfig.rateController.KpYaw, config.controllerConfig.rateController.KiYaw, config.controllerConfig.rateController.KdYaw, 0.001, 50.0, -50.0);
}

void Controller::update(void)
{
    if (1)//data.armMotor)
    {
        if (data.newPacket)
        {
            throttleRemote = data.remote.throttle;
            rollRemote = data.remote.roll;
            pitchRemote = data.remote.pitch;
            yawRemote = data.remote.yaw;
            data.newPacket = false;
        }

        throttle = throttle * (1 - 0.1) + throttleRemote * 0.1;
        roll = roll * (1 - 0.1) + rollRemote * 0.1;
        pitch = pitch * (1 - 0.1) + pitchRemote * 0.1;
        yaw = yaw * (1 - 0.1) + yawRemote * 0.1;
        if (data.acroMode)
        {
            velocitySetpoint[0] = roll*config.controllerConfig.ratePrescalerRoll;
            velocitySetpoint[1] = pitch*config.controllerConfig.ratePrescalerPitch;
        }
        else
        {
            positionController();
        }
        velocitySetpoint[2] = yaw*config.controllerConfig.ratePrescalerYaw;
        velocityController();
        updateMotorControllers();
    }
    else {
        for (int i = 0; i<=3;i++){
             escController[i].update(0.0);
        }
    }
}

void Controller::positionController(void)
{
    velocitySetpoint[0] = pidRoll.calculate(roll, data.imu.roll * config.controllerConfig.angleImuPrescalerRoll);
    velocitySetpoint[1] = pidPitch.calculate(pitch, data.imu.pitch * config.controllerConfig.angleImuPrescalerPitch);
}

void Controller::velocityController(void)
{
    for (int i = 0; i <= 3; i++)
    {
        setpoint[i] = config.controllerConfig.signs[i][0] * pidRollVelocity.calculate(velocitySetpoint[0], data.imu.rollVelocity * config.controllerConfig.rateImuPrescalerRoll) +
                      config.controllerConfig.signs[i][1] * pidPitchVelocity.calculate(velocitySetpoint[1], data.imu.pitchVelocity * config.controllerConfig.rateImuPrescalerPitch) +
                      config.controllerConfig.signs[i][2] * pidYawVelocity.calculate(velocitySetpoint[2], data.imu.yawVelocity * config.controllerConfig.rateImuPrescalerYaw);
    }
}

void Controller::updateMotorControllers(void)
{
    for (int i = 0; i <= 3; i++)
    {
        escController[i].update(throttle * 125.0 + setpoint[i]);
    }
}
