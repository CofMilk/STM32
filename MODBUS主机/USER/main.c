#include"stdio.h"
#include"usart.h"
#include"TIM.h"
#include"modbusCRC.h"
#include"modbus.h"
void UART1_PUTC(unsigned char c)
{  
    while(!(UART1_SR&(1<<6))&&(!(UART1_SR&(1<<7))));
	UART1_DR=c;
}	
int fputc(int ch,FILE *f)
{
    UART1_PUTC(ch);
	return (ch);
}

unsigned volatile char flag=0;

void TIM3_IRQHandler(void)
{
 if((TIM3_SR&1)==1)
	{
		TIM3_SR=0;
		if(modbus.timrun!=0)  //��ʱ����ʼ��ʱ
		{
			modbus.timout++;  //ÿһ�����һ�Σ��ﵽ4�������Ϊһ֡�������յ���
			if(modbus.timout>=8)//���ʱ��ﵽ��ʱ��
			{
				modbus.timrun=0;//�رն�ʱ��--ֹͣ��ʱ
				modbus.reflag=1;//�յ�һ֡����
			}
				
		}
	}	

}
/*ÿ�յ�һ���ֽڴ���һ���ж�*/
void USART2_IRQHandler(void)
{
	if(UART2_SR&1<<5)
	{
		flag=UART2_DR;
		if(modbus.reflag==1)//�����ݰ����ڴ���
		{
			return;
		}
		modbus.rcbuf[modbus.recount++]=flag;
		modbus.timout=0;//ÿ�յ�һ���ֽڣ����ݶ���ʱ���Ҫ��0
		if(modbus.recount==1)//�յ�����������һ֡���ݵĵ�һ�ֽ�
		{
			modbus.timrun=1;//������ʱ
		}
	}
}


int main(void)
{
    
    Modbus_Init();
    UART1_Init();
	TIM3_Init();
	while(1)
	{	
		Modbus_fun3(2,0X2A,1);
		Modbus_Event();//����Modbus����
	}	
}
