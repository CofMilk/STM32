#include "stm32f10x.h"
#define LEN (u16)(1<<5)
#define RXNE ((USART1->SR)&LEN)

extern u16 IT_flag;
extern u8 num[200];
void USART1_IRQHandler(void)
{
	u8 temp;
	if(RXNE)  // �ж�
	{
		temp=USART1->DR;
		if((IT_flag&0x8000)==0) //����δ���
		{
			if((IT_flag&0x4000))  //���յ�0x0d
			{
				if(temp==0x0a)
					IT_flag|=0x8000;
				else
					IT_flag=0;
			}
			else //û�н��յ�0X0d
			{
				if(temp==0x0d)
					IT_flag|=0x4000;
				else
				{
					num[IT_flag&0x3FFF]=temp;
					IT_flag++;
				}
				if((IT_flag&0X3FFF)>200-1) //CHU CUO
					IT_flag=0;
			}
			
		}
	}
}

