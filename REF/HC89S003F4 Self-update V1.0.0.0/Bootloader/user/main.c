/**
*   ************************************************************************************
*								�Ϻ�оʥ���ӹɷ����޹�˾
*								    www.holychip.cn
*	************************************************************************************
*	@Examle Version		V1.0.1.0
*	@Demo 	Version		V1.0.3.0
*	@Date				2018.5.7
*	************************************************************************************
*  								       �ͻ�����
*	��л��ʹ�����ǵĵ�Ƭ���������ִ����Ժ�����ʹ�ô������ʡ�������Ϻ�оʥ���ӹٷ�QQȺ
*	****************************����֧��Ⱥ��201030494***********************************
*   ************************************************************************************
**/

#define	ALLOCATE_EXTERN
#include "Include.h"

unsigned char xdata guc_UartReceiveBuf_a[135] = {0x00};//���ݽ���BUF
unsigned char guc_Uartcnt = 0;			//�������ݼ���
unsigned char guc_UartMode = 0;			//��ǰģʽ
unsigned char guc_UartFlag = 0;			//���ڽ�����ɱ�־λ
unsigned int  gui_CRCValue = 0x00;		//CRCУ��ֵ
unsigned int  gui_SectorAdd = 0x00;		//������ַ 

unsigned char code guc_OK_One_a[] = "The current mode is ISP mode.\0";	//��ǰģʽΪISPģʽ
unsigned char code guc_OK_Two_a[] = "Sector wipe success.\0";		//���������ɹ�
unsigned char code guc_OK_Three_a[] = "Data write success.\0";		//����д��ɹ�

unsigned char code guc_Error_One_a[] = "Sorry,FH Error\0";		//����֡ͷ����
unsigned char code guc_Error_Two_a[] = "Sorry,CRC Error\0";		//CRCУ�����
unsigned char code guc_Error_Three_a[] = "Sorry,Directive Error\0";	//ָ�����


void main()
{
	SystemInit();
	if(RSTFR&0x80)						//������ϵ縴λ
	{
		RSTFR = 0x00;					//�����λ��־
		ResetNoReadOption();			//��λ���ض�����ѡ�������APP����
	}
	else if(RSTFR&0x08)					//����������λ
	{
		UART_Send_Str(guc_OK_One_a);	//���͵�ǰģʽ��ʾ
	} 
	while(1)
	{
		if(guc_UartMode == 0)
		{
			P2_2 = 0;
		}			
		if(guc_UartFlag)
		{
			P2_2 = 1;
			switch(guc_UartMode)
			{
				case 1 : 							//���ݴ���ģʽ
					UART_Send_Str(guc_Error_One_a);	//���ʹ�����ʾ
					guc_UartMode = 0;				//���صȴ�ģʽ			
					guc_Uartcnt = 0;				//����ֵ��0
					memset(guc_UartReceiveBuf_a,0,135);//�������
					guc_UartFlag = 0;				//��־λ����
					break;
				case 2 :							//CRCУ��
					CRCC = 0x07;					//CRC��λ��LSB first����λ��ֵΪ0xFFFF
					gui_CRCValue = CRC_CalcCRC(guc_UartReceiveBuf_a,guc_UartReceiveBuf_a[1]-2);			
					if(((gui_CRCValue/256)==guc_UartReceiveBuf_a[guc_UartReceiveBuf_a[1]-2])&&
						((gui_CRCValue%256)==guc_UartReceiveBuf_a[guc_UartReceiveBuf_a[1]-1]))
					{
						guc_UartMode = 3;			//CRCУ��ɹ�
					}
					else{guc_UartMode = 4;}			//CRCУ�����
					break;
				case 3 :		
					if(guc_UartReceiveBuf_a[2] == 0x02)
					{
						gui_SectorAdd = (guc_UartReceiveBuf_a[3]*256) + guc_UartReceiveBuf_a[4];
						Flash_EraseBlock(gui_SectorAdd);
						UART_Send_Str(guc_OK_Two_a);
						Flash_WriteArr(gui_SectorAdd,guc_UartReceiveBuf_a[1]-7,&guc_UartReceiveBuf_a[5]);
						UART_Send_Str(guc_OK_Three_a);
						guc_UartMode = 0;				//���صȴ�ģʽ
						guc_Uartcnt = 0;				//����ֵ��0
						memset(guc_UartReceiveBuf_a,0,135);//�������
						guc_UartFlag = 0;				//��־λ����	
					}	
					else{guc_UartMode = 5;}			//ָ�����
					break;
				case 4 :							//CRCУ�����ģʽ
					UART_Send_Str(guc_Error_Two_a);	//���ʹ�����ʾ	
					guc_UartMode = 0;				//���صȴ�ģʽ
					guc_Uartcnt = 0;				//����ֵ��0
					memset(guc_UartReceiveBuf_a,0,135);//�������
					guc_UartFlag = 0;				//��־λ����											
					break;
				case 5 :							//ָ�����ģʽ
					UART_Send_Str(guc_Error_Three_a);//���ʹ�����ʾ	
					guc_UartMode = 0;				//���صȴ�ģʽ
					guc_Uartcnt = 0;				//����ֵ��0
					memset(guc_UartReceiveBuf_a,0,135);//�������
					guc_UartFlag = 0;				//��־λ����											
					break;				
				default : 
					guc_UartMode = 0;				//���صȵ�ģʽ
					guc_Uartcnt = 0;				//����ֵ��0
					memset(guc_UartReceiveBuf_a,0,135);//�������
					guc_UartFlag = 0;				//��־λ����						
					break;
			}				
		}		

	}
}

/***************************************************************************************
  * @˵��  	UART1�жϷ�����
  *	@����	��
  * @����ֵ ��
  * @ע		��
***************************************************************************************/
void UART1_Rpt(void) interrupt UART1_VECTOR
{
	if(SCON & 0x01)						//�жϽ����жϱ�־λ
	{
		guc_UartReceiveBuf_a[guc_Uartcnt++] = SBUF;//ת��8λ���ڽ�������
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
		SCON &=~ 0x01;					//��������жϱ�־λ
	}									
}