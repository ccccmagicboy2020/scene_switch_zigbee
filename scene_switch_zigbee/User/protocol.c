/**
* @file  protocol.c
* @brief this file contains protocol analysis and construct response function when received zigbee module send message
* @author luchao
* @date 2020.03.13
* @par email:
* luchao@tuya.com
* @copyright HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
* @par company
* http://www.tuya.com
*/

#include "zigbee.h"
#include "HC89S003F4.h"

extern ulong free_s_timer;

void send_data(u8 d);
void Flash_EraseBlock(unsigned int fui_Address);//
void FLASH_WriteData(unsigned char fuc_SaveData, unsigned int fui_Address);//
/******************************************************************************
                                移植须知:
1:MCU必须在while中直接调用mcu_api.c内的zigbee_uart_service()函数
2:程序正常初始化完成后,建议不进行关串口中断,如必须关中断,关中断时间必须短,关中断会引起串口数据包丢失
3:请勿在中断/定时器中断内调用上报函数
******************************************************************************/
/******************************************************************************
                              第一步:初始化
1:在需要使用到zigbee相关文件的文件中include "zigbee.h"
2:在MCU初始化中调用mcu_api.c文件中的zigbee_protocol_init()函数
3:将MCU串口单字节发送函数填入protocol.c文件中uart_transmit_output函数内,并删除#error
4:在MCU串口接收函数中调用mcu_api.c文件内的uart_receive_input函数,并将接收到的字节作为参数传入
5:单片机进入while循环后调用mcu_api.c文件内的zigbee_uart_service()函数
******************************************************************************/

/******************************************************************************
                        1:dp数据点序列类型对照表
          **此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK**         
******************************************************************************/

///> dp data list, this will be generated by cloud platform
const DOWNLOAD_CMD_S download_cmd[] =
{
  {DPID_FACTORY_OP, DP_TYPE_ENUM},
  {DPID_FREE_TIMER, DP_TYPE_VALUE},
  {DPID_FAIL_REPORT, DP_TYPE_FAULT},
  {DPID_STRING_REPORT, DP_TYPE_STRING},
};


/******************************************************************************
                           2:串口单字节发送函数
请将MCU串口发送函数填入该函数内,并将接收到的数据作为参数传入串口发送函数
******************************************************************************/

static void report_mcu_ota_result(unsigned char  res);


/**
* @brief encapsulates a generic send function, developer should use their own function to completing this fuction 
* @param[in] {value} send signle data 
* @return  void
*/
void uart_transmit_output(unsigned char value)
{
//  #error "please use your own uart send fuction complete this fuction, exmaple"
	send_data(value);
/*
*  //demo:
*  extern void Uart_PutChar(unsigned char value);
*  Uart_PutChar(value);	                             
*/
}

/******************************************************************************
                           第二步:实现具体用户函数
1:APP下发数据处理
2:数据上报处理
******************************************************************************/

/******************************************************************************
                            1:所有数据上报处理
当前函数处理全部数据上报(包括可下发/可上报和只上报)
  需要用户按照实际情况实现:
  1:需要实现可下发/可上报数据点上报
  2:需要实现只上报数据点上报
此函数为MCU内部必须调用
用户也可调用此函数实现全部数据上报
******************************************************************************/

//自动化生成数据上报函数

/**
* @brief Upload all dp information of the system, and realize the synchronization of APP and muc data
* @param[in] {void}
* @return  void
*/
void all_data_update(void)
{
//  #error "mcu must realize function internal function"
   /*
  * these fucntion will be generated by cloud platform, but when use these function must reference the function in  mcu_api.c
 */
    mcu_dp_enum_update(DPID_FACTORY_OP, 6); //枚举型数据上报;
    mcu_dp_value_update(DPID_FREE_TIMER, 0 ); //VALUE型数据上报;
    mcu_dp_fault_update(DPID_FAIL_REPORT, 0); //故障型数据上报;
    mcu_dp_string_update(DPID_STRING_REPORT, "hello!", sizeof("hello!")); //STRING型数据上报;

}

