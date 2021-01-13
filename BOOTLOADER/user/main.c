#define ALLOCATE_EXTERN
#include "include.h"

unsigned char xdata Uart_Buf[200];//数据缓存数组
unsigned char xdata magic_flag = 0;
unsigned char xdata guc_Read_a[2] = {0x00}; //用于存放读取的数据
unsigned char xdata tick_lo;
unsigned char xdata tick_hi;

//与上位机的时序调整
//Earse_Flash()函数修改，增加LVD检测，每擦除前判断LVD电压
//Receive_Packet()增加LVD检测，程序结构修改，烧写错误则全擦，查空修改为查空前128个字节
//HandShake()握手时间间隔修改，看门狗打开，TXD引脚初始化靠后，RXD为施密特输入上拉（必须带上拉）BOR 2.0V，LVD 2.4V

int main(void)
{
	SystemInit();//初始化
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
		uart1_init();//
		while(1)
		{
			WDTC |= 0x10;                   //清狗
			switch(Receive_Packet_tuya(Uart_Buf))//接收判断
			{
				case SUCCESS://成功发ACK   0x01
									//
									break;
				case ERROR://失败发NACK		 0x00
									//
									break;
				case NACK_TIME://超时跳转APP	0x02
									//超时
									break;
				default://其他不操作
									break;			
			}		
		}		
	}
	return	0;
}

