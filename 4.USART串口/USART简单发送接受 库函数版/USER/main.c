#include "stm32f10x.h"


void USART_TR(void)
{
	GPIO_InitTypeDef GPIO_InitStruce;
	USART_InitTypeDef USART_InitStruce;
	NVIC_InitTypeDef NVIC_InitStruce;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ�ܴ���1��ʱ��
	
	GPIO_InitStruce.GPIO_Mode=GPIO_Mode_AF_PP; //����PA9  TX  �����������
	GPIO_InitStruce.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStruce.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruce);
	
	GPIO_InitStruce.GPIO_Mode=GPIO_Mode_IN_FLOATING; //����PA10 RX  �������������������
	GPIO_InitStruce.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStruce.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruce);
	
	USART_DeInit(USART1); //���ڸ�λ
	USART_InitStruce.USART_BaudRate=115200;    //���ڳ�ʼ��   ������115200
	USART_InitStruce.USART_HardwareFlowControl=USART_HardwareFlowControl_None; //Ӳ��������
	USART_InitStruce.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;  //usart���ͽ���ʹ��
	USART_InitStruce.USART_Parity=USART_Parity_No;  //��żУ��λ����
	USART_InitStruce.USART_StopBits=USART_StopBits_1;  //ֹͣλ����
	USART_InitStruce.USART_WordLength=USART_WordLength_8b;  //���ݿ��

	USART_Init(USART1,&USART_InitStruce); //USART��ʼ��
	
	USART_Cmd(USART1,ENABLE);   //����ʹ��
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE); //�����ж����� 
	
	
	NVIC_InitStruce.NVIC_IRQChannel=USART1_IRQn;  //ͨ��
	NVIC_InitStruce.NVIC_IRQChannelCmd=ENABLE;  //ͨ��ʹ��
	NVIC_InitStruce.NVIC_IRQChannelPreemptionPriority=1;  //
	NVIC_InitStruce.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStruce);
	
	
}

void USART1_IRQHandler(void)
{
	u8 temp;
	if(USART_GetITStatus(USART1,USART_IT_RXNE))
	{	temp=USART_ReceiveData(USART1);
	
		USART_SendData(USART1,temp);
	}
	
}


int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	USART_TR();
	while(1);
}

