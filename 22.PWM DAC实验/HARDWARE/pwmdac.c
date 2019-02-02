#include "pwmdac.h"




void Pwm_DAC_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitTypeStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitTypeStruct;
	TIM_OCInitTypeDef TIM_OCInitTypeStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	//PA8/TIM1_CH1
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitTypeStruct);
	
	//TIM1 CH1 Init
	TIM_TimeBaseInitTypeStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitTypeStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitTypeStruct.TIM_Period = arr;
	TIM_TimeBaseInitTypeStruct.TIM_Prescaler = psc;
	TIM_TimeBaseInitTypeStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitTypeStruct);
	//pwm
	TIM_OCInitTypeStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitTypeStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitTypeStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM1,&TIM_OCInitTypeStruct);


	TIM_ARRPreloadConfig(TIM1, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable); //Ԥװ�ؼĴ���ʹ��
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE �����ʹ��,�߼���ʱ�����뿪�����
	TIM_Cmd(TIM1,ENABLE);
	
}





