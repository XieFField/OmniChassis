/**
 * @platform	keil5平台STM32F103C8T6型号单片机
 * @mainpage	机器人学院十期项目设计课第5、6组单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年5月16日
 */


#ifndef __VL5300_I2C_h
#define __VL5300_I2C_h


#include <stdint.h>
#include "stm32f10x.h"
#include "Delay_API_z.h"


/* ————————————————————————————————————*/
/* I2C在MCU的引脚配置 */
#define GPIO_Speed_x			GPIO_Speed_50MHz

#define RCC_SCL					RCC_APB2Periph_GPIOB		// SCL
#define GPIOx_SCL				GPIOB
#define GPIO_Pinx_SCL			GPIO_Pin_1

#define RCC_SDA					RCC_APB2Periph_GPIOB		// SDA
#define GPIOx_SDA				GPIOB
#define GPIO_Pinx_SDA			GPIO_Pin_0

/* ————————————————————————*/


static inline void I2C_Delay(void)
{
	Delay_us(2);
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

static void I2C_ConsecutiveRead(uint8_t I2C_Address,uint8_t RegisterAddress, uint8_t* DataPoint, uint16_t Length)
{
	I2C_Start();							/* 起始信号 */

	I2C_SendByte(I2C_Address);				/* 发送I2C通讯写地址 */

	I2C_SendByte(RegisterAddress);			/* 发送寄存器地址 */

	I2C_Start();							/* 起始信号 */

	I2C_SendByte(I2C_Address | 0x01);	/* 发送I2C通讯读地址 */

	while (Length)							/* 循环接收数据 */
	{
		*DataPoint = I2C_ReadByte((Length > 1) ? 1 : 0);
		Length--;
		DataPoint++;
	}

	I2C_Stop();								/* 停止信号 */
}

static void I2C_ConsecutiveSend(uint8_t I2C_Address, uint8_t RegisterAddress, uint8_t* DataPoint, uint16_t Length)
{
	I2C_Start();							/* 起始信号 */

	I2C_SendByte(I2C_Address);				/* 发送IIC通讯地址 */

	I2C_SendByte(RegisterAddress);          /* 发送寄存器地址 */

	for (uint16_t i = 0; i < Length; i++)	/* 循环发送数据 */
	{
		I2C_SendByte(DataPoint[i]);
	}

	I2C_Stop();								/* 停止信号 */
}

static void I2C_SingleRead(uint8_t I2C_Address, uint8_t RegisterAddress, uint8_t* Data)
{
	I2C_ConsecutiveRead(I2C_Address, RegisterAddress, Data, 1);
}

static void I2C_SingleSend(uint8_t I2C_Address, uint8_t RegisterAddress, uint8_t Data)
{
	I2C_ConsecutiveSend(I2C_Address, RegisterAddress, &Data, 1);
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
