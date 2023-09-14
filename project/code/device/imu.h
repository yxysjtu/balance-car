#ifndef IMU_H
#define IMU_H

typedef enum{
    IMU_NOT_INIT,
    IMU_RUN,
    IMU_ERR
} IMU_state;

typedef struct{
    IMU_state state;
    float ax;
    float ay;
    float az;
    float wx;
    float wy;
    float wz;
} IMU;

void IMU_init(IMU *this);
void IMU_Handle(IMU *this);


#endif
