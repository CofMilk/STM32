#include "modbus.h"
#include  "sys.h"
#include "misc.h"
#include "ModbusCRC.h"
#include "stdio.h"

extern vu8 temp;
Modbus modbus;

void modbus_init()
{
	GPIO_InitTypeDef GPIO_InitTypeStruct;
	USART_InitTypeDef USART_InitTypeStruct;
	NVIC_InitTypeDef NVIC_InitTypeStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitTypeStruct);   //    �շ�����
	
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitTypeStruct);   //PA2 USART2 TX
	
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_3;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitTypeStruct);    //PA3 USART2 RX
	
	USART_InitTypeStruct.USART_BaudRate=9600;
	USART_InitTypeStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitTypeStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitTypeStruct.USART_Parity=USART_Parity_No;
	USART_InitTypeStruct.USART_StopBits=USART_StopBits_2;
	USART_InitTypeStruct.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART2,&USART_InitTypeStruct);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	NVIC_InitTypeStruct.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitTypeStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitTypeStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitTypeStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitTypeStruct);
	
	USART_Cmd(USART2,ENABLE);
	T_R_Mode=1;  //Ĭ��Ϊ����ģʽ
	
}

void modbus_fun3(u8 add,u16 readd,u16 renum)
{
	u16 crc,i;
	modbus.secount=0;
	
	modbus.sendbuf[modbus.secount++]=add; //��ȡ�豸��ַ
	modbus.sendbuf[modbus.secount++]=0x03; //������
	modbus.sendbuf[modbus.secount++]=readd/256; //��ȡ�Ĵ����ߵ�ַ
	modbus.sendbuf[modbus.secount++]=readd%256; //��ȡ�Ĵ����͵�ַ
	modbus.sendbuf[modbus.secount++]=renum/256; //��ȡ�Ĵ��������߰�λ
	modbus.sendbuf[modbus.secount++]=renum%256;  //��ȡ�Ĵ��������Ͱ�λ
	crc=crc16(modbus.sendbuf,modbus.secount);   //����CRCУ����
	modbus.sendbuf[modbus.secount++]=crc/256;  //У����߰�λ
	modbus.sendbuf[modbus.secount++]=crc%256;  //У����Ͱ�λ
 	
	T_R_Mode=1; //send
	for(i=0;i<modbus.secount;i++)
	{
		USART_SendData(USART2,modbus.sendbuf[i]);
		while(!USART_GetFlagStatus(USART2,USART_FLAG_TC));
		
	}
	T_R_Mode=0;
}


void modbus_display()
{
	u8 i;
	printf("----------------------------------\r\n");
	printf("-----------���͵�����Ϊ-----------\r\n");
	for(i=0;i<modbus.secount;i++)
	{
		printf("%2X  ",modbus.sendbuf[i]);
	}
	printf("\r\n");
	printf("------------���յ�����Ϊ----------\r\n");
	for(i=0;i<modbus.recount;i++)
	{
		printf("%2X  ",modbus.recbuf[i]);
	}
	printf("\r\n");
	printf("------------------------------------\r\n");
	printf("*************************************\r\n");
}

void modbus_event()
{
	u16 crc,rccrc;
	
	if(modbus.reflag==0)  return ;  //û������
	crc=crc16(modbus.recbuf,modbus.recount);
	rccrc=modbus.recbuf[modbus.recount-2]*256+modbus.recbuf[modbus.recount-1];
	if(crc==rccrc)
	{
		modbus_display();
	}
	modbus.secount=0;
	modbus.recount=0;
	modbus.reflag=0; 
}



void USART2_IRQHandler()
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
	{
		temp=USART_ReceiveData(USART2);
		if(modbus.reflag==1) return ;  //���������ڴ���
		
		modbus.recbuf[modbus.recount++]=temp;
		modbus.timflag=0; //�����ʱλ
		if(modbus.recount==1)   //�������͵�һ�����ݵĵ�һ�ֽ�
				modbus.timrun=1;  //������ʱ
	}
}





