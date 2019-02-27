#include "can.h"


u8 Sdata[DATA_MAX];
u8 Rdata[DATA_MAX];




static void Can_Gpio_Config(void)
{
	GPIO_InitTypeDef GPIO_InitTypeStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);  //ʹ��CAN1 
	//CAN1 Rx
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitTypeStruct);
	//CAN1 Tx
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitTypeStruct);
	
}
static void Can_Mode_Config(void)
{
	CAN_InitTypeDef CAN_InitTypeStruct;
#if CAN_Rx_INT_ENABLE	
	NVIC_InitTypeDef NVIC_InitTypeStruct;
#endif
	
	CAN_InitTypeStruct.CAN_Mode = CAN_Mode_LoopBack; //�ػ�ģʽ
	CAN_InitTypeStruct.CAN_NART = DISABLE; //��ֹ�����Զ��ش� disable ����ֹ
	CAN_InitTypeStruct.CAN_RFLM = DISABLE; //����FIFO����ģʽ
	CAN_InitTypeStruct.CAN_TTCM = DISABLE; //ʱ�䴥��ͨ��ģʽ
	CAN_InitTypeStruct.CAN_TXFP = DISABLE; //���ķ������ȼ�
	CAN_InitTypeStruct.CAN_ABOM = DISABLE; //�Զ����߹���
	CAN_InitTypeStruct.CAN_AWUM = ENABLE;  //�Զ�����ģʽ
	
	CAN_InitTypeStruct.CAN_Prescaler = 4;  //4/36MHz = 1/9MHz
	CAN_InitTypeStruct.CAN_BS1 = CAN_BS1_5tq;
	CAN_InitTypeStruct.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitTypeStruct.CAN_SJW = CAN_SJW_2tq;
	CAN_Init(CAN1,&CAN_InitTypeStruct);
	
#if CAN_Rx_INT_ENABLE
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	NVIC_InitTypeStruct.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitTypeStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitTypeStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitTypeStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitTypeStruct);
	
#endif
}
static void Can_Filter_Config(void)
{
	CAN_FilterInitTypeDef CAN_FilterInitTypeStruct;
	
	CAN_FilterInitTypeStruct.CAN_FilterActivation = ENABLE; //���������
	CAN_FilterInitTypeStruct.CAN_FilterFIFOAssignment = CAN_FilterFIFO0; //������ FIFO 0
	CAN_FilterInitTypeStruct.CAN_FilterMode = CAN_FilterMode_IdMask; //������ģʽ  ����ģʽ
	CAN_FilterInitTypeStruct.CAN_FilterNumber = 0; //�������������
	CAN_FilterInitTypeStruct.CAN_FilterScale = CAN_FilterScale_32bit; //��ʮ��λģʽ
	
	CAN_FilterInitTypeStruct.CAN_FilterIdHigh = ( ((0x00001314<<3)| CAN_ID_EXT| CAN_RTR_DATA ) &(0xFFFF0000) )>>16;
	CAN_FilterInitTypeStruct.CAN_FilterIdLow = ( ((0x00001314<<3)| CAN_ID_EXT| CAN_RTR_DATA ) & 0x0000FFFF );
	CAN_FilterInitTypeStruct.CAN_FilterMaskIdHigh = 0xFFFF;
	CAN_FilterInitTypeStruct.CAN_FilterMaskIdLow =	0xFFFF;
	
	CAN_FilterInit(&CAN_FilterInitTypeStruct);
}


void Can_Init()
{
	Can_Gpio_Config();
	Can_Mode_Config();
	Can_Filter_Config();
}





void Can_SendMessages(u8 * p, u8 len)
{
	u8 i,numb;
	CanTxMsg CanTxMsgStruct;
	
	//CanTxMsgStruct.StdId=
	CanTxMsgStruct.ExtId = 0x1314;
	CanTxMsgStruct.IDE = CAN_ID_EXT; //��չID
	CanTxMsgStruct.RTR = CAN_RTR_DATA;
	CanTxMsgStruct.DLC = len;
	for(i=0; i<len; i++)
	{
		CanTxMsgStruct.Data[i] = p[i];
	}
	numb = CAN_Transmit(CAN1,&CanTxMsgStruct); //���ط��͵������
	while( CAN_TransmitStatus(CAN1, numb) == CAN_TxStatus_Failed);
}




u8 Can_ReceiveMessages(u8 * p)
{
	u8 i;
	CanRxMsg CanRxMsgStruct;
	if( CAN_MessagePending(CAN1, CAN_FIFO0) == 0 ) return 0;
	
	CAN_Receive(CAN1, CAN_FIFO0, &CanRxMsgStruct);
	for(i=0; i<CanRxMsgStruct.DLC; i++)
	{
		p[i] = CanRxMsgStruct.Data[i];
	}
	return CanRxMsgStruct.DLC;
}


#if CAN_Rx_INT_ENABLE

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	u8 i;
	CanRxMsg CanRxMsgStruct;
	CAN_Receive(CAN1, CAN_FIFO0, &CanRxMsgStruct);
	for(i=0; i<CanRxMsgStruct.DLC; i++)
	{
		Rdata[i] = CanRxMsgStruct.Data[i];
		printf("%d ",Rdata[i]);
	}
	printf("\r\n");
}

#endif



