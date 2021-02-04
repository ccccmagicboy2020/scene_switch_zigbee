#include "include.h"

unsigned char  xdata  ID_DAT[2]={0};
/*************************************************************
  Function   : CRC_FLASH_BLANK 
  Description: add ��ַ  Size_Flash ����
  Input      : guc_Uartbuf-���͵��ֽ�      
  return     : 0  !0    
*************************************************************/
unsigned char  XOR_FLASH_BLANK(unsigned int Add,unsigned int Size_Flash)
{
	unsigned int  i=0;
	WDTC |= 0x10;                   //�幷
	while(Size_Flash--)
	if(*(unsigned char code *)(Add++)) return 1;
	return 0;
}
/*********************************
	�������ƣ�IAP�����������
	�������ܣ�ȫ��������										   
*********************************/
void IAR_Clear_arrang(unsigned int clradd,unsigned int len)
{
	unsigned int i;
	WDTC |= 0x10;                   //�幷	
	for(i=0;i<=len;i++)
	{
		if(clradd>=0x3000) 			return;	
		if(XOR_FLASH_BLANK(clradd,128))		IAR_Clear(clradd);
		clradd=clradd+0x80;
	}
}
/*********************************
	�������ƣ� Earse_Flash(void)����	
	�������ܣ� ����Flash ǰ12K						   
*********************************/
unsigned char Earse_Flash(void)
{//003 16K 0000~3fff (����ǰ12K)
	IAR_Clear_arrang(0x0000,32);	
	IAR_Clear_arrang(0x1000,32);
	IAR_Clear_arrang(0x2000,32);
	if(XOR_FLASH_BLANK(0x0000,0x2FFF)!=0x00)  return 0;
	else  return 1;						
}
/*********************************
	�������ƣ�IAP�����������
	�������ܣ����һ������������
			  ����������ַΪ��ǰ�����������ַ										   
*********************************/
void IAR_Clear(unsigned int clradd)
{	
	enable_timer(0);
	IAP_CMD = 0xF00F;
	IAP_ADDR = clradd;
	IAP_CMD = 0xD22D;
	IAP_CMD = 0xE11E;
	enable_timer(1);
}
/*********************************
	�������ƣ�FLASHдһ���ֽ�
	�������ܣ���FLASH����λ��д��һ���ֽڵ�����										   
*********************************/
void IAR_Write_Byte(unsigned int add,unsigned char datt)
{
	enable_timer(0);	
	IAP_DATA = datt;
	IAP_CMD=0xF00F;
	IAP_ADDR=add;
	IAP_CMD=0xB44B;
	IAP_CMD=0xE11E;
	enable_timer(1);
}

/*********************************
	�������ƣ�FLASHд����ֽ�
	�������ܣ�FLASH����λ�ÿ�ʼ��д���ⳤ�ȵ�����										   
*********************************/
void IAR_Write_arrang(unsigned int add,unsigned char *datt,unsigned int len)
{
	unsigned int i;
	for(i=0;i<len;i++)
	{	
		if(add>=0x3000) return; 	
		IAR_Write_Byte(add,datt[i] ^ MAGIC_KEY);
		add++;
	}
}

/*******************************************************
	�������ƣ�FLASH��������ֽ�
	�������ܣ�FLASH����λ�ÿ�ʼ�������ⳤ�ȵ�����										   
*******************************************************/
void IAR_Read(unsigned int add,unsigned char *buf,unsigned int len)
{
	while(len--)
	*(buf++)=*((unsigned char code *)(add++));//��ȡ����
}

/*******************************************************
	�������ƣ�FLASH�����λ
	�������ܣ����ض�Option									   
*******************************************************/
void IAR_Soft_Rst_No_Option(void)
{
	IAP_CMD = 0xF00F;
	IAP_CMD = 0x8778;
}
/*******************************************************
	�������ƣ�FLASH�����λ
	�������ܣ��ض�Option									   
*******************************************************/
//void IAR_Soft_Rst_Option(void)
//{
//	IAP_CMD = 0xF00F;
//	IAP_CMD = 0x7887;
//}

/*********************************
	�������ƣ� Read_ID(void)����	
	�������ܣ� ��ȡоƬ�ͺ�								   
*********************************/
#ifdef HOLYCHIP
bit Read_ID(void)
{
 	INSCON=0x10;
	IAR_Read(0x0120,ID_DAT,1);
	INSCON=0x00;

	if((ID_DAT[0])==Chip_ID)
	{   
        return SUCCESS;
	}
	else
	{	  
        return ERROR;
	}
}
#endif


