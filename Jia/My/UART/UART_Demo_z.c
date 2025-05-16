/**
 * @file		UART_Demo_z.c
 * @brief		串口(异步中断点对点)测试函数源文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月10日
 */


#include <stdint.h>
#include "UART_API_z.h"
#include "Delay_API_z.h"


void UART_Demo(void)
{
	MyUART_Init(115200);

	volatile uint8_t* UART_RX_ReadFlagBit;
	UART_RX_ReadFlagBit = UART_GetPoint_RX_ReadFlagBit();

	UART_DataTypeDef* UART_RXData_Point;
	UART_RXData_Point = UART_GetPoint_RXData();

	printf("Start\r\n");
	
	uint8_t count_forwards = 0;
	uint8_t count_backwards = 0;	

	while (1)
	{
		printf("Start\r\n");

		if (1 == *UART_RX_ReadFlagBit)
		{
			UART_ReadData();

			count_forwards = 0;
			count_backwards = UART_RXData_Point->UART_RX_Count;

			while (count_backwards > 0)
			{
				printf("%c\r\n", UART_RXData_Point->UART_RX_BUF[count_forwards]);

				count_forwards++;
				count_backwards--;
			}
		}

		Delay_s(1);
	}
}
