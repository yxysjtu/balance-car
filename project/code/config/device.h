#ifndef DEVICE_H
#define DEVICE_H

#include "JGA25.h"
#include "qencoder.h"
#include "imu.h"
#include "remote.h"
#include "balance.h"

#include "driver.h"

extern JGA25 motorL, motorR;
extern IMU imu;
extern Remote remote;
extern Balance_chassis balance;

#endif
