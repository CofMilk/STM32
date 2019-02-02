#include "stm32f10x.h"
#include "adc.h"
#include "pwmdac.h"
#include "usart.h"
#include "delay.h"
#include "key.h"


u8 flag;


int main()
{
	u16 num,pwmval = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	uart_init(115200);
	delay_init();
	Adc_Init();		
	Key_Init();                /*KEY��ʼ�������ڴ��ڳ�ʼ���ĺ��棬�������������ѹ����ȷ��ԭ��δ֪������������*/
	Pwm_DAC_Init(255,0);

	TIM_SetCompare1(TIM1,pwmval);//��ʼֵ	

	while(1)
	{
		flag = Key_Scan(0);
		if(flag == 1)
		{
			pwmval += 10;
			if(pwmval >= 250)
				pwmval = 0;
			TIM_SetCompare1(TIM1,pwmval);
			
		//	delay_ms(500);
			num = Adc_Goon();
			
			printf("Ӧ�õĵ�ѹֵ: %f\r\n",(float)pwmval/256*3.3);
			printf("ʵ�ʼ�⵽�ĵ�ѹֵ: %f\r\n",(float)num/4096*3.3);
			
		}
	}
}









