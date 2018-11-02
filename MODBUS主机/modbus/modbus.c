#include"modbus.h"
#include"usart.h"
#include"ModbusCRC.h"
#include"stdio.h"
MODBUS modbus;

/*
��Ϊ������Ϊ9600
1λ���ݵ�ʱ��Ϊ 1000000us/9600bit/s=104us
һ���ֽ�Ϊ 104us*10λ
���� Modbusһ������֡��ɸ�ʱ��Ϊ13.5*104=3.6ms
*/


void Modbus_Init()
{
  modbus.timrun=0;//Modbus��ʱ��ֹͣ��ʱ
  RS485_Init();

}

void Modbus_fun3(unsigned char Addr,unsigned short int ReadAdd,unsigned short int ReadNum)//3�Ź����봦��(����Ҫ��ȡ���ӻ��ļĴ���)
{
	//Addr    ����Ҫ���Ĵӻ���ַ
	//ReadAdd ����Ҫ��ȡ�ļĴ����׵�ַ
	//ReadNum Ҫ��ȡ�ļĴ�������
	unsigned short int j;
	unsigned short int crc;
	modbus.TXcount=0;
	modbus.sendbuf[modbus.TXcount++]=Addr;//��������Ҫ��ȡ�Ĵӻ���ַ
	modbus.sendbuf[modbus.TXcount++]=0X03;//���빦����
	modbus.sendbuf[modbus.TXcount++]=ReadAdd/256;//����Ҫ��ȡ�ĵ�һ���Ĵ����ĸ�λ��ַ
	modbus.sendbuf[modbus.TXcount++]=ReadAdd%256;//����Ҫ��ȡ�ĵ�һ���Ĵ����ĵ�λ��ַ
	modbus.sendbuf[modbus.TXcount++]=ReadNum/256;//����Ҫ��ȡ�Ĵ��������ĸ�λ��ַ
	modbus.sendbuf[modbus.TXcount++]=ReadNum%256;//����Ҫ��ȡ�Ĵ��������ĵ�λ��ַ
	crc=crc16(modbus.sendbuf,modbus.TXcount);//����У����
	modbus.sendbuf[modbus.TXcount++]=crc/256;//����У����
	modbus.sendbuf[modbus.TXcount++]=crc%256;
	
	RS485_1;//��485תΪ����ģʽ
	for(j=0;j<modbus.TXcount;j++)
	{
		RS485_Send_Data(modbus.sendbuf[j]);
	}
	RS485_0;//��485תΪ����ģʽ
}

void Modbus_fun6(unsigned char Addr,unsigned short int WriteAdd,unsigned short int val)//6�Ź����봦��
{
	//Addr    ����Ҫд�Ĵӻ���ַ
	//WriteAdd Ҫ��д�ļĴ�����ַ
	//val      �޸ĺ�üĴ�����ֵ
	unsigned short int j;
	unsigned short int crc;
	
	modbus.TXcount=0;
	modbus.sendbuf[modbus.TXcount++]=Addr;//����Ҫ��д�Ĵӻ���ַ
	modbus.sendbuf[modbus.TXcount++]=0x06;//���빦���� 
	modbus.sendbuf[modbus.TXcount++]=WriteAdd/256;//д��ļĴ����ĵ�ַ
	modbus.sendbuf[modbus.TXcount++]=WriteAdd%256;
	modbus.sendbuf[modbus.TXcount++]=val/256;//д���ֵ
	modbus.sendbuf[modbus.TXcount++]=val%256;
	crc=crc16(modbus.sendbuf,modbus.TXcount);//����У����
	modbus.sendbuf[modbus.TXcount++]=crc/256;//����У����
	modbus.sendbuf[modbus.TXcount++]=crc%256;

	RS485_1;//��485תΪ����ģʽ
	for(j=0;j<modbus.TXcount;j++)
	{
		RS485_Send_Data(modbus.sendbuf[j]);
	}
	RS485_0;//��485תΪ����ģʽ
}
void Modbus_Event()
{
   /* unsigned short int	crc;
	unsigned short int  rccrc;
    if(modbus.reflag==0)//û���յ�Modbus�����ݰ�
    {
		return;
	}
	
	crc=crc16(&modbus.rcbuf[0],modbus.recount-2);//�ӽ��ջ������ĵ�0����ַ��ʼ �����յ������ݸ�������������CRCУ��
	rccrc=modbus.rcbuf[modbus.recount-2]*256+modbus.rcbuf[modbus.recount-1];//�յ���У����
	if(crc==rccrc)//���ݰ�����CRCУ�����*/
	{
		ModBus_Display();
	}
	modbus.recount=0;
	modbus.reflag=0;
}
void ModBus_Display(void)
{
	char i =0;
	
	printf("**************************************************\r\n");
	printf("�����ѷ��͵�����Ϊ:0x:");	
	for(i=0;i<modbus.TXcount;i++)
	{
		printf(" %02X",modbus.sendbuf[i]);
	}
	printf("\r\n");
	modbus.TXcount=0;
	printf("\r\n");
	printf("�������յ��Ļ�ӦΪ:0x:");	
	for(i=0;i<modbus.recount;i++)
	{
		printf(" %02X",modbus.rcbuf[i]);
	}
	printf("\r\n");
	printf("**************************************************\r\n");
	printf("\r\n");	
}