/******************************************************************************
                                WARNING!!!    
                            2:所有数据上报处理
自动化代码模板函数,具体请用户自行实现数据处理
******************************************************************************/

///> this will realize by  cloud platform

/*****************************************************************************
函数名称 : dp_download_factory_op_handle
功能描述 : 针对DPID_FACTORY_OP的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_factory_op_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为ENUM
    unsigned char ret;
    unsigned char factory_op;
    
    factory_op = mcu_get_dp_download_enum(value,length);
    switch(factory_op) {
        case 0:// soft reset the mcu
					soft_reset_mcu();
        break;
        
        case 1:// go bootloader and fw ota
					go_bootloader_ota();
        break;
        
        case 2:// tuya re-config the network
					tuya_re_config_network();
        break;
        
        case 3:// tuya reset module
					tuya_reset_module();
        break;
        
        case 4:// tuya retry the ota (send fail)
					tuya_retry_ota();
        break;
        
        case 5:// reset_default_parameter
					reset_default_parameter();
					
        break;
        case 6:// clear the free timer
					clear_timer();
        break;
        case 7:// tuya 产测
			//
        break;
        
        default:
    
        break;
    }
    
    //处理完DP数据后应有反馈
    ret = mcu_dp_enum_update(DPID_FACTORY_OP, factory_op);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}




#ifdef SUPPORT_MCU_RTC_CHECK
/**
* @brief mcu check local RTC time 
* @param[in] {time} timestamp
* @return  void
*/
void mcu_write_rtctime(unsigned char time[])
{
  #error "mcu should realize RTC time wtriting fuction, and delete this line"
  /*
  time[0]~time[3]：standard time
  time[4]~time[7]: Local time
 */
	my_memcpy((void *)timestamp,(const char *)time,4);	//get timestamp
	zigbee_timestamp_to_time();	
/*
	year = _time.w_year;	//year
	month = _time.w_month;	//month
	date = _time.w_date;	//date
	hour = _time.hour + 8;	//hour(8:BeiJing time)
	min = _time.min;	//minute
	sec = _time.sec;	//second
*/
}
#endif


/**
* @brief Zigbee functional test feedback
* @param[in] {void} 
* @return  void
*/
// void zigbee_test_result(void)
// {
 //#error "this test is makesure the rf fuction of zigbee module, if test pass or not should do something, mcu should realize"
  // unsigned char rssi = zigbee_uart_rx_buf[DATA_START];
	
  // if(rssi > 0x3C)	{
    //test sucess the range of rssi is 0% ~ 100%
	
  // }
  // else{
    //test failure
  // }
  
// }

/******************************************************************************
                                WARNING!!!                     
以下函数用户请勿修改!!
******************************************************************************/

/**
* @brief this function will handle uart received frame data  
* @param[in] {dpid}   dp id
* @param[in] {value}  dp data 
* @param[in] {length} lenght of dp data 
* @return  handle result 
*/
unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length)
{
  /* only list of function, mcu need realize these fuction*/
  unsigned char ret;
  switch(dpid){
          case DPID_FACTORY_OP:
            //工厂操作处理函数
            ret = dp_download_factory_op_handle(value,length);
        break;

  
  default:
    break;
  }
  return ret;
}

/**
* @brief get received cmd total number
* @param[in] {void}   
* @return  received cmd total number
*/
unsigned char get_download_cmd_total(void)
{
  return(sizeof(download_cmd) / sizeof(download_cmd[0]));
}


/**
* @brief received zigbee net_work state handle 
* @param[in] {zigbee_work_state}  zigbee current network state
* @return  void 
*/
void zigbee_work_state_event(unsigned char zigbee_work_state)
{	
	unsigned short length= 0;
	
	length = set_zigbee_uart_byte(length,0x10);
	zigbee_uart_write_frame(ZIGBEE_STATE_CMD, length);	

	switch(zigbee_work_state){
		case ZIGBEE_NOT_JION:	

			break;
		
		case ZIGBEE_JOIN_GATEWAY:	
				all_data_update();
			break;
		
		case ZIGBEE_JOIN_ERROR:	

			break;
		
		case ZIGBEE_JOINING:	

			break;
		
		default:
			break;
	}
}


