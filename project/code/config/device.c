#include "device.h"

JGA25 motorL = {
    .attr = {0, 0, 0, 0},
    .PWM_pin = PWM2_MODULE0_CHA_C6,
    .IN1_pin = C7,
    .IN2_pin = C8,
    .encoder = {QTIMER1_ENCODER1, QTIMER1_ENCODER1_CH1_C0, QTIMER1_ENCODER1_CH2_C1, 360},
    .reduction_rate = 34,
    .radius = 37.2 / 1000
};

JGA25 motorR = {
    .attr = {0, 0, 0, 0},
    .PWM_pin = PWM2_MODULE2_CHA_C10,
    .IN1_pin = C11,
    .IN2_pin = D2,
    .encoder = {QTIMER1_ENCODER2, QTIMER1_ENCODER2_CH1_C2, QTIMER1_ENCODER2_CH2_C24, 360},
    .reduction_rate = 34,
    .radius = 37.2 / 1000
};

IMU imu = {
    .state = IMU_NOT_INIT
};
