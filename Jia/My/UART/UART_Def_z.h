/**
 * @file		UART_Def_z.h
 * @brief		����(�첽�жϵ�Ե�)����ӿ�(API)���岿��ͷ�ļ�
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��8��
 */


#ifndef __UART_Def_z_h
#define __UART_Def_z_h


#include <stdint.h>


// UARTͨѶ���һ���Խ����ֽ���
#define UART_REC_LEN		50			// UART_REC_LEN ��ȫ�ƿ����� UART Receive Length


typedef struct
{
	uint8_t UART_RX_BUF[UART_REC_LEN];	// ���ջ���,���USART_REC_LEN���ֽ�
	uint8_t UART_RX_Count;				// ���ڽ����ֽڼ���
}UART_DataTypeDef;


#endif
