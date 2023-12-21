#ifndef PID_H
#define PID_H

#include "config.h"

typedef struct{
    float *target;
    float *feedback;
    float err;

    float component_p;
    float component_i;
    float component_d;
    float output;

    float kp;
    float ki;
    float kd;

    float component_p_max;
    float component_i_max;
    float component_d_max;
    float output_max;

} PID;

float PID_calc(PID *this);

#endif
