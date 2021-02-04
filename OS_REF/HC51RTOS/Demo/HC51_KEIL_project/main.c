/*****************************************************************************************  
 *  DOME�ļ�
 *
 *  Copyright (C) 2019  OKWEI ����
 *  QQ:1926929399
 *  2019-08-08   HC51sRTOS 
 *****************************************************************************************/

#include "HC51SRTOS.h"
#include "math.h"
//#define LED_RED 	P0_0		// ��P20����Ϊ�̵�
sbit LED_RED     = P0^0;
sbit LED_GREEN     = P0^1;
/******************��������Ķ�ջ***************/
INT8U Task0Stack[20];
INT8U Task1Stack[20];
INT8U Task2Stack[20];
//INT8U Task3Stack[17];
/******************ϵͳ���ĳ�ʼ��***************/
	 void TimerInit(void)
{
	  TMOD=0x01;
    TH0=0xf9;
    TL0=0xfe;
    EA=1;
    ET0=1;
}
/******************����Task0����***************/
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
///******************����Task1����***************/
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

///******************����Task2����***************/
//void Task2(void)
//{

// while(1)
// {
//   P2++;
//   RTOS_TimeDelay(400);

// }
//}

///******************����Task3����***************/
//void Task3(void)
//{

// while(1)
// {
//   P3++;
//   RTOS_TimeDelay(200);

// }
//}

/****************������***************/
int main(void)
{
	 WDTCCR = 0x00;						//�رտ��Ź�
		                                //������Ϊ������Թرտ��Ź���ʵ��ʹ���У�����ͻ��򿪿��Ź������WDT��λ����
	 CLKSWR = 0x51;						//ѡ���ڲ���ƵRCΪϵͳʱ�ӣ��ڲ���ƵRC 2��Ƶ��Fosc=16MHz
	 CLKDIV = 0x01;						//Fosc 1��Ƶ�õ�Fcpu��Fcpu=16MHz 
	 P0M0 =0x88;		//��P00��01����Ϊ�������		
	 LED_RED =1;
	 LED_GREEN =0;
	 RTOS_Init();
	 RTOS_TaskCreate(Task0,Task0Stack,0);
// RTOS_TaskCreate(Task1,Task1Stack,1);
//	 RTOS_TaskCreate(Task2,Task2Stack,2);
	// RTOS_TaskCreate(Task3,Task3Stack,3);
	 RTOS_Start();	 //�˺���ִ����ϵͳ����������
	 while(1);
}