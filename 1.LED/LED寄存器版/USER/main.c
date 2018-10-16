
#include "delay.h"
#include "led.h"
#include "sys.h"
#include "usart.h"	
int main(void)
{
	Stm32_Clock_Init(9);	//ϵͳʱ������
	delay_init(72);	  		//��ʱ��ʼ��
	uart_init(72,115200); 	//���ڳ�ʼ��Ϊ115200
	led_init();
	while(1)
	{
		GPIOB->ODR&=(~(1<<5));
		delay_ms(500);
		GPIOB->ODR|=(1<<5);
		
		GPIOE->ODR&=(~(1<<5));
		delay_ms(500);
		GPIOE->ODR|=(1<<5);
		
	}
}

