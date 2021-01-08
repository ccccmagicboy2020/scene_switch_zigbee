#include "Include.h"

/**
  * @˵��  	����������Լ����5ms��ʱ��
  * @����  	fui_Address ���������������ڵ�����һ����ַ
  * @����ֵ ��
  * @ע		ֻҪ�����������������һ����ַ���Ϳ��Բ���������
  */
#pragma disable							//ȷ������ʱ������жϵ��µ���ʧ��
void Flash_EraseBlock(unsigned int fui_Address)
{
	IAP_CMD = 0xF00F;				//Flash����
	IAP_ADDR = fui_Address;			//д�������ַ
	IAP_CMD = 0xD22D;				//ѡ�������ʽ�� ��������
	IAP_CMD = 0xE11E; 				//������IAP_ADDRL&IAP_ADDRHָ��0xFF��ͬʱ�Զ�����
}

/**
  * @˵��  	д��һ���ֽ����ݵ�Flash����
  * @����  	fui_Address ��FLASH��ַ
  *	@����	fucp_SaveData��д�������
  * @����ֵ ��
  * @ע		д֮ǰ�����ȶԲ������������в���
  */
#pragma disable							//ȷ������ʱ������жϵ��µ���ʧ��
void FLASH_WriteData(unsigned char fuc_SaveData, unsigned int fui_Address)
{
	IAP_DATA = fuc_SaveData;
	IAP_CMD=0xF00F;				//Flash����
	IAP_ADDR = fui_Address;
	IAP_CMD=0xB44B;				//�ֽڱ��
	IAP_CMD=0xE11E;				//����һ�β���
}

/**
  * @˵��  	д�����ⳤ�ȵ����ݵ�FLASH����
  * @����  	fui_Address ��FLASH��ʼ��ַ
  *	@����	fuc_Length �� д�����ݳ���
  *			ȡֵ��Χ��0x00-0xFF
  *	@����	*fucp_SaveArr��д������ݴ��������׵�ַ
  * @����ֵ ��
  * @ע		д֮ǰ�����ȶԲ������������в���
  */
#pragma disable							//ȷ������ʱ������жϵ��µ���ʧ��
void Flash_WriteArr(unsigned int fui_Address,unsigned char fuc_Length,unsigned char *fucp_SaveArr)
{
	unsigned char fui_i = 0;
	for(fui_i=0;fui_i<fuc_Length;fui_i++)
	{
		FLASH_WriteData(*(fucp_SaveArr++), fui_Address++); 
	}
}
