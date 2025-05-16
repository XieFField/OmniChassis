/**
 * @platform	keil5平台STM32F103C8T6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月13日
 */


#include <stdint.h>
#include "MPU6050_API.h"
#include "OLED_API.h"
#include "Delay_API_z.h"


void MPU6050_Demo(void)
{
	OLED_Init();
	MPU6050_Init();
	
	while (1)
	{
		OLED_ShowString(1, 1, "123");

		uint32_t ID = 0;
		ID = (uint32_t)MPU6050_GetID();
		OLED_ShowHexNum(1, 5, ID, 2);

		MPU6050_DataTypeDef* MPU6050Data;
		MPU6050Data = MPU6050_GetPoint_Data();
		MPU6050_ReadData();
		OLED_Show_s32(2, 1, (int32_t)(MPU6050Data->AccX * 1000), 7);
		OLED_Show_s32(3, 1, (int32_t)(MPU6050Data->AccY * 1000), 7);
		OLED_Show_s32(4, 1, (int32_t)(MPU6050Data->AccZ * 1000), 7);
		OLED_Show_s32(2, 9, (int32_t)(MPU6050Data->GyroX * 1000), 7);
		OLED_Show_s32(3, 9, (int32_t)(MPU6050Data->GyroY * 1000), 7);
		OLED_Show_s32(4, 9, (int32_t)(MPU6050Data->GyroZ * 1000), 7);

		Delay_ms(100);
	}
}
