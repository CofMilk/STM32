#include "dac.h"



void Dac_Init()
{
	GPIO_InitTypeDef GPIO_InitTypeStruct;
	DAC_InitTypeDef DAC_InitTypeStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//ģ������  DAC_OUT1 PA4
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitTypeStruct);
	
	//ʹ��DACʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
	
	DAC_InitTypeStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	DAC_InitTypeStruct.DAC_OutputBuffer = DAC_OutputBuffer_Disable; //�ر��������
	DAC_InitTypeStruct.DAC_Trigger = DAC_Trigger_None; //��ʹ��dac������ʽ
	DAC_InitTypeStruct.DAC_WaveGeneration = DAC_WaveGeneration_None; //��ʹ�ò��η���
	DAC_Init(DAC_Channel_1,&DAC_InitTypeStruct);
	
	DAC_Cmd(DAC_Channel_1,ENABLE);
	
	DAC_SetChannel1Data(DAC_Align_12b_R,0);
	
}



//0-3300 : 0-3.3v
void Dac_Set_VVal(u16 val)
{
	float temp = val;
	temp /= 1000;
	
	temp = temp*4095/3.3; 
	DAC_SetChannel1Data(DAC_Align_12b_R,temp);
}





