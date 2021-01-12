
#include "Hc89s_Config.h"

/*
*********************************************************************************************************
*	                                   ��������
*********************************************************************************************************
*/
static void SystemClock_Init(void);  

#if GPIO_INIT_ENABLE
static void GPIO_Init(void);
#endif

#if RESET_INIT_ENABLE
static void Reset_Init(void);
#endif

#if WDT_INIT_ENABLE
static void WDT_Init(void);
#endif

#if SYSTEM_EA_INIT_ENABLE
static void SystemEA_Init(void);
#endif
/*
*********************************************************************************************************
*	�� �� ��: SystemClock_Init
*	����˵��: ϵͳʱ�� ��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void SystemClock_Init(void)
{
/*************************************** �ڲ���Ƶ ���� ************************************************/	
   #if  CLOCK_IN_HIGH_ENABLE
		CLKSWR = CLOCK_IN_HIGH_CLKSWR_INIT;
		CLKDIV = CLOCK_IN_HIGH_CLKDIV_INIT;
/*************************************** �ڲ���Ƶ ���� ************************************************/		
	#elif CLOCK_IN_LOW_ENABLE
		while((CLKCON&0x10)!=0x10);
		CLKSWR &=~ 0x30;
		while((CLKSWR&0xC0)!=0x00);
		CLKCON &=~ 0x02;
		CLKDIV = CLOCK_IN_LOW_CLKDIV_INIT;
/*************************************** �ⲿ��Ƶ ���� ************************************************/		
	#elif CLOCK_OUT_HIGH_ENABLE
		CLKCON |= 0x04;	
		XTALCFG |= 0x01+CLOCK_OUT_HIGH_HXTAL_MODE_SEL_INIT+CLOCK_OUT_HIGH_HXTAL_CFG_INIT;
		while((CLKCON&0x80)!=0x80);	
		CLKSWR = 0xF0;
		while((CLKSWR&0xC0)!=0xC0);
		CLKCON &=~ 0x02;
		CLKDIV = CLOCK_OUT_HIGH_CLKDIV_INIT;	
/*************************************** �ⲿ��Ƶ ���� ************************************************/		
	#elif CLOCK_OUT_LOW_ENABLE
		CLKCON |= 0x04;	
		XTALCFG |= CLOCK_OUT_HIGH_LXTAL_CFG_INIT;
		while((CLKCON&0x40)!=0x40);	
		CLKSWR = 0xF0;
		while((CLKSWR&0x80)!=0x80);
		CLKCON &=~ 0x02;
		CLKDIV = CLOCK_OUT_HIGH_CLKDIV_INIT;	 
	#endif
	
  #if CLOCK_OUTPUT_ENABLE
	CLKOUT  = CLOCK_CLK_OUT_SEL_INIT + 0x10;
	CLKO_MAP = CLOCK_CLOCKMAP_INIT ;
	#endif	
	
  #if CLOCK_FREQ_CLK_ENABLE
  FREQ_CLK = CLOCK_FREQ_CLK_INIT;
	#endif	
	
  #if CLOCK_RC_EN_PD_INIT
  XTALCFG = CLOCK_RC_EN_PD_INIT;
	#endif		
}

