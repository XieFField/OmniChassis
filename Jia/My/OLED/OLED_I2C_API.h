/**
 * @file		OLED_I2C_API.h
 * @brief		OLED的I2C软件通信函数封装对外接口(API)头文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月14日
 */


#ifndef __OLED_I2C_API_h
#define __OLED_I2C_API_h


#include <stdint.h>
#include "stm32f10x.h"


/* ————————————————————————————————————*/
/* I2C在MCU的引脚配置 */
#define GPIO_Speed_x			GPIO_Speed_50MHz

#define RCC_SCL					RCC_APB2Periph_GPIOB		// SCL
#define GPIOx_SCL				GPIOB
#define GPIO_Pinx_SCL			GPIO_Pin_8

#define RCC_SDA					RCC_APB2Periph_GPIOB		// SDA
#define GPIOx_SDA				GPIOB
#define GPIO_Pinx_SDA			GPIO_Pin_9

/* ————————————————————————*/


static inline void I2C_Delay(void)
{
	// 是否需要通信延时取决与硬件的通信能力和电路的上拉能力
	// 经验证后，在电路上拉能力足够时，1.15MHz的通信时钟频率可以正常通信
	// TODO
}

static inline void I2C_SCL_Write(uint8_t x)
{
	GPIO_WriteBit(GPIOx_SCL, GPIO_Pinx_SCL, (BitAction)x);
	//I2C_Delay();
}

static inline void I2C_SDA_Write(uint8_t x)
{
	GPIO_WriteBit(GPIOx_SDA, GPIO_Pinx_SDA, (BitAction)x);
	//I2C_Delay();
}

static inline uint8_t I2C_SDA_Read(void)
{
	return GPIO_ReadInputDataBit(GPIOx_SDA, GPIO_Pinx_SDA);
}

/**
 * @brief       产生ACK应答
 * @param       无
 * @retval      无
 */
static inline void I2C_ACK(void)
{
	/* SCL 0 -> 1  时 SDA = 0, 表示应答 */
	I2C_SDA_Write(0);
	I2C_SCL_Write(1);

	/* 产生一个时钟，让从机读取ACK信号 */
	I2C_SCL_Write(0);

	/* 主机释放SDA线 */
	I2C_SDA_Write(1);
}

/**
 * @brief       不产生ACK应答
 * @param       无
 * @retval      无
 */
static inline void I2C_NACK(void)
{
	/* SCL 0 -> 1  时 SDA = 1,表示不应答 */
	I2C_SDA_Write(1);
	I2C_SCL_Write(1);

	/* 产生一个时钟，让从机读取ACK信号 */
	I2C_SCL_Write(0);
}

/**
 * @brief       产生I2C停止信号
 * @param       无
 * @retval      无
 */
static inline void I2C_Stop(void)
{
	/* 确保SCL为高，SDA为低，为结束信号的产生做准备 */
	I2C_SDA_Write(0);
	I2C_SCL_Write(1);

	/* 当SCL为高时, SDA从低变成高, 表示结束信号 */
	I2C_SDA_Write(1);
}

/**
 * @brief       产生I2C起始信号
 * @param       无
 * @retval      无
 */
static inline void I2C_Start(void)
{
	/* 确保SCL和SDA为高，为起始信号的产生做准备 */
	I2C_SDA_Write(1);
	I2C_SCL_Write(1);

	/* 当SCL为高时, SDA从高变成低, 表示起始信号 */
	I2C_SDA_Write(0);

	/* 钳住I2C总线，准备发送或接收数据 */
	I2C_SCL_Write(0);
}

static inline void I2C_WaitAck(void)
{
	I2C_SCL_Write(1);
	if (I2C_SDA_Read())
	{
		I2C_Stop();
	}
	I2C_SCL_Write(0);
}

static void I2C_SendByte(uint8_t Byte)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		I2C_SDA_Write((Byte & 0x80) >> 7);    /* 高位先发送 */
		I2C_SCL_Write(1);
		I2C_SCL_Write(0);
		Byte <<= 1;					/* 左移1位,用于下一次发送 */
	}
	I2C_SDA_Write(1);				/* 发送完成, 主机释放SDA线，同时SCL线为低 */

	I2C_WaitAck();					/* 等待从机应答 */
}

static uint8_t I2C_ReadByte(uint8_t ACK)
{
	uint8_t Byte = 0;

	/* 接收1个字节数据 */
	for (uint8_t i = 0; i < 8; i++)
	{
		Byte <<= 1;				/* 高位先输出,所以先收到的数据位要左移 */
		I2C_SCL_Write(1);

		if (I2C_SDA_Read())
		{
			Byte++;
		}

		I2C_SCL_Write(0);
	}

	/* 产生应答位 */
	if (ACK)
	{
		/* 发送ACK */
		I2C_ACK();
	}
	else
	{
		/* 发送NACK */
		I2C_NACK();
	}

	return Byte;
}

static inline void MyI2C_Init(void)
{
	/* 使能GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_SCL, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_SDA, ENABLE);

	/* 初始化GPIO口 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_x;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_SCL;			/* SCL */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		/* 配置开漏输出模式 */
	GPIO_Init(GPIOx_SCL, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_SDA;			/* SDA */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		/* 配置开漏输出模式 */
	GPIO_Init(GPIOx_SDA, &GPIO_InitStructure);

	/* 设置GPIO初始状态 */
	I2C_Stop();
}



#endif
