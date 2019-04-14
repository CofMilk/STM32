#include "ov7725.h"
#include "ov7725config.h"
#include "delay.h"
#include "stdio.h"
#include "lcd.h"




volatile uint8_t Ov7725_vsync ;	 /* ֡ͬ���źű�־�����жϺ�����main��������ʹ�� */

 /*****************************************************************************************
 * ��������SCCB_WriteByte
 * ����  ��дһ�ֽ�����
 * ����  ��- WriteAddress: ��д���ַ 	- SendByte: ��д������	- DeviceAddress: ��������
 * ע��  ����        
 *****************************************************************************************/ 
void SCCB_WriteByte(u16 WriteAddress, u8 Sendbyte)
{
	sccb_start(); 					//����SCCB����
	sccb_write(OV_ID);
	wait_ack();	//д����ID	  
	delay_us(100);
  	sccb_write(WriteAddress);
	wait_ack();		//д�Ĵ�����ַ	  
	delay_us(100);
  	sccb_write(Sendbyte);
	wait_ack(); 	//д����	 
  	sccb_stop();	  
}
/******************************************************************************************************************
 * ��������SCCB_ReadByte
 * ����  ����ȡһ������
 * ����  ��- ReadAddress: ��������ַ
 * ע��  ����        
 **********************************************************************************************************************/ 
u8 SCCB_ReadByte(u8 ReadAddress)
{
	u8 val=0;
	sccb_start(); //wait_ack();				//����SCCB����
	sccb_write(OV_ID);
	wait_ack();	//д����ID	  
	delay_us(100);	 
  	sccb_write(ReadAddress);
	wait_ack();			//д�Ĵ�����ַ	  
	delay_us(100);	  
	sccb_stop();   
	delay_us(100);	   
	//���üĴ�����ַ�󣬲��Ƕ�
	sccb_start();//wait_ack();
	sccb_write(OV_ID|0X01);
	wait_ack();//���Ͷ�����	  
	delay_us(100);
  	val=sccb_read();		 	//��ȡ����
  	sccb_nack();
  	sccb_stop();
  	return val;
}


/******************************************************************************************************************
 * ��������SCCB_ReadBuf
 * ����  ����ȡһ������
 * ����  ��- pBuffer: ��Ŷ������� 	- length: ����������	- ReadAddress: ��������ַ		 - DeviceAddress: ��������
 * ע��  ����        
 **********************************************************************************************************************/ 
void SCCB_ReadBuf(u8*pBuff, u16 length, u8 ReadAddress)
{
	sccb_start();
	sccb_write(OV_ID); //������ַ
	if(!wait_ack())
	{
		sccb_stop();
		return ;
	}
	sccb_write((u8)(ReadAddress&0x00FF)); //�Ĵ�����ַ
	wait_ack();
	sccb_stop();
		
	sccb_start();
	sccb_write(OV_ID + 1);
	if(!wait_ack())
	{
		sccb_stop();
		return ;
	}
	while(length)
	{
		*pBuff = sccb_read();
		if(length == 1)
			sccb_nack();
		else
			sccb_ack();
		pBuff++;
		length--;	
	}
	sccb_stop();
}




/* ��ʼ�� */

