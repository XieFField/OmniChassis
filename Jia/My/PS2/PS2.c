/**
 * @file		PS2.c
 * @brief		PS2�ֱ�(���SPIЭ��ͨ��)��������Դ�ļ�
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��10��
 */


// ��ȡһ���ֱ�������Ҫ����9���ֽڣ����Ͽ�ʼ����ֹʱ��һ��220������ʽDelay

// ����ʱ���Ż�(Optimize for Time),O3�������Ż�
// Program Size: Code=2736 RO-data=1856 RW-data=4 ZI-data=1148  
// Delay3us��һ�ζ�ȡʱ��0.87ms
// Delay4us��һ�ζ�ȡʱ��1.08ms
// Delay5us��һ�ζ�ȡʱ��1.30ms
// Delay10us��һ�ζ�ȡʱ��2.39ms
// Delay50us��һ�ζ�ȡʱ��11.07ms
// ���Իع�(��������С���˷�)����ȡʱ��(ms) = ��ʱʱ��(us) * 0.217 + 0.216��������0.999999

// ������ʱ���Ż�(Optimize for Time),O3�������Ż�
// Program Size: Code=2302 RO-data=1858 RW-data=4 ZI-data=1148  
// Delay10us��һ�ζ�ȡʱ��2.41ms

// ������ʱ���Ż�(Optimize for Time),O2�������Ż�
// Program Size: Code=2294 RO-data=1858 RW-data=4 ZI-data=1148  
// Delay10us��һ�ζ�ȡʱ��2.41ms

// ������ʱ���Ż�(Optimize for Time),O1�������Ż�
// Program Size: Code=2334 RO-data=1858 RW-data=4 ZI-data=1148  
// Delay10us��һ�ζ�ȡʱ��2.41ms

// ������ʱ���Ż�(Optimize for Time),O0�������Ż�
// Program Size: Code=2722 RO-data=1858 RW-data=4 ZI-data=1148  
// Delay4us��һ�ζ�ȡʱ��1.18ms
// Delay10us��һ�ζ�ȡʱ��2.48ms


#include <stdint.h>
#include "PS2_Def.h"
#include "PS2_SPI_API.h"


static PS2_DataTypeDef PS2_Data;


/* �ֱ��ӿڳ�ʼ�� */
void PS2_Init(void)
{
	MySPI_Init();
}

PS2_DataTypeDef* PS2_GetDataPoint(void)
{
	return &PS2_Data;
}

/**
  * @brief  ��ȡ�ֱ�����
  * @param  ��
  * @retval ��
  */
void PS2_ReadData(void)
{
	u8 tem = 0x00;

	/* �ֱ����ݶ�ȡ */ 
	SPI_Start();			// ��ʼͨ��

	/* ��1���ֽ� */
	SPI_SwapByte(0x01);		// 0x01 �� 0x42 ���ֱ��������ֱ�ʹ��˵����
	
	/* ��2���ֽ� */
	PS2_Data.PS2_ID = SPI_SwapByte(0x42);		// 0x01 �� 0x42 ���ֱ��������ֱ�ʹ��˵����

	/* ��3���ֽ� */
	SPI_SwapByte(0x00);		// û���Թ������ֽڵĹ���

	/* ��4���ֽ� */
	tem = SPI_SwapByte(0x00);		// ת���������ڿ����Ҳ��С�𶯵����0x00�أ�����ֵΪ����û���Թ���������

	PS2_Data.PS2_SELECT		=	tem & 0x01;
	PS2_Data.PS2_L3			=	(tem >> 1) & 0x01;
	PS2_Data.PS2_R3			=	(tem >> 2) & 0x01;
	PS2_Data.PS2_START		=	(tem >> 3) & 0x01;
	PS2_Data.PS2_UP			=	(tem >> 4) & 0x01;
	PS2_Data.PS2_RIGHT		=	(tem >> 5) & 0x01;
	PS2_Data.PS2_DOWN		=	(tem >> 6) & 0x01;
	PS2_Data.PS2_LEFT		=	(tem >> 7) & 0x01;

	/* ��5���ֽ� */
	tem = SPI_SwapByte(0x00);		// ת���������ڿ������Ĵ��𶯵����0x40~0xFF�������ֵԽ�󣬵��ת��Խ�죬��Խ���ԣ�û���Թ���������

	PS2_Data.PS2_L2 = tem & 0x01;
	PS2_Data.PS2_R2 = (tem >> 1) & 0x01;
	PS2_Data.PS2_L1 = (tem >> 2) & 0x01;
	PS2_Data.PS2_R1 = (tem >> 3) & 0x01;
	PS2_Data.PS2_Triangle = (tem >> 4) & 0x01;
	PS2_Data.PS2_Circle = (tem >> 5) & 0x01;
	PS2_Data.PS2_Fork = (tem >> 6) & 0x01;
	PS2_Data.PS2_Rectangle = (tem >> 7) & 0x01;

	/* ��6���ֽ� */
	PS2_Data.PS2_RX = SPI_SwapByte(0x00);		// ���⴫���ֽڣ����ڽ�������

	/* ��7���ֽ� */
	PS2_Data.PS2_RY = SPI_SwapByte(0x00);		// ���⴫���ֽڣ����ڽ�������

	/* ��8���ֽ� */
	PS2_Data.PS2_LX = SPI_SwapByte(0x00);		// ���⴫���ֽڣ����ڽ�������

	/* ��9���ֽ� */
	PS2_Data.PS2_LY = SPI_SwapByte(0x00);		// ���⴫���ֽڣ����ڽ�������

	SPI_Stop();			// ����ͨ��


	/* �ж��ֱ�ģʽ */
	if (PS2_Data.PS2_ID == 0x41)
		PS2_Data.PS2_Mode = 1;				// ҡ��������ģʽ
	else if (PS2_Data.PS2_ID == 0x73)
		PS2_Data.PS2_Mode = 2;				// ҡ��ģ����ģʽ
	else
		PS2_Data.PS2_Mode = 0;				// �ֱ�δ����
}



