#include "imu.h"

#include "driver.h"

void IMU_init(IMU *this){
    if(imu660ra_init()) this->state = IMU_ERR;
    else this->state = IMU_RUN;

}

void IMU_Handle(IMU *this){
	if(this->state == IMU_RUN){
		imu660ra_get_acc();                                                         // 获取 IMU660RA 的加速度测量数值
		imu660ra_get_gyro(); 
		this->ax = imu660ra_acc_transition(imu660ra_acc_x);
		this->ay = imu660ra_acc_transition(imu660ra_acc_y);
		this->az = imu660ra_acc_transition(imu660ra_acc_z);
		this->wx = imu660ra_gyro_transition(imu660ra_gyro_x);
		this->wy = imu660ra_gyro_transition(imu660ra_gyro_y);
		this->wz = imu660ra_gyro_transition(imu660ra_gyro_z);
	}
}