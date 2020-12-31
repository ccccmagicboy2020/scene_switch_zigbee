#include "Include.h"

/**	
  * @说明  	UART写字符串函数
  *	@参数	*fucp_Str ：写入的数据首地址
  * @返回值 无
  * @注		无
  */
void UART_Send_Str(unsigned char *fucp_Str)
{
	IE &=~ 0x10;				//失能UART1中断
    while(*fucp_Str!='\0')
    {
        SBUF = *fucp_Str;
        while(!(SCON & 0x02));
		SCON &=~ 0x02;			//清除发送中断标志位
        fucp_Str++;
    }
	SBUF = 0x0D;
	while(!(SCON & 0x02));
	SCON &=~ 0x02;			//清除发送中断标志位
	SBUF = 0x0A;
	while(!(SCON & 0x02));
	SCON &=~ 0x02;			//清除发送中断标志位
    IE |= 0x10;					//UART1中断使能
	SCON |= 0x10;				//UART1接收使能
}