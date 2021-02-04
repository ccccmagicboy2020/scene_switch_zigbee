; /*****************************************************************************************  
;  *  8051 CPU相关ASM文件
;  *
;  *  Copyright (C) 2019  OKWEI 黄少
;  *  QQ:1926929399
;  *  2019-08-08   HC51sRTOS 
;  *****************************************************************************************/
$NOMOD51
;#include "REG52.INC"
EA	BIT		0A8H.7
SP	DATA	081H
B	DATA	0F0H
ACC	DATA	0E0H
DPH	DATA	083H
DPL	DATA	082H
PSW	DATA	0D0H
TR0	BIT		088H.4
TH0	DATA	08CH
TL0	DATA	08AH

NAME	CPU_ASM

;定义重定位段
?PR?OSSched?CPU_ASM    SEGMENT CODE 
?PR?Timer0ISR?CPU_ASM  SEGMENT CODE 

	EXTRN	CODE (TickIntHook)
	EXTRN	DATA (TCB)
	EXTRN	DATA (OSPrioCur)
	EXTRN	DATA (OSPrioHighRdy)
;对外声明2个不可重入函数
	PUBLIC	Timer0ISR
	PUBLIC	OSSched

	RSEG  ?PR?OSSched?CPU_ASM
OSSched:
	USING	0
	CLR  	EA
	MOV  	A,OSPrioHighRdy
	XRL  	A,OSPrioCur
	JZ   	?C0001
	 PUSH ACC        
	 PUSH B
	 PUSH DPH
	 PUSH DPL
	 PUSH PSW
	 PUSH AR0	      
	 PUSH AR1
	 PUSH AR2
	 PUSH AR3
	 PUSH AR4
	 PUSH AR5
	 PUSH AR6   
	 PUSH AR7   

	MOV  	A,OSPrioCur
	MOV  	B,#03H
	MUL  	AB
	ADD  	A,#LOW (TCB)
	MOV  	R0,A
	MOV  	@R0,SP

	MOV  	OSPrioCur,OSPrioHighRdy

	MOV  	A,OSPrioCur
	MOV  	B,#03H
	MUL  	AB
	ADD  	A,#LOW (TCB)
	MOV  	R0,A
	MOV  	A,@R0
	MOV  	SP,A
	 POP AR7
	 POP AR6
	 POP AR5
	 POP AR4
	 POP AR3
	 POP AR2
	 POP AR1
	 POP AR0
	 POP PSW
	 POP DPL
	 POP DPH
	 POP B
	 POP ACC
?C0001:
	SETB 	EA
	RET 




; /*
; +---------------------------------------------------------------------------------------+
; |函数名称：	Timer0ISR																	|
; |---------------------------------------------------------------------------------------|
; |函数原型：	void Timer0ISR(prio)														|
; |																						|
; |函数功能：	中断级切换函数 														   		|																						|
; +---------------------------------------------------------------------------------------+
; */

CSEG	AT	0000BH
	LJMP	Timer0ISR 

	RSEG  ?PR?Timer0ISR?CPU_ASM
	USING	0
Timer0ISR:
	PUSH 	ACC
	PUSH 	B
	PUSH 	DPH
	PUSH 	DPL
	PUSH 	PSW
	USING	0
	MOV  	PSW,#00H
	CLR  	EA
	MOV  	TH0,#0FEH
	MOV  	TL0,#00H
	CLR  	TR0

	 PUSH AR0	      
	 PUSH AR1
	 PUSH AR2
	 PUSH AR3
	 PUSH AR4
	 PUSH AR5
	 PUSH AR6   
	 PUSH AR7 
	LCALL	TickIntHook
	MOV  	A,OSPrioCur
	MOV  	B,#03H
	MUL  	AB
	ADD  	A,#LOW (TCB)
	MOV  	R0,A
	MOV  	@R0,SP

	MOV  	OSPrioCur,OSPrioHighRdy

	MOV  	A,OSPrioCur
	MOV  	B,#03H
	MUL  	AB
	ADD  	A,#LOW (TCB)
	MOV  	R0,A
	MOV  	A,@R0
	MOV  	SP,A

	 POP AR7
	 POP AR6
	 POP AR5
	 POP AR4
	 POP AR3
	 POP AR2
	 POP AR1
	 POP AR0
	SETB 	TR0
	SETB 	EA
	POP  	PSW
	POP  	DPL
	POP  	DPH
	POP  	B
	POP  	ACC
	RETI 

	END
