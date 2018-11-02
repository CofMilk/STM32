#include "rs485.h"
#include "stm32f10x.h"
#include "misc.h"
#include "sys.h"
#include "delay.h"
#define k0 PEin(4)
#define led  PBout(5)
u8 key;
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//ʹ��PORTA,PORTEʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//KEY0-KEY2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4

	//��ʼ�� WK_UP-->GPIOA.0	  ��������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0

}
u8 key_scan(u8 mode)
{
	static u8 flag;
	if(mode)
		flag=1;                                                /***************��������**************************************/
	if(flag&&k0==0)
	{
		delay_ms(5);
		flag=0;
		if(k0==0)
			return 1;
	}else if(k0==1)
		flag=1;	
	return 0;
}
u8 a[5];
u8 i;
int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	a[0]=89;
	a[1]=72;
	a[2]=12;
	a[3]=12;
	a[4]=11;
	KEY_Init();
	rs485_init();
	
	delay_init();
	
	while(1)
	{
		key=key_scan(0);
		if(key==1)
		{
			rs485_send_data(a,5);
			for(i=0;i<5;i++)
			{
				a[i]++;
			}
		}
		PBout(5)=~PBout(5);
		delay_ms(200);
		
	}
}

