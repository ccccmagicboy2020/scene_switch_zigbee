/*****************************************************************************************  
 *  DOME文件
 *
 *  Copyright (C) 2019  OKWEI 黄少
 *  QQ:1926929399
 *  2019-08-08   HC51sRTOS 
 *****************************************************************************************/

#include "HC51SRTOS.h"
#include "math.h"
//#define LED_RED 	P0_0		// 将P20设置为绿灯
sbit LED_RED     = P0^0;
sbit LED_GREEN     = P0^1;
/******************定义任务的堆栈***************/
INT8U Task0Stack[20];
INT8U Task1Stack[20];
INT8U Task2Stack[20];
//INT8U Task3Stack[17];
/******************系统节拍初始化***************/
	 void TimerInit(void)
{
	  TMOD=0x01;
    TH0=0xf9;
    TL0=0xfe;
    EA=1;
    ET0=1;
}
/******************任务Task0定义***************/
void Task0(void)
{
 INT8U asi=0;
 TimerInit();
 
 while(1)
 {
   LED_RED =1;
   RTOS_TimeDelay(1000);
	 LED_RED =0;
   RTOS_TimeDelay(1000);
 }
}
///******************任务Task1定义***************/
void Task1(void)
{

 while(1)
 {
   LED_GREEN=1;
   RTOS_TimeDelay(400);
	 LED_GREEN=2;
   RTOS_TimeDelay(400);

 }
}

///******************任务Task2定义***************/
//void Task2(void)
//{

// while(1)
// {
//   P2++;
//   RTOS_TimeDelay(400);

// }
//}

///******************任务Task3定义***************/
//void Task3(void)
//{

// while(1)
// {
//   P3++;
//   RTOS_TimeDelay(200);

// }
//}

/****************主函数***************/
int main(void)
{
	 WDTCCR = 0x00;						//关闭看门狗
		                                //本例程为方便测试关闭看门狗，实际使用中，建议客户打开看门狗，详见WDT复位例程
	 CLKSWR = 0x51;						//选择内部高频RC为系统时钟，内部高频RC 2分频，Fosc=16MHz
	 CLKDIV = 0x01;						//Fosc 1分频得到Fcpu，Fcpu=16MHz 
	 P0M0 =0x88;		//将P00、01设置为推完输出		
	 LED_RED =1;
	 LED_GREEN =0;
	 RTOS_Init();
	 RTOS_TaskCreate(Task0,Task0Stack,0);
// RTOS_TaskCreate(Task1,Task1Stack,1);
//	 RTOS_TaskCreate(Task2,Task2Stack,2);
	// RTOS_TaskCreate(Task3,Task3Stack,3);
	 RTOS_Start();	 //此函数执行完系统即发生调度
	 while(1);
}