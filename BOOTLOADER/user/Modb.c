#include "include.h"

#define  Version_Order_Internal_Length   7
#define  Version_Order_OutSide_Length    6
#define  MCU_ID_Length                   10

volatile ulong	HandShake_Count=0;

extern unsigned char xdata guc_Read_a[2];
extern unsigned char guc_Read_a1[20];
extern unsigned char xdata tick_lo;
extern unsigned char xdata tick_hi;
extern unsigned char xdata Uart_Buf[150];
extern unsigned char xdata Uart_send_Buf[30];
_ota_mcu_fw xdata ota_fw_info;
unsigned int fw_file_sum = 0;
short ota_packet_total_num = 0;
short ota_packet_current_num = 0;
unsigned char ota_last_packet_size = 0;
unsigned short led_speed = 0;

unsigned char code ISP_Version_Internal_Order[]={       
														Version_Order_Internal_Length,
														Version,
														Get_Version_OutSide_Order, 
														Get_Version_Internal_Order,	
														Get_ID,
														Erase_Flash_ALL, 
														Write_Memory,
														Go_APP
																		   };
unsigned char  code ISP_Version_OutSide_Order[]={       
	                        	Version_Order_OutSide_Length,
														Version,
														Get_Version_OutSide_Order,  
														Get_ID,
														Erase_Flash_ALL, 
														Write_Memory,
														Go_APP
																   };

unsigned char code MCU_ID[]={MCU_ID_Length,0x48,0x43,0x38,0x39,0x53,0x30,0x30,0x33,0x46,0x34};	//HC89S003F4

void HandShake(void)
{
	unsigned char Data;
/**********************************����Ӧ�����ʴ��ڳ�ʼ��**************************************/	
	enable_timer(1);
	do{	
		if(HandShake_Count>=30)//�ϵ�30msû�м�⵽����
		{
			 IAR_Soft_Rst_No_Option();//����û�м�⵽���ݣ�����APP			  
		}
	}
//////////////////////////////////////////////////////////////////////////////
	while(P0_4);
	uart1_init(0xff, 0xf8);	//250000
//////////////////////////////////////////////////////////////////////////////  
	Uart_SendByte(ACK);                 //����OX79
	if(Uart_RecvByte(&Data,HandShark_TIMEOUT)!=SUCCESS) IAR_Soft_Rst_No_Option();//��ʱ��û�н��յ����ݣ�����APP	
	if(Data!=0x7f)                                      IAR_Soft_Rst_No_Option();//���ݲ���OX7F������APP
		
	Uart_SendByte(0x1f);//��һ�η���0x1f
					
	Data=0;
	if(Uart_RecvByte(&Data,HandShark_TIMEOUT)!=SUCCESS) IAR_Soft_Rst_No_Option();//��ʱ��û�н��յ����ݣ�����APP	
	if(Data!=0x7f)                                      IAR_Soft_Rst_No_Option();//���ݲ���OX7F������APP
/*****************************************************************************/	

	
	BORC  =0xC1;			  //BOR:2.0V
	BORDBC=0x14;			  //BOR����  ����ʱ�� = BORDBC[7:0] * 8Tcpu +2 Tcpu	
	
	LVDC = 0xA2;						//LVD����2.4V,��ֹ�ж�
	LVDDBC = 0xFF;				  //��������ʱ��16uS
	
	Uart_SendByte(0x1f);   //�ڶ��η���0x1f	
  
}

