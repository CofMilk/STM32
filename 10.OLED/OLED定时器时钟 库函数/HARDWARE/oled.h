#ifndef __OLED_H__
#define __OLED_H__
#include "sys.h"


//д���� ��1  д���� ��0
#define OLED_CMD 0
#define OLED_DATA 1   




//�˿ڶ���
#define CLK PDout(4)   //spiʱ����
#define SDA PDout(5)   //spi������
#define RST PDout(6)   //��λ��
#define DC  PDout(7)   //��������ѡ����
#define CS  PDout(8)   //Ƭѡ��  /* ģ����Ƭѡ�ӿ�Ĭ��Ϊ��*/


//OLED���䷽ʽ 1:8080����  0:SPI
#define OLED_MODE  0


void OLED_init(void);  //OLED��ʼ��
void OLED_WR_Byte(u8 dat,u8 cmd);  //дһ���ֽ����ݻ�����
u32 mypow(u8 m,u8 n);  //����m^n
void OLED_Refresh_Gram(void);   //������ʾ�ڴ�
void OLED_Clear(void); //����
void OLED_DrawPoint(u8 x,u8 y,u8 t);  //����
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);  //��ʾһ���ַ�
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);    //��ʾһ������
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size);     //��ʾһ���ַ���


#endif


