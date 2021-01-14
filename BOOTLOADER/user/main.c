#define ALLOCATE_EXTERN
#include "include.h"

unsigned char xdata Uart_Buf[200];//���ݻ�������
unsigned char xdata Uart_send_Buf[30];
unsigned char xdata magic_flag = 0;
unsigned char xdata guc_Read_a[2] = {0x00}; //���ڴ�Ŷ�ȡ������
unsigned char xdata tick_lo;
unsigned char xdata tick_hi;

int main(void)
{
	SystemInit();//��ʼ��
	magic_flag = read_magic_flag();
	
	if (0 == magic_flag)
	{
		HandShake();//����
		while(1)
		{
			WDTC |= 0x10;                   //�幷
			switch(Receive_Packet(Uart_Buf))//�����ж�
			{
				case SUCCESS://�ɹ���ACK   0x01
									Uart_SendByte(ACK);		//0x79
									break;
				case ERROR://ʧ�ܷ�NACK		 0x00
									Uart_SendByte(NACK);	//0x1F
									break;
				case NACK_TIME://��ʱ��תAPP	0x02
									IAR_Soft_Rst_No_Option();
									break;
				default://����������
									break;			
			}
		}		
	}
	else
	{
		uart1_init();//
		while(1)
		{
			WDTC |= 0x10;		                  //�幷
			Receive_Packet_tuya(Uart_Buf);		//�����ж�	
		}		
	}
	return	0;
}