/*
*********************************************************************************************************
*	�� �� ��: GPIO_Init
*	����˵��: �˿� ��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if  GPIO_INIT_ENABLE
static void GPIO_Init(void)
{  
		P0M0=GPIO_MODE_P0M0;
		P0M1=GPIO_MODE_P0M1;
		P0M2=GPIO_MODE_P0M2;	
		P0M3=GPIO_MODE_P0M3;	
		
		P1M0=GPIO_MODE_P1M0;

		P2M0=GPIO_MODE_P2M0;
		P2M1=GPIO_MODE_P2M1;
		P2M2=GPIO_MODE_P2M2;	
		P2M3=GPIO_MODE_P2M3;	
		
		#if  GPIO_INIT_PULLRES_ENABLE
		P0LPU = GPIO_PULLRESVALUE_P02;
		#endif
		
		#if  GPIO_INIT_DBC_P00_ENABLE
		P00DBC = GPIO_DBC_INIT_P00;
		#endif	
		
		#if  GPIO_INIT_DBC_P01_ENABLE
		P01DBC = GPIO_DBC_INIT_P01;
		#endif

		#if  GPIO_INIT_DBC_P02_ENABLE
		P02DBC = GPIO_DBC_INIT_P02;
		#endif
}

#endif

/*
*********************************************************************************************************
*	�� �� ��: Reset_Init
*	����˵��: ��λ ��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if  RESET_INIT_ENABLE
static void Reset_Init(void)
{
	#if  RESET_INIT_ENABLE
	  BORC   = RESET_BORVS_INIT;	
	  BORDBC = RESET_BORDBC_INIT;
	#endif
	
	#if  RESET_RSTDBC_INIT_ENABLE
		RSTDBC = RESET_RSTDBC_INIT;
	#endif	
	
	#if  RESET_SPOV_INIT_ENABLE
		SPOV_RSTEN = RESET_SPOV_INIT;
	#endif
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: WDTInit
*	����˵��: ���Ź���ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if WDT_INIT_ENABLE
static void WDT_Init(void)
{
    WDTC=WDT_WDTC_INIT;
    WDTCCR=WDT_WDTCCR_INIT;
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: LVD_Init
*	����˵��: ���Ź���ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if LVD_INIT_ENABLE
static void LVD_Init(void)
{
    LVDC=LVD_LVDC_INIT;
	  #if  LVD_LVDDBC_INIT_ENABLE
    LVDDBC=LVD_LVDDBC_INIT;
  	#endif	
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: TIMER3_Init
*	����˵��: TIMER3��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if TIMER3_INIT_ENABLE
static void TIMER3_Init(void)
{
  #if TIMER3_T3MAP_ENABLE
	 T3_MAP = TIMER3_T3MAP_INIT;	
	#endif
   T3CON = TIMER3_TCON_INIT;
	 TH3 = TIMER3_TH3_INIT;
	 TL3 = TIMER3_TL3_INIT;
	
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: TIMER4_Init
*	����˵��: ��ʱ��4��ʼ��
*	��    �Σ��� 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if TIMER4_INIT_ENABLE
static void TIMER4_Init(void)
{
  #if TIMER4_T4MAP_ENABLE
	 T4_MAP = TIMER4_T4MAP_INIT;	
	#endif
   T4CON = TIMER4_TCON_INIT;
	 TH4 = TIMER4_TH4_INIT;
	 TL4 = TIMER4_TL4_INIT;
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: TIMER5_Init
*	����˵��: ��ʱ��5��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if TIMER5_INIT_ENABLE
static void TIMER5_Init(void)
{
  #if TIMER5_T5MAP_ENABLE
	 T5_MAP = TIMER5_T5MAP_INIT;	
	#endif
   T5CON = TIMER5_TCON_INIT;
   T5CON1 = TIMER5_TCON1_INIT;	
	 TH5 = TIMER5_TH5_INIT;
	 TL5 = TIMER5_TL5_INIT;
	
}
#endif
/*
*********************************************************************************************************
*	�� �� ��: PWM0_Init
*	����˵��: PWM0��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if PWM0_INIT_ENABLE
static void PWM0_Init(void)
{
	
	#if PWM0OUT_INIT_ENABLE
	 PWM0_MAP = PWM0_PWM0MAP_INIT;
  #endif	
	#if PWM01OUT_INIT_ENABLE
	 PWM01_MAP = PWM0_PWM01MAP_INIT;
  #endif
   PWM0C = PWM0_PWM0C_INIT;

   PWM0PH = PWM0PH_INIT;
   PWM0PL = PWM0PL_INIT;

	#if PWM0OUT_INIT_ENABLE
	 PWM0DH = PWM0DH_INIT;
	 PWM0DL = PWM0DL_INIT;	
  #endif
	
	#if PWM0_DTHL_ENABLE
	 PWM0DTH = PWM0DTH_INIT;
	 PWM0DTL = PWM0DTL_INIT;	
	#elif PWM01OUT_INIT_ENABLE
	 PWM0DTH = PWM01DH_INIT;
	 PWM0DTL = PWM01DL_INIT;	
  #endif
	
   PWM0EN = PWM0_PWM0EN_INIT;		
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: PWM1_Init
*	����˵��: PWM1��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if PWM1_INIT_ENABLE
static void PWM1_Init(void)
{
	
	#if PWM1OUT_INIT_ENABLE
	 PWM1_MAP = PWM1_PWM1MAP_INIT;
  #endif	
	#if PWM11OUT_INIT_ENABLE
	 PWM11_MAP = PWM1_PWM11MAP_INIT;
  #endif
   PWM1C = PWM1_PWM1C_INIT;

   PWM1PH = PWM1PH_INIT;
   PWM1PL = PWM1PL_INIT;

	#if PWM1OUT_INIT_ENABLE
	 PWM1DH = PWM1DH_INIT;
	 PWM1DL = PWM1DL_INIT;	
  #endif
	
	#if PWM1_DTHL_ENABLE
	 PWM1DTH = PWM1DTH_INIT;
	 PWM1DTL = PWM1DTL_INIT;	
	#elif PWM11OUT_INIT_ENABLE
	 PWM1DTH = PWM11DH_INIT;
	 PWM1DTL = PWM11DL_INIT;	
  #endif
	
   PWM1EN = PWM1_PWM1EN_INIT;		
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: PWM2_Init
*	����˵��: PWM2��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if PWM2_INIT_ENABLE
static void PWM2_Init(void)
{
	
	#if PWM2OUT_INIT_ENABLE
	 PWM2_MAP = PWM2_PWM2MAP_INIT;
  #endif
	
	#if PWM21OUT_INIT_ENABLE
	 PWM21_MAP = PWM2_PWM21MAP_INIT;
  #endif
	
   PWM2C = PWM2_PWM2C_INIT;

   PWM2PH = PWM2PH_INIT;
   PWM2PL = PWM2PL_INIT;

	#if PWM2OUT_INIT_ENABLE
	 PWM2DH = PWM2DH_INIT;
	 PWM2DL = PWM2DL_INIT;	
  #endif
	
	#if PWM2_DTHL_ENABLE
	 PWM2DTH = PWM2DTH_INIT;
	 PWM2DTL = PWM2DTL_INIT;	
	#elif PWM21OUT_INIT_ENABLE
	 PWM2DTH = PWM21DH_INIT;
	 PWM2DTL = PWM21DL_INIT;	
  #endif
	
   PWM2EN = PWM2_PWM2EN_INIT;		
}
#endif


/*
*********************************************************************************************************
*	�� �� ��: PWM3_Init
*	����˵��: PWM3��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if PWM3_INIT_ENABLE
static void PWM3_Init(void)
{
	 PWM3_MAP = PWM3_PWM3MAP_INIT;
   PWM3C = PWM3_PWM3C_INIT;
   PWM3P = PWM3P_INIT;	
   PWM3D = PWM3D_INIT;		
}
#endif
/*
*********************************************************************************************************
*	�� �� ��: ADC_Init
*	����˵��: ADC��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if ADC_INIT_ENABLE
static void ADC_Init(void)
{
	ADCC0= ADC_ADCC0_INIT ;
	ADCC1= ADC_ADCC1_INIT ;
	ADCC2= ADC_ADCC2_INIT ;
	#if ADC_ADCWC_INIT
  ADCWC= ADC_ADCWC_INIT ; 
  #endif	
}
#endif



/*
*********************************************************************************************************
*	�� �� ��: UART1_Init
*	����˵��: ADC��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if UART1_INIT_ENABLE
static void UART1_Init(void)
{
	TXD_MAP = UART1_TXDMAP_INIT;
	RXD_MAP = UART1_RXDMAP_INIT;
	SCON = UART1_SCON_INIT;
	#if UART1_MODEL0_ENABLE
	
	SCON2=UART1_MODEL0_SCON2_INIT ;
	#elif UART1_MODEL1_ENABLE
	
	SCON2=UART1_MODEL1_SCON2_INIT ;	
	
	#elif UART1_MODEL2_ENABLE
	SCON2=UART1_MODEL2_SCON2_INIT ;	
  #if UART1_MODEL2_SM2_INIT
	 SADDR = UART1_MODEL2_SADDR_INIT;
   SADEN = UART1_MODEL2_SADEN_INIT;
	#endif	
	#elif UART1_MODEL3_ENABLE	
	SCON2=UART1_MODEL3_SCON2_INIT ;	
  #if UART1_MODEL3_SM2_INIT
	 SADDR = UART1_MODEL3_SADDR_INIT;
   SADEN = UART1_MODEL3_SADEN_INIT;
	#endif	
	#endif
	
	
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: UART2_Init
*	����˵��: UART2��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if UART2_INIT_ENABLE
static void UART2_Init(void)
{
	TXD2_MAP = UART2_TXD2MAP_INIT;
	RXD2_MAP = UART2_RXD2MAP_INIT;
	S2CON = UART2_REN_INIT;
	S2CON2 = UART2_S2CON2_INIT;
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: SPI_Init
*	����˵��: SPI��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if SPI_INIT_ENABLE
static void SPI_Init(void)
{
	SS_MAP = SPI_SSMAP_INIT;
	SCK_MAP = SPI_SCKMAP_INIT;
	MOSI_MAP = SPI_MOSIMAP_INIT;
	MISO_MAP = SPI_MISOMAP_INIT;	
	
	SPCTL = SPI_SPCTL_INIT;
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: EXTI_Init
*	����˵��: �ⲿ�ж�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if EXIT_INIT_ENABLE
static void EXTI_Init(void)
{
/**************************************** �����ⲿ�ж�0-1�ܽ� *****************************************/	
  #if ( EXIT0_INIT_ENABLE || EXIT1_INIT_ENABLE )
	INT01_PINS = EXIT_INT01_PINS_INIT ;
  #endif	
