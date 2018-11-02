#include "usart.h"


/********usart1��ʼ������***********/
void UART1_Init(void)
{
    //ʹ��USART1��ʱ�� ʹ��GPIOAʱ��
    RCC_APB2ENR|=1<<14|1<<2;
	//PA9����Ϊ�������������PA10����Ϊ��������
	GPIOA_CRH=9<<4|4<<8;
	//���ڲ�����ʼ���������ֳ���ֹͣλ
	UART1_CR1|=0<<12|0<<10;
	//����������Ϊ9600
	UART1_BRR|=468<<4|12;
	//�����ж�
//	NVIC_ISER1|=1<<5;
	//����USART1�շ�ʹ��,ʹ�ܽ����ж�
	UART1_CR1|=1<<2|1<<3;
	//����USART1ʹ�ܿ���
	UART1_CR1|=1<<13;
 } 

/********RS485��ʼ������***********/
void RS485_Init(void)
{
    //ʹ��USART2��ʱ�� 
    RCC_APB1ENR|=1<<17;
	//ʹ��GPIOA��GPIODʱ��
	RCC_APB2ENR|=1<<2|1<<5;
	//PA2����Ϊ�������������PA3����Ϊ��������
	GPIOA_CRL=9<<8|4<<12;
	//����PD7Ϊ�������
	GPIOD_CRL=1<<28;
	//���ڲ�����ʼ���������ֳ���ֹͣλ
	UART2_CR1|=0<<12|0<<10;
	//����������Ϊ9600
	UART2_BRR|=234<<4|6;
	//�����ж�
	NVIC_ISER1|=1<<6;
	//����USART1�շ�ʹ��,ʹ�ܽ����ж�
	UART2_CR1|=1<<2|1<<3|1<<5;
	UART2_SR=0;
	//����USART1ʹ�ܿ���
	UART2_CR1|=1<<13;
     RS485_0 ;   

}
/********RS485�������ݺ���***********/
void RS485_Send_Data(char data)
{
	UART2_DR=data;
	while(!(UART2_SR&(1<<6)));//�ȴ��������
}

void LED_Init(void)
{
	//��ʼ��GPIOB��GPIOE	
	RCC_APB2ENR|=(1<<3)|(1<<6);   
	//����PB5,PE5Ϊͨ���������������ٶ�Ϊ2MHz
	GPIOB_CRL=2<<20;
	GPIOE_CRL=2<<20;
	//����PB8Ϊ�������������ٶ�Ϊ2MHZ
	GPIOB_CRH=2<<0;
}

void Delay(long counter)
{
while(counter--);
} 	
