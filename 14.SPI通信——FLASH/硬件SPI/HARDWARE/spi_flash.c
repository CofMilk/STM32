#include "spi_flash.h"
#include "stm32f10x.h"

static int time_check = time_out;

//��ʼ��SPI2 ʱ�� ����  �Ĵ���
void spi_init()
{
	GPIO_InitTypeDef GPIO_InitTypeStruct;
	SPI_InitTypeDef SPI_InitTypeStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //spi2 GPIOB
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);  //spi2 init
	
	//NSS   PB12
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_12;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitTypeStruct);
	
	//MISO PB14
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_14;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitTypeStruct);
	
	//MOSI  PB15
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_15;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitTypeStruct);
	
	//SCK PB13
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitTypeStruct);
	
	//spi init
	SPI_InitTypeStruct.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;
	SPI_InitTypeStruct.SPI_CPHA=SPI_CPHA_2Edge;
	SPI_InitTypeStruct.SPI_CPOL=SPI_CPOL_High;
	SPI_InitTypeStruct.SPI_CRCPolynomial=7;
	SPI_InitTypeStruct.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitTypeStruct.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI_InitTypeStruct.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_InitTypeStruct.SPI_Mode=SPI_Mode_Master;
	SPI_InitTypeStruct.SPI_NSS=SPI_NSS_Soft;
	SPI_Init(SPI2,&SPI_InitTypeStruct);
		
	//spi cmd
	SPI_Cmd(SPI2,ENABLE);
	
	//NSS high
	NSS_High;
	
}

//��ͬʱ��дһ���ֽ�
u8 flash_write_byte(u8 data)
{
	time_check = time_out;
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE) == RESET)
	{
		if((time_check--) == 0)	return spi_check_timeout();
	}
	SPI_I2S_SendData(SPI2,data);
	
	time_check = time_out;
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((time_check--) == 0)	return spi_check_timeout();
	}
	return SPI_I2S_ReceiveData(SPI2);
}

//��ȡһ���ֽ�
u8 flash_read_byte()
{
	return   flash_write_byte(DUMMY); 
}

//��ȡFLASH��ID
u32 flash_read_id(void)
{
	vu32 id_temp;
	NSS_Low;
	flash_write_byte(ID_COM);
	
	id_temp = flash_read_byte();
	id_temp <<= 8;
	id_temp |= flash_read_byte();
	id_temp <<= 8;
	id_temp |= flash_read_byte();
	NSS_High;
	return id_temp;
}

//�������
void flash_sector_erase(u32 addr)
{
	flash_write_enable();
	NSS_Low;
	flash_write_byte(Sector_Erase);
	flash_write_byte((addr>>16) & 0xFF);
	flash_write_byte((addr>>8) & 0XFF);
	flash_write_byte(addr & 0XFF);
	NSS_High;
	flash_wait_writewait();
}

//��ȡ�ַ���
void flash_read_buf(u32 addr , u8* buf , u32 num_read)
{
	NSS_Low;
	flash_write_byte(Flash_Read_COM); 
	flash_write_byte(addr>>16 & 0xFF);
	flash_write_byte(addr>>8 & 0XFF);
	flash_write_byte(addr & 0XFF);
	while(num_read--)
	{
		*buf = flash_read_byte();
		buf++; 
	}
	NSS_High;
}

//д���ַ���  num_write <= 256
void flash_write_buf(u32 addr , u8* buf ,u32 num_write)
{
	flash_write_enable();
	NSS_Low;
	
	flash_write_byte(Flash_Write_COM);
	flash_write_byte((addr>>16) & 0xFF);
	flash_write_byte((addr>>8) & 0xFF);
	flash_write_byte(addr & 0xFF);
	while(num_write--)
	{
		flash_write_byte(*buf);
		buf++;
	}
	
	NSS_High;
	flash_wait_writewait();
}

//д�����ⳤ�ȵ��ַ���
void flash_write_anybuf(u32 addr , u8* buf ,u32 num_write)
{
	u16 pageremain;
	pageremain = 256 - addr%256;  //��ҳʣ����ֽ���
	
	if(num_write <= pageremain)  pageremain = num_write; //��ҳʣ��ռ��ֱ�ӷ���
	
	while(1)
	{
		flash_write_buf(addr , buf , pageremain);
		if(pageremain == num_write)  break;  //д�����
		else
		{
			buf += pageremain ;
			addr += pageremain ;
			
			num_write -= pageremain ;   //��ȥ�Ѿ�д���˵��ֽ���
			if(num_write > 256) pageremain = 256;   //һ�ο���д��256���ֽ�
			else pageremain = num_write ; 	  //����256���ֽ���
		}
	}
}



//�ȴ�д�����  �����Ӧ�Ĵ�����ֵ
void flash_wait_writewait()
{
	u8 temp;
	NSS_Low;
	
	flash_write_byte(Flash_Status);
	do{	
		temp=flash_read_byte();
	}while((temp&0x01) == 1);
	
	NSS_High;
} 

//FLASH д��ʹ��
void  flash_write_enable()
{
	NSS_Low;
	flash_write_byte(Flash_Write_Enable);
	NSS_High;
}

u8 spi_check_timeout()
{
	FLASH_ERROR("�ȴ���ʱ"); 
	return 0;
}

    