/**************************************** �����ⲿ�ж�0-3��ƽ *****************************************/
  #if ( EXIT0_INIT_ENABLE || EXIT1_INIT_ENABLE || EXIT2_INIT_ENABLE || EXIT3_INIT_ENABLE )
	PITS0 = EXIT_PITS0_INIT;
  #endif
/**************************************** �����ⲿ�ж�4-7��ƽ *****************************************/	
  #if ( EXIT4_INIT_ENABLE || EXIT5_INIT_ENABLE || EXIT6_INIT_ENABLE || EXIT7_INIT_ENABLE )
	PITS1 = EXIT_PITS1_INIT;
  #endif
/**************************************** �����ⲿ�ж�8-11��ƽ *****************************************/	
  #if ( EXIT8_INIT_ENABLE || EXIT9_INIT_ENABLE || EXIT10_INIT_ENABLE || EXIT11_INIT_ENABLE )
	PITS2 = EXIT_PITS2_INIT;
  #endif
/**************************************** �����ⲿ�ж�12-15��ƽ *****************************************/	
  #if ( EXIT12_INIT_ENABLE || EXIT13_INIT_ENABLE || EXIT14_INIT_ENABLE || EXIT15_INIT_ENABLE )
	PITS3 = EXIT_PITS3_INIT;
  #endif	
