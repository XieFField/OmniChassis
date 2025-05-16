/**
 * @file		OLED.c
 * @brief		0.96��OLED��(4��I2CЭ�����ͨ��)��������Դ�ļ�
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��14��
 */


#include <stdint.h>
#include "OLED_I2C_API.h"
#include "OLED_Font.h"


#define OLED_I2C_Address		0x78		// д��ַΪ 0111 1000


// ����ʹ�õ��ǿ�©���ģʽ��

// ������ʱ���Ż�(Optimize for Time),O0�������Ż�
// Program Size: Code=2318 RO-data=1858 RW-data=4 ZI-data=1132  
// ʵ��ͨ��ʱ��Ƶ��Լ500kHz

// ����ʱ���Ż�(Optimize for Time),O3�������Ż�
// Program Size: Code=2672 RO-data=1856 RW-data=4 ZI-data=1132  
// ʵ��ͨ��ʱ��Ƶ��Լ1.15MHz

/**
 * @brief       OLED�η�����
 * @param       ��
 * @retval		X��Y�η�
 */
static uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

static void OLED_WriteData(uint8_t Data)
{
	I2C_Start();

	I2C_SendByte(OLED_I2C_Address);		/* �ӻ���ַ */
	I2C_SendByte(0x40);					/* д���� */
	I2C_SendByte(Data);

	I2C_Stop();
}

static void OLED_WriteCommand(uint8_t Command)
{
	I2C_Start();

	I2C_SendByte(OLED_I2C_Address);		/* �ӻ���ַ */
	I2C_SendByte(0x00);					/* д���� */
	I2C_SendByte(Command);

	I2C_Stop();
}

/**
 * @brief       OLED���λ������
 * @brief		X*Y �� ��*��
 * @brief		1��X��ʾ1�����أ�1��Y��ʾ8������
 * @brief		1��Y��һ���ֽڣ�����8������������ص�
 * @brief		OLED��ʾ�������ص��� 128*64
 * @param       Y �����Ͻ�Ϊԭ�㣬���·�������꣬��Χ��0~7
 * @param		X �����Ͻ�Ϊԭ�㣬���ҷ�������꣬��Χ��0~127
 * @retval		��
 */
static void OLED_SetCursor(uint8_t X, uint8_t Y)
{
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	// ����Xλ�ø�4λ
	OLED_WriteCommand(0x00 | (X & 0x0F));			// ����Xλ�õ�4λ
	OLED_WriteCommand(0xB0 | Y);					// ����Yλ��
}

/**
 * @brief       OLED����
 * @param       ��
 * @retval      ��
 */
void OLED_Clear(void)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		OLED_SetCursor(0, i);
		for (uint8_t j = 0; j < 128; j++)
			OLED_WriteData(0x00);
	}
}

static inline void OLED_HW_Init(void)
{
	// �ӡ��ر���ʾ������������ʾ����һ����ָ��󲿷���Ҳ��֪��Ϊʲô���������ã������ü���
	OLED_WriteCommand(0xAE);			/* �ر���ʾ */

	OLED_WriteCommand(0xD5);			/* ������ʾʱ�ӷ�Ƶ��/����Ƶ�� */
	OLED_WriteCommand(0x80);

	OLED_WriteCommand(0xA8);			/* ���ö�·������ */
	OLED_WriteCommand(0x3F);

	OLED_WriteCommand(0xD3);			/* ������ʾƫ�� */
	OLED_WriteCommand(0x00);

	OLED_WriteCommand(0x40);			/* ������ʾ��ʼ�� */

	OLED_WriteCommand(0xA1);			/* �������ҷ���0xA1���� 0xA0���ҷ��� */

	OLED_WriteCommand(0xC8);			/* �������·���0xC8���� 0xC0���·��� */

	OLED_WriteCommand(0xDA);			/* ����COM����Ӳ������ */
	OLED_WriteCommand(0x12);

	OLED_WriteCommand(0x81);			/* ���öԱȶȿ��� */
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);			/* ����Ԥ������� */
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);			/* ����VCOMHȡ��ѡ�񼶱� */
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0x20);			/* ���õ�ַѰַģʽ */
	OLED_WriteCommand(0x02);

	OLED_WriteCommand(0xA4);			/* ����������ʾ��/�ر� */

	OLED_WriteCommand(0xA6);			/* ��������/��ת��ʾ */

	OLED_WriteCommand(0x8D);			/* ���ó��� */
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);			/* ������ʾ */
	 
	OLED_Clear();						/* OLED���� */
}

