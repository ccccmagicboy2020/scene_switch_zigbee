#define ALLOCATE_EXTERN
#include "HC89S003F4.h"
#include "zigbee.h"

volatile ulong Timer_Counter = 0;

volatile ulong free_timer = 0;
ulong free_s_timer = 0;
unsigned char second_flag = 0;
unsigned char upload_disable = 0;

void Flash_EraseBlock(unsigned int fui_Address); //��������
void FLASH_WriteData(unsigned char fuc_SaveData, unsigned int fui_Address);
void Delay_ms(unsigned int t);
void Delay_us(unsigned int q1);

void Delay_ms(unsigned int t)
{
	Timer_Counter = 0;
	while (Timer_Counter < t)
	{
		WDTC |= 0x10; //�忴�Ź�
	}
}

void Delay_us(unsigned int q1)
{
	unsigned int j;
	for (j = 0; j < q1; j++)
	{
		;
	}
}

/***************************************************************************************
  * @˵��  	ϵͳ��ʼ������
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/
void InitSYS()
{
	/********************************ϵͳƵ�ʳ�ʼ��***************************************/

	CLKSWR = 0x51;	 //ѡ���ڲ���ƵRCΪϵͳʱ�ӣ��ڲ���ƵRC 2��Ƶ��Fosc=16MHz
	CLKDIV = 0x01;	 //Fosc 1��Ƶ�õ�Fcpu��Fcpu=16MHz
	FREQ_CLK = 0x10; //IAPƵ��

	/**********************************��ѹ��λ��ʼ��**************************************/

	//	BORC = 0xC0;											 //ʹ�ܵ�ѹ��λ1.8V��������ʹ��
	//	BORDBC = 0x01;										 //����ʱ��BORDBC*8TCPU+2TCPU

	/***********************************���ſڳ�ʼ��***************************************/
	WDTC = 0x5F;   //����WDT��λ������ģʽ�½�ֹWDT��ѡ��1024��Ƶ���ڲ���Ƶʱ��44K��
	WDTCCR = 0X20; //0X20/44	=0.73��						//0xFF;	 //���ʱ��Լ6��
				   //�������ʱ��=��WDT��Ƶϵ��*��WDTCCR+1����/�ڲ���ƵRCƵ��
}

/***************************************************************************************
  * @˵��  	��ʱ����ʼ������
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/
void Timer_Init()
{
	/**********************************TIM1���ó�ʼ��**************************************/
	TCON1 = 0x00; //T1��ʱ��ʱ��ΪFosc
	TMOD = 0x01;  //T1-16λ��װ�ض�ʱ��/������,T0-16λ��ʱ��

	//Tim1����ʱ�� 	= (65536 - 0xFACB) * (1 / (Fosc /Timer��Ƶϵ��))
	//				= 1333 / (16000000 / 12)
	//				= 1 ms

	//T1��ʱ1ms
	//���Ƴ�ֵ 	= 65536 - ((1/1000) / (1/(Fosc / Timer��Ƶϵ��)))
	//		   	= 65536 - ((1/1000) / (1/(16000000 / 12)))
	//			= 65536 - 1333
	//			= 0xFACB

	TH1 = 0xFA;
	TL1 = 0xCB;	  //T1��ʱ1ms
	IE |= 0x08;	  //��T1�ж�
	TCON |= 0x40; //ʹ��T1

	TH0 = 0xCB;
	TL0 = 0xEB; //T0��ʱʱ��10ms

	TCON |= 0x10; //ʹ��T0
}

/***************************************************************************************
  * @˵��  	UART1��ʼ������
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/
void UART1_Init()
{
	/**********************************UART���ó�ʼ��**************************************/
	P2M0 = P2M0 & 0xF0 | 0x08; //P20����Ϊ�������
	P0M2 = P0M2 & 0xF0 | 0x02; //P04����Ϊ��������
	P0_4 = 1;
	TXD_MAP = 0x20; //TXDӳ��P20
	RXD_MAP = 0x04; //RXDӳ��P04
	T4CON = 0x06;	//T4����ģʽ��UART1�����ʷ�����

	//�����ʼ���
	//������ = 1/16 * (T4ʱ��ԴƵ�� / ��ʱ��4Ԥ��Ƶ��) / (65536 - 0xFF98)
	//       = 1/16 * ((16000000 / 1) / 104)
	//		 = 9615.38(���0.16%)

	//������9600
	//���Ƴ�ֵ = (65536 - ((T4ʱ��ԴƵ�� / ��ʱ��4Ԥ��Ƶ��) * (1 / 16)) / ������)
	//		   = (65536 - (16000000 * (1 / 16) / 9600))
	//		   = (65536 - 104.167)
	//         = FF98
	//0xFF98->9600
	//0xFFCC->19200
	//0xFFEF->57600

	TH4 = 0xFF;
	TL4 = 0x98;	  //������9600		//0xEE;				//������56000
	SCON2 = 0x02; //8λUART�������ʿɱ�
	SCON = 0x10;  //�����н���
	IE |= 0X10;	  //ʹ�ܴ����ж�
				  //EA = 1;							              	 //ʹ�����ж�
}

