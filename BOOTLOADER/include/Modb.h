#ifndef __MODB_H__
#define __MODB_H__

#define    Version             0x04//bootloader�汾��
#define    Get_Version_Internal_Order 0x00
#define    Get_Version_OutSide_Order  0x01
#define    Get_ID                     0x02
#define    Erase_Flash_ALL            0x13
#define    Write_Memory               0x21
#define    Go_APP                     0x91

#define		 MAGIC_SECTOR_ADDRESS0			0x2E80	//at app zone

#define         HEAD_FIRST                      0
#define         HEAD_SECOND                     1
#define         PROTOCOL_VERSION                2
#define         SEQ_HIGH                        3
#define         SEQ_LOW                         4
#define         FRAME_TYPE                      5
#define         LENGTH_HIGH                     6
#define         LENGTH_LOW                      7
#define         DATA_START                      8

#define FW_SINGLE_PACKET_SIZE								0x32	     ///< firmware single packet size, define by mcu

#define SERIAL_PROTOCOL_VER                 0x02                                            // the version of frame 
#define PROTOCOL_HEAD                       0x09                                            //the lcation of frame length except for tail
#define FIRST_FRAME_HEAD                    0x55                                            // first byte of frame 
#define SECOND_FRAME_HEAD                   0xaa                                            // second byte of frame 

#define     DATA_REPORT_CMD                 6                               //mcu report DP data to zigbee
#define			MCU_OTA_VERSION_CMD				      0x0B							//zigbee request mcu version 
#define			MCU_OTA_NOTIFY_CMD				    	0x0C							//mcu ota notify 
#define			MCU_OTA_DATA_REQUEST_CMD			  0x0D							//MCU OTA data request 
#define			MCU_OTA_RESULT_CMD					    0x0E							//MCU OTA result

#define     DP_TYPE_ENUM                    0x04				//enum
#define     DATA_REPORT_CMD                 6
#define			DPID_OTA_RESULT									154

void          HandShake(void);//����
unsigned char Receive_Packet (unsigned char *Data);//���մ�������
/////////////////////////////////////////////////////////////////////////////////////
unsigned char Receive_Packet_tuya (unsigned char *Data);//���մ�������
unsigned char read_magic_flag(void);
void uart1_init(unsigned char th, unsigned char tl);
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len);
void response_mcu_ota_version_event(unsigned char ver);
void response_mcu_ota_notify_event(void);
void mcu_ota_result_event(void);
unsigned char mcu_ota_fw_request_event(void);
void mcu_ota_fw_request(void);
static void report_mcu_ota_result(unsigned char  res);
void ota_fw_data_handle(unsigned int fw_offset,char *data0, unsigned char size0);
void mcu_ota_result_report(unsigned char status);
void my_memset(void *src, unsigned short count);
void zigbee_uart_write_frame(unsigned char fr_cmd, unsigned short len, unsigned char seq_hi, unsigned char seq_lo);
unsigned short set_zigbee_uart_byte(unsigned short dest, unsigned char byte);
void enable_timer(unsigned char en);
void set_magic_flag(unsigned char temp);
void send_ota_result_dp(unsigned char status);
void read_ota_struct(void);
void Delay_us(unsigned int q1);
int strcmp_barry(unsigned char *str1,unsigned char *str2);
/////////////////////////////////////////////////////////////////////////////////////
unsigned int  CRC_CalcCRC_Process(unsigned char *fucp_CheckArr,unsigned int fui_CheckLen,unsigned char *Data,bit CRC_Flag);//CRCУ��
bit           LVD_Check(unsigned long TimeOut);
#endif