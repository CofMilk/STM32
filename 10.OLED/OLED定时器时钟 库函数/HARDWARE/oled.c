#include "oled.h"
#include "oledfont.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "delay.h"
u8 GRAM_NUM[128][8];

u32 mypow(u8 m,u8 n)  //����m^n
{
	u32 num=1;
	while(n--)
		num *= m;
	return num;
}

#if OLED_MODE

void OLED_WR_Byte(u8 dat,u8 cmd)  //дһ���ֽ����ݻ�����
{
	/***********************/
	/***********************/
	/***********************/
}

#else
// dat Ҫд���ֵ   cmdд�����ݻ�������ѡ��
void OLED_WR_Byte(u8 dat,u8 cmd)  //дһ���ֽ����ݻ�����
{
	u8 i;
	u8 temp=dat;
	CS=0;
	DC=cmd;
	for(i=0;i<8;i++)
	{
		CLK=0;
		if(temp&0x80)
			SDA=1;
		else
			SDA=0;
		CLK=1;
		temp<<=1;
		
	}
	
	CS=1;
	DC=1;
}
#endif


void OLED_Refresh_Gram(void)   //������ʾ�ڴ�
{
	u8 i,j;
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte(0xB0+i,OLED_CMD);
		OLED_WR_Byte(0x00,OLED_CMD);
		OLED_WR_Byte(0x10,OLED_CMD);
		for(j=0;j<128;j++)
		{
			OLED_WR_Byte(GRAM_NUM[j][i],OLED_DATA);                     /***********���ʣ�д��ʱ��˳��***********/
		}
	}
	
}


void OLED_Clear(void) //����
{
	u8 i,j;
	for(i=0;i<8;i++)
		for(j=0;j<128;j++)
			GRAM_NUM[j][i]=0x00;
	OLED_Refresh_Gram();
	
}

//  x:0-127
//  y:0-63
//  t: 0 ���  1 ����
void OLED_DrawPoint(u8 x,u8 y,u8 t)  //����             
{
	u8 y0,y1,tep;
	if((x>127)||(y>63)) return;
	y0=7-y/8;                                         /**********���ʣ�����ʱ����������**************/
	y1=7-(y%8);
	
	tep=1<<y1;
	if(t)
		GRAM_NUM[x][y0]|=tep;
	else
		GRAM_NUM[x][y0]&=~tep;   /*����*/
		
}
		/********************x��0-127,y��0-63*****************************************/
		/********************chr��Ҫ��ʾ���ַ� num��Ҫ��ʾ������  *p��Ҫ��ʾ���ַ���**/
		/********************size����ʾģ�͵Ĵ�С len�����ֵ�λ��*********************/
		/********************mode��0������ʾ 1����ʾ**********************************/

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)  //��ʾһ���ַ�
{
	u8 num_size,temp,y0;
	u8 i,j;
	y0=y;
	num_size=((size/8)+((size%8)?(1):(0)))*(size/2);
	chr=chr-' ';
	for(i=0;i<num_size;i++)
	{
		if(size==12) temp=asc2_1206[chr][i];
		else if(size==16) temp=asc2_1608[chr][i];
		else if(size==24) temp=asc2_2412[chr][i];
		else return ;
		for(j=0;j<8;j++)
		{
			if(temp&0x80)  OLED_DrawPoint(x,y,mode);
				else       OLED_DrawPoint(x,y,!mode);
			
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}		
	}
}


void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)    //��ʾһ������
{
	u8 temp;
	u8 i=0;

	while(i<len)
	{
		temp=(num/(mypow(10,len-i-1)))%10;
		if(temp==0)
		{
			//if(i==len-1)           /***�ж��Ƿ������һλΪ0***/
				OLED_ShowChar(x+(size/2)*i,y,'0',size,1);
			//else
				//OLED_ShowChar(x+(size/2)*i,y,' ',size,1);
		}
		else
			OLED_ShowChar(x+(size/2)*i,y,'0'+temp,size,1);
		i++;
	}
}


void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)    //��ʾһ���ַ���
{
	while(*p<='~'&&*p>=' ')  //�ж��Ƿ�
	{
		if(x>128-(size/2))
		{ x=0; y+=size;}
		if(y>64-(size))
		{ y=0; OLED_Clear(); }
		OLED_ShowChar(x,y,*p,size,1);
		p++;
		x+=size/2;	
	}
}

void OLED_init()
{
	GPIO_InitTypeDef GPIO_InitTypeStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOD,&GPIO_InitTypeStruct);
	GPIO_SetBits(GPIOD,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8);
	
	DC=1;
	CS=1;
	RST=0;
	delay_ms(100);
	RST=1;
	
	OLED_WR_Byte(0xAE,OLED_CMD); //�ر���ʾ
	OLED_WR_Byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(0x80,OLED_CMD);   //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WR_Byte(0xA8,OLED_CMD); //��������·��
	OLED_WR_Byte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //������ʾƫ��
	OLED_WR_Byte(0X00,OLED_CMD); //Ĭ��Ϊ0

	OLED_WR_Byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],64����.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD); //bit2������/�ر�
	OLED_WR_Byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_Byte(0xDA,OLED_CMD); //����COMӲ����������
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]����
		 
	OLED_WR_Byte(0x81,OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD9,OLED_CMD); //����Ԥ�������
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //������ʾ	 
	OLED_Clear();	
	
	
	
}



