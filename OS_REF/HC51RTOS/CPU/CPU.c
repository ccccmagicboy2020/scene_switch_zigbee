/*****************************************************************************************  
 *  8051 CPU���C�ļ�
 *
 *  Copyright (C) 2019  OKWEI ����
 *  QQ:1926929399
 *  2019-08-08   HC51sRTOS 
 *****************************************************************************************/

#include "HC51SRTOS.h"

/******************ȫ�ֱ�������***************/
  INT8U   StackIdle[StackSizeIdle]; 				// ������������ջ
  INT8U   OSRdyTbl;     							// ���������б�
  INT8U   OSPrioCur;		            			// ��ǰ��������ȼ�
  INT8U   OSPrioHighRdy	;           		    	// ����Ҫ������������ȼ�	
  INT8U  IdleCount;								// ���������Լ���
  struct  TaskCtrBlock TCB[OS_TASKS + 1];			// ����������ƿ�TCB����


void  CPU_TaskCreate(void (*p_Task)(void),INT8U *p_Stack,INT8U t_Prio)
	{
		*(  p_Stack) = (unsigned int)p_Task & 0xff;	// �������
		*(++p_Stack) = (unsigned int)p_Task>>8;    	// �������
		*(++p_Stack) = 0;//ACC    
		*(++p_Stack) = 0;//B   
		*(++p_Stack) = 0;//DPH   
		*(++p_Stack) = 0;//DPL   
		*(++p_Stack) = 0;//PSW    
		*(++p_Stack) = 0;//R0   		
		*(++p_Stack) = 1;//R1    
		*(++p_Stack) = 2;//R2   
		*(++p_Stack) = 3;//R3   
		*(++p_Stack) = 4;//R4   
		*(++p_Stack) = 5;//R5    
		*(++p_Stack) = 6;//R6 
		*(++p_Stack) = 7;//R7					  	// ��pc�Ᵽ��13���Ĵ���		                      
		TCB[t_Prio].OSTCBStkPtr = (INT8U)p_Stack;	// ���˹���ջ��ջ�������浽��ջ��������
		TCB[t_Prio].OSTCBDly = 0;				  	// ��ʼ��������ʱ
	}
void CPU_OSStart(void)
   {
   CPU_TaskCreate(Idle_Task,StackIdle,OS_TASKS);
   SP=TCB[OSPrioHighRdy].OSTCBStkPtr-13;			// ��ջ���Ƶ�pcλ��,׼������
   TR0=1;
   }
   
	