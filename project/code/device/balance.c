#include "balance.h"

#include "driver.h"
#include "math.h"



void Balance_init(Balance_chassis* this){
    this->chassis.pitch_ang_set = sin(this->balance_ang / 180 * M_PI) / M_PI * 180;
    this->pitch_ang_pid.feedback = &(this->pitch_sin);
    this->pitch_ang_pid.target = &(this->chassis.pitch_ang_set);

    this->forward_v_pid.feedback = &(this->chassis.forward_v);
    this->forward_v_pid.target = &(this->chassis.forward_v_set);

    this->rotate_v_pid.feedback = &(this->chassis.rotate_v);
    this->rotate_v_pid.target = &(this->chassis.rotate_v_set);

}

/****************************** BEFIN ********************************
**
**@Name       : Kalman_Filter_x
**@Brief      : 获取x轴角度简易卡尔曼滤波  
**@Param Accel: 加速度算出的角度
**		  Gyro: 陀螺仪的角速度
**@Return     : None
**@Author     : @mayuxin
**@Data	      : 2022-06-04
******************************** END *********************************/ 
#define dt (0.000200)
float Kalman_Filter_x(float angle, float Accel,float Gyro)		
{
 
	static float angle_dot;
	//static float angle;
	float Q_angle=0.001; // 过程噪声的协方差
	float Q_gyro=0.003;	//0.003 过程噪声的协方差 过程噪声的协方差为一个一行两列矩阵
	float R_angle=0.5;		// 测量噪声的协方差 既测量偏差
	char  C_0 = 1;
	static float Q_bias, Angle_err;
	static float PCt_0, PCt_1, E;
	static float K_0, K_1, t_0, t_1;
	static float Pdot[4] ={0,0,0,0};
	static float PP[2][2] = { { 1, 0 },{ 0, 1 } };
	angle+=(Gyro - Q_bias) * dt; //先验估计
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-先验估计误差协方差的微分

	Pdot[1]=-PP[1][1];
	Pdot[2]=-PP[1][1];
	Pdot[3]=Q_gyro;
	PP[0][0] += Pdot[0] * dt;   // Pk-先验估计误差协方差微分的积分
	PP[0][1] += Pdot[1] * dt;   // =先验估计误差协方差
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;
		
	Angle_err = Accel - angle;	//zk-先验估计
	
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //后验估计误差协方差
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
		
	angle	+= K_0 * Angle_err;	 //后验估计
	Q_bias	+= K_1 * Angle_err;	 //后验估计
	angle_dot   = Gyro - Q_bias;	 //输出值(后验估计)的微分=角速度
	return angle;
}

float gyro, accel;
float kv = 0.995;
void Balance_Handle(Balance_chassis* this){
    this->chassis.pitch_w = this->chassis.pitch_w * 0.99 - (this->imu->wy + 0.09) * 0.01;
    float k = 0.8;
    accel = atan2f(this->imu->ax, this->imu->az) / M_PI * 180;
    gyro = this->chassis.pitch_w * dt + this->chassis.pitch_ang;
    //float temp_ang = k * accel + (1 - k) * gyro;
    //this->chassis.pitch_ang += this->chassis.pitch_w * dt;
    this->chassis.pitch_ang = Kalman_Filter_x(this->chassis.pitch_ang, accel, -(this->imu->wy + 0.09));
    //this->chassis.pitch_ang = this->chassis.pitch_ang * 0.99 + (temp_ang) * 0.01;
    this->chassis.forward_v = this->chassis.forward_v * kv + ((this->motorL->v + this->motorR->v) / 2 - this->chassis.pitch_w / 180 * M_PI) * (1 - kv);
    this->chassis.rotate_v = this->chassis.rotate_v * 0.99 + (this->motorL->v - this->motorR->v) * 0.01; //顺时针+
    this->pitch_sin = sin(this->chassis.pitch_ang / 180 * M_PI) / M_PI * 180;

    PID_calc(&(this->pitch_ang_pid));
    this->forward_v_pid.ki = this->forward_v_pid.kp / 200 / 25;
    PID_calc(&(this->forward_v_pid));
    PID_calc(&(this->rotate_v_pid));

    switch (this->chassis.state){
    case CHASSIS_STOP:{
        this->motorL->state = MOTOR_STOP;
        this->motorR->state = MOTOR_STOP;
        break;
    }
    case CHASSIS_RUN:{
        //test balance when not move
        //float output = this->pitch_ang_pid.output * this->pitch_ang_pid.output * this->pitch_ang_pid.output / 100;
        float output = this->pitch_ang_pid.output
                         - this->pitch_kd * this->chassis.pitch_w
                        + this->forward_v_pid.output;
        float output1 = output + this->rotate_v_pid.output;
        float output2 = output - this->rotate_v_pid.output;
        INRANGE(output1, 10000);
        INRANGE(output2, 10000);
        this->motorL->intensity = this->motorL->intensity * 0.9 + (int)(output1 * 0.1);
        this->motorR->intensity = this->motorR->intensity * 0.9 + (int)(output2 * 0.1);

        this->motorL->state = MOTOR_RUN;
        this->motorR->state = MOTOR_RUN;
        break;
    }
    case CHASSIS_RELEASE:{
        this->motorL->state = MOTOR_RELEASE;
        this->motorR->state = MOTOR_RELEASE;
        break;
    }
    default:
        break;
    }
}