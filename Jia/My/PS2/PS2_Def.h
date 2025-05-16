/**
 * @file		PS2_Def.c
 * @brief		PS2�ֱ�(���SPIЭ��ͨ��)����ӿ�(API)���岿��ͷ�ļ�
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��8��
 */


#ifndef __PS2_Def_h
#define __PS2_Def_h


#include <stdint.h>


/**
  * @brief  �洢��ȡ���ֱ����ݵĽṹ��
  */
typedef struct
{
	uint8_t PS2_ID;			// ҡ��ģ����ģʽʱ��0x41��ҡ��������ģʽʱ��0x73��δ����ʱ��0x00��0xFF
	uint8_t PS2_Mode;		// ҡ��ģ����ģʽʱ��2��ҡ��������ģʽʱ��1��δ����ʱ��0
	uint8_t PS2_SELECT;		// ����Ϊ0������Ϊ1
	uint8_t PS2_L3;			// ����Ϊ0������Ϊ1
	uint8_t PS2_R3;			// ����Ϊ0������Ϊ1
	uint8_t PS2_START;		// ����Ϊ0������Ϊ1
	uint8_t PS2_UP;			// ����Ϊ0������Ϊ1
	uint8_t PS2_RIGHT;		// ����Ϊ0������Ϊ1
	uint8_t PS2_DOWN;		// ����Ϊ0������Ϊ1
	uint8_t PS2_LEFT;		// ����Ϊ0������Ϊ1
	uint8_t PS2_L2;			// ����Ϊ0������Ϊ1
	uint8_t PS2_R2;			// ����Ϊ0������Ϊ1
	uint8_t PS2_L1;			// ����Ϊ0������Ϊ1
	uint8_t PS2_R1;			// ����Ϊ0������Ϊ1
	uint8_t PS2_Triangle;	// ����Ϊ0������Ϊ1
	uint8_t PS2_Circle;		// ����Ϊ0������Ϊ1
	uint8_t PS2_Fork;		// ����Ϊ0������Ϊ1
	uint8_t PS2_Rectangle;	// ����Ϊ0������Ϊ1
	uint8_t PS2_RX;			// 0x00 = left , 0xFF = right
	uint8_t PS2_RY;			// 0x00 = up   , 0xFF = down
	uint8_t PS2_LX;			// 0x00 = left , 0xFF = right
	uint8_t PS2_LY;			// 0x00 = up   , 0xFF = down
}PS2_DataTypeDef;


#endif
