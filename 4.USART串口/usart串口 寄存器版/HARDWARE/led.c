#include "led.h"
#include "stm32f10x.h"

void led_init()
{
	//RCC->APB2ENR&=0x00000000;   /*��Ҫ��λ�Ĵ��������򽫻�Ӱ����������*/
	RCC->APB2ENR|=1<<3;
	
	GPIOB->CRL&=0XFF0FFFFF;
	GPIOB->CRL|=0X00300000;
	
	GPIOB->BSRR|=1<<5;
	
	
}

