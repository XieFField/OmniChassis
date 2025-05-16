/**
 * @platform	keil5ƽ̨STM32F103C8T6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ����Ŀ��ƿε�5��6�鵥Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��5��16��
 */


#include <stdint.h>
#include "Delay_API_z.h"
#include "OLED_API.h"
#include "VL5300_API.h"


void VL5300_Demo(void)
{
	VL5300_DataSetTypeDef* VL5300_DataSet;
	VL5300_DataSet = VL5300_GetDataSetPoint();

	OLED_Init();
	VL5300_Init();
	
	while (1)
	{
		VL5300_ReadData();

		Delay_ms(50);

		OLED_Show_u32(1, 1, (uint8_t)VL5300_DataSet->Front.Correction_TOF, 4);
		OLED_Show_u32(1, 6, (uint8_t)VL5300_DataSet->Front.Confidence, 4);
		OLED_Show_u32(1, 11, (uint8_t)VL5300_DataSet->Front.Intecounts, 4);

		OLED_Show_u32(2, 1, (uint8_t)VL5300_DataSet->Left.Correction_TOF, 4);
		OLED_Show_u32(2, 6, (uint8_t)VL5300_DataSet->Left.Confidence, 4);
		OLED_Show_u32(2, 11, (uint8_t)VL5300_DataSet->Left.Intecounts, 4);

		OLED_Show_u32(3, 1, (uint8_t)VL5300_DataSet->Right.Correction_TOF, 4);
		OLED_Show_u32(3, 6, (uint8_t)VL5300_DataSet->Right.Confidence, 4);
		OLED_Show_u32(3, 11, (uint8_t)VL5300_DataSet->Right.Intecounts, 4);


		Delay_ms(50);
	}
}

