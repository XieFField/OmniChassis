/**
 * @platform	keil5ƽ̨STM32F103C8T6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ����Ŀ��ƿε�5��6�鵥Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��5��16��
 */


#ifndef __VL5300_I2C_h
#define __VL5300_I2C_h


#include <stdint.h>
#include "stm32f10x.h"
#include "Delay_API_z.h"


/* ������������������������������������������������������������������������*/
/* I2C��MCU���������� */
#define GPIO_Speed_x			GPIO_Speed_50MHz

#define RCC_SCL					RCC_APB2Periph_GPIOB		// SCL
#define GPIOx_SCL				GPIOB
#define GPIO_Pinx_SCL			GPIO_Pin_1

#define RCC_SDA					RCC_APB2Periph_GPIOB		// SDA
#define GPIOx_SDA				GPIOB
#define GPIO_Pinx_SDA			GPIO_Pin_0

/* ������������������������������������������������*/


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
 * @brief       ����ACKӦ��
 * @param       ��
 * @retval      ��
 */
static inline void I2C_ACK(void)
{
	/* SCL 0 -> 1  ʱ SDA = 0, ��ʾӦ�� */
	I2C_SDA_Write(0);
	I2C_SCL_Write(1);

	/* ����һ��ʱ�ӣ��ôӻ���ȡACK�ź� */
	I2C_SCL_Write(0);

	/* �����ͷ�SDA�� */
	I2C_SDA_Write(1);
}

/**
 * @brief       ������ACKӦ��
 * @param       ��
 * @retval      ��
 */
static inline void I2C_NACK(void)
{
	/* SCL 0 -> 1  ʱ SDA = 1,��ʾ��Ӧ�� */
	I2C_SDA_Write(1);
	I2C_SCL_Write(1);

	/* ����һ��ʱ�ӣ��ôӻ���ȡACK�ź� */
	I2C_SCL_Write(0);
}

/**
 * @brief       ����I2Cֹͣ�ź�
 * @param       ��
 * @retval      ��
 */
static inline void I2C_Stop(void)
{
	/* ȷ��SCLΪ�ߣ�SDAΪ�ͣ�Ϊ�����źŵĲ�����׼�� */
	I2C_SDA_Write(0);
	I2C_SCL_Write(1);

	/* ��SCLΪ��ʱ, SDA�ӵͱ�ɸ�, ��ʾ�����ź� */
	I2C_SDA_Write(1);
}

/**
 * @brief       ����I2C��ʼ�ź�
 * @param       ��
 * @retval      ��
 */
static inline void I2C_Start(void)
{
	/* ȷ��SCL��SDAΪ�ߣ�Ϊ��ʼ�źŵĲ�����׼�� */
	I2C_SDA_Write(1);
	I2C_SCL_Write(1);

	/* ��SCLΪ��ʱ, SDA�Ӹ߱�ɵ�, ��ʾ��ʼ�ź� */
	I2C_SDA_Write(0);

	/* ǯסI2C���ߣ�׼�����ͻ�������� */
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
		I2C_SDA_Write((Byte & 0x80) >> 7);    /* ��λ�ȷ��� */
		I2C_SCL_Write(1);
		I2C_SCL_Write(0);
		Byte <<= 1;					/* ����1λ,������һ�η��� */
	}
	I2C_SDA_Write(1);				/* �������, �����ͷ�SDA�ߣ�ͬʱSCL��Ϊ�� */

	I2C_WaitAck();					/* �ȴ��ӻ�Ӧ�� */
}

static uint8_t I2C_ReadByte(uint8_t ACK)
{
	uint8_t Byte = 0;

	/* ����1���ֽ����� */
	for (uint8_t i = 0; i < 8; i++)
	{
		Byte <<= 1;				/* ��λ�����,�������յ�������λҪ���� */
		I2C_SCL_Write(1);

		if (I2C_SDA_Read())
		{
			Byte++;
		}

		I2C_SCL_Write(0);
	}

	/* ����Ӧ��λ */
	if (ACK)
	{
		/* ����ACK */
		I2C_ACK();
	}
	else
	{
		/* ����NACK */
		I2C_NACK();
	}

	return Byte;
}

static void I2C_ConsecutiveRead(uint8_t I2C_Address,uint8_t RegisterAddress, uint8_t* DataPoint, uint16_t Length)
{
	I2C_Start();							/* ��ʼ�ź� */

	I2C_SendByte(I2C_Address);				/* ����I2CͨѶд��ַ */

	I2C_SendByte(RegisterAddress);			/* ���ͼĴ�����ַ */

	I2C_Start();							/* ��ʼ�ź� */

	I2C_SendByte(I2C_Address | 0x01);	/* ����I2CͨѶ����ַ */

	while (Length)							/* ѭ���������� */
	{
		*DataPoint = I2C_ReadByte((Length > 1) ? 1 : 0);
		Length--;
		DataPoint++;
	}

	I2C_Stop();								/* ֹͣ�ź� */
}

static void I2C_ConsecutiveSend(uint8_t I2C_Address, uint8_t RegisterAddress, uint8_t* DataPoint, uint16_t Length)
{
	I2C_Start();							/* ��ʼ�ź� */

	I2C_SendByte(I2C_Address);				/* ����IICͨѶ��ַ */

	I2C_SendByte(RegisterAddress);          /* ���ͼĴ�����ַ */

	for (uint16_t i = 0; i < Length; i++)	/* ѭ���������� */
	{
		I2C_SendByte(DataPoint[i]);
	}

	I2C_Stop();								/* ֹͣ�ź� */
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
	/* ʹ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_SCL, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_SDA, ENABLE);

	/* ��ʼ��GPIO�� */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_x;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_SCL;			/* SCL */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		/* ���ÿ�©���ģʽ */
	GPIO_Init(GPIOx_SCL, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_SDA;			/* SDA */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		/* ���ÿ�©���ģʽ */
	GPIO_Init(GPIOx_SDA, &GPIO_InitStructure);

	/* ����GPIO��ʼ״̬ */
	I2C_Stop();
}



#endif
