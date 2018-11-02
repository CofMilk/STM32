#include "usart.h"
#include "stm32f10x.h"
#include "stdio.h"

#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

void usart_init()
{
	//RCC->APB2ENR&=0x00000000;                                    /*��Ҫ��λ��ؼĴ��� ���������Ӱ������*/
	RCC->APB2ENR|=1<<2|1<<14;  //GPIOB USART1                      /*�������*/    
	
	GPIOA->CRH&=0xFFFFF00F;  //GPIOB USART1                          
	GPIOA->CRH|=0x000004B0;
	
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ
	
	USART1->BRR=0x0271; //115200
	//USART1->CR1&=0x0000; //RESET
	//USART1->CR1|=0x212B; 
	USART1->CR1|=0X202C; // UE=1 M_len=8 У���� �ж�ʹ�� RXNEIE=1 TE/RE=1 
	//USART1->CR1|=1<<5;
	/******����ط�*****/
	/****CR1�Ĵ���TX��RX���ô�����û�п���RX����***/
	
	
	USART1->CR2&=0x0000; //RESET
	USART1->CR3&=0x0000; //RESET
	
	//NVIC->ISER[1]&=0xFFFFFFDFF;
	NVIC->ISER[1]|=1<<(USART1_IRQn%32);   //SET ENABLE USAT1
	
	NVIC->IP[37]=0x50; //  1  :  1	
	
}

