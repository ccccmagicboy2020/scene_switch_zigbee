#include "include.h"

#define  Version_Order_Internal_Length   7
#define  Version_Order_OutSide_Length    6
#define  MCU_ID_Length                   10

unsigned char  HandShake_Count=0;
extern unsigned char xdata guc_Read_a[2];
extern unsigned char xdata tick_lo;
extern unsigned char xdata tick_hi;
extern unsigned char xdata Uart_Buf[150];
_ota_mcu_fw xdata ota_fw_info;
unsigned char current_mcu_pid[8];

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
/**********************************自适应波特率串口初始化**************************************/	
	TR1=1;//开定时器1
	EA=1;	
	do{	
		if(HandShake_Count>=30)//上电30ms没有检测到数据
		{
			 IAR_Soft_Rst_No_Option();//引脚没有检测到数据，进入APP			  
		}
	}
//////////////////////////////////////////////////////////////////////////////
	while(P0_4);
	TR1=0;//关定时器T1
	EA=0;
	P2M0 = P2M0 & 0xF0 | 0x08;				      //P20设置为推挽输出
	TXD_MAP = 0x20;													//TXD映射P20
	RXD_MAP = 0x04;													//RXD映射P04
//////////////////////////////////////////////////////////////////////////////
	
	T4CON = 0x06;						//T4工作模式：UART1波特率发生器
	TH4 = 0xFF;
	TL4 = 0xF8;							//波特率250000
	SCON2 = 0x02;						//8位UART，波特率可变
	SCON = 0x10;						//允许串行接收
	//ES1 = 1;
  
	Uart_SendByte(ACK);                 //发送OX79
	if(Uart_RecvByte(&Data,HandShark_TIMEOUT)!=SUCCESS) IAR_Soft_Rst_No_Option();//长时间没有接收到数据，进入APP	
	if(Data!=0x7f)                                      IAR_Soft_Rst_No_Option();//数据不是OX7F，进入APP
		
	Uart_SendByte(0x1f);//第一次发送0x1f
					
	Data=0;
	if(Uart_RecvByte(&Data,HandShark_TIMEOUT)!=SUCCESS) IAR_Soft_Rst_No_Option();//长时间没有接收到数据，进入APP	
	if(Data!=0x7f)                                      IAR_Soft_Rst_No_Option();//数据不是OX7F，进入APP
/*****************************************************************************/	

	
	BORC  =0xC1;			  //BOR:2.0V
	BORDBC=0x14;			  //BOR消抖  消抖时间 = BORDBC[7:0] * 8Tcpu +2 Tcpu	
	
	LVDC = 0xA2;						//LVD设置2.4V,禁止中断
	LVDDBC = 0xFF;				  //设置消抖时间16uS
	
	Uart_SendByte(0x1f);   //第二次发送0x1f	
  
}

