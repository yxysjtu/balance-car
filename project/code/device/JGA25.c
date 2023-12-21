#include "JGA25.h"

/*************************** driver layer ************************************/
#include "driver.h"
#define JGA25_PWM_FREQ	(50000)

void JGA25_init(JGA25* this);
void JGA25_measure_parameter(JGA25* this); //connect to measurement device such as encoder
void JGA25_output_intensity(JGA25* this); //connect to motor driver such as PWM

void JGA25_init(JGA25* this){
    pwm_init(this->PWM_pin, JGA25_PWM_FREQ, 0);    
    gpio_init(this->IN1_pin, GPO, 1, GPO_PUSH_PULL);
	gpio_init(this->IN2_pin, GPO, 1, GPO_PUSH_PULL);
	QEncoder_init(&(this->encoder));
}

void JGA25_measure_parameter(JGA25* this){
	float inc = QEncoder_get_ang_inc(&this->encoder) / this->reduction_rate;
    this->motor.v = this->motor.v * 0.98 + inc / 180 * M_PI * MAIN_LOOP_FREQ * 0.02;
    this->motor.ang += inc;
	if(this->motor.ang >= 360){
		this->motor.ang -= (int)(this->motor.ang / 360) * 360; 
	}else if(this->motor.ang < 0){
		this->motor.ang -= (int)(this->motor.ang / 360) * 360 - 360; 
	}
}


void JGA25_output_intensity(JGA25* this){
	switch(this->motor.state){
        case MOTOR_STOP: {
            this->motor.intensity = 0;
            gpio_set_level(this->IN1_pin, 0);
            gpio_set_level(this->IN2_pin, 0);
            pwm_set_duty(this->PWM_pin, abs(this->motor.intensity)); 
        }break;
        case MOTOR_RUN: {
            if(this->motor.intensity >= 0){
                gpio_set_level(this->IN1_pin, 1);
                gpio_set_level(this->IN2_pin, 0);
            }else{
                gpio_set_level(this->IN2_pin, 1);
                gpio_set_level(this->IN1_pin, 0);
            }
            pwm_set_duty(this->PWM_pin, (int)MAP(abs(this->motor.intensity), 0, 10000, this->deadzone, 10000)); 
        }break;
        case MOTOR_RELEASE: {
            this->motor.intensity = 0;
            gpio_set_level(this->IN1_pin, 1);
            gpio_set_level(this->IN2_pin, 1);
            pwm_set_duty(this->PWM_pin, abs(this->motor.intensity)); 
        }break;
    }
}

/*****************************************************************************/


void JGA25_Handle(JGA25* this){
	JGA25_measure_parameter(this);
	JGA25_output_intensity(this);
}