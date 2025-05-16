/**
 * @file		UART_Func_z.h
 * @brief		����(�첽�жϵ�Ե�)����ӿ�(API)��������ͷ�ļ�
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��10��
 */


#ifndef __UART_Func_z_h
#define __UART_Func_z_h


#include <stdio.h>
#include "UART_Def_z.h"


void MyUART_Init(uint32_t bound);
volatile uint8_t* UART_GetPoint_RX_ReadFlagBit(void);
UART_DataTypeDef* UART_GetPoint_RXData(void);
void UART_ReadData(void);


int fputc(int ch, FILE* f);		// ����printf()��������ض��������ڣ�
								// ����ʹ��ʱ���ù����������ֱ��ʹ��printf()��������


#endif
