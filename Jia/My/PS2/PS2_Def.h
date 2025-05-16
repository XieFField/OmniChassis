/**
 * @file		PS2_Def.c
 * @brief		PS2手柄(软件SPI协议通信)对外接口(API)定义部分头文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月8日
 */


#ifndef __PS2_Def_h
#define __PS2_Def_h


#include <stdint.h>


/**
  * @brief  存储读取的手柄数据的结构体
  */
typedef struct
{
	uint8_t PS2_ID;			// 摇杆模拟量模式时是0x41，摇杆数字量模式时是0x73，未连接时是0x00或0xFF
	uint8_t PS2_Mode;		// 摇杆模拟量模式时是2，摇杆数字量模式时是1，未连接时是0
	uint8_t PS2_SELECT;		// 按下为0，其他为1
	uint8_t PS2_L3;			// 按下为0，其他为1
	uint8_t PS2_R3;			// 按下为0，其他为1
	uint8_t PS2_START;		// 按下为0，其他为1
	uint8_t PS2_UP;			// 按下为0，其他为1
	uint8_t PS2_RIGHT;		// 按下为0，其他为1
	uint8_t PS2_DOWN;		// 按下为0，其他为1
	uint8_t PS2_LEFT;		// 按下为0，其他为1
	uint8_t PS2_L2;			// 按下为0，其他为1
	uint8_t PS2_R2;			// 按下为0，其他为1
	uint8_t PS2_L1;			// 按下为0，其他为1
	uint8_t PS2_R1;			// 按下为0，其他为1
	uint8_t PS2_Triangle;	// 按下为0，其他为1
	uint8_t PS2_Circle;		// 按下为0，其他为1
	uint8_t PS2_Fork;		// 按下为0，其他为1
	uint8_t PS2_Rectangle;	// 按下为0，其他为1
	uint8_t PS2_RX;			// 0x00 = left , 0xFF = right
	uint8_t PS2_RY;			// 0x00 = up   , 0xFF = down
	uint8_t PS2_LX;			// 0x00 = left , 0xFF = right
	uint8_t PS2_LY;			// 0x00 = up   , 0xFF = down
}PS2_DataTypeDef;


#endif
