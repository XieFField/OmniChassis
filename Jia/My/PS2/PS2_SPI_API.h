/**
 * @file		PS2_SPI_API.h
 * @brief		PS2的SPI软件通信函数封装对外接口(API)头文件
 * @details		SPI通信模式：Mode 1，CPOL = 0，CPHA = 1
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月8日
 */


#ifndef __PS2_SPI_API_h
#define __PS2_SPI_API_h


#include <stdint.h>
#include "stm32f10x.h"
#include "Delay_API_z.h"


 /* ————————————————————————————————————*/
 /* PS2使用引脚宏定义 */

 /* GPIO口速度 */
#define GPIO_Speed_x			GPIO_Speed_50MHz

/* MISO */
#define RCC_MISO				RCC_APB2Periph_GPIOB
#define GPIOx_MISO				GPIOB
#define GPIO_Pinx_MISO			GPIO_Pin_14

/* MOSI */
#define RCC_MOSI				RCC_APB2Periph_GPIOB
#define GPIOx_MOSI				GPIOB
#define GPIO_Pinx_MOSI			GPIO_Pin_15

/* CS */
#define RCC_CS					RCC_APB2Periph_GPIOC
#define GPIOx_CS				GPIOC
#define GPIO_Pinx_CS			GPIO_Pin_13

/* CLK，即SCK */
#define RCC_CLK					RCC_APB2Periph_GPIOB
#define GPIOx_CLK				GPIOB
#define GPIO_Pinx_CLK			GPIO_Pin_13

/* ————————————————————————*/


static inline void SPI_Delay(void)
{
	//for (uint8_t i = 0; i < 30; i++)
	//{

	//}

	Delay_us(4);

	// 自己测试了一下，4-50us都是能稳定正常运行的，3us不太稳定，具体理论未知，能用就行
	// 可以用系统定时器(SysTick)做Delay延时函数使用，也可以用for跑空循环来延时
	// 如果使用for循环延时，则具体的延时时间需要根据代码运行速度决定
	// 但是建议使用定时器做Delay延时以保证较精准的演示效果
}

static inline void SPI_MOSI_Write(uint8_t x)
{
	GPIO_WriteBit(GPIOx_MOSI, GPIO_Pinx_MOSI, (BitAction)x);
	SPI_Delay();
}

static inline void SPI_CLK_Write(uint8_t x)
{
	GPIO_WriteBit(GPIOx_CLK, GPIO_Pinx_CLK, (BitAction)x);
	SPI_Delay();
}

static inline void SPI_CS_Write(uint8_t x)
{
	GPIO_WriteBit(GPIOx_CS, GPIO_Pinx_CS, (BitAction)x);
	SPI_Delay();
}

static inline uint8_t SPI_MISO_Read(void)
{
	return GPIO_ReadInputDataBit(GPIOx_MISO, GPIO_Pinx_MISO);
}

/**
 * @brief       产生SPI停止信号
 * @param       无
 * @retval      无
 */
static inline void SPI_Stop(void)
{
	SPI_CS_Write(1);
	SPI_CLK_Write(1);
}

/**
 * @brief       产生SPI起始信号
 * @param       无
 * @retval      无
 */
static inline void SPI_Start(void)
{
	SPI_CLK_Write(1);
	SPI_CS_Write(0);
}

/**
  * @brief  SPI协议通信进行单字节交换（作为主机）
  * @brief  注意里面并未包含“开始时序”和“结束时序”信号
  * @brief  注意里面包含了延时函数
  * @param  无
  * @retval 一个字节
  */
static uint8_t SPI_SwapByte(uint8_t ByteSend)
{
	uint8_t ByteReceive = 0x00;

	for (uint16_t ref = 0x01; ref < 0x100; ref <<= 1)
	{
		if (ref & ByteSend)			/* 高写低读 */ 
			SPI_MOSI_Write(1);
		else
			SPI_MOSI_Write(0);

		SPI_CLK_Write(0);

		if (SPI_MISO_Read())
			ByteReceive |= ref;

		SPI_CLK_Write(1);
	}

	return ByteReceive;
}

static inline void MySPI_Init(void)
{
	/* 使能GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_MISO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_MOSI, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_CS, ENABLE);

	/* 初始化GPIO口 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_x;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_MISO;			/* MISO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	/* 配置浮空输入模式 */
	GPIO_Init(GPIOx_MISO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_MOSI;			/* MOSI */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* 配置推挽输出模式 */
	GPIO_Init(GPIOx_MOSI, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_CLK;			/* CLK，即SCK */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* 配置推挽输出模式 */
	GPIO_Init(GPIOx_CLK, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_CS;				/* CS */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* 配置推挽输出模式 */
	GPIO_Init(GPIOx_CS, &GPIO_InitStructure);

	/* 设置GPIO初始状态 */
	SPI_Stop();
}





#endif
