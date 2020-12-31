/**
*   ************************************************************************************
*								上海芯圣电子股份有限公司
*								    www.holychip.cn
*	************************************************************************************
*	@Examle Version		V1.0.1.0
*	@Demo 	Version		V1.0.3.0
*	@Date				2018.5.7
*	************************************************************************************
*  								       客户服务
*	感谢您使用我们的单片机，若发现错误或对函数的使用存在疑问。请添加上海芯圣电子官方QQ群
*	****************************技术支持群：201030494***********************************
*   ************************************************************************************
**/

#define	ALLOCATE_EXTERN
#include "Include.h"

unsigned char xdata guc_UartReceiveBuf_a[135] = {0x00};//数据接收BUF
unsigned char guc_Uartcnt = 0;			//接收数据计数
unsigned char guc_UartMode = 0;			//当前模式
unsigned char guc_UartFlag = 0;			//串口接收完成标志位
unsigned int gui_CRCValue = 0x00;		//CRC校验值

unsigned char code guc_Error_One_a[] = "Sorry,FH Error\0";		//数据帧头错误
unsigned char code guc_Error_Two_a[] = "Sorry,CRC Error\0";		//CRC校验错误
unsigned char code guc_Error_Three_a[] = "Sorry,Directive Error\0";	//指令错误
unsigned char code guc_OK_One_a[] = "Entering ISP Mode...\0";	//正在进入ISP模式

void main()
{
	SystemInit();
	while(1)
	{
		if(guc_UartMode == 0)
		{
			P1_0 = 0;
		}		
		if(guc_UartFlag)
		{
			P1_0 = 1;
			switch(guc_UartMode)
			{
				case 1 : 							//数据错误模式
					UART_Send_Str(guc_Error_One_a);	//发送错误提示
					guc_UartMode = 0;				//返回等待模式			
					guc_Uartcnt = 0;				//计数值清0
					memset(guc_UartReceiveBuf_a,0,135);//清除数据
					guc_UartFlag = 0;				//标志位清零
					break;
				case 2 :							//CRC校验
					CRCC = 0x07;					//CRC复位，LSB first，复位初值为0xFFFF
					gui_CRCValue = CRC_CalcCRC(guc_UartReceiveBuf_a,guc_UartReceiveBuf_a[1]-2);			
					if(((gui_CRCValue/256)==guc_UartReceiveBuf_a[guc_UartReceiveBuf_a[1]-2])&&
						((gui_CRCValue%256)==guc_UartReceiveBuf_a[guc_UartReceiveBuf_a[1]-1]))
					{
						guc_UartMode = 3;			//CRC校验成功
					}
					else{guc_UartMode = 4;}			//CRC校验错误
					break;
				case 3 :							//握手模式
					if(guc_UartReceiveBuf_a[2] == 0x01)
					{
						UART_Send_Str(guc_OK_One_a);
						ResetReadOption();
					}
					else{guc_UartMode = 5;}			//指令错误
					break;
				case 4 :							//CRC校验错误模式
					UART_Send_Str(guc_Error_Two_a);	//发送错误提示	
					guc_UartMode = 0;				//返回等待模式
					guc_Uartcnt = 0;				//计数值清0
					memset(guc_UartReceiveBuf_a,0,135);//清除数据
					guc_UartFlag = 0;				//标志位清零				
					break;
				case 5 :							//指令错误模式
					UART_Send_Str(guc_Error_Three_a);//发送错误提示	
					guc_UartMode = 0;				//返回等待模式
					guc_Uartcnt = 0;				//计数值清0
					memset(guc_UartReceiveBuf_a,0,135);//清除数据
					guc_UartFlag = 0;				//标志位清零				
					break;				
				default : 
					guc_UartMode = 0;				//返回等到模式
					guc_Uartcnt = 0;				//计数值清0
					memset(guc_UartReceiveBuf_a,0,135);//清除数据
					guc_UartFlag = 0;				//标志位清零						
					break;
			}				
		}
	}
}

/***************************************************************************************
  * @说明  	UART1中断服务函数
  *	@参数	无
  * @返回值 无
  * @注		无
***************************************************************************************/
void UART1_Rpt(void) interrupt UART1_VECTOR
{
	if(SCON & 0x01)						//判断接收中断标志位
	{
		guc_UartReceiveBuf_a[guc_Uartcnt++] = SBUF;//转存8位串口接收数据
		if((guc_UartReceiveBuf_a[0] == 0xAA)&&(guc_Uartcnt >= 2))
		{
			if(guc_Uartcnt >= guc_UartReceiveBuf_a[1])
			{
				SCON &=~ 0x10;		
				guc_UartMode = 2;
				guc_UartFlag = 1;	
			}
		}
		else if(guc_UartReceiveBuf_a[0] != 0xAA)
		{	
			SCON &=~ 0x10;	
			guc_UartFlag = 1;
			guc_UartMode = 1;
		}
		SCON &=~ 0x01;					//清除接收中断标志位
	}									
}

