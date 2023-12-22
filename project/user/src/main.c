#include "include.h"

//TODO WATCHDOG

int main(void){
    clock_init(SYSTEM_CLOCK_600M);  // 不可删除
    debug_init();                   // 调试端口初始化

	gpio_init(LED, GPO, 0, GPO_PUSH_PULL);
	
	JGA25_init(&motorL);
	JGA25_init(&motorR);
	
	IMU_init(&imu);
	
	Balance_init(&balance);
	
	Remote_init(&remote);
	
	pit_us_init(PIT_CH0, 200);    
	pit_ms_init(PIT_CH1, 2);   
	pit_ms_init(PIT_CH2, 1); 	
    interrupt_set_priority(PIT_IRQn, 0);  
	

    /*motorL.motor.state = MOTOR_RUN;
	motorL.motor.intensity = 5000;
	motorR.motor.state = MOTOR_RUN;
	motorR.motor.intensity = 5000;*/
    while(1)
    {
        gpio_toggle_level(LED);              // 翻转引脚电平
        system_delay_ms(500);   
		
    }
}

unsigned long int main_handle_cnt = 0;
void main_Handle(){
	main_handle_cnt++;
	
	if(remote.state == REMOTE_START){
		balance.chassis.state = CHASSIS_RUN;
		float v_set = (float)(remote.RY - 0x7f) / 256 * 10;
		if(abs(v_set) > abs(balance.chassis.forward_v_set)){
			balance.chassis.forward_v_set = balance.chassis.forward_v_set * 0.9998 + v_set * 0.0002;
		}else{
			balance.chassis.forward_v_set = balance.chassis.forward_v_set * 0.999 + v_set * 0.001;
		}
		
		balance.chassis.rotate_v_set = balance.chassis.rotate_v_set * 0.99 + (float)(remote.RX - 0x7f) / 256 * 16 * 0.01;
	}else{
		balance.chassis.state = CHASSIS_STOP;
	}
	
	JGA25_Handle(&motorL);
	JGA25_Handle(&motorR);
	
	IMU_Handle(&imu);
	
	Balance_Handle(&balance);
	
}

void debug_Handle(){
	uart_write_byte(DEBUG_UART_INDEX, (int8_t)(balance.chassis.pitch_ang * 100));  
	
}

void PIT_IRQHandler(void)
{
    if(pit_flag_get(PIT_CH0))
    {
		main_Handle();
        pit_flag_clear(PIT_CH0);
    }
    
    if(pit_flag_get(PIT_CH1))
    {
        Remote_Handle(&remote);
		pit_flag_clear(PIT_CH1);
    }
    
    if(pit_flag_get(PIT_CH2))
    {
		debug_Handle();
        pit_flag_clear(PIT_CH2);
    }
    
    if(pit_flag_get(PIT_CH3))
    {
        pit_flag_clear(PIT_CH3);
    }

    __DSB();
}


