#define ALLOCATE_EXTERN
#include "include.h"
unsigned char xdata Uart_Buf[200];//���ݻ�������

int main(void)
{
	SystemInit();//��ʼ��
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