unsigned char Receive_Packet (unsigned char *Data)
{
	unsigned char i;
	unsigned int  TempCRC;

	if (Uart_RecvByte(Data,  Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;//长时间没有接收到数据，进入APP
	if (Uart_RecvByte(Data+1,Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;//长时间没有接收到数据，进入APP	        
	if (Data[0]!=((Data[1]^0xff)&0xff))                    return ERROR;	  //验证反码
	
	switch(Data[0])
	{
		case Get_Version_Internal_Order://获取当前版本以及允许使用的命令  0x00 0xff
				     Uart_SendByte(ACK);//接收头码，反码成功
             Uart_SendByte(Version_Order_Internal_Length);//发送有效数据长度		
			       Uart_SendPacket(ISP_Version_Internal_Order+1, Version_Order_Internal_Length);//发送有效数据	
             TempCRC=CRC_CalcCRC_Process(ISP_Version_Internal_Order,Version_Order_Internal_Length+1,Data,0);//获取CRC校验数据	
             Uart_SendByte(TempCRC >> 8);//发送高八位
             Uart_SendByte(TempCRC & 0xFF);//发送低八位	
             return SUCCESS;				
			                                        break;			
		
		case Get_Version_OutSide_Order://获取当前版本以及允许使用的命令			0x01 0xfe
						 Uart_SendByte(ACK);//接收头码，反码成功
             Uart_SendByte(Version_Order_OutSide_Length);//发送有效数据长度		
			       Uart_SendPacket(ISP_Version_OutSide_Order+1, Version_Order_OutSide_Length);//发送有效数据	
             TempCRC=CRC_CalcCRC_Process(ISP_Version_OutSide_Order,Version_Order_OutSide_Length+1,Data,0);//获取CRC校验数据	
             Uart_SendByte(TempCRC >> 8);//发送高八位
             Uart_SendByte(TempCRC & 0xFF);//发送低八位	     
             return SUCCESS;				
			                                        break;
		
		case Get_ID:          //读取芯片型号												0x02 0xfd
						 Uart_SendByte(ACK);//接收头码，反码成功
			       if(Read_ID()!=SUCCESS)  return ERROR;	//芯片型号错误		
             Uart_SendByte(MCU_ID_Length);//发送有效数据长度			 
			       Uart_SendPacket(MCU_ID+1, MCU_ID_Length);//发送有效数据	
             TempCRC=CRC_CalcCRC_Process(MCU_ID,MCU_ID_Length+1,Data,0);//获取CRC校验数据			
             Uart_SendByte(TempCRC >> 8);//发送高八位
             Uart_SendByte(TempCRC & 0xFF);//发送低八位
             return SUCCESS;
			                                        break;

		case Erase_Flash_ALL:    //擦除 Flash 数据					0x13 0xEC
			Uart_SendByte(ACK);//接收头码，反码成功
			for (i = 0; i < 2; i ++)//保存要擦除的扇区
			{
				if (Uart_RecvByte(Data + 2 + i, NAK_TIMEOUT) != SUCCESS) return NACK_TIME;//长时间没有接收到数据，进入APP
			}
      if(Data[2]==0XFF&&Data[3]==0X00)//判断是否全扇区擦除	0xff 0x00
			{
						if (XOR_FLASH_BLANK(0x0000,0x2FFF)==0x00) return SUCCESS;//全扇区插空，擦除成功	
            if (LVD_Check(LVD_TIMEOUT)!=SUCCESS)      return ERROR;//电压不正常						
						if (Earse_Flash())                        return SUCCESS;//全扇区擦除成功
						else                                      return ERROR;//全扇区擦除失败
			}	
			else
					   return ERROR;			
			                                        break;

		case Write_Memory:  //写 Flash 数据				0x21 0xDE
				Uart_SendByte(ACK);//接收头码，反码成功			
				for (i=0; i<6; i++)//接受起始地址和CRC校验，存入Data[i+2]
				{
					if (Uart_RecvByte(Data+2+i, NAK_TIMEOUT) != SUCCESS)  return NACK_TIME;//长时间没有接收到数据，进入APP
				}
				if (CRC_CalcCRC_Process(Data+2,4,Data+6,1)!=SUCCESS )return ERROR;//CRC校验错误
				
	           Uart_SendByte(ACK);					

				if (Uart_RecvByte(Data+8, NAK_TIMEOUT) !=SUCCESS)       return NACK_TIME;//开始接收数据长度，从Data[8]开始存起

					for (i=0; i<(Data[8]+2); i++)//接收数据和CRC的值
					{
						if (Uart_RecvByte(Data+9+i, NAK_TIMEOUT) != SUCCESS)      return NACK_TIME;//长时间没有接收到数据，进入APP
					}				
       if (CRC_CalcCRC_Process(Data+8,Data[8]+1,Data+Data[8]+9,1)!=SUCCESS)  return ERROR;//CRC校验错误	
       if(LVD_Check(LVD_TIMEOUT)!=SUCCESS)  return ERROR;//电压不正常
				 IND_ADDR=0x0f;
				 IND_DATA=0x00;
				 IAR_Write_arrang((unsigned int)(Data[4]<<8)|(unsigned int)Data[5],Data+9,Data[8]);
				 IAR_Read        ((unsigned int)(Data[4]<<8)|(unsigned int)Data[5],Data+9,Data[8]);														  								 

      if (CRC_CalcCRC_Process(Data+8,Data[8]+1,Data+Data[8]+9,1)!=SUCCESS) 
			{
				        IAR_Clear((unsigned int)(Data[4]<<8)|(unsigned int)Data[5]);//擦除错误扇区
								return ERROR;//CRC校验错误					
			}
		  else                                                                
    				return SUCCESS;	
			                                        break;	

		case Go_APP:       //退出 Bootloader 返回 APP 程序		0x91 0x6E
			Uart_SendByte(ACK);//接收头码，反码成功
			 IAR_Soft_Rst_No_Option();//引脚没有检测到数据，进入APP
					    return SUCCESS;			
			                                        break;
		default:		 
						    return ERROR;//发送NACK
			break;
	
	}
}

/*************************************************************
  Function   : CRC_CalcCRC_Compare
  Description: 写入需要校验的数据,计较并返回比较结果
  Input      : *fucp_CheckArr - CRC校验数据首地址   fui_CheckLen - CRC校验数据长度 
               *CRC_Data      - 比较的CRC首地址     CRC_Flag     - 0不比较 1比较
  return     : none    
*************************************************************/
unsigned int CRC_CalcCRC_Process(unsigned char *fucp_CheckArr,unsigned int fui_CheckLen,unsigned char *CRC_Data,bit CRC_Flag)
{

 	CRCC = 0x01;//CRC复位，MSB first，复位初值为0x0000   1021		
	while(fui_CheckLen--)CRCL = *(fucp_CheckArr++);
	if(CRC_Flag==1)
	{
		if((unsigned char)(CRCR>>8) ==*(CRC_Data )&& (unsigned char)(CRCR & 0xFF)==*(CRC_Data+1))//CRC校验高八位
		{
				return SUCCESS; //接收写入地址，与CRC校验数据成功							
		}
		else
				return ERROR;//CRC校验失败		
	}
	else
		return CRCR;
	
}
/***************************************************************************************
  * @说明  	LVD查询函数
  *	@参数	无
  * @返回值 无
  * @注		无
***************************************************************************************/
bit LVD_Check(unsigned long TimeOut)
{
		WDTC |= 0x10;                   //清狗			
	while(TimeOut-- > 0)
	{
		LVDC &=~ 0x08;					        //清除LVD中断标志位 									
		if((LVDC&0x08)==0)						  //判断LVD中断标志位
		{	
			return SUCCESS;//电压正常
		}    
	}
		return ERROR;
}

/***************************************************************************************
  * @说明  	T1中断服务函数
  *	@参数	无
  * @返回值 无
  * @注		无
***************************************************************************************/
void TIMER1_Rpt(void) interrupt TIMER1_VECTOR
{
     HandShake_Count++;
}

unsigned char Receive_Packet_tuya (unsigned char *Data)
{
	unsigned char command_byte;
	unsigned char num_lo;
	unsigned char num_hi;
	unsigned char i;
	unsigned char sum = 0;

	if (Uart_RecvByte(Data,  Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;
	if (FIRST_FRAME_HEAD == Data[0])
	{
		if (Uart_RecvByte(Data+1,Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;
		if (SECOND_FRAME_HEAD == Data[1])
		{
			if (Uart_RecvByte(Data+2,Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;
			if (SERIAL_PROTOCOL_VER == Data[2])
			{
				if (Uart_RecvByte(Data+3,Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;
				tick_hi = Data[3];
				if (Uart_RecvByte(Data+4,Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;
				tick_lo = Data[4];
				if (Uart_RecvByte(Data+5,Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;
				command_byte = Data[5];
				if (Uart_RecvByte(Data+6,Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;
				num_hi = Data[6];
				if (Uart_RecvByte(Data+7,Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;
				num_lo = Data[7];
				for (i = 0; i < num_lo; i++)
				{
					if (Uart_RecvByte(Data+8+i,Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;
				}
				if (Uart_RecvByte(Data+PROTOCOL_HEAD+num_lo-1,Command_TIMEOUT)!= SUCCESS)   return NACK_TIME;

				sum = get_check_sum(Data, PROTOCOL_HEAD+num_lo-1);

				if (Data[PROTOCOL_HEAD+num_lo-1] == sum)
				{
					switch (command_byte)
					{
					case MCU_OTA_VERSION_CMD:
						response_mcu_ota_version_event();
						return	SUCCESS;
						break;
					case MCU_OTA_NOTIFY_CMD:
						response_mcu_ota_notify_event();
						return	SUCCESS;
						break;
					case MCU_OTA_DATA_REQUEST_CMD:
						if (ERROR == mcu_ota_fw_request_event())
						{
							mcu_ota_result_report(0x01);
						}
						return	SUCCESS;
						break;
					case MCU_OTA_RESULT_CMD:
						mcu_ota_result_event();
						return	SUCCESS;
						break;
					default:
						return ERROR;
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

unsigned char read_magic_flag(void)
{
	//read from flash
	IAR_Read(MAGIC_SECTOR_ADDRESS0, guc_Read_a, 2);
	return guc_Read_a[0];
}

void uart1_init(void)
{
	//init the uart1 again
	T4CON = 0x06;						//T4工作模式：UART1波特率发生器
	TH4 = 0xFF;
	TL4 = 0x30;							//波特率9600
	SCON2 = 0x02;						//8位UART，波特率可变
	SCON = 0x10;						//允许串行接收
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

void response_mcu_ota_version_event(void)
{
	unsigned short length = 0;
	unsigned char sum = 0;
	unsigned char ver = 0;

	ver = get_current_mcu_fw_ver();
	sum += FIRST_FRAME_HEAD;
	sum += SECOND_FRAME_HEAD;
	sum += SERIAL_PROTOCOL_VER;
	sum += tick_hi;
	sum += tick_lo;
	sum += MCU_OTA_VERSION_CMD;
	sum += 0x01;
	sum += ver;
	
	Uart_SendByte(FIRST_FRAME_HEAD);
	Uart_SendByte(SECOND_FRAME_HEAD);
	Uart_SendByte(SERIAL_PROTOCOL_VER);
	Uart_SendByte(tick_hi);
	Uart_SendByte(tick_lo);
	Uart_SendByte(MCU_OTA_VERSION_CMD);
	Uart_SendByte(0x00);
	Uart_SendByte(0x01);
	Uart_SendByte(ver);
	Uart_SendByte(sum);
}

unsigned char get_current_mcu_fw_ver(void)
{
	unsigned char *fw_ver = (unsigned char*) MCU_VER;	//Current version
	unsigned char current_mcu_fw_ver = 0;
	current_mcu_fw_ver = assic_to_hex(fw_ver[0]) << 6;	//high ver
	current_mcu_fw_ver |= assic_to_hex(fw_ver[2]) << 4;	//mid ver
	current_mcu_fw_ver |= assic_to_hex(fw_ver[4]);	//low ver
	return current_mcu_fw_ver;
}

char assic_to_hex(unsigned char assic_num)
{
	if(assic_num<0x30 && assic_num > 0x39)	//0~9
		return -1;
	else
		return assic_num % 0x30;
}

void response_mcu_ota_notify_event(void)
{
	unsigned char i = 0;
	unsigned short result = 0;
	unsigned char sum = 0;	
	
	current_mcu_fw_pid();	//current PID
	
	while(i<8){
		ota_fw_info.mcu_ota_pid[i] = Uart_Buf[DATA_START + i];								//ota fw PID
		i++;
	}
	ota_fw_info.mcu_ota_ver = Uart_Buf[DATA_START + 8];											//ota fw version
	ota_fw_info.mcu_ota_fw_size = Uart_Buf[DATA_START + 9] << 24 | \
																Uart_Buf[DATA_START + 10] << 16 | \
																Uart_Buf[DATA_START + 11] << 8 | \
																Uart_Buf[DATA_START + 12];								//ota fw size
	ota_fw_info.mcu_ota_checksum = Uart_Buf[DATA_START + 13] << 24 | \
																 Uart_Buf[DATA_START + 14] << 16 | \
																 Uart_Buf[DATA_START + 15] << 8 | \
																 Uart_Buf[DATA_START + 16];							//ota fw checksum
	
	if((!strcmp_barry(&ota_fw_info.mcu_ota_pid[0],&current_mcu_pid[0])) && \
		 (ota_fw_info.mcu_ota_ver > get_current_mcu_fw_ver() &&\
		  ota_fw_info.mcu_ota_fw_size > 0)	
		){		//check fw pid and fw version and fw size
		result = 0x00;	//OK
		//flag
		//flag
		//flag
		Earse_Flash();
		//flag
		//flag
		//flag
	}
	else{
		result = 0x01;	//error
	}
  
	ota_fw_info.mcu_current_offset = 0;
	
	sum += FIRST_FRAME_HEAD;
	sum += SECOND_FRAME_HEAD;
	sum += SERIAL_PROTOCOL_VER;
	sum += tick_hi;
	sum += tick_lo;
	sum += MCU_OTA_NOTIFY_CMD;
	sum += 0x01;
	sum += result;
	
	Uart_SendByte(FIRST_FRAME_HEAD);
	Uart_SendByte(SECOND_FRAME_HEAD);
	Uart_SendByte(SERIAL_PROTOCOL_VER);
	Uart_SendByte(tick_hi);
	Uart_SendByte(tick_lo);
	Uart_SendByte(MCU_OTA_NOTIFY_CMD);
	Uart_SendByte(0x00);
	Uart_SendByte(0x01);
	Uart_SendByte(result);
	Uart_SendByte(sum);
}

void current_mcu_fw_pid(void)
{
	unsigned char i = 0;
	unsigned char *fw_pid = (unsigned char*) PRODUCT_KEY;
	
	while(i < 8){
		current_mcu_pid[i] = fw_pid[i];
		i++;
	}
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

void mcu_ota_result_event(void)
{
	unsigned char status = Uart_Buf[DATA_START];
	
	if(status == 0x00)
	{
		//ok
		//wirte/clear flash flag
		//
		//
		//go app
		IAR_Soft_Rst_No_Option();
	}
	else if(status == 0x01)
	{
		//fail
	}
}

unsigned char mcu_ota_fw_request_event(void)
{	
	char fw_data[FW_SINGLE_PACKET_SIZE] = {-1};	//
	unsigned char i = 0;
	unsigned int fw_offset;

	if(Uart_Buf[DATA_START] == 0x01)				//status check 0x01 == fail
		return ERROR;
	while(i < 8){
		if(current_mcu_pid[i] != Uart_Buf[DATA_START + 1 + i])	//pid check
			return ERROR;
		i++;
	}
	if(ota_fw_info.mcu_ota_ver != Uart_Buf[DATA_START + 9]) //version check
		return ERROR;
	
	i = 0;
	while(i < 4){
		fw_offset |= (Uart_Buf[DATA_START + 10 + i] << (24 - i * 8));		//offset
		i++;
	}
	i = 0;
	if(ota_fw_info.mcu_current_offset ==  fw_offset)
	{
		if((ota_fw_info.mcu_ota_fw_size - fw_offset) / FW_SINGLE_PACKET_SIZE != 0){
			while(i < FW_SINGLE_PACKET_SIZE){
				fw_data[i] = Uart_Buf[DATA_START + 14 + i];   //fw data
				i++;
			}
			ota_fw_info.mcu_current_offset += FW_SINGLE_PACKET_SIZE;
		}
		else {		//the last packet!!!
			i = 0;
			while(i < (ota_fw_info.mcu_ota_fw_size - fw_offset)){
				fw_data[i] = Uart_Buf[DATA_START + 14 + i];
				i++;
			}
			if(ota_fw_info.mcu_ota_checksum !=\
				(fw_data[i -1 - 3] << 24 |\
					fw_data[i -1 - 2] << 16 |\
					fw_data[i -1 - 1] << 8 |\
					fw_data[i -1 - 0] ))	
					{
						return ERROR;
					}	
					else
					{
						ota_fw_data_handle(fw_offset,&fw_data[0], ota_fw_info.mcu_ota_fw_size - fw_offset);
						mcu_ota_result_report(0x00);
						return SUCCESS;
					}																	
		}
	  ota_fw_data_handle(fw_offset,&fw_data[0], FW_SINGLE_PACKET_SIZE);	//OTA paket data handle
	}
	else
	{
		return ERROR;
	}
}

//主动发
void mcu_ota_fw_request(void)
{
	unsigned char sum = 0;	
	unsigned char i = 0;
	unsigned char pack_size = 0;

	if(ota_fw_info.mcu_current_offset >= ota_fw_info.mcu_ota_fw_size)   //outside the boarder
	{
		return;
	}
	else
	{
		//
		if((ota_fw_info.mcu_ota_fw_size - ota_fw_info.mcu_current_offset) / FW_SINGLE_PACKET_SIZE != 0)
		{
			pack_size = FW_SINGLE_PACKET_SIZE;
		}
		else
		{
			pack_size = (ota_fw_info.mcu_ota_fw_size - ota_fw_info.mcu_current_offset) % FW_SINGLE_PACKET_SIZE;
		}
	}
	
	sum += FIRST_FRAME_HEAD;
	sum += SECOND_FRAME_HEAD;
	sum += SERIAL_PROTOCOL_VER;
	sum += MCU_OTA_DATA_REQUEST_CMD;
	sum += pack_size + 9;
	i = 0;
	while(i < 8){
		sum += ota_fw_info.mcu_ota_pid[i];	//PID
		i++;
	}
	sum += ota_fw_info.mcu_ota_ver;
	i = 0;
	while(i < 4){
		sum += ota_fw_info.mcu_current_offset >> (24 - i * 8);	//pakage offset request
		i++;
	}
	sum += pack_size;
	
	Uart_SendByte(FIRST_FRAME_HEAD);
	Uart_SendByte(SECOND_FRAME_HEAD);
	Uart_SendByte(SERIAL_PROTOCOL_VER);
	Uart_SendByte(0x00);
	Uart_SendByte(0x00);
	Uart_SendByte(MCU_OTA_DATA_REQUEST_CMD);
	Uart_SendByte(0x00);
	Uart_SendByte(pack_size + 9);	//size
	i = 0;
	while(i < 8){
		Uart_SendByte(ota_fw_info.mcu_ota_pid[i]);	//PID
		i++;
	}
	Uart_SendByte(ota_fw_info.mcu_ota_ver);
	i = 0;
	while(i < 4){
		Uart_SendByte(ota_fw_info.mcu_current_offset >> (24 - i * 8));	//pakage offset request
		i++;
	}
	Uart_SendByte(pack_size);
	Uart_SendByte(sum);
}

//主动发
void mcu_ota_result_report(unsigned char status)
{
	unsigned char sum = 0;
	unsigned char i = 0;
	
	//upgrade result status(0x00:ota success;0x01:ota failed)
	if (0x01 == status)	//fail
	{
		//ota failure report ota failure and clear ota struct 
		my_memset(&ota_fw_info, sizeof(ota_fw_info));
	}
	else if (0x00 == status)
	{
		//ota sucess
		//should report ota sucess notify
		//
		//
	}
	
	sum += FIRST_FRAME_HEAD;
	sum += SECOND_FRAME_HEAD;
	sum += SERIAL_PROTOCOL_VER;
	sum += MCU_OTA_RESULT_CMD;
	sum += 0x01;
	sum += status;
	i = 0;
	while(i < 8){
		sum += ota_fw_info.mcu_ota_pid[i];	//PID
		i++;
	}
	sum += ota_fw_info.mcu_ota_ver;
	
	Uart_SendByte(FIRST_FRAME_HEAD);
	Uart_SendByte(SECOND_FRAME_HEAD);
	Uart_SendByte(SERIAL_PROTOCOL_VER);
	Uart_SendByte(0x00);
	Uart_SendByte(0x00);
	Uart_SendByte(MCU_OTA_RESULT_CMD);
	Uart_SendByte(0x00);
	Uart_SendByte(0x0A);	//size
	Uart_SendByte(status);
	i = 0;
	while(i < 8){
		Uart_SendByte(ota_fw_info.mcu_ota_pid[i]);	//PID
		i++;
	}
	Uart_SendByte(ota_fw_info.mcu_ota_ver);
	Uart_SendByte(sum);
}

void ota_fw_data_handle(unsigned int fw_offset,char *data0, unsigned char size0)
{	
	//
	//
	//
}

void my_memset(void *src, unsigned short count)
{
  unsigned char *tmp = (unsigned char *)src;
  
  while(count --){
    *tmp ++ = 0;
  }
}


