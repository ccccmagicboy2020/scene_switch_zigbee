#ifndef __Flash_H__
#define __Flash_H__

void Flash_EraseBlock(unsigned int fui_Address);//��������
void FLASH_WriteData(unsigned char fui_Address, unsigned int fuc_SaveData);//д��һ������
void Flash_WriteArr(unsigned int fui_Address,unsigned char fuc_Length,unsigned char *fucp_SaveArr);//д�����ⳤ������

#endif