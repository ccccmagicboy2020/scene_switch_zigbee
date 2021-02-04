/*****************************************************************************************  
 *  HC51sRTOS ����ͷ�ļ�
 * 
 *  Copyright (C) 2019  OKWEI ����
 *  QQ:1926929399
 *  2019-08-08   HC51sRTOS 
 *****************************************************************************************/

#include "CPU_CFG.h"

			
/*--------------------------------------------------------------------------------------+
|ϵͳ��������																			|
+---------------------------------------------------------------------------------------*/
void RTOS_TaskCreate(void (*Task)(void),STACK_TypeDef *Stack,PRIO_TypeDef Prio);//��������
void RTOS_Start(void);					//ϵͳ��������

#if(TASK_SUSPEND_EN==1)
void RTOS_TaskResume(PRIO_TypeDef Prio);	 	//����ָ�����
void RTOS_TaskSuspend(PIRO_TypeDef Prio);		//���������
#endif

void OSSched(void);						//�����Ⱥ���
void RTOS_TimeDelay(TICKS_TypeDef ticks); 		//������ʱ����
void Idle_Task(void);
void RTOS_Init(void);					//ϵͳ��ʼ��
void TickIntHook(void); 				//ϵͳ���Ĺ��Ӻ���




