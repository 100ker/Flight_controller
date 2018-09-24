#ifndef _HELPERS_HPP_
#define _HELPERS_HPP_

#include <mbed.h>
#include "iniparser.h"
#include <config.hpp>
#include <iostream>

extern configStruct config;
extern LocalFileSystem local;

void loadConfig(void){
    dictionary *dir = iniparser_load("/local/config.ini");

    // Read config for radio
    config.radioConfig.channel = iniparser_getint(dir, "radio:channel",101);
    config.radioConfig.txAddress = iniparser_getlongint(dir, "radio:txaddress",0x007DEADBEE);
    config.radioConfig.rxAddress = iniparser_getlongint(dir, "radio:rxaddress",0x007DEADBEE);
    config.radioConfig.transferSize = iniparser_getint(dir, "radio:transfersize",17);

    // Read remote prescalers
    config.controllerConfig.anglePrescalerRoll = (float)iniparser_getdouble(dir, "anglecontroller:prescaler_roll",0);
    config.controllerConfig.anglePrescalerPitch = (float)iniparser_getdouble(dir, "anglecontroller:prescaler_pitch",0);
    config.controllerConfig.angleImuPrescalerRoll = (float)iniparser_getdouble(dir, "controller:prescaler_rollangle_imu",0);
    config.controllerConfig.angleImuPrescalerPitch = (float)iniparser_getdouble(dir, "controller:prescaler_pitchangle_imu",0);

    config.controllerConfig.ratePrescalerRoll = (float)iniparser_getdouble(dir, "ratecontroller:prescaler_roll",0);
    config.controllerConfig.ratePrescalerPitch = (float)iniparser_getdouble(dir, "ratecontroller:prescaler_pitch",0);
    config.controllerConfig.ratePrescalerYaw = (float)iniparser_getdouble(dir, "ratecontroller:prescaler_yaw",0);
    config.controllerConfig.rateImuPrescalerRoll = (float)iniparser_getdouble(dir, "ratecontroller:prescaler_rollrate_imu",0);
    config.controllerConfig.rateImuPrescalerPitch = (float)iniparser_getdouble(dir, "ratecontroller:prescaler_pitchrate_imu",0);
    config.controllerConfig.rateImuPrescalerYaw = (float)iniparser_getdouble(dir, "ratecontroller:prescaler_yawrate_imu",0);

    // Read config for controllers
    config.controllerConfig.rateController.KpRoll = (float)iniparser_getdouble(dir, "acromode:kp_roll",0);
    config.controllerConfig.rateController.KiRoll = (float)iniparser_getdouble(dir, "acromode:ki_roll",0);
    config.controllerConfig.rateController.KdRoll = (float)iniparser_getdouble(dir, "acromode:kd_roll",0);
    config.controllerConfig.rateController.KpPitch = (float)iniparser_getdouble(dir, "acromode:kp_pitch",0);
    config.controllerConfig.rateController.KiPitch = (float)iniparser_getdouble(dir, "acromode:ki_pitch",0);
    config.controllerConfig.rateController.KdPitch = (float)iniparser_getdouble(dir, "acromode:kd_pitch",0);
    config.controllerConfig.rateController.KpYaw = (float)iniparser_getdouble(dir, "acromode:kp_yaw",0);
    config.controllerConfig.rateController.KiYaw = (float)iniparser_getdouble(dir, "acromode:ki_yaw",0);
    config.controllerConfig.rateController.KdYaw = (float)iniparser_getdouble(dir, "acromode:kd_yaw",0);
    config.controllerConfig.angleController.KpRoll = (float)iniparser_getdouble(dir, "stabilizemode:kp_roll",0);
    config.controllerConfig.angleController.KiRoll = (float)iniparser_getdouble(dir, "stabilizemode:ki_roll",0);
    config.controllerConfig.angleController.KdRoll = (float)iniparser_getdouble(dir, "stabilizemode:kd_roll",0);
    config.controllerConfig.angleController.KpPitch = (float)iniparser_getdouble(dir, "stabilizemode:kp_pitch",0);
    config.controllerConfig.angleController.KiPitch = (float)iniparser_getdouble(dir, "stabilizemode:ki_pitch",0);
    config.controllerConfig.angleController.KdPitch = (float)iniparser_getdouble(dir, "stabilizemode:kd_pitch",0);

    // Read config for motor direction compensation
    char * keysSigns[12];
    const char ** keysSignsPtr = (const char **) &keysSigns;
    keysSignsPtr = iniparser_getseckeys(dir, "motordirections", keysSignsPtr);
    for (int i = 0; i<=3; i++){
        for (int j = 0; j<=2; j++){
            config.controllerConfig.signs[i][j] = iniparser_getint(dir, (keysSigns[3*i+j]),0);
        }
    }

    // Reading filter values
    config.imuconfig.itg3200.a = (float)iniparser_getdouble(dir, "itg3200:a",0);
    config.imuconfig.itg3200.b = (float)iniparser_getdouble(dir, "itg3200:b",0);
    config.imuconfig.itg3200.c = (float)iniparser_getdouble(dir, "itg3200:c",0);

    config.imuconfig.hmc5883l.a = (float)iniparser_getdouble(dir, "hmc5883l:a",0);
    config.imuconfig.hmc5883l.b = (float)iniparser_getdouble(dir, "hmc5883l:b",0);
    config.imuconfig.hmc5883l.c = (float)iniparser_getdouble(dir, "hmc5883l:c",0);

    config.imuconfig.adxl345.a = (float)iniparser_getdouble(dir, "adxl345:a",0);
    config.imuconfig.adxl345.b = (float)iniparser_getdouble(dir, "adxl345:b",0);
    config.imuconfig.adxl345.c = (float)iniparser_getdouble(dir, "adxl345:c",0);

    config.flightTickerFrequency = (uint16_t) iniparser_getint(dir, "misc:flighttickerfrequency",1000);
    config.gyroTickerFrequency = (uint16_t) iniparser_getint(dir, "misc:gyrofrequency",1000);
    config.angleTickerFrequency = (uint16_t) iniparser_getint(dir, "misc:anglefrequency",250);
    iniparser_freedict(dir);
}

#endif