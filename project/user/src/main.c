/*********************************************************************************************************************
* RT1064DVL6A Opensourec Library ����RT1064DVL6A ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
* 
* ���ļ��� RT1064DVL6A ��Դ���һ����
* 
* RT1064DVL6A ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
* 
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
* 
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
* 
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
* 
* �ļ�����          main
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          IAR 8.32.4 or MDK 5.33
* ����ƽ̨          RT1064DVL6A
* ��������          https://seekfree.taobao.com/
* 
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-21        SeekFree            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"

// ���µĹ��̻��߹����ƶ���λ�����ִ�����²���
// ��һ�� �ر��������д򿪵��ļ�
// �ڶ��� project->clean  �ȴ��·�����������

// �������ǿ�Դ����ֲ�ÿչ���
#define LED		(B9)
#define PWM_CH1                 (PWM2_MODULE0_CHA_C6)
#define PWM_CH2                 (PWM2_MODULE2_CHA_C10)
#define INT1	(C7)
#define INT2	(C8)
#define INT3	(C11)
#define INT4	(D2)
#define PWM_FREQ	(17000)

int16 motorL_v = 0;
int16 motorR_v = 0;

void motor_init(){
	gpio_init(INT1, GPO, 1, GPO_PUSH_PULL);
	gpio_init(INT2, GPO, 1, GPO_PUSH_PULL);
	gpio_init(INT3, GPO, 1, GPO_PUSH_PULL);
	gpio_init(INT4, GPO, 1, GPO_PUSH_PULL);
	pwm_init(PWM_CH1, PWM_FREQ, 0);                                                // ��ʼ�� PWM ͨ�� Ƶ�� 17KHz ��ʼռ�ձ� 0%
    pwm_init(PWM_CH2, PWM_FREQ, 0); 
}



int main(void)
{
    clock_init(SYSTEM_CLOCK_600M);  // ����ɾ��
    debug_init();                   // ���Զ˿ڳ�ʼ��

    // �˴���д�û����� ���������ʼ�������
	gpio_init(LED, GPO, 0, GPO_PUSH_PULL);
	
	motor_init();
    
    // �˴���д�û����� ���������ʼ�������
    while(1)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���
        gpio_toggle_level(LED);              // ��ת���ŵ�ƽ
        system_delay_ms(500);   
        
		for(motorL_v = 0; motorL_v < PWM_DUTY_MAX; motorL_v++){
			pwm_set_duty(PWM_CH1, motorL_v); 
			pwm_set_duty(PWM_CH2, PWM_DUTY_MAX / 2); 
			system_delay_us(1000);
		}
		for(; motorL_v >0; motorL_v--){
			pwm_set_duty(PWM_CH1, motorL_v); 
			pwm_set_duty(PWM_CH2, PWM_DUTY_MAX / 2); 
			system_delay_us(1000);
		}
        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}



