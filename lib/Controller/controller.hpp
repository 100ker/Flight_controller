#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <mbed.h>
#include "PID.hpp"
#include "ESCController.hpp"
#include "config.hpp"

class Controller
{
private:
  ESCController escController[4];
  PID pidRoll, pidPitch, pidRollVelocity, pidPitchVelocity, pidYawVelocity;
  float setpoint[4], velocitySetpoint[3];
  float throttle, roll, pitch, yaw;
  float throttleRemote, rollRemote, pitchRemote, yawRemote;
  float rollError, pitchError, yawError;
  float rollVelocityError, pitchVelocityError, yawVelocityError;
  float rollControlValue, pitchControlValue, yawControlValue;
  float Kp[3], Ki[3], Kd[3];

public:
  Controller(PinName pin1, PinName pin2, PinName pin3, PinName pin4) : escController{ESCController(pin1), ESCController(pin2), ESCController(pin3), ESCController(pin4)} {}; // delegation of constructors
  void initialize(void);
  void update(void);
  void positionController(void);
  void velocityController(void);
  void updateMotorControllers(void);
};

#endif