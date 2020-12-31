#ifndef __Flash_H__
#define __Flash_H__

void Flash_EraseBlock(unsigned int fui_Address);//扇区擦除
void FLASH_WriteData(unsigned char fui_Address, unsigned int fuc_SaveData);//写入一个数据
void Flash_WriteArr(unsigned int fui_Address,unsigned char fuc_Length,unsigned char *fucp_SaveArr);//写入任意长度数据

#endif