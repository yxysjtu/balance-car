#include "imu.h"

#include "driver.h"

void IMU_init(IMU *this){
    if(imu660ra_init()) this->state = IMU_ERR;
    else this->state = IMU_RUN;

}
float k_imu = 0;
void IMU_Handle(IMU *this){
	if(this->state == IMU_RUN){
		imu660ra_get_acc();                                                         // 获取 IMU660RA 的加速度测量数值
		imu660ra_get_gyro(); 

		this->ax = this->ax * k_imu + (1 - k_imu) * imu660ra_acc_transition(imu660ra_acc_x);
		this->ay = this->ay * k_imu + (1 - k_imu) * imu660ra_acc_transition(imu660ra_acc_y);
		this->az = this->az * k_imu + (1 - k_imu) * imu660ra_acc_transition(imu660ra_acc_z);
		this->wx = this->wx * k_imu + (1 - k_imu) * imu660ra_gyro_transition(imu660ra_gyro_x);
		this->wy = this->wy * k_imu + (1 - k_imu) * imu660ra_gyro_transition(imu660ra_gyro_y);
		this->wz = this->wz * k_imu + (1 - k_imu) * imu660ra_gyro_transition(imu660ra_gyro_z);
	}
}