/**
 * @brief       OLED��ʾһ���ַ�
 * @brief		��OLED�ķֱ����� ��128*��64 ���� ��8*��16 ��ʾһ���ַ�
 * @brief		һ���ֽڿ���8�����ŵ����ص����������д���ֽ����ҿ�����8�����ŵ����ص������
 * @param       Line ��ʼ��λ�ã���Χ��1~4
 * @param		Column ��ʼ��λ�ã���Χ��1~16
 * @param		Char Ҫ��ʾ��һ���ַ�����Χ��ASCII�ɼ��ַ�
 * @retval		��
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
	/* ������ַ����ϰ벿�� */ 
	OLED_SetCursor((Column - 1) * 8, (Line - 1) * 2);		/* ���ù��λ�����ϰ벿�� */ 
	for (uint8_t i = 0; i < 8; i++)
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			/* ��ʾ�ϰ벿������ */ 

	/* ������ַ����°벿�� */
	OLED_SetCursor((Column - 1) * 8, (Line - 1) * 2 + 1);	/* ���ù��λ�����°벿�� */ 
	for (uint8_t i = 0; i < 8; i++)
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		/* ��ʾ�°벿������ */ 
}



/**
 * @brief       OLED��ʾ�ַ���
 * @param       Line ��ʼ��λ�ã���Χ��1~4
 * @param		Column ��ʼ��λ�ã���Χ��1~16
 * @param		String Ҫ��ʾ���ַ�������Χ��ASCII�ɼ��ַ�
 * @retval		��
 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char* String)
{
	for (uint8_t i = 0; String[i] != '\0'; i++)
		OLED_ShowChar(Line, Column + i, String[i]);
}

/**
 * @brief       OLED��ʾ���֣�ʮ���ƣ�uint32_t�������ݣ�
 * @brief		�� Length > Number ʱ�������ݺ�����0���� Length < Number ʱ�������ݵĵ�λ�ضϣ�������λ
 * @param       Line ��ʼ��λ�ã���Χ��1~4
 * @param		Column ��ʼ��λ�ã���Χ��1~16
 * @param		Number Ҫ��ʾ�����֣���Χ��0~4294967295
 * @param		Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~10
 * @retval		��
 */
void OLED_Show_u32(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	for (u8 i = 0; i < Length; i++)
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
}

/**
  * @brief  OLED��ʾ���֣�ʮ���ƣ�int32_t�������ݣ�����ֵǰ��������ţ�ռһ����ʾ���ȣ�
  * @brief  �� Length > Number ʱ�������ݺ�����0
  * @brief  �� Length < Number ʱ�������ݵĵ�λ�ضϣ�������λ
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��-2147483648~2147483647
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~10
  * @retval ��
  */
void OLED_Show_s32(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint32_t AbsoluteNum;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		AbsoluteNum = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		AbsoluteNum = -Number;
	}
	OLED_Show_u32(Line, Column + 1, AbsoluteNum, Length - 1);
}

/**
  * @brief  OLED��ʾ���֣�ʮת�����ƣ�������
  * @brief ������������ʾ��Χ 0~1111 1111 1111 1111
  * @brief �� Length > ʵ������λ�� ʱ����ʵ������ǰ����0
  * @brief �� Length < ʵ������λ�� ʱ����ʵ�����ݵĵ�λ�ضϣ�������λ
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ�����ʮ�������֣���Χ��0~255
  * @param  Length ������ʾ���������ֵĳ��ȣ���Χ��1~16
  * @retval ��
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	for (u8 i = 0; i < Length; i++)
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');		// �о���ʮת�����Ƶ��Ǹ���ʽ�ر����棬��Ҳû���о�ԭ�������ü���
																								// ��ʵ������Գ���������λ������ת�������ƣ���������
}

/**
  * @brief  OLED��ʾ���֣�ʮתʮ�����ƣ�������
  * @brief  ʮ������������ʾ��Χ 0~0xFFFF FFFF
  * @brief  �� Length > ʵ������λ�� ʱ����ʵ������ǰ����0
  * @brief  �� Length < ʵ������λ�� ʱ����ʵ�����ݵĵ�λ�ضϣ�������λ
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ�����ʮ�������֣���Χ��0~4294967295
  * @param  Length ������ʾʮ���������ֵĳ��ȣ���Χ��1~8
  * @retval ��
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	u8 SingleNumber;
	for (u8 i = 0; i < Length; i++)
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		else
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
	}
}

void OLED_Init(void)
{
	MyI2C_Init();

	OLED_HW_Init();
}
