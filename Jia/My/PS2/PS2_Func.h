/**
 * @file		OLED_Func.h
 * @brief		0.96寸OLED屏(4针I2C协议软件通信)对外接口(API)函数部分头文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月10日
 */


#ifndef __PS2_Func_z_h
#define __PS2_Func_z_h


#include "PS2_Def.h"


void PS2_Init(void);
PS2_DataTypeDef* PS2_GetDataPoint(void);
void PS2_ReadData(void);
 

#endif