void send_data(u8 d)
{
	SBUF = d;
	while (!(SCON & 0x02))
		;
	SCON &= ~0x02;
}

/***************************************************************************************
  * @˵��  	������
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/
void main()
{
	zigbee_protocol_init(); //mcu_sdk
	InitSYS();
	Timer_Init();
	UART1_Init();

	LVDC = 0xAA; //LVD����2.4V,��ֹ�ж�
	LVDDBC = 0xFF; //��������ʱ��
	LVDC &= ~0x08; //���LVD�жϱ�־λ
				   //
	EA = 1;
	Delay_ms(500);
	
	upload_disable = 0;
	mcu_network_start();
	
	my_memset(&ota_fw_info, 0, sizeof(ota_fw_info));
	while (1)
	{
		WDTC |= 0x10; //�忴�Ź�
		Delay_ms(2);
		zigbee_uart_service();
		
		if (second_flag)
		{
			second_flag = 0;
			if (upload_disable == 0)
			{
				free_s_timer++;
				mcu_dp_value_update(DPID_FREE_TIMER, free_s_timer);
			}
		}
	}
}

/***************************************************************************************
  * @˵��  	T1�жϷ�����
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/
void TIMER1_Rpt(void) interrupt TIMER1_VECTOR
{
	Timer_Counter++;
	
	if (free_timer > 2000)
	{
		free_timer = 0;
		second_flag = 1;
	}
	else
	{
		free_timer++;
	}
}

/***************************************************************************************
  * @˵��  	UART1�жϷ�����
  *	@����	  ��
  * @����ֵ ��
  * @ע		  ��
***************************************************************************************/

void UART1_Rpt(void) interrupt UART1_VECTOR
{
	u8 i;
	//u16 t;

	if (SCON & 0x01) //�жϽ����жϱ�־λ
	{
		i = SBUF;
		uart_receive_input(i); //mcu_sdk
		SCON &= ~0x01;		   //��������жϱ�־λ
		EA = 1;
	}
}

#pragma disable
void Flash_EraseBlock(unsigned int fui_Address)
{
	while (1)
	{
		LVDC &= ~0x08; //���LVD�жϱ�־λ
		if ((LVDC & 0x08) == 0)
			break;
	}
	EA = 0;
	IAP_CMD = 0xF00F;		//Flash����
	IAP_ADDR = fui_Address; //д�������ַ
	IAP_CMD = 0xD22D;		//ѡ�������ʽ�� ��������
	IAP_CMD = 0xE11E;		//������IAP_ADDRL&IAP_ADDRHָ��0xFF��ͬʱ�Զ�����
							//EA=1;
}

/**
  * @˵��  	д��һ���ֽ����ݵ�Flash����
  *         �ú�������Ե�ַ���룬���������IAP����Ӧ���ֲ�
  * @����  	fui_Address ��FLASH��ַ
  *	@����	  fucp_SaveData��д�������
  * @����ֵ ��
  * @ע		  д֮ǰ�����ȶԲ������������в���
  */
#pragma disable
void FLASH_WriteData(unsigned char fuc_SaveData, unsigned int fui_Address)
{
	while (1)
	{
		LVDC &= ~0x08; //���LVD�жϱ�־λ
		if ((LVDC & 0x08) == 0)
			break;
	}
	EA = 0;
	IAP_DATA = fuc_SaveData;
	IAP_CMD = 0xF00F; //Flash����
	IAP_ADDR = fui_Address;
	IAP_CMD = 0xB44B; //�ֽڱ��
	IAP_CMD = 0xE11E; //����һ�β���
					  //EA=1;
}
