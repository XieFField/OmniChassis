/**
 * @file		UART_Func_z.h
 * @brief		串口(异步中断点对点)对外接口(API)函数部分头文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月10日
 */


#ifndef __UART_Func_z_h
#define __UART_Func_z_h


#include <stdio.h>
#include "UART_Def_z.h"


void MyUART_Init(uint32_t bound);
volatile uint8_t* UART_GetPoint_RX_ReadFlagBit(void);
UART_DataTypeDef* UART_GetPoint_RXData(void);
void UART_ReadData(void);


int fputc(int ch, FILE* f);		// 用于printf()函数输出重定向至串口，
								// 具体使用时不用管这个函数，直接使用printf()函数即可


#endif