/**
* @brief received reset zigbee response 
* @param[in] {state} response state 
* @return  void 
*/
void mcu_reset_zigbee_event(unsigned char state)
{	
	switch(state){
		case RESET_ZIGBEE_OK:
		
			break;
		
		case RESET_ZIGBEE_ERROR:
		
			break;
		
		default:
			break;
	}
}


/**
* @brief check mcu version response
* @param[in] {void}
* @return  void 
*/
void response_mcu_ota_version_event(void)
{
	unsigned short length = 0;
	length = set_zigbee_uart_byte(length,get_current_mcu_fw_ver());	//current fw version
	zigbee_uart_write_frame(MCU_OTA_VERSION_CMD,length);
}

void soft_reset_mcu(void)
{
	IAR_Soft_Rst_No_Option();
}
	
void go_bootloader_ota(void)
{
	//write flash flag
	Flash_EraseBlock(MAGIC_SECTOR_ADDRESS0);
	FLASH_WriteData(0x01, MAGIC_SECTOR_ADDRESS0);
	//string tips
	mcu_dp_string_update(DPID_STRING_REPORT, "already in bootloader", sizeof("already in bootloader"));
	//bootloader
	IAR_Soft_Rst_Option();
}
		
void tuya_re_config_network(void)
{
	mcu_network_start();
}
			
void tuya_reset_module(void)
{
	mcu_reset_zigbee();
}
				
void tuya_retry_ota(void)
{
	//
}
					
void reset_default_parameter(void)
{
	//
}

void IAR_Soft_Rst_No_Option(void)
{
	EA = 0;
	IAP_CMD = 0xF00F;
	IAP_CMD = 0x8778;
	EA = 1;
}

void IAR_Soft_Rst_Option(void)
{
	EA = 0;
	IAP_CMD = 0xF00F;
	IAP_CMD = 0x7887;
	EA = 1;	
}

void clear_timer(void)
{
	free_s_timer = 0;
}

#ifdef SUPPORT_MCU_OTA
/**
* @brief mcu ota update notify response
* @param[in] {offset} data offset 
* @return  void 
*/
void response_mcu_ota_notify_event(unsigned char offset)
{
	unsigned char i = 0;
	unsigned short length = 0;
	
	current_mcu_fw_pid();	//current PID
	
	while(i<8){
		ota_fw_info.mcu_ota_pid[i] = zigbee_uart_rx_buf[offset + DATA_START + i];								//ota fw PID
		i++;
	}
	ota_fw_info.mcu_ota_ver = zigbee_uart_rx_buf[offset + DATA_START + 8];											//ota fw version
	ota_fw_info.mcu_ota_fw_size = zigbee_uart_rx_buf[offset + DATA_START + 9] << 24 | \
																zigbee_uart_rx_buf[offset +DATA_START + 10] << 16 | \
																zigbee_uart_rx_buf[offset + DATA_START + 11] << 8 | \
																zigbee_uart_rx_buf[offset + DATA_START + 12];								//ota fw size
	ota_fw_info.mcu_ota_checksum = zigbee_uart_rx_buf[offset + DATA_START + 13] << 24 | \
																 zigbee_uart_rx_buf[offset + DATA_START + 14] << 16 | \
																 zigbee_uart_rx_buf[offset + DATA_START + 15] << 8 | \
																 zigbee_uart_rx_buf[offset + DATA_START + 16];								//ota fw checksum
	
	if((!strcmp_barry(&ota_fw_info.mcu_ota_pid[0],&current_mcu_pid[0])) && \
		 (ota_fw_info.mcu_ota_ver > get_current_mcu_fw_ver() &&\
		  ota_fw_info.mcu_ota_fw_size > 0)	
		){		//check fw pid and fw version and fw size
		length = set_zigbee_uart_byte(length,0x00);	//OK
	}
	else{
		length = set_zigbee_uart_byte(length,0x01);	//error
	}
    ota_fw_info.mcu_current_offset = 0;
	zigbee_uart_write_frame(MCU_OTA_NOTIFY_CMD,length);
}