/**************************************** �����ⲿ�ж�2-07ʹ�� *****************************************/	
  #if ( EXIT2_INIT_ENABLE || EXIT3_INIT_ENABLE || EXIT4_INIT_ENABLE ||\
	      EXIT5_INIT_ENABLE || EXIT6_INIT_ENABLE  || EXIT7_INIT_ENABLE)
	PINTE0 = EXIT_PINTE0_INIT;
  #endif
/**************************************** �����ⲿ�ж�12-15ʹ�� *****************************************/	
  #if ( EXIT8_INIT_ENABLE  || EXIT9_INIT_ENABLE  || EXIT10_INIT_ENABLE || EXIT11_INIT_ENABLE ||\
       	EXIT12_INIT_ENABLE || EXIT13_INIT_ENABLE || EXIT14_INIT_ENABLE || EXIT15_INIT_ENABLE )
	PINTE1 = EXIT_PINTE1_INIT;
  #endif
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: TIMER0Init
*	����˵��: ���Ź���ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if ( TIMER0_INIT_ENABLE || TIMER1_INIT_ENABLE )
static void TIMER01_Init(void)
{
#if (TIMER0_T0OUT_INIT || TIMER0_CT0_INIT)	
	  T0_MAP==TIMER0_T0MAP_INIT;
#endif	

#if (TIMER1_T1OUT_INIT || TIMER1_CT1_INIT)	
	  T1_MAP==TIMER1_T1MAP_INIT;
#endif
	
    TMOD=TIMER01_TMOD_INIT;
    TCON1=TIMER01_TCON1_INIT;
    TCON=TIMER01_TCON_INIT;
#if TIMER0_INIT_ENABLE 
	  TH0=TIMER0_TH0_INIT;	
	  TL0=TIMER0_TL0_INIT;
#endif
#if TIMER1_INIT_ENABLE 
	  TH1=TIMER1_TH1_INIT;	
	  TL1=TIMER1_TL1_INIT;
#endif	
	
}
#endif


