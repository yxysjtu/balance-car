#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

typedef enum{
    MOTOR_STOP = 0,
    MOTOR_RUN,
    MOTOR_RELEASE
} motor_state;

typedef struct{
    motor_state state;
    int16_t intensity;
    float ang;  // (degree)
    float v;
} Motor_t;

#endif