/**
* @brief received mcu ota data request response
* @param[in] {fw_offset}  offset of file 
* @param[in] {data}  received data  
* @return  void 
*/
void reveived_mcu_ota_data_handle(unsigned int fw_offset,char *data)
{
	#error "received frame data, should save in flash, mcu should realize this fuction, and delete this line "
}

/**
* @brief mcu send ota data request 
* @param[in] {void}  
* @return  void 
*/
void mcu_ota_fw_request_event(unsigned char offset)
{	
	unsigned int fw_offset;
	char fw_data[FW_SINGLE_PACKET_SIZE] = {-1};	//
	unsigned char i = 0;

	if(zigbee_uart_rx_buf[offset + DATA_START] == 0x01)				//status check
		return;
	while(i < 8){
		if(current_mcu_pid[i] != zigbee_uart_rx_buf[offset + DATA_START + 1 + i])	//pid check
			return;
		i++;
	}
	if(ota_fw_info.mcu_ota_ver != zigbee_uart_rx_buf[offset + DATA_START + 9]) //version check
		return;
	
	i = 0;
	while(i < 4){
		fw_offset |= (zigbee_uart_rx_buf[offset + DATA_START + 10 + i] << (24 - i * 8));		//offset
		i++;
	}
	i = 0;
	if(ota_fw_info.mcu_current_offset ==  fw_offset)
	{
		if((ota_fw_info.mcu_ota_fw_size - fw_offset) / FW_SINGLE_PACKET_SIZE != 0){
			while(i < FW_SINGLE_PACKET_SIZE){
				fw_data[i] = zigbee_uart_rx_buf[offset + DATA_START + 14 + i];   //fw data
				i++;
			}
			ota_fw_info.mcu_current_offset += FW_SINGLE_PACKET_SIZE;
		}
		else {
			i = 0;
			while(i < (ota_fw_info.mcu_ota_fw_size - fw_offset)){
				fw_data[i] = zigbee_uart_rx_buf[offset + DATA_START + 14 + i];
				i++;
			}
			if(ota_fw_info.mcu_ota_checksum !=\
				(fw_data[i -1 - 3] << 24 |\
					fw_data[i -1 - 2] << 16 |\
					fw_data[i -1 - 1] << 8 |\
					fw_data[i -1 - 0] ))	
					{
						//ota failure report ota failure and clear ota struct 
					    my_memset(&ota_fw_info,0,sizeof(ota_fw_info));
						report_mcu_ota_result(0);
						return;	
					}	
					else
					{
						//ota sucess 
						//should report ota sucess notify 
						report_mcu_ota_result(1);
					}																	
		}
	   ota_fw_data_handle(fw_offset,&fw_data[0]);	//OTA paket data handle
	}
	else
	{
		// ota request timeout, then restart ota request from  ota_fw_info.mcu_ota_fw_size
	}
}

static void report_mcu_ota_result(unsigned char  res)
{
	unsigned short length;
	if((res==0)||(res == 1))
	{
		length = set_zigbee_uart_byte(length,res);	
		zigbee_uart_write_frame(MCU_OTA_NOTIFY_CMD,length);
	}
}


/**
* @brief mcu ota data result notify
* @param[in] {void} 
* @return  void 
*/
void mcu_ota_result_event(unsigned char offset)
{
	unsigned char status = zigbee_uart_rx_buf[offset + DATA_START];
	
	if(status == 0x00){
	}
	else if(status == 0x01)	{

	}
}

/**
* @brief mcu ota data handle 
* @param[in] {fw_offset} frame offset 
* @param[in] {data} received data
* @return  void 
*/
void ota_fw_data_handle(unsigned int fw_offset,char *data)
{
	#error "请在该函数处理固件包数据,并删除该行"
}
#endif
