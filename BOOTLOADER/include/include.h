#ifndef __include_H__
#define __include_H__

#include "HC89S003F4.H"
#include "Public.h"
#include "Uart.h"
#include "Modb.h"
#include "Flash.h"
#include <HC_DEFINE.H>

#ifndef         NULL
#define         NULL                ((void *) 0)
#endif

typedef struct
{
	unsigned char mcu_ota_pid[8];	  //OTA fw PID
	unsigned char mcu_ota_ver;	     //OTA fw version
	unsigned int mcu_ota_fw_size;	 //OTA fw size
	unsigned int mcu_current_offset; //current offset 
	unsigned int mcu_ota_checksum;  //OTA fw checksum	
}xdata _ota_mcu_fw;

#endif


