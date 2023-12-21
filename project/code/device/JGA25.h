#ifndef JGA25_H
#define JGA25_H

#include "motor.h"
#include "qencoder.h"

typedef struct{
    Motor_t motor;

    uint8_t PWM_pin;
    uint8_t IN1_pin;
    uint8_t IN2_pin;

    QEncoder encoder;
    float reduction_rate;
    float radius;
    float deadzone;
    //TODO: PID controller

} JGA25;

void JGA25_init(JGA25* this);
void JGA25_Handle(JGA25* this);

#endif