#define ALLOCATE_EXTERN
#include "include.h"

unsigned char xdata Uart_Buf[200];//���ݻ������� rev buffer
unsigned char xdata Uart_send_Buf[30];//send buffer
unsigned char xdata magic_flag = 0;
unsigned char xdata guc_Read_a[2] = {0x00}; //���ڴ�Ŷ�ȡ������
unsigned char xdata tick_lo;
unsigned char xdata tick_hi;

int main(void)
{
	SystemInit();//��ʼ��
	GPIO_Init();
	
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
		//clear here?
		//clear_magic_flag();
		enable_timer(1);
		uart1_init(0xff, 0x30);
		while(1)
		{
			WDTC |= 0x10;		                  //�幷
			Receive_Packet_tuya(Uart_Buf);		//�����ж�
		}		
	}
	return	0;
}

