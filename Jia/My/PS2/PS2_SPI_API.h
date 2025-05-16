/**
 * @file		PS2_SPI_API.h
 * @brief		PS2��SPI���ͨ�ź�����װ����ӿ�(API)ͷ�ļ�
 * @details		SPIͨ��ģʽ��Mode 1��CPOL = 0��CPHA = 1
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��8��
 */


#ifndef __PS2_SPI_API_h
#define __PS2_SPI_API_h


#include <stdint.h>
#include "stm32f10x.h"
#include "Delay_API_z.h"


 /* ������������������������������������������������������������������������*/
 /* PS2ʹ�����ź궨�� */

 /* GPIO���ٶ� */
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

/* CLK����SCK */
#define RCC_CLK					RCC_APB2Periph_GPIOB
#define GPIOx_CLK				GPIOB
#define GPIO_Pinx_CLK			GPIO_Pin_13

/* ������������������������������������������������*/


static inline void SPI_Delay(void)
{
	//for (uint8_t i = 0; i < 30; i++)
	//{

	//}

	Delay_us(4);

	// �Լ�������һ�£�4-50us�������ȶ��������еģ�3us��̫�ȶ�����������δ֪�����þ���
	// ������ϵͳ��ʱ��(SysTick)��Delay��ʱ����ʹ�ã�Ҳ������for�ܿ�ѭ������ʱ
	// ���ʹ��forѭ����ʱ����������ʱʱ����Ҫ���ݴ��������ٶȾ���
	// ���ǽ���ʹ�ö�ʱ����Delay��ʱ�Ա�֤�Ͼ�׼����ʾЧ��
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
 * @brief       ����SPIֹͣ�ź�
 * @param       ��
 * @retval      ��
 */
static inline void SPI_Stop(void)
{
	SPI_CS_Write(1);
	SPI_CLK_Write(1);
}

/**
 * @brief       ����SPI��ʼ�ź�
 * @param       ��
 * @retval      ��
 */
static inline void SPI_Start(void)
{
	SPI_CLK_Write(1);
	SPI_CS_Write(0);
}

/**
  * @brief  SPIЭ��ͨ�Ž��е��ֽڽ�������Ϊ������
  * @brief  ע�����沢δ��������ʼʱ�򡱺͡�����ʱ���ź�
  * @brief  ע�������������ʱ����
  * @param  ��
  * @retval һ���ֽ�
  */
static uint8_t SPI_SwapByte(uint8_t ByteSend)
{
	uint8_t ByteReceive = 0x00;

	for (uint16_t ref = 0x01; ref < 0x100; ref <<= 1)
	{
		if (ref & ByteSend)			/* ��д�Ͷ� */ 
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
	/* ʹ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_MISO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_MOSI, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_CS, ENABLE);

	/* ��ʼ��GPIO�� */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_x;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_MISO;			/* MISO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	/* ���ø�������ģʽ */
	GPIO_Init(GPIOx_MISO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_MOSI;			/* MOSI */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* �����������ģʽ */
	GPIO_Init(GPIOx_MOSI, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_CLK;			/* CLK����SCK */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* �����������ģʽ */
	GPIO_Init(GPIOx_CLK, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_CS;				/* CS */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* �����������ģʽ */
	GPIO_Init(GPIOx_CS, &GPIO_InitStructure);

	/* ����GPIO��ʼ״̬ */
	SPI_Stop();
}





#endif