u8 ov7725_init(void)
{
	u8 raw,temp;
	u16 sx=0,sy=0;
	u16 i=0;
	u16 reg=0;
	GPIO_InitTypeDef GPIO_InitTypeStruct;
	EXTI_InitTypeDef EXTI_InitTypeStruct;
	NVIC_InitTypeDef NVIC_InitTypeStruct;
	
	/* ov7725 gpio config */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|OV7725_OE_GPIO_CLK|
							OV7725_WRST_GPIO_CLK|OV7725_RRST_GPIO_CLK|
							OV7725_RCLK_GPIO_CLK|OV7725_WE_GPIO_CLK|
							OV7725_DATA_GPIO_CLK|OV7725_VSYNC_GPIO_CLK, ENABLE);
	
	/*(FIFO_OE--FIFO���ʹ��)*/
	GPIO_InitTypeStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitTypeStruct.GPIO_Pin = OV7725_OE_GPIO_PIN;
	GPIO_Init(OV7725_OE_GPIO_PORT, &GPIO_InitTypeStruct);
	
	/*(FIFO_WRST--FIFOд��λ)*/
	GPIO_InitTypeStruct.GPIO_Pin = OV7725_WRST_GPIO_PIN;
	GPIO_Init(OV7725_WRST_GPIO_PORT, &GPIO_InitTypeStruct);
	
	/*(FIFO_RRST--FIFO����λ) */
	GPIO_InitTypeStruct.GPIO_Pin = OV7725_RRST_GPIO_PIN;
	GPIO_Init(OV7725_RRST_GPIO_PORT, &GPIO_InitTypeStruct);
		
	/*(FIFO_RCLK-FIFO��ʱ��)*/
	GPIO_InitTypeStruct.GPIO_Pin = OV7725_RCLK_GPIO_PIN;
	GPIO_Init(OV7725_RCLK_GPIO_PORT, &GPIO_InitTypeStruct);

	/*(FIFO_WE--FIFOдʹ��)*/
	GPIO_InitTypeStruct.GPIO_Pin = OV7725_WE_GPIO_PIN;	
	GPIO_Init(OV7725_WE_GPIO_PORT, &GPIO_InitTypeStruct);
	

    /*(FIFO_DATA--FIFO�������)*/
	GPIO_InitTypeStruct.GPIO_Pin = 	OV7725_DATA_0_GPIO_PIN | OV7725_DATA_1_GPIO_PIN |
									OV7725_DATA_2_GPIO_PIN | OV7725_DATA_3_GPIO_PIN |
									OV7725_DATA_4_GPIO_PIN | OV7725_DATA_5_GPIO_PIN |
									OV7725_DATA_6_GPIO_PIN | OV7725_DATA_7_GPIO_PIN;
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OV7725_DATA_GPIO_PORT, &GPIO_InitTypeStruct);
	FIFO_OE_L();	  					/*����ʹFIFO���ʹ��*/
	FIFO_WE_H();   						/*����ʹFIFOд����*/
	
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	sccb_init();
	
	
	/*��ʼ��ʱ�ӣ�ע���ж�Ҫ��AFIO*/
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_AFIO|OV7725_VSYNC_GPIO_CLK, ENABLE );	 
    
	/*��ʼ������*/
	GPIO_InitTypeStruct.GPIO_Pin =  OV7725_VSYNC_GPIO_PIN;
    GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OV7725_VSYNC_GPIO_PORT, &GPIO_InitTypeStruct);

	/*�����ж�*/
    GPIO_EXTILineConfig(OV7725_VSYNC_EXTI_SOURCE_PORT, OV7725_VSYNC_EXTI_SOURCE_PIN);
    EXTI_InitTypeStruct.EXTI_Line = OV7725_VSYNC_EXTI_LINE;
    EXTI_InitTypeStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitTypeStruct.EXTI_Trigger = EXTI_Trigger_Falling ; 
    EXTI_InitTypeStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitTypeStruct);
    EXTI_GenerateSWInterrupt(OV7725_VSYNC_EXTI_LINE);		
	
	/*�������ȼ�*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitTypeStruct.NVIC_IRQChannel = OV7725_VSYNC_EXTI_IRQ;
    NVIC_InitTypeStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitTypeStruct.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitTypeStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitTypeStruct);
	
	/* ov7725 init */
	SCCB_WriteByte(0x12,0x80);	//��λOV7725
	delay_ms(50); 
	reg=SCCB_ReadByte(0X1c);		//��ȡ����ID �߰�λ
	reg<<=8;
	reg|=SCCB_ReadByte(0X1d);		//��ȡ����ID �Ͱ�λ
	if(reg!=OV7725_MID)
	{
		printf("MID:%d\r\n",reg);
		return 1;
	}
	reg=SCCB_ReadByte(0X0a);		//��ȡ����ID �߰�λ
	reg<<=8;
	reg|=SCCB_ReadByte(0X0b);		//��ȡ����ID �Ͱ�λ
	if(reg!=OV7725_PID)
	{
		printf("HID:%d\r\n",reg);
		return 2;
	}   
 	//��ʼ�� OV7725,����QVGA�ֱ���(320*240)  
	for(i=0;i<sizeof(ov7725_init_reg_tb1)/sizeof(ov7725_init_reg_tb1[0]);i++)
	{								
	   	SCCB_WriteByte(ov7725_init_reg_tb1[i][0],ov7725_init_reg_tb1[i][1]);
 	} 

	SCCB_WriteByte(COM7,0x46);		//����ΪQVGAģʽ
	SCCB_WriteByte(HSTART,0x3f); 	//ˮƽ��ʼλ��
	SCCB_WriteByte(HSIZE, 0x50); 	//ˮƽ�ߴ�
	SCCB_WriteByte(VSTRT, 0x03); 	//��ֱ��ʼλ��
	SCCB_WriteByte(VSIZE, 0x78); 	//��ֱ�ߴ�
	SCCB_WriteByte(HREF,  0x00);
	SCCB_WriteByte(HOutSize,0x50);	//����ߴ�
	SCCB_WriteByte(VOutSize,0x78); //����ߴ�
	
	raw=SCCB_ReadByte(HSTART);
	temp=raw+(sx>>2);//sx��8λ����HSTART,��2λ����HREF[5:4]
	SCCB_WriteByte(HSTART,temp);
	SCCB_WriteByte(HSIZE,320>>2);//width��8λ����HSIZE,��2λ����HREF[1:0]
	
	raw=SCCB_ReadByte(VSTRT);
	temp=raw+(sy>>1);//sy��8λ����VSTRT,��1λ����HREF[6]
	SCCB_WriteByte(VSTRT,temp);
	SCCB_WriteByte(VSIZE,240>>1);//height��8λ����VSIZE,��1λ����HREF[2]
	
	raw=SCCB_ReadByte(HREF);
	temp=((sy&0x01)<<6)|((sx&0x03)<<4)|((240&0x01)<<2)|(240&0x03)|raw;
	SCCB_WriteByte(HREF,temp);
	
	SCCB_WriteByte(HOutSize,320>>2);
	SCCB_WriteByte(VOutSize,240>>1);
	
	SCCB_ReadByte(EXHCH);	
	temp = (raw|(320&0x03)|((240&0x01)<<2));	
	SCCB_WriteByte(EXHCH,temp);
  	return 0; 	//ok
}
















