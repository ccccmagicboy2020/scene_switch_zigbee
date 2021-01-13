#ifndef __MODB_H__
#define __MODB_H__

#define    Version             0x04//bootloader版本号

#define    Get_Version_Internal_Order 0x00
#define    Get_Version_OutSide_Order  0x01
#define    Get_ID                     0x02

#define    Erase_Flash                0x11
#define    Erase_Option               0x12
#define    Erase_Flash_ALL            0x13

#define    Write_Memory               0x21
#define    Write_Option               0x22

#define    Read_Memory                0x31
#define    Read_Option                0x32

#define    Go_APP                     0x91
#define    Rst_Read_Option            0x92

#define			MAGIC_SECTOR_ADDRESS0					0x3F80

#define         DATA_START                      8

#define SERIAL_PROTOCOL_VER                 0x02                                            // the version of frame 
#define PROTOCOL_HEAD                       0x09                                           //the lcation of frame length except for tail
#define FIRST_FRAME_HEAD                    0x55                                            // first byte of frame 
#define SECOND_FRAME_HEAD                   0xaa                                            // second byte of frame 

#define			MCU_OTA_VERSION_CMD				        0x0B							//zigbee request mcu version 
#define			MCU_OTA_NOTIFY_CMD				    	0x0C							//mcu ota notify 
#define			MCU_OTA_DATA_REQUEST_CMD			    0x0D							//MCU OTA data request 
#define			MCU_OTA_RESULT_CMD					    0x0E							//MCU OTA result

void          HandShake(void);//握手
unsigned char Receive_Packet (unsigned char *Data);//接收处理命令
/////////////////////////////////////////////////////////////////////////////////////
unsigned char Receive_Packet_tuya (unsigned char *Data);//接收处理命令
unsigned char read_magic_flag(void);
void uart1_init(void);
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len);
void response_mcu_ota_version_event(void);
unsigned char get_current_mcu_fw_ver(void);
char assic_to_hex(unsigned char assic_num);
void response_mcu_ota_notify_event(void);
void current_mcu_fw_pid(void);
int strcmp_barry(unsigned char *str1,unsigned char *str2);
void mcu_ota_result_event(void);
void mcu_ota_fw_request_event(void);
void mcu_ota_fw_request(void);
void mcu_ota_result_report(unsigned char status);
/////////////////////////////////////////////////////////////////////////////////////
unsigned int  CRC_CalcCRC_Process(unsigned char *fucp_CheckArr,unsigned int fui_CheckLen,unsigned char *Data,bit CRC_Flag);//CRC校验
bit           LVD_Check(unsigned long TimeOut);
#endif