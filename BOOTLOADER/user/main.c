#define ALLOCATE_EXTERN
#include "include.h"

//#define HOLYCHIP

unsigned char xdata Uart_Buf[200];//���ݻ������� rev buffer
unsigned char xdata Uart_send_Buf[30];//send buffer
unsigned char xdata magic_flag = 0;
unsigned char xdata guc_Read_a[2] = {0x00}; //���ڴ�Ŷ�ȡ������
unsigned char guc_Read_a1[20] = {0x00}; //���ڴ�Ŷ�ȡ������
unsigned char xdata tick_lo;
unsigned char xdata tick_hi;
extern short ota_packet_total_num;
extern short ota_packet_current_num;
extern unsigned short led_speed;

int main(void)
{
	SystemInit();//��ʼ��
	GPIO_Init();
	
	magic_flag = read_magic_flag();
	led_speed = 100;
	
	if (0 == magic_flag)
	{
		HandShake();//����
		
#ifdef HOLYCHIP
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
#endif
	}
	else
	{
		enable_timer(1);
		uart1_init(0xff, 0x30);
		
		ota_packet_current_num = 0;
		ota_packet_total_num = -1;
		
		read_ota_struct();
		
		while(1)
		{
			WDTC |= 0x10;		                  //�幷
			switch(Receive_Packet_tuya(Uart_Buf))//�����ж�
			{
				case SUCCESS://�ɹ���ACK   0x01
									break;
				case ERROR://ʧ�ܷ�NACK		 0x00
									break;
				case NACK_TIME://��ʱ��תAPP	0x02
									if (ota_packet_current_num <= ota_packet_total_num)
									{
										mcu_ota_fw_request();
									}
									break;
				default://����������
									break;			
			}			
		}		
	}
	return	0;
}

