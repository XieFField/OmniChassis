/**
 * @file		PS2ToSlave1_API.h
 * @brief		主机发送PS2数据给从机1对外接口(API)头文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月11日
 */


#ifndef __PS2ToSlave1_API_h
#define __PS2ToSlave1_API_h


#include <stdio.h>
#include "PS2ToSlave1_Demo.h"


void PS2ToSlave1_Init(void);
void PS2ToSlave1_SetPoint_PS2Data(uint8_t* DataPoint);
void PS2ToSlave1_SendData(void);


// 这两个位于"PS2ToSlave1.c文件"的函数未声明成"文件内静态函数"
// 有对外的瑕疵，但是我不想修复了
//PS2_DataTypeDef* PS2FromMaster_GetPoint_PS2Data(void);
//int8_t PS2FromMaster_ReadData(void);


#endif
