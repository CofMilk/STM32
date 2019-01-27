#include "temper.h"
#include "stm32f10x.h"
#include "delay.h"

//��ʼ��
void Temp_Init()
{
	ADC_InitTypeDef ADC_InitTypeStruct;
	//����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	//��λ
	ADC_DeInit(ADC1);
	//ADC1��ʼ��
	ADC_InitTypeStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitTypeStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitTypeStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitTypeStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitTypeStruct.ADC_NbrOfChannel = 1;
	ADC_InitTypeStruct.ADC_ScanConvMode = DISABLE;
	ADC_Init(ADC1,&ADC_InitTypeStruct);
	
	//���������޷������¶�
	ADC_TempSensorVrefintCmd(ENABLE); //�����ڲ��¶ȴ�����
	//����ͨ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_16,1,ADC_SampleTime_239Cycles5);
	ADC_Cmd(ADC1,ENABLE);
	//У׼
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
}


//����
u16 Temp_Goon()
{
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	//�ȵ����
	while( ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET );
	
	return ADC_GetConversionValue(ADC1);	
}

//����
float Temp_Val()
{
	u8 i; u32 val = 0;
	float vval,temp;
	for(i=0 ; i<10 ; i++)
	{
		val += Temp_Goon();
		delay_ms(5);
	}
	val /= 10;
	vval = (float)val*(3.3/4096);
	temp = (1.43-vval)/0.0043 + 25;
	
	return temp;		
}




