#ifndef __LCD_H__
#define __LCD_H__
#include "stm32f10x.h"


typedef struct {
	u16 width;   //��Ļ���
	u16 height;  //��Ļ�߶�
	u16 id;  	 //����ic ID
	u8 dir;      //����ɨ�跽��
	u16 wramcmd; //��ʼдGRAMָ��
	u16 setxcmd; //����x��������
	u16 setycmd; //����y����ָ��
	
}LCD_DEV;

//ɨ�跽����
#define L2R_U2D   0 //�����ң����ϵ���
#define L2R_D2U   1 //�����ң����µ���
#define R2L_U2D   2 //���ҵ��󣬴��ϵ���  
#define R2L_D2U   3 //���ҵ��󣬴��µ���

#define U2D_L2R   4 //���ϵ��£�������
#define U2D_R2L   5 //���ϵ��£����ҵ���
#define D2U_L2R   6 //���µ��ϣ�������
#define D2U_R2L   7 //���µ��ϣ����ҵ���

//Ĭ�ϻ�����ɫ�ͱ�����ɫ
#define PRINT_COLOR  RED
#define BACK_COLOR   GREEN

//Ĭ��ɨ�跽��
#define TFT_SCAN   L2R_U2D
//������ɫ����
#define WHILE  0xFFFF
#define BLACK  0x0000
#define BLUE   0x001F
#define RED    0xF800
#define GREEN  0x07E0


typedef struct lcd{   //RSѡ��  ��ַA10
	vu16 LCD_REG;   //д����
	vu16 LCD_RAM;   //д����
}TftlcdTypedef;
//LCD �ṹ��
#define LCD_BASE   ((u32)(0X6C000000|0X7FE))
#define LCD        ((TftlcdTypedef*)LCD_BASE)


void TFTLCD_Init(void);                                                     //LCD��ʼ��
void LCD_Display_on(void);                                                  //��ʾ��
void LCD_Display_off(void);                                                 //��ʾ��
void LCD_WR_REG(u16 regval);												//д���� �Ĵ���
void LCD_WR_DATA(u16 data);													//д����
u16  LCD_RD_DATA(void);														//������
void LCD_WriteReg(u16 LCD_Reg,u16 RegVal);									//д����+����
u16  LCD_ReadReg(u16 LCD_Reg);												//���Ĵ���ֵ
void LCD_WriteRam_Start(void);												//��ʼдRGB����
void LCD_WriteRam(u16 RGBcode);												//дRGB����
void LCD_SetCursor(u16 Xpos, u16 Ypos);										//�������꺯��
void LCD_Clear(u16 Color);	 												//��������
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);					//���ô��ں���
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);						//��亯��
void LCD_Scan_Dir(u8 dir);													//X Yɨ�跽����
void LCD_Display_Dir(u8 dir);												//LCD��ʾ������

u32 LCD_Pow(u16 m,u16 n);													//����m��n�η�����
void LCD_DrawPoint(u16 x,u16 y);											//���㺯��
u16  LCD_ReadPoint(u16 x,u16 y); 											//���㺯��
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);								//���ٻ��㺯��
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);						 			//��Բ����
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);							//���ߺ���
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);	   					//�����κ���
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);						//��ʾһ���ַ�����
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);  				//��ʾһ�����ֺ���
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);		//��ʾһ���ַ�������


#endif


