/**
 * @file		PS2_Demo.c
 * @brief		PS2�ֱ�(���SPIЭ��ͨ��)���Ժ���Դ�ļ�
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��10��
 */


#include "OLED_API.h"
#include "PS2_API.h"
#include "Delay_API_z.h"


void PS2_Demo(void)
{
	OLED_Init();
	PS2_Init();

	PS2_DataTypeDef* PS2_Data_Point;
	PS2_Data_Point = PS2_GetDataPoint();

	while (1)
	{
		OLED_ShowChar(1, 1, 'B');

		PS2_ReadData();

		OLED_ShowHexNum(2, 1, PS2_Data_Point->PS2_L1, 1);
		OLED_ShowHexNum(2, 3, PS2_Data_Point->PS2_L2, 1);
		OLED_ShowHexNum(2, 5, PS2_Data_Point->PS2_R1, 1);
		OLED_ShowHexNum(2, 7, PS2_Data_Point->PS2_R2, 1);

		OLED_ShowHexNum(3, 1, PS2_Data_Point->PS2_LX, 2);
		OLED_ShowHexNum(3, 4, PS2_Data_Point->PS2_LY, 2);
		OLED_ShowHexNum(3, 7, PS2_Data_Point->PS2_RX, 2);
		OLED_ShowHexNum(3, 10, PS2_Data_Point->PS2_RY, 2);

		OLED_ShowHexNum(4, 1, PS2_Data_Point->PS2_ID, 2);
		OLED_ShowHexNum(4, 4, PS2_Data_Point->PS2_Mode, 2);

		Delay_ms(50);
	}
}
