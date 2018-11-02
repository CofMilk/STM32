#ifndef __MODBUS_H__
#define __MODBUS_H__
#include "stm32f10x.h"
#define T_R_Mode  PDout(7)


typedef struct mod{
	u8 address; //�豸��ַ
	u8 sendbuf[64];//���ͻ�����
	u8 recbuf[64]; //���ܻ�����
	u8 timflag;  //���ݽ���ʱ��
	u8 timrun;   //��ʱ��������־
	u8 reflag;   //���յ�һ֡���ݵı�־
	u8 recount;  //���ܵ����ݸ���
	u8 secount;  //���͵����ݸ���
	
}Modbus;

void modbus_init(void);
void modbus_fun3(u8 add,u16 readd,u16 renum);
void modbus_display(void);
void modbus_event(void);


#endif


