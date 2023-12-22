#include "device.h"

JGA25 motorL = {
    .motor = {0, 0, 0, 0},

    .PWM_pin = PWM2_MODULE0_CHA_C6,
    .IN1_pin = C7,
    .IN2_pin = C8,
    .encoder = {QTIMER1_ENCODER1, QTIMER1_ENCODER1_CH1_C0, QTIMER1_ENCODER1_CH2_C1, 360},

    .reduction_rate = 34,
    .radius = 37.2 / 1000,
    .deadzone = 4580
};

JGA25 motorR = {
    .motor = {0, 0, 0, 0},

    .PWM_pin = PWM2_MODULE2_CHA_C10,
    .IN1_pin = C11,
    .IN2_pin = D2,
    .encoder = {QTIMER2_ENCODER2, QTIMER2_ENCODER2_CH1_C5, QTIMER2_ENCODER2_CH2_C25, 360},

    .reduction_rate = 34,
    .radius = 37.2 / 1000,
    .deadzone = 4320
};

IMU imu = {
    .state = IMU_NOT_INIT
};

Balance_chassis balance = {
    .chassis = {0, 0, 0, 0},
    
    .motorL = (Motor_t*)&motorL,
    .motorR = (Motor_t*)&motorR,
    .imu = &imu,

    .balance_ang = 1.8,
    .pitch_kd = 0,
    .pitch_ang_pid = {
        .target = 0,

        .kp = 600,
        .ki = 0,
        .kd = 100000,

        .component_p_max = 10000,
        .component_i_max = 3000,
        .component_d_max = 10000,
        .output_max = 10000
    },
    .forward_v_pid = {
        .target = 0,

        .kp = -600,
        .ki = -0.12,
        .kd = 100000,

        .component_p_max = 10000,
        .component_i_max = 3000,
        .component_d_max = 10000,
        .output_max = 5000
    },
    .rotate_v_pid = {
        .target = 0,

        .kp = 500,
        .ki = 0,
        .kd = 0,

        .component_p_max = 10000,
        .component_i_max = 600,
        .component_d_max = 10000,
        .output_max = 5000
    }
};

Remote remote = {0};
