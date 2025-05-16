/**
 * @file		OLED_Func.h
 * @brief		0.96寸OLED屏(4针I2C协议软件通信)对外接口(API)函数部分头文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @author		桑叁
 * @date		2025年4月14日
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