#ifdef HOLYCHIP
unsigned char Receive_Packet (unsigned char *Data)
{
	unsigned char i;
	unsigned int  TempCRC;

	if (Uart_RecvByte(Data,  Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;//��ʱ��û�н��յ����ݣ�����APP
	if (Uart_RecvByte(Data+1,Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;//��ʱ��û�н��յ����ݣ�����APP	        
	if (Data[0]!=((Data[1]^0xff)&0xff))                    return ERROR;	  //��֤����
	
	switch(Data[0])
	{
		case Get_Version_Internal_Order://��ȡ��ǰ�汾�Լ�����ʹ�õ�����  0x00 0xff
				     Uart_SendByte(ACK);//����ͷ�룬����ɹ�
             Uart_SendByte(Version_Order_Internal_Length);//������Ч���ݳ���		
			       Uart_SendPacket(ISP_Version_Internal_Order+1, Version_Order_Internal_Length);//������Ч����	
             TempCRC=CRC_CalcCRC_Process(ISP_Version_Internal_Order,Version_Order_Internal_Length+1,Data,0);//��ȡCRCУ������	
             Uart_SendByte(TempCRC >> 8);//���͸߰�λ
             Uart_SendByte(TempCRC & 0xFF);//���͵Ͱ�λ	
             return SUCCESS;				
			                                        break;			
		
		case Get_Version_OutSide_Order://��ȡ��ǰ�汾�Լ�����ʹ�õ�����			0x01 0xfe
						 Uart_SendByte(ACK);//����ͷ�룬����ɹ�
             Uart_SendByte(Version_Order_OutSide_Length);//������Ч���ݳ���		
			       Uart_SendPacket(ISP_Version_OutSide_Order+1, Version_Order_OutSide_Length);//������Ч����	
             TempCRC=CRC_CalcCRC_Process(ISP_Version_OutSide_Order,Version_Order_OutSide_Length+1,Data,0);//��ȡCRCУ������	
             Uart_SendByte(TempCRC >> 8);//���͸߰�λ
             Uart_SendByte(TempCRC & 0xFF);//���͵Ͱ�λ	     
             return SUCCESS;				
			                                        break;
		
		case Get_ID:          //��ȡоƬ�ͺ�												0x02 0xfd
						 Uart_SendByte(ACK);//����ͷ�룬����ɹ�
			       if(Read_ID()!=SUCCESS)  return ERROR;	//оƬ�ͺŴ���		
             Uart_SendByte(MCU_ID_Length);//������Ч���ݳ���			 
			       Uart_SendPacket(MCU_ID+1, MCU_ID_Length);//������Ч����	
             TempCRC=CRC_CalcCRC_Process(MCU_ID,MCU_ID_Length+1,Data,0);//��ȡCRCУ������			
             Uart_SendByte(TempCRC >> 8);//���͸߰�λ
             Uart_SendByte(TempCRC & 0xFF);//���͵Ͱ�λ
             return SUCCESS;
			                                        break;

		case Erase_Flash_ALL:    //���� Flash ����					0x13 0xEC
			Uart_SendByte(ACK);//����ͷ�룬����ɹ�
			for (i = 0; i < 2; i ++)//����Ҫ����������
			{
				if (Uart_RecvByte(Data + 2 + i, NAK_TIMEOUT) != SUCCESS) return NACK_TIME;//��ʱ��û�н��յ����ݣ�����APP
			}
      if(Data[2]==0XFF&&Data[3]==0X00)//�ж��Ƿ�ȫ��������	0xff 0x00
			{
						if (XOR_FLASH_BLANK(0x0000,0x2FFF)==0x00) return SUCCESS;//ȫ������գ������ɹ�	
            if (LVD_Check(LVD_TIMEOUT)!=SUCCESS)      return ERROR;//��ѹ������						
						if (Earse_Flash())                        return SUCCESS;//ȫ���������ɹ�
						else                                      return ERROR;//ȫ��������ʧ��
			}	
			else
					   return ERROR;			
			                                        break;

		case Write_Memory:  //д Flash ����				0x21 0xDE
				Uart_SendByte(ACK);//����ͷ�룬����ɹ�			
				for (i=0; i<6; i++)//������ʼ��ַ��CRCУ�飬����Data[i+2]
				{
					if (Uart_RecvByte(Data+2+i, NAK_TIMEOUT) != SUCCESS)  return NACK_TIME;//��ʱ��û�н��յ����ݣ�����APP
				}
				if (CRC_CalcCRC_Process(Data+2,4,Data+6,1)!=SUCCESS )return ERROR;//CRCУ�����
				
	           Uart_SendByte(ACK);					

				if (Uart_RecvByte(Data+8, NAK_TIMEOUT) !=SUCCESS)       return NACK_TIME;//��ʼ�������ݳ��ȣ���Data[8]��ʼ����

					for (i=0; i<(Data[8]+2); i++)//�������ݺ�CRC��ֵ
					{
						if (Uart_RecvByte(Data+9+i, NAK_TIMEOUT) != SUCCESS)      return NACK_TIME;//��ʱ��û�н��յ����ݣ�����APP
					}				
       if (CRC_CalcCRC_Process(Data+8,Data[8]+1,Data+Data[8]+9,1)!=SUCCESS)  return ERROR;//CRCУ�����	
       if(LVD_Check(LVD_TIMEOUT)!=SUCCESS)  return ERROR;//��ѹ������
				 IND_ADDR=0x0f;
				 IND_DATA=0x00;
				 IAR_Write_arrang((unsigned int)(Data[4]<<8)|(unsigned int)Data[5],Data+9,Data[8]);
				 IAR_Read        ((unsigned int)(Data[4]<<8)|(unsigned int)Data[5],Data+9,Data[8]);														  								 

      if (CRC_CalcCRC_Process(Data+8,Data[8]+1,Data+Data[8]+9,1)!=SUCCESS) 
			{
				        IAR_Clear((unsigned int)(Data[4]<<8)|(unsigned int)Data[5]);//������������
								return ERROR;//CRCУ�����					
			}
		  else                                                                
    				return SUCCESS;	
			                                        break;	

		case Go_APP:       //�˳� Bootloader ���� APP ����		0x91 0x6E
			Uart_SendByte(ACK);//����ͷ�룬����ɹ�
			 IAR_Soft_Rst_No_Option();//����û�м�⵽���ݣ�����APP
					    return SUCCESS;			
			                                        break;
		default:		 
						    return ERROR;//����NACK
			break;
	
	}
}

/*************************************************************
  Function   : CRC_CalcCRC_Compare
  Description: д����ҪУ�������,�ƽϲ����رȽϽ��
  Input      : *fucp_CheckArr - CRCУ�������׵�ַ   fui_CheckLen - CRCУ�����ݳ��� 
               *CRC_Data      - �Ƚϵ�CRC�׵�ַ     CRC_Flag     - 0���Ƚ� 1�Ƚ�
  return     : none    
*************************************************************/
unsigned int CRC_CalcCRC_Process(unsigned char *fucp_CheckArr,unsigned int fui_CheckLen,unsigned char *CRC_Data,bit CRC_Flag)
{

 	CRCC = 0x01;//CRC��λ��MSB first����λ��ֵΪ0x0000   1021		
	while(fui_CheckLen--)CRCL = *(fucp_CheckArr++);
	if(CRC_Flag==1)
	{
		if((unsigned char)(CRCR>>8) ==*(CRC_Data )&& (unsigned char)(CRCR & 0xFF)==*(CRC_Data+1))//CRCУ��߰�λ
		{
				return SUCCESS; //����д���ַ����CRCУ�����ݳɹ�							
		}
		else
				return ERROR;//CRCУ��ʧ��		
	}
	else
		return CRCR;
	
}
/***************************************************************************************
  * @˵��  	LVD��ѯ����
  *	@����	��
  * @����ֵ ��
  * @ע		��
***************************************************************************************/
bit LVD_Check(unsigned long TimeOut)
{
		WDTC |= 0x10;                   //�幷			
	while(TimeOut-- > 0)
	{
		LVDC &=~ 0x08;					        //���LVD�жϱ�־λ 									
		if((LVDC&0x08)==0)						  //�ж�LVD�жϱ�־λ
		{	
			return SUCCESS;//��ѹ����
		}    
	}
		return ERROR;
}
#endif
/***************************************************************************************
  * @˵��  	T1�жϷ�����
  *	@����	��
  * @����ֵ ��
  * @ע		��
***************************************************************************************/
void TIMER1_Rpt(void) interrupt TIMER1_VECTOR
{
	HandShake_Count++;
	
	if (HandShake_Count >= led_speed)	//T = 200ms
	{
		HandShake_Count = 0;
		P1_0 = ~P1_0;
	}
}

void mcu_ota_fw_req_cv(void)
{
	unsigned char result = 0;
	
	result = mcu_ota_fw_request_event();
	
	switch (result)
	{
		case ERROR_STATUS:
			send_ota_result_dp(0x02);
			mcu_ota_result_report(0x01);
			break;
		case ERROR_PID:	//
			send_ota_result_dp(0x03);
			mcu_ota_fw_request();
			break;
		case ERROR_VER:
			send_ota_result_dp(0x04);
			mcu_ota_fw_request();
			break;
		case ERROR_SUM:
			send_ota_result_dp(0x05);
			mcu_ota_result_report(0x01);
			break;
		case ERROR_OFFSET:	//
			send_ota_result_dp(0x06);
			mcu_ota_fw_request();
			break;
		case SUCCESS_ALL:
			send_ota_result_dp(0x00);
			mcu_ota_result_report(0x00);
			break;
		case SUCCESS:
			mcu_ota_fw_request();
			break;
		default:
			break;
	}
}

unsigned char Receive_Packet_tuya(unsigned char *Data)
{
	unsigned char command_byte;
	unsigned char num_lo;
	unsigned char num_hi;
	unsigned char i;
	unsigned char sum = 0;

	if (Uart_RecvByte(Data,  Command_TIMEOUT_tuya)!= SUCCESS)   return NACK_TIME;
	if (FIRST_FRAME_HEAD == Data[0])
	{
		if (Uart_RecvByte(Data+1,Command_TIMEOUT_tuya)!= SUCCESS)   return NACK_TIME;
		if (SECOND_FRAME_HEAD == Data[1])
		{
			if (Uart_RecvByte(Data+2,Command_TIMEOUT_tuya)!= SUCCESS)   return NACK_TIME;
			if (SERIAL_PROTOCOL_VER == Data[2])
			{
				if (Uart_RecvByte(Data+3,Command_TIMEOUT_tuya)!= SUCCESS)   return NACK_TIME;
				tick_hi = Data[3];
				if (Uart_RecvByte(Data+4,Command_TIMEOUT_tuya)!= SUCCESS)   return NACK_TIME;
				tick_lo = Data[4];
				if (Uart_RecvByte(Data+5,Command_TIMEOUT_tuya)!= SUCCESS)   return NACK_TIME;
				command_byte = Data[5];
				if (Uart_RecvByte(Data+6,Command_TIMEOUT_tuya)!= SUCCESS)   return NACK_TIME;
				num_hi = Data[6];
				if (Uart_RecvByte(Data+7,Command_TIMEOUT_tuya)!= SUCCESS)   return NACK_TIME;
				num_lo = Data[7];
				for (i = 0; i < num_lo; i++)
				{
					if (Uart_RecvByte(Data+8+i,Command_TIMEOUT_tuya)!= SUCCESS)   return NACK_TIME;
				}
				if (Uart_RecvByte(Data+PROTOCOL_HEAD+num_lo-1,Command_TIMEOUT_tuya)!= SUCCESS)   return NACK_TIME;

				sum = get_check_sum(Data, PROTOCOL_HEAD+num_lo-1);

				if (Data[PROTOCOL_HEAD+num_lo-1] == sum)
				{
					switch (command_byte)
					{
					case MCU_OTA_VERSION_CMD:
						response_mcu_ota_version_event(ota_fw_info.mcu_ota_ver);
						return	SUCCESS;
						break;
					case MCU_OTA_NOTIFY_CMD:
						response_mcu_ota_notify_event();
						return	SUCCESS;
						break;
					case MCU_OTA_DATA_REQUEST_CMD:
						mcu_ota_fw_req_cv();
						return	SUCCESS;
						break;
					case MCU_OTA_RESULT_CMD:
						mcu_ota_result_event();
						return	SUCCESS;
						break;
					case DATA_REPORT_CMD:
						return SUCCESS;
						break;					
					default:
						return NACK_TIME;		//no support command bypass
						break;
					}
				}
				else
				{
					return	ERROR;
				}
			}
			else
			{
				return	ERROR;
			}
		}
		else
		{
			return	ERROR;
		}
	}
	else
	{
		return	ERROR;
	}
}

void read_ota_struct(void)
{
	unsigned char i = 0;
	
	IAR_Read(MAGIC_SECTOR_ADDRESS0 + 1, guc_Read_a1, 20);
	
	if (0x00 != guc_Read_a1[0])
	{
		i = 0;
		while(i<17){
			Uart_Buf[DATA_START + i] = guc_Read_a1[i];								//ota struct
			i++;
		}
		response_mcu_ota_notify_event();		
	}
	else
	{
		response_mcu_ota_version_event(0x40);
	}
}

unsigned char read_magic_flag(void)
{
	//read from flash
	unsigned char magic_byte = 0;
	
	IAR_Read(MAGIC_SECTOR_ADDRESS0, guc_Read_a, 2);
	magic_byte = guc_Read_a[0];
	
	return magic_byte;
	//return 0;			//force isp
	//return 1;			//force tuya ota
}

void set_magic_flag(unsigned char temp)
{
	IAR_Clear(MAGIC_SECTOR_ADDRESS0);
	Delay_us(10000);
	IAR_Write_Byte(MAGIC_SECTOR_ADDRESS0, temp);
	Delay_us(100);
}

void Delay_us(unsigned int q1)
{
	uint j;
	for (j = 0; j < q1; j++)
	{
		;
	}
}

void uart1_init(unsigned char th, unsigned char tl)
{
	P2M0 = P2M0 & 0xF0 | 0x08;				      //P20����Ϊ�������
	TXD_MAP = 0x20;													//TXDӳ��P20
	RXD_MAP = 0x04;													//RXDӳ��P04
	
	//init the uart1 again
	T4CON = 0x06;						//T4����ģʽ��UART1�����ʷ�����
	TH4 = th;
	TL4 = tl;							//������9600
	SCON2 = 0x02;						//8λUART�������ʿɱ�
	SCON = 0x10;						//�����н���
}

unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len)
{
  unsigned short i;
  unsigned char check_sum = 0;
  
  for(i = 0; i < pack_len; i ++){
    check_sum += *pack ++;
  }
  
  return check_sum;
}

void response_mcu_ota_version_event(unsigned char ver)
{
	unsigned short length = 0;
	length = set_zigbee_uart_byte(length, ver);
	zigbee_uart_write_frame(MCU_OTA_VERSION_CMD,length, tick_hi, tick_lo);
}

int strcmp_barry(unsigned char *str1,unsigned char *str2)
{
   int ret=0;
   while( !(ret = *(unsigned char*)str1 - *(unsigned char*)str2 ) && *str1 ){
		 str1++;
		 str2++;
	 }
	 if(ret < 0)	//str1 < str2
			return -1;
	 else if(ret > 0)	//str1 > str2
			return 1;
	 return 0;	//str1 == str2
}

void response_mcu_ota_notify_event(void)
{
	unsigned char i = 0;
	unsigned short result = 0;
	unsigned short length = 0;
	unsigned char key = 0;
	unsigned char pid_ota_string[9] = "";
	
	i = 0;
	while(i<8){
		ota_fw_info.mcu_ota_pid[i] = Uart_Buf[DATA_START + i];								//ota fw PID
		pid_ota_string[i] = Uart_Buf[DATA_START + i];
		i++;
	}
	
	pid_ota_string[8] = '\0';
	
	ota_fw_info.mcu_ota_ver = Uart_Buf[DATA_START + 8];											//ota fw version
	ota_fw_info.mcu_ota_fw_size = Uart_Buf[DATA_START + 9] << 24 | \
																Uart_Buf[DATA_START + 10] << 16 | \
																Uart_Buf[DATA_START + 11] << 8 | \
																Uart_Buf[DATA_START + 12];								//ota fw size
	ota_fw_info.mcu_ota_checksum = Uart_Buf[DATA_START + 13] << 24 | \
																 Uart_Buf[DATA_START + 14] << 16 | \
																 Uart_Buf[DATA_START + 15] << 8 | \
																 Uart_Buf[DATA_START + 16];							//ota fw checksum
	
	if (
		!strcmp_barry(pid_ota_string, (unsigned char*)PRODUCT_KEY0) || 
		!strcmp_barry(pid_ota_string, (unsigned char*)PRODUCT_KEY1) ||	
		!strcmp_barry(pid_ota_string, (unsigned char*)PRODUCT_KEY2) || 
		!strcmp_barry(pid_ota_string, (unsigned char*)PRODUCT_KEY3)
	)
	{
		key = 1;
	}
	else
	{
		key = 0;
	}
	
	if((key &&\
		ota_fw_info.mcu_ota_ver > 0x00 &&\
		ota_fw_info.mcu_ota_fw_size > 0)	
		){		//check fw pid and fw version and fw size
		result = 0x00;	//OK
		ota_fw_info.mcu_current_offset = 0x00;
		ota_packet_total_num = ota_fw_info.mcu_ota_fw_size / FW_SINGLE_PACKET_SIZE;
		ota_last_packet_size = ota_fw_info.mcu_ota_fw_size % FW_SINGLE_PACKET_SIZE;
		ota_packet_current_num = 0;
		fw_file_sum = 0;
		Earse_Flash();
		set_magic_flag(1);
		led_speed = 500;
	}
	else{
		result = 0x01;	//error
	}
  
	length = set_zigbee_uart_byte(length,result);
	zigbee_uart_write_frame(MCU_OTA_NOTIFY_CMD,length, tick_hi, tick_lo);
	
	if (0x00 == result)
	{
		mcu_ota_fw_request();
	}
	else if (0x01 == result)
	{
		send_ota_result_dp(0x07);
		mcu_ota_result_report(0x01);
	}
}

void mcu_ota_result_event(void)
{
	unsigned char status = Uart_Buf[DATA_START];
	
	if(status == 0x00)
	{
		//ok
	}
	else if(status == 0x01)
	{
		//fail
	}
}
//rev
////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char mcu_ota_fw_request_event(void)
{	
	unsigned char fw_data[FW_SINGLE_PACKET_SIZE] = {0};	//
	unsigned char i = 0;
	unsigned int fw_offset;

	if(Uart_Buf[DATA_START] == 0x01)				//status check 0x01 == fail
	{
		return ERROR_STATUS;
	}
	
	i = 0;
	while(i < 8){
		if(ota_fw_info.mcu_ota_pid[i] != Uart_Buf[DATA_START + 1 + i])	//pid check
		{
			return ERROR_PID;		
		}
		i++;
	}
	
	if(ota_fw_info.mcu_ota_ver != Uart_Buf[DATA_START + 9]) //version check
	{	
		return ERROR_VER;
	}

	i = 0;
	fw_offset = 0;
	while(i < 4){
		fw_offset |= (Uart_Buf[DATA_START + 10 + i] << (24 - i * 8));		//offset
		i++;
	}
	
	//the very first pack
	if (0x00 == fw_offset)
	{
		send_ota_result_dp(0x07);
	}
	
	if(ota_fw_info.mcu_current_offset ==  fw_offset)
	{
		if (ota_packet_current_num < ota_packet_total_num)
		{
			i = 0;
			while(i < FW_SINGLE_PACKET_SIZE)
			{
				fw_data[i] = Uart_Buf[DATA_START + 14 + i];   //fw data
				fw_file_sum += fw_data[i];
				i++;
			}
			ota_fw_info.mcu_current_offset += FW_SINGLE_PACKET_SIZE;
		}
		else
		{
			i = 0;
			while(i < ota_last_packet_size)
			{
				fw_data[i] = Uart_Buf[DATA_START + 14 + i];
				fw_file_sum += fw_data[i];
				i++;
			}
			
			if(ota_fw_info.mcu_ota_checksum != fw_file_sum)
			{
				return ERROR_SUM;
			}	
			else
			{
				ota_fw_data_handle(fw_offset,&fw_data[0], ota_last_packet_size);
				ota_packet_current_num++;
				return SUCCESS_ALL;
			}
		}
		ota_packet_current_num++;
		ota_fw_data_handle(fw_offset,&fw_data[0], FW_SINGLE_PACKET_SIZE);	//OTA paket data handle
		return SUCCESS;
	}
	else
	{
		return ERROR_OFFSET;
	}
}

//������
void mcu_ota_fw_request(void)
{
	unsigned short length = 0;	
	unsigned char i = 0;
	unsigned char pack_size = 0;

	if(ota_fw_info.mcu_current_offset >= ota_fw_info.mcu_ota_fw_size)   //outside the boarder
	{
		return;
	}
	else
	{
		//here
		if (ota_packet_current_num < ota_packet_total_num)
		{
			pack_size = FW_SINGLE_PACKET_SIZE;
		}
		else
		{
			pack_size = ota_last_packet_size;
		}
	}

	while(i < 8){
		length = set_zigbee_uart_byte(length,ota_fw_info.mcu_ota_pid[i]); //ota fw pid
		i++;
	}
	length = set_zigbee_uart_byte(length,ota_fw_info.mcu_ota_ver);		//ota fw version
	i = 0;
	while(i < 4){
		length = set_zigbee_uart_byte(length , ota_fw_info.mcu_current_offset >> (24 - i * 8));	//pakage offset request
		i++;
	}
	length = set_zigbee_uart_byte(length, pack_size);	// packet size request
	
	zigbee_uart_write_frame(MCU_OTA_DATA_REQUEST_CMD,length, 0x00, 0x00);
}

void send_ota_result_dp(unsigned char status)
{
	unsigned short length = 0;	
	//9A 04 00 01 00
	length = set_zigbee_uart_byte(length, DPID_OTA_RESULT);
	length = set_zigbee_uart_byte(length, DP_TYPE_ENUM);
	length = set_zigbee_uart_byte(length, 0x00);
	length = set_zigbee_uart_byte(length, 0x01);
	length = set_zigbee_uart_byte(length, status);
	zigbee_uart_write_frame(DATA_REPORT_CMD, length, 0x00, 0x00);
}

//������
void mcu_ota_result_report(unsigned char status)
{
	unsigned short length = 0;
	unsigned char i = 0;
	
	length = set_zigbee_uart_byte(length,status); //upgrade result status(0x00:ota success;0x01:ota failed)
	while(i < 8){
		length = set_zigbee_uart_byte(length,ota_fw_info.mcu_ota_pid[i]);	//PID
		i++;
	}
	length = set_zigbee_uart_byte(length,ota_fw_info.mcu_ota_ver);	//ota fw version
	
	zigbee_uart_write_frame(MCU_OTA_RESULT_CMD,length, 0x00, 0x00);	//response
	
	if (0x01 == status)	//fail
	{
		//active report the new version
		response_mcu_ota_version_event(ota_fw_info.mcu_ota_ver);
		//ota failure report ota failure and clear ota struct 
		my_memset(&ota_fw_info, sizeof(ota_fw_info));
		//led fast
		led_speed = 100;
	}
	else if (0x00 == status)
	{
		//wirte/clear flash flag
		set_magic_flag(0);
		//go app
		IAR_Soft_Rst_No_Option();
	}
}

void ota_fw_data_handle(unsigned int fw_offset,char *data0, unsigned char size0)
{	
	//write the flash here!
	IAR_Write_arrang(fw_offset, data0, size0);
}

void my_memset(void *src, unsigned short count)
{
  unsigned char *tmp = (unsigned char *)src;
  
  while(count --){
    *tmp ++ = 0;
  }
}

void zigbee_uart_write_frame(unsigned char fr_cmd, unsigned short len, unsigned char seq_hi, unsigned char seq_lo)
{
  unsigned char check_sum = 0;
	
  Uart_send_Buf[HEAD_FIRST] = FIRST_FRAME_HEAD;
  Uart_send_Buf[HEAD_SECOND] = SECOND_FRAME_HEAD;
  Uart_send_Buf[PROTOCOL_VERSION] = SERIAL_PROTOCOL_VER;
	
  Uart_send_Buf[SEQ_HIGH] = seq_hi;
  Uart_send_Buf[SEQ_LOW] = seq_lo;

  Uart_send_Buf[FRAME_TYPE] = fr_cmd;
  Uart_send_Buf[LENGTH_HIGH] = len >> 8;
  Uart_send_Buf[LENGTH_LOW] = len & 0xff;
  
  len += PROTOCOL_HEAD;
	
  check_sum = get_check_sum((unsigned char *)Uart_send_Buf, len-1);
  Uart_send_Buf[len -1] = check_sum;
  
  Uart_SendPacket((unsigned char *)Uart_send_Buf, len);
}

unsigned short set_zigbee_uart_byte(unsigned short dest, unsigned char byte)
{
  unsigned char *obj = (unsigned char *)Uart_send_Buf + DATA_START + dest;
	
  *obj = byte;
  dest += 1;
  
  return dest;
}

//void mcu_reset_zigbee(unsigned char network)
//{
//  unsigned short length = 0;

//  length = set_zigbee_uart_byte(length,network);
//  zigbee_uart_write_frame(ZIGBEE_CFG_CMD, length, 0x00, 0x00);
//}

void enable_timer(unsigned char en)
{
	TR1= en;		//start the timer
	EA = en;		//start the interrupt
}
