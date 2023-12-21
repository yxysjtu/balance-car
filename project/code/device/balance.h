#ifndef BALANCE_H
#define BALANCE_H

#include "chassis.h"
#include "motor.h"
#include "imu.h"
#include "pid.h"

typedef struct{
    Chassis_t chassis;

    Motor_t* motorL;
    Motor_t* motorR;
    IMU* imu;
    
    float balance_ang;
    float pitch_kd;
    float pitch_sin;
    PID pitch_ang_pid;
    PID forward_v_pid;
    PID rotate_v_pid;

} Balance_chassis;

void Balance_init(Balance_chassis* this);
void Balance_Handle(Balance_chassis* this);

#endif
