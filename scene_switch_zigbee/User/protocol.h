/**
* @file  protocol.h
* @brief declaration of fuction in  protocol.c
* @author luchao
* @date 2020.03.13
* @par email:
* luchao@tuya.com
* @copyright HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
* @par company
* http://www.tuya.com
*/
#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#ifdef __cplusplus
extern "C"
{
#endif

///< product INFORMATION

#define PRODUCT_KEY "bttqbchl"    //开发平台创建产品后生成的16位字符产品唯一标识


///< mcu version 
#define MCU_VER "1.0.0"                          
 
#define ZIGBEE_UART_QUEUE_LMT             64             // using to save data received from uart
#define ZIGBEE_UART_RECV_BUF_LMT          64             //
#define ZIGBEE_UART_SEND_BUF_LMT          32             //


typedef enum
{
    MCU_TYPE_DC_POWER = 1,
    MCU_TYPE_LOWER_POWER,
    MCU_TYPE_SCENE
}xdata MCU_TYPE_E;

/**
 * if mcu need to support the time function, this macro definition should be opened
 * and need complete mcu_write_rtctime function 
 * 
 */
 
//#define    SUPPORT_MCU_RTC_CHECK             //start time calibration

/**
 * if mcu need to support OTA, this macro definition should be opened
 */
#define    SUPPORT_MCU_OTA                  //support mcu ota


/**
 * if mcu need to support mcu type checking, this macro definition should be opened
 * 
 */
//#define    CHECK_MCU_TYPE               //support mcu type check 


/**
 * if mcu need to support zigbee network parameter setting, this macro definition should be opened
 * 
 */
//#define  SET_ZIGBEE_NWK_PARAMETER        //support zigbee nwk parameter setting 


/**
 * if mcu need to send a broadcast data, this macro definition should be opened
 * 
 */
//#define  BROADCAST_DATA_SEND           //support broadcast data sending



/**
 * DP data list,this code will be generate by cloud platforms
 */

//工厂操作(可下发可上报)
//备注:工厂操作菜单
#define DPID_FACTORY_OP 150
//计数器 (只上报)
//备注:自由计数器
//
#define DPID_FREE_TIMER 151
//故障上报(只上报)
//备注:
#define DPID_FAIL_REPORT 152
//字串上报(只上报)
//备注:
#define DPID_STRING_REPORT 153
//OTA结果(可下发可上报)
//备注:ota结果
#define DPID_OTA_RESULT 154



/**
* @brief encapsulates a generic send function, developer should use their own function to completing this fuction 
* @param[in] {value} send signle data 
* @return  void
*/
void uart_transmit_output(unsigned char value);

/**
* @brief Upload all dp information of the system, and realize the synchronization of APP and muc data
* @param[in] {void}
* @return  void
*/
void all_data_update(void);

/**
* @brief mcu check local RTC time 
* @param[in] {time} timestamp
* @return  void
*/
void mcu_write_rtctime(unsigned char time[]);

/**
* @brief Zigbee functional test feedback
* @param[in] {void} 
* @return  void
*/
//void zigbee_test_result(void);

/**
* @brief this function will handle uart received frame data  
* @param[in] {dpid}   dp id
* @param[in] {value}  dp data 
* @param[in] {length} lenght of dp data 
* @return  handle result 
*/
unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length);

/**
* @brief get received cmd total number
* @param[in] {void}   
* @return  received cmd total number
*/
unsigned char get_download_cmd_total(void);

/**
* @brief received zigbee net_work state handle 
* @param[in] {zigbee_work_state}  zigbee current network state
* @return  void 
*/
void zigbee_work_state_event(unsigned char zigbee_work_state);
/**
* @brief received reset zigbee response 
* @param[in] {state} response state 
* @return  void 
*/
void mcu_reset_zigbee_event(unsigned char state);

/**
* @brief check mcu version response
* @param[in] {void}
* @return  void 
*/
void response_mcu_ota_version_event(void);

void soft_reset_mcu(void);
void go_bootloader_ota(void);
void tuya_re_config_network(void);
void tuya_reset_module(void);
void tuya_retry_ota(void);
void reset_default_parameter(void);

void IAR_Soft_Rst_No_Option(void);
void IAR_Soft_Rst_Option(void);

void clear_timer(void);

void report_mcu_ota_result(unsigned char  res);

#ifdef __cplusplus
}
#endif
#endif
