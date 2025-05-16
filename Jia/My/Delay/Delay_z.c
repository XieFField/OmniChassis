/**
 * @file		Delay_z.c
 * @brief		系统滴答定时器(SysTick)实现阻塞式延时源文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月14日
 */


#include <stdio.h>
#include "stm32f10x.h"


 /**
   * @brief  纳秒级延时
   * @param  (x / 72)ns 延时时长，x的范围：0~16,777,215
   * @retval 无
   */
void Delay_ns(uint32_t xns)
{
	SysTick->LOAD = xns;					/* 设置定时器重装值 */
	SysTick->VAL = 0x00;					/* 清空当前计数值 */
	SysTick->CTRL = 0x00000005;				/* 设置时钟源为HCLK，启动定时器 */
	while (!(SysTick->CTRL & 0x00010000));	/* 等待计数到0 */
	SysTick->CTRL = 0x00000004;				/* 关闭定时器 */
}

/**
  * @brief  微秒级延时
  * @param  xus 延时时长，x的范围：0~233015
  * @retval 无
  */
void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;				/* 设置定时器重装值 */
	SysTick->VAL = 0x00;					/* 清空当前计数值 */
	SysTick->CTRL = 0x00000005;				/* 设置时钟源为HCLK，启动定时器 */
	while (!(SysTick->CTRL & 0x00010000));	/* 等待计数到0 */
	SysTick->CTRL = 0x00000004;				/* 关闭定时器 */
}

/**
  * @brief  毫秒级延时
  * @param  xms 延时时长，x的范围：0~4294967295
  * @retval 无
  */
void Delay_ms(uint32_t xms)
{
	while (xms--)
	{
		Delay_us(1000);
	}
}

/**
  * @brief  秒级延时
  * @param  xs 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_s(uint32_t xs)
{
	while (xs--)
	{
		Delay_ms(1000);
	}
}