/*
*********************************************************************************************************
*	�� �� ��: INT_Init
*	����˵��: �ж��������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if (EXIT_INIT_ENABLE || WDT_INT_ENABLE || TIMER0_INIT_ENABLE || TIMER1_INIT_ENABLE \
  || TIMER3_INIT_ENABLE || TIMER4_INIT_ENABLE || TIMER5_INIT_ENABLE || PWM_INIT_ENABLE || ADC_INT_ENABLE)
static void INT_Init(void)
{
/**************************************** ����IEʹ�� *****************************************/	
#if (WDT_INT_ENABLE || EXIT0_INIT_ENABLE || EXIT1_INIT_ENABLE || TIMER0_INIT_ENABLE || TIMER1_INIT_ENABLE \
	|| UART1_INT_ENABLE || UART2_INT_ENABLE)
   IE=INT_IE_INIT;
#endif	
/**************************************** ����IE1ʹ�� *****************************************/	
#if (EXIT2_7_INIT_ENABLE || EXIT8_15_INIT_ENABLE || TIMER3_INIT_ENABLE \
  || TIMER4_INIT_ENABLE  || TIMER5_INIT_ENABLE   || ADC_INT_ENABLE || SPI_INT_ENABLE)
   IE1=INT_IE1_INIT;
#endif
/**************************************** ����IP0(���ȼ�) *****************************************/		
#if (EXIT0_INIT_ENABLE || EXIT1_INIT_ENABLE || TIMER0_INIT_ENABLE || TIMER1_INIT_ENABLE)
   IP0=INT_IP0_INIT;
#endif
/**************************************** ����IP1(���ȼ�) *****************************************/	
#if (WDT_INT_ENABLE || UART1_INT_ENABLE || UART2_INT_ENABLE || LVD_LVDIE_INIT)
   IP1=INT_IP1_INIT;
#endif
/**************************************** ����IP2(���ȼ�) *****************************************/	
#if (TIMER3_INIT_ENABLE || TIMER4_INIT_ENABLE || PWM_INIT_ENABLE  || SPI_INT_ENABLE)
   IP2=INT_IP2_INIT;
#endif	
/**************************************** ����IP3(���ȼ�) *****************************************/
#if (EXIT8_15_INIT_ENABLE || EXIT2_7_INIT_ENABLE || TIMER5_INIT_ENABLE || ADC_INT_ENABLE)
   IP3=INT_IP3_INIT;
#endif
/**************************************** �������ж� *****************************************/
#if (SYSTEM_EA_INIT_ENABLE)
   EA=SYSTEM_EA_INIT;
#endif
}
#endif


/*
*********************************************************************************************************
*	�� �� ��: System_Init
*	����˵��: ϵͳ��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void System_Init(void)
{
    SystemClock_Init();
#if GPIO_INIT_ENABLE
    GPIO_Init();
#endif
	
#if RESET_INIT_ENABLE
    Reset_Init();
#endif

#if WDT_INIT_ENABLE
   WDT_Init();
#endif	
	
#if EXIT_INIT_ENABLE
	EXTI_Init();
#endif	

#if ( TIMER0_INIT_ENABLE || TIMER1_INIT_ENABLE )
  TIMER01_Init();
#endif	

#if TIMER3_INIT_ENABLE
  TIMER3_Init();
#endif

#if TIMER4_INIT_ENABLE
  TIMER4_Init();
#endif

#if TIMER5_INIT_ENABLE
  TIMER5_Init();
#endif

#if PWM0_INIT_ENABLE
  PWM0_Init();
#endif

#if PWM1_INIT_ENABLE
  PWM1_Init();
#endif

#if PWM2_INIT_ENABLE
  PWM2_Init();
#endif

#if PWM3_INIT_ENABLE
 PWM3_Init();
#endif

#if ADC_INIT_ENABLE
 ADC_Init();
#endif

#if UART1_INIT_ENABLE
 UART1_Init();
#endif

#if UART2_INIT_ENABLE
 UART2_Init();
#endif

#if SPI_INIT_ENABLE
 SPI_Init();

#endif

#if (EXIT_INIT_ENABLE || WDT_INIT_ENABLE || TIMER0_INIT_ENABLE || TIMER1_INIT_ENABLE \ 
  || TIMER3_INIT_ENABLE || TIMER4_INIT_ENABLE || TIMER5_INIT_ENABLE  || PWM_INIT_ENABLE || ADC_INT_ENABLE \
	|| UART1_INT_ENABLE || UART1_INT_ENABLE || SPI_INT_ENABLE)
  INT_Init();
#endif

}
