#include "Include.h"

/**	
  * @˵��  	UARTд�ַ�������
  *	@����	*fucp_Str ��д��������׵�ַ
  * @����ֵ ��
  * @ע		��
  */
void UART_Send_Str(unsigned char *fucp_Str)
{
	IE &=~ 0x10;				//ʧ��UART1�ж�
    while(*fucp_Str!='\0')
    {
        SBUF = *fucp_Str;
        while(!(SCON & 0x02));
		SCON &=~ 0x02;			//��������жϱ�־λ
        fucp_Str++;
    }
	SBUF = 0x0D;
	while(!(SCON & 0x02));
	SCON &=~ 0x02;			//��������жϱ�־λ
	SBUF = 0x0A;
	while(!(SCON & 0x02));
	SCON &=~ 0x02;			//��������жϱ�־λ
    IE |= 0x10;					//UART1�ж�ʹ��
	SCON |= 0x10;				//UART1����ʹ��
}