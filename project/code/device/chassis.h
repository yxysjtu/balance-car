#ifndef CHASSIS_H
#define CHASSIS_H


typedef enum{
    CHASSIS_STOP = 0,
    CHASSIS_RUN,
    CHASSIS_RELEASE
} chassis_state;

typedef struct{
    chassis_state state;

    float pitch_ang;
    float pitch_w;
    float forward_v;
    float rotate_v;

    float pitch_ang_set;
    float forward_v_set;
    float rotate_v_set;

} Chassis_t;


#endif
