#include "dma.h"
#include "stm32f10x.h"

u8 buff[SIZEBUFF];


//  �洢�������裨���ڣ�
void dma_init()
{
	DMA_InitTypeDef DMA_InitTypeStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_DeInit(DMA1_Channel4);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitTypeStruct.DMA_BufferSize = SIZEBUFF;
	DMA_InitTypeStruct.DMA_DIR = DMA_DIR_PeripheralDST; //������ΪĿ��
	DMA_InitTypeStruct.DMA_M2M = DMA_M2M_Disable;  //�洢�����洢��ģʽ��
	DMA_InitTypeStruct.DMA_MemoryBaseAddr = (u32)buff; //�洢������ַ
	DMA_InitTypeStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //�洢���������С �ֽ�
	DMA_InitTypeStruct.DMA_MemoryInc =  DMA_MemoryInc_Enable; //�洢������ģʽ
	DMA_InitTypeStruct.DMA_Mode =  DMA_Mode_Normal; //DMAģʽ  ����
	DMA_InitTypeStruct.DMA_PeripheralBaseAddr = (u32) &USART1->DR; //�������ַ
	DMA_InitTypeStruct.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Byte;  //���贫���С �ֽ�
	DMA_InitTypeStruct.DMA_PeripheralInc =  DMA_PeripheralInc_Disable; //��������ģʽ
	DMA_InitTypeStruct.DMA_Priority =  DMA_Priority_Medium;  //ͨ�����ȼ�
	DMA_Init(DMA1_Channel4,&DMA_InitTypeStruct);
	
}


void dma_goon()
{
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
	DMA_Cmd(DMA1_Channel4,ENABLE);
	DMA_SetCurrDataCounter(DMA1_Channel4,SIZEBUFF);
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);
	DMA_ClearFlag(DMA1_FLAG_TC4);//���ͨ��4������ɱ�־
	
	DMA_Cmd(DMA1_Channel4,DISABLE);
	USART_DMACmd(USART1,USART_DMAReq_Tx,DISABLE);
}











