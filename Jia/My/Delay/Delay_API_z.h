/**
 * @file		Delay_API_z.h
 * @brief		系统滴答定时器(SysTick)实现阻塞式延时对外接口(API)头文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月14日
 */


#ifndef __Delay_API_z_h
#define __Delay_API_z_h


#include <stdint.h>


void Delay_ns(uint32_t ns);
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);


#endif
