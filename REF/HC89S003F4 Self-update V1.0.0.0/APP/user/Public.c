#include "Include.h"

void SystemInit(void)
{
	WDTCCR = 0x00;						//关闭看门狗
	CLKSWR = 0x51;						//选择内部高频RC为系统时钟，内部高频RC 2分频，Fosc=16MHz
	CLKDIV = 0x01;						//Fosc 1分频得到Fcpu，Fcpu=16MHz 
	
	P2M0 = P2M0&0x0F|0x80;				//P21设置为推挽输出
	P0M1 = P0M1&0x0F|0x20;				//P03设置为上拉输入
	TXD_MAP = 0x21;						//TXD映射P21
	RXD_MAP = 0x03;						//RXD映射P03		
	T4CON = 0x06;						//T4工作模式：UART1波特率发生器
    TH4 = 0xFF;
	TL4 = 0x98;							//波特率9600
	SCON2 = 0x02;						//8位UART，波特率可变
	SCON = 0x10;						//允许串行接收
	IE |= 0X10;							//使能串口中断
	
	P1M0 = P1M0&0xF0|0x08;				//P10设置为推挽输出
	
	EA = 1;
}

/**
  * @说明  	写入需要校验的数据并返回校验结果
  * @参数  	*fucp_CheckArr : CRC校验数据首地址
  * @参数  	fui_CheckLen : CRC校验数据长度
  *         取值范围 0 - 65535
  * @返回值 CRC校验结果。
  			低位在前，高位在后
  * @注		无
  */
unsigned int CRC_CalcCRC(unsigned char *fucp_CheckArr,unsigned int fui_CheckLen)
{
	while(fui_CheckLen--)CRCL = *(fucp_CheckArr++);
	return CRCR;
}

/**
  * @说明  	复位重读代码选项
  * @参数  	无
  * @返回值 无
  * @注		无
  */
void ResetReadOption(void)
{
	IAP_CMD = 0xF00F;
	IAP_CMD = 0x7887;
}


