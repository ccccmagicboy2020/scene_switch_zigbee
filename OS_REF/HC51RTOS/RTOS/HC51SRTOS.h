/*****************************************************************************************  
 *  HC51sRTOS 总体头文件
 * 
 *  Copyright (C) 2019  OKWEI 黄少
 *  QQ:1926929399
 *  2019-08-08   HC51sRTOS 
 *****************************************************************************************/

#include "CPU_CFG.h"

			
/*--------------------------------------------------------------------------------------+
|系统函数声明																			|
+---------------------------------------------------------------------------------------*/
void RTOS_TaskCreate(void (*Task)(void),STACK_TypeDef *Stack,PRIO_TypeDef Prio);//创建任务
void RTOS_Start(void);					//系统启动函数

#if(TASK_SUSPEND_EN==1)
void RTOS_TaskResume(PRIO_TypeDef Prio);	 	//任务恢复函数
void RTOS_TaskSuspend(PIRO_TypeDef Prio);		//任务挂起函数
#endif

void OSSched(void);						//汇编调度函数
void RTOS_TimeDelay(TICKS_TypeDef ticks); 		//任务延时函数
void Idle_Task(void);
void RTOS_Init(void);					//系统初始化
void TickIntHook(void); 				//系统节拍钩子函数