////////////////////////////////////////////////////////////////////////////
//OV7725��������
//��ƽ������
//0:�Զ�ģʽ
//1:����
//2,����
//3,�칫��
//4,����
//5,ҹ��
void OV7725_Light_Mode(u8 mode)
{
	switch(mode)
	{
		case 0:	//Auto���Զ�ģʽ
			SCCB_WriteByte(0x13, 0xff); //AWB on 
			SCCB_WriteByte(0x0e, 0x65);
			SCCB_WriteByte(0x2d, 0x00);
			SCCB_WriteByte(0x2e, 0x00);
			break;
		case 1://sunny������
			SCCB_WriteByte(0x13, 0xfd); //AWB off
			SCCB_WriteByte(0x01, 0x5a);
			SCCB_WriteByte(0x02, 0x5c);
			SCCB_WriteByte(0x0e, 0x65);
			SCCB_WriteByte(0x2d, 0x00);
			SCCB_WriteByte(0x2e, 0x00);
			break;	
		case 2://cloudy������
			SCCB_WriteByte(0x13, 0xfd); //AWB off
			SCCB_WriteByte(0x01, 0x58);
			SCCB_WriteByte(0x02, 0x60);
			SCCB_WriteByte(0x0e, 0x65);
			SCCB_WriteByte(0x2d, 0x00);
			SCCB_WriteByte(0x2e, 0x00);
			break;	
		case 3://office���칫��
			SCCB_WriteByte(0x13, 0xfd); //AWB off
			SCCB_WriteByte(0x01, 0x84);
			SCCB_WriteByte(0x02, 0x4c);
			SCCB_WriteByte(0x0e, 0x65);
			SCCB_WriteByte(0x2d, 0x00);
			SCCB_WriteByte(0x2e, 0x00);
			break;	
		case 4://home������
			SCCB_WriteByte(0x13, 0xfd); //AWB off
			SCCB_WriteByte(0x01, 0x96);
			SCCB_WriteByte(0x02, 0x40);
			SCCB_WriteByte(0x0e, 0x65);
			SCCB_WriteByte(0x2d, 0x00);
			SCCB_WriteByte(0x2e, 0x00);
			break;	

		case 5://night��ҹ��
			SCCB_WriteByte(0x13, 0xff); //AWB on
			SCCB_WriteByte(0x0e, 0xe5);
			break;
	}
}		  
//ɫ������
//sat:-4~+4
void OV7725_Color_Saturation(s8 sat)
{
 	if(sat>=-4 && sat<=4)
	{	
		SCCB_WriteByte(USAT,(sat+4)<<4); 
		SCCB_WriteByte(VSAT,(sat+4)<<4);
	}
}
//��������
//bright��-4~+4
void OV7725_Brightness(s8 bright)
{
	u8 bright_value,sign;
  	switch(bright)
	{
		case 4:
			bright_value = 0x48;
			sign = 0x06;
			break;
		case 3:
			bright_value = 0x38;
			sign = 0x06;		
			break;	
		case 2:
			bright_value = 0x28;
			sign = 0x06;			
			break;	
		case 1:
			bright_value = 0x18;
			sign = 0x06;			
			break;
		case 0:
			bright_value = 0x08;
			sign = 0x06;			
			break;	
		case -1:
			bright_value = 0x08;
			sign = 0x0e;		
			break;		
		case -2:
			bright_value = 0x18;
			sign = 0x0e;		
			break;	
		case -3:
			bright_value = 0x28;
			sign = 0x0e;		
			break;	
		case -4:
			bright_value = 0x38;
			sign = 0x0e;		
			break;	
	}
	SCCB_WriteByte(BRIGHT, bright_value);
	SCCB_WriteByte(SIGN, sign);
}
//�Աȶ�����
//contrast��-4~+4
void OV7725_Contrast(s8 contrast)
{
	if(contrast >= -4 && contrast <=4)
	{
		SCCB_WriteByte(CNST,(0x30-(4-contrast)*4));
	}
}
//��Ч����
//0:��ͨģʽ    
//1,��Ƭ
//2,�ڰ�   
//3,ƫ��ɫ
//4,ƫ��ɫ
//5,ƫ��ɫ
//6,����	    
void OV7725_Special_Effects(u8 eft)
{
	switch(eft)
	{
		case 0://����
			SCCB_WriteByte(0xa6, 0x06);//TSLB����
			SCCB_WriteByte(0x60, 0x80);//MANV,�ֶ�Vֵ
			SCCB_WriteByte(0x61, 0x80);//MANU,�ֶ�Uֵ
			break;
		case 1://��Ƭ
			SCCB_WriteByte(0xa6, 0x46);
			break;
		case 2://�ڰ�
			SCCB_WriteByte(0xa6, 0x26);
			SCCB_WriteByte(0x60, 0x80);
			SCCB_WriteByte(0x61, 0x80);
			break;		
		case 3://ƫ��
			SCCB_WriteByte(0xa6, 0x1e);
			SCCB_WriteByte(0x60, 0x80);
			SCCB_WriteByte(0x61, 0xc0);		
			break;
		case 4://ƫ��
			SCCB_WriteByte(0xa6, 0x1e);
			SCCB_WriteByte(0x60, 0x60);
			SCCB_WriteByte(0x61, 0x60);		
			break;
		case 5://ƫ��
			SCCB_WriteByte(0xa6, 0x1e);
			SCCB_WriteByte(0x60, 0xa0);
			SCCB_WriteByte(0x61, 0x40);	
			break;
		case 6://����
			SCCB_WriteByte(0xa6, 0x1e);
			SCCB_WriteByte(0x60, 0x40);
			SCCB_WriteByte(0x61, 0xa0);
			break;	

	}
}	
//����ͼ���������
//width:���ͼ����,<=320
//height:���ͼ��߶�,<=240
//mode:0��QVGA���ģʽ��1��VGA���ģʽ
//QVGAģʽ���ӷ�Χ�㵫���ﲻ�Ǻ�������VGAģʽ���ӷ�ΧС��������
void OV7725_Window_Set(u16 width,u16 height,u8 mode)
{
	u8 raw,temp;
	u16 sx,sy;	
	if(mode)
	{
		sx=(640-width)/2;
		sy=(480-height)/2;
		SCCB_WriteByte(COM7,0x06);		//����ΪVGAģʽ
		SCCB_WriteByte(HSTART,0x23); 	//ˮƽ��ʼλ��
		SCCB_WriteByte(HSIZE,0xA0); 	//ˮƽ�ߴ�
		SCCB_WriteByte(VSTRT,0x07); 	//��ֱ��ʼλ��
		SCCB_WriteByte(VSIZE,0xF0); 	//��ֱ�ߴ�
		SCCB_WriteByte(HREF,0x00);
		SCCB_WriteByte(HOutSize,0xA0); //����ߴ�
		SCCB_WriteByte(VOutSize,0xF0); //����ߴ�
	}
	else
	{
		sx=(320-width)/2;
		sy=(240-height)/2;
		SCCB_WriteByte(COM7,0x46);		//����ΪQVGAģʽ
		SCCB_WriteByte(HSTART,0x3f); 	//ˮƽ��ʼλ��
		SCCB_WriteByte(HSIZE, 0x50); 	//ˮƽ�ߴ�
		SCCB_WriteByte(VSTRT, 0x03); 	//��ֱ��ʼλ��
		SCCB_WriteByte(VSIZE, 0x78); 	//��ֱ�ߴ�
		SCCB_WriteByte(HREF,  0x00);
		SCCB_WriteByte(HOutSize,0x50);	//����ߴ�
		SCCB_WriteByte(VOutSize,0x78); //����ߴ�
	}
	raw=SCCB_ReadByte(HSTART);
	temp=raw+(sx>>2);//sx��8λ����HSTART,��2λ����HREF[5:4]
	SCCB_WriteByte(HSTART,temp);
	SCCB_WriteByte(HSIZE,width>>2);//width��8λ����HSIZE,��2λ����HREF[1:0]
	
	raw=SCCB_ReadByte(VSTRT);
	temp=raw+(sy>>1);//sy��8λ����VSTRT,��1λ����HREF[6]
	SCCB_WriteByte(VSTRT,temp);
	SCCB_WriteByte(VSIZE,height>>1);//height��8λ����VSIZE,��1λ����HREF[2]
	
	raw=SCCB_ReadByte(HREF);
	temp=((sy&0x01)<<6)|((sx&0x03)<<4)|((height&0x01)<<2)|(width&0x03)|raw;
	SCCB_WriteByte(HREF,temp);
	
	SCCB_WriteByte(HOutSize,width>>2);
	SCCB_WriteByte(VOutSize,height>>1);
	
	SCCB_ReadByte(EXHCH);	
	temp = (raw|(width&0x03)|((height&0x01)<<2));	
	SCCB_WriteByte(EXHCH,temp);	
}





/**
  * @brief  ������ʾλ��
	* @param  sx:x��ʼ��ʾλ��
	* @param  sy:y��ʼ��ʾλ��
	* @param  width:��ʾ���ڿ��,Ҫ���OV7725_Window_Set�����е�widthһ��
	* @param  height:��ʾ���ڸ߶ȣ�Ҫ���OV7725_Window_Set�����е�heightһ��
  * @retval ��
  */
void OV_Lcd_Display(u16 sx,u16 sy,u16 width,u16 height)
{
	u32 i;
	u16 Camera_Data;
	LCD_Scan_Dir(U2D_L2R);//���ϵ���,������
	
	LCD_Set_Window(sx,sy,width,height);
	LCD_WriteRam_Start();
	
	FIFO_PREPARE;  			/*FIFO׼��*/
	for(i=0;i<width*height;i++)
	{	
		READ_FIFO_PIXEL(Camera_Data);
	//	printf("0x%x\t",Camera_Data);
		LCD_WriteRam(Camera_Data);
	}
	LCD_Scan_Dir(TFT_SCAN);	
}
























