/**
 * @file		PS2ToSlave1_Demo.c
 * @brief		主机发送PS2数据给从机1测试函数源文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月11日
 */


#include <stdio.h>
#include "OLED_API.h"
#include "PS2_API.h"
#include "Delay_API_z.h"
#include "PS2ToSlave1_API.h"


PS2_DataTypeDef* PS2FromMaster_GetPoint_PS2Data(void);
int8_t PS2FromMaster_ReadData(void);


void PS2ToSlave1_Demo(void)
{
	OLED_Init();
	PS2_Init();
	PS2ToSlave1_Init();

	PS2_DataTypeDef* PS2ToSlave1_TXData_Point;
	PS2ToSlave1_TXData_Point = PS2_GetDataPoint();

	PS2ToSlave1_SetPoint_PS2Data((uint8_t*)PS2ToSlave1_TXData_Point);

	PS2_DataTypeDef* PS2FromMaster_RXData_Point;
	PS2FromMaster_RXData_Point = PS2FromMaster_GetPoint_PS2Data();

	int8_t Error = 0;

	while (1)
	{
		PS2_ReadData();

		PS2ToSlave1_SendData();
		Error = PS2FromMaster_ReadData();

		OLED_ShowChar(1, 1, 'B');

		OLED_ShowHexNum(2, 1, PS2FromMaster_RXData_Point->PS2_L1, 1);
		OLED_ShowHexNum(2, 3, PS2FromMaster_RXData_Point->PS2_L2, 1);
		OLED_ShowHexNum(2, 5, PS2FromMaster_RXData_Point->PS2_R1, 1);
		OLED_ShowHexNum(2, 7, PS2FromMaster_RXData_Point->PS2_R2, 1);

		OLED_ShowHexNum(3, 1, PS2FromMaster_RXData_Point->PS2_LX, 2);
		OLED_ShowHexNum(3, 4, PS2FromMaster_RXData_Point->PS2_LY, 2);
		OLED_ShowHexNum(3, 7, PS2FromMaster_RXData_Point->PS2_RX, 2);
		OLED_ShowHexNum(3, 10, PS2FromMaster_RXData_Point->PS2_RY, 2);

		OLED_ShowHexNum(4, 1, PS2FromMaster_RXData_Point->PS2_ID, 2);
		OLED_ShowHexNum(4, 4, PS2FromMaster_RXData_Point->PS2_Mode, 2);

		OLED_Show_s32(4, 7, Error, 2);

		Delay_ms(10);
	}
}
