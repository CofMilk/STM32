#ifndef _modbus_
#define _modbus_
typedef struct
{
  unsigned char myadd;//���豸�ĵ�ַ
  unsigned char rcbuf[128];//Modbus���ջ�����
  unsigned short int timout;//Modbus�����ݶ���ʱ��
  unsigned char recount;//Modbus�˿��Ѿ��յ������ݸ���
  unsigned char TXcount;//Modbus�˿��Ѿ����͵����ݸ���
  unsigned char timrun;//Modbus��ʱ���Ƿ��ʱ�ı�־
  unsigned char reflag;//�յ�һ֡���ݵı�־
  unsigned char sendbuf[128];//Modbus���ͻ�����
}MODBUS;

extern MODBUS modbus;
void Modbus_Init(void);
void Modbus_Event(void);
void ModBus_Display(void);
void Modbus_fun3(unsigned char Addr,unsigned short int ReadAdd,unsigned short int ReadNum);//3�Ź����봦��(����Ҫ��ȡ���ӻ��ļĴ���)
void Modbus_fun6(unsigned char Addr,unsigned short int WriteAdd,unsigned short int val);//6�Ź����봦��


#endif

