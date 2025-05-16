/**
 * @file		OLED_Func.h
 * @brief		0.96��OLED��(4��I2CЭ�����ͨ��)����ӿ�(API)��������ͷ�ļ�
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��10��
 */


#ifndef __PS2_Func_z_h
#define __PS2_Func_z_h


#include "PS2_Def.h"


void PS2_Init(void);
PS2_DataTypeDef* PS2_GetDataPoint(void);
void PS2_ReadData(void);
 

#endif
