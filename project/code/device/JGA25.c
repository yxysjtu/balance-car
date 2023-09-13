#include "JGA25.h"

/*************************** driver layer ************************************/
#include "driver.h"
#define JGA25_PWM_FREQ	(17000)

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
    this->attr.v = inc / 180 * M_PI * this->radius * MAIN_LOOP_FREQ;
    this->attr.ang += inc;
	if(this->attr.ang >= 360){
		this->attr.ang -= (int)(this->attr.ang / 360) * 360; 
	}else if(this->attr.ang < 0){
		this->attr.ang -= (int)(this->attr.ang / 360) * 360 - 360; 
	}
}


void JGA25_output_intensity(JGA25* this){
	switch(this->attr.state){
        case MOTOR_STOP: {
            this->attr.intensity = 0;
            gpio_set_level(this->IN1_pin, 0);
            gpio_set_level(this->IN2_pin, 0);
        }break;
        case MOTOR_RUN: {
            if(this->attr.intensity >= 0){
                gpio_set_level(this->IN1_pin, 1);
                gpio_set_level(this->IN2_pin, 0);
            }else{
                gpio_set_level(this->IN2_pin, 1);
                gpio_set_level(this->IN1_pin, 0);
            }
        }break;
        case MOTOR_RELEASE: {
            this->attr.intensity = 0;
            gpio_set_level(this->IN1_pin, 1);
            gpio_set_level(this->IN2_pin, 1);
        }break;
    }
    pwm_set_duty(this->PWM_pin, abs(this->attr.intensity)); 
}

/*****************************************************************************/


void JGA25_Handle(JGA25* this){
	JGA25_measure_parameter(this);
	JGA25_output_intensity(this);
}