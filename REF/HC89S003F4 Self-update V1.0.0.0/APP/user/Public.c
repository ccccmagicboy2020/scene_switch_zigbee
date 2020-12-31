#include "Include.h"

void SystemInit(void)
{
	WDTCCR = 0x00;						//�رտ��Ź�
	CLKSWR = 0x51;						//ѡ���ڲ���ƵRCΪϵͳʱ�ӣ��ڲ���ƵRC 2��Ƶ��Fosc=16MHz
	CLKDIV = 0x01;						//Fosc 1��Ƶ�õ�Fcpu��Fcpu=16MHz 
	
	P2M0 = P2M0&0x0F|0x80;				//P21����Ϊ�������
	P0M1 = P0M1&0x0F|0x20;				//P03����Ϊ��������
	TXD_MAP = 0x21;						//TXDӳ��P21
	RXD_MAP = 0x03;						//RXDӳ��P03		
	T4CON = 0x06;						//T4����ģʽ��UART1�����ʷ�����
    TH4 = 0xFF;
	TL4 = 0x98;							//������9600
	SCON2 = 0x02;						//8λUART�������ʿɱ�
	SCON = 0x10;						//�����н���
	IE |= 0X10;							//ʹ�ܴ����ж�
	
	P1M0 = P1M0&0xF0|0x08;				//P10����Ϊ�������
	
	EA = 1;
}

/**
  * @˵��  	д����ҪУ������ݲ�����У����
  * @����  	*fucp_CheckArr : CRCУ�������׵�ַ
  * @����  	fui_CheckLen : CRCУ�����ݳ���
  *         ȡֵ��Χ 0 - 65535
  * @����ֵ CRCУ������
  			��λ��ǰ����λ�ں�
  * @ע		��
  */
unsigned int CRC_CalcCRC(unsigned char *fucp_CheckArr,unsigned int fui_CheckLen)
{
	while(fui_CheckLen--)CRCL = *(fucp_CheckArr++);
	return CRCR;
}

/**
  * @˵��  	��λ�ض�����ѡ��
  * @����  	��
  * @����ֵ ��
  * @ע		��
  */
void ResetReadOption(void)
{
	IAP_CMD = 0xF00F;
	IAP_CMD = 0x7887;
}


