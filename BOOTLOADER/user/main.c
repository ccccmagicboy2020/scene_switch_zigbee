#define ALLOCATE_EXTERN
#include "include.h"
unsigned char xdata Uart_Buf[200];//数据缓存数组

int main(void)
{
	SystemInit();//初始化
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
