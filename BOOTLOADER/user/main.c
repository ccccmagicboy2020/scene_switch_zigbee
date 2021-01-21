#define ALLOCATE_EXTERN
#include "include.h"

unsigned char xdata Uart_Buf[200];//数据缓存数组 rev buffer
unsigned char xdata Uart_send_Buf[30];//send buffer
unsigned char xdata magic_flag = 0;
unsigned char xdata guc_Read_a[2] = {0x00}; //用于存放读取的数据
unsigned char xdata tick_lo;
unsigned char xdata tick_hi;
extern unsigned char flag0;

int main(void)
{
	SystemInit();//初始化
	GPIO_Init();
	
	magic_flag = read_magic_flag();
	
	if (0 == magic_flag)
	{
		HandShake();//握手
		while(1)
		{
			WDTC |= 0x10;                   //清狗
			switch(Receive_Packet(Uart_Buf))//接收判断
			{
				case SUCCESS://成功发ACK   0x01
									Uart_SendByte(ACK);		//0x79
									break;
				case ERROR://失败发NACK		 0x00
									Uart_SendByte(NACK);	//0x1F
									break;
				case NACK_TIME://超时跳转APP	0x02
									IAR_Soft_Rst_No_Option();
									break;
				default://其他不操作
									break;			
			}
		}		
	}
	else
	{
		//
		enable_timer(1);
		uart1_init(0xff, 0x30);//
		//mcu_reset_zigbee(0);
		//mcu_reset_zigbee(1);
		//mcu_ota_result_report(0x01);
		while(1)
		{
			WDTC |= 0x10;		                  //清狗
			Receive_Packet_tuya(Uart_Buf);		//接收判断

			if (1 == flag0)
			{
				mcu_ota_fw_request();
			}
		}		
	}
	return	0;
}

