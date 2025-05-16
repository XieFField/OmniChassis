/**
 * @file		UART_Def_z.h
 * @brief		串口(异步中断点对点)对外接口(API)定义部分头文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月8日
 */


#ifndef __UART_Def_z_h
#define __UART_Def_z_h


#include <stdint.h>


// UART通讯最大一次性接收字节数
#define UART_REC_LEN		50			// UART_REC_LEN 的全称可能是 UART Receive Length


typedef struct
{
	uint8_t UART_RX_BUF[UART_REC_LEN];	// 接收缓冲,最大USART_REC_LEN个字节
	uint8_t UART_RX_Count;				// 串口接收字节计数
}UART_DataTypeDef;


#endif
