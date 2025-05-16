/**
 * @file		OLED_Func.h
 * @brief		0.96��OLED��(4��I2CЭ�����ͨ��)����ӿ�(API)��������ͷ�ļ�
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @author		ɣ��
 * @date		2025��4��14��
 */


#ifndef __OLED_Func_h
#define __OLED_Func_h


#include <stdint.h>


void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char* String);
void OLED_Show_u32(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_Show_s32(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);



#endif

