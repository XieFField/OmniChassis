/**
 * @file		OLED_Demo.c
 * @brief		0.96寸OLED屏(4针I2C协议软件通信)测试函数源文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @author		桑叁
 * @date		2025年4月14日
 */


#include "OLED_API.h"
#include "Delay_API_z.h"


void OLED_Demo(void)
{
	OLED_Init();

	while (1)
	{
		OLED_ShowChar(1, 1, 'A');
		OLED_ShowString(1, 3, "bcd");
		OLED_Show_u32(1, 7, 10, 3);
		OLED_Show_s32(1, 11, -10, 3);
		OLED_ShowBinNum(2, 1, 5, 4);
		OLED_ShowHexNum(2, 7, 5, 4);
		OLED_ShowString(3, 1, "SangSan");
		
		Delay_ms(500);

		OLED_Clear();

		Delay_ms(500);
	}
}
