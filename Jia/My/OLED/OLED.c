/**
 * @file		OLED.c
 * @brief		0.96寸OLED屏(4针I2C协议软件通信)操作函数源文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月14日
 */


#include <stdint.h>
#include "OLED_I2C_API.h"
#include "OLED_Font.h"


#define OLED_I2C_Address		0x78		// 写地址为 0111 1000


// 引脚使用的是开漏输出模式！

// 不开启时间优化(Optimize for Time),O0级代码优化
// Program Size: Code=2318 RO-data=1858 RW-data=4 ZI-data=1132  
// 实测通信时钟频率约500kHz

// 开启时间优化(Optimize for Time),O3级代码优化
// Program Size: Code=2672 RO-data=1856 RW-data=4 ZI-data=1132  
// 实测通信时钟频率约1.15MHz

/**
 * @brief       OLED次方函数
 * @param       无
 * @retval		X的Y次方
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

	I2C_SendByte(OLED_I2C_Address);		/* 从机地址 */
	I2C_SendByte(0x40);					/* 写数据 */
	I2C_SendByte(Data);

	I2C_Stop();
}

static void OLED_WriteCommand(uint8_t Command)
{
	I2C_Start();

	I2C_SendByte(OLED_I2C_Address);		/* 从机地址 */
	I2C_SendByte(0x00);					/* 写命令 */
	I2C_SendByte(Command);

	I2C_Stop();
}

/**
 * @brief       OLED光标位置设置
 * @brief		X*Y 是 底*高
 * @brief		1个X表示1个像素，1个Y表示8个像素
 * @brief		1个Y是一个字节，控制8个竖方向的像素点
 * @brief		OLED显示屏的像素点是 128*64
 * @param       Y 以左上角为原点，向下方向的坐标，范围：0~7
 * @param		X 以左上角为原点，向右方向的坐标，范围：0~127
 * @retval		无
 */
static void OLED_SetCursor(uint8_t X, uint8_t Y)
{
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	// 设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			// 设置X位置低4位
	OLED_WriteCommand(0xB0 | Y);					// 设置Y位置
}

/**
 * @brief       OLED清屏
 * @param       无
 * @retval      无
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
	// 从“关闭显示”到“开启显示”的一连串指令，大部分我也不知道为什么是这样设置，拿来用即可
	OLED_WriteCommand(0xAE);			/* 关闭显示 */

	OLED_WriteCommand(0xD5);			/* 设置显示时钟分频比/振荡器频率 */
	OLED_WriteCommand(0x80);

	OLED_WriteCommand(0xA8);			/* 设置多路复用率 */
	OLED_WriteCommand(0x3F);

	OLED_WriteCommand(0xD3);			/* 设置显示偏移 */
	OLED_WriteCommand(0x00);

	OLED_WriteCommand(0x40);			/* 设置显示开始行 */

	OLED_WriteCommand(0xA1);			/* 设置左右方向，0xA1正常 0xA0左右反置 */

	OLED_WriteCommand(0xC8);			/* 设置上下方向，0xC8正常 0xC0上下反置 */

	OLED_WriteCommand(0xDA);			/* 设置COM引脚硬件配置 */
	OLED_WriteCommand(0x12);

	OLED_WriteCommand(0x81);			/* 设置对比度控制 */
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);			/* 设置预充电周期 */
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);			/* 设置VCOMH取消选择级别 */
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0x20);			/* 设置地址寻址模式 */
	OLED_WriteCommand(0x02);

	OLED_WriteCommand(0xA4);			/* 设置整个显示打开/关闭 */

	OLED_WriteCommand(0xA6);			/* 设置正常/倒转显示 */

	OLED_WriteCommand(0x8D);			/* 设置充电泵 */
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);			/* 开启显示 */
	 
	OLED_Clear();						/* OLED清屏 */
}

/**
 * @brief       OLED显示一个字符
 * @brief		该OLED的分辨率是 底128*高64 ，用 底8*高16 显示一个字符
 * @brief		一个字节控制8个竖着的像素点的亮灭，连续写入字节向右控制下8个竖着的像素点的亮灭
 * @param       Line 起始行位置，范围：1~4
 * @param		Column 起始列位置，范围：1~16
 * @param		Char 要显示的一个字符，范围：ASCII可见字符
 * @retval		无
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
	/* 先输出字符的上半部分 */ 
	OLED_SetCursor((Column - 1) * 8, (Line - 1) * 2);		/* 设置光标位置在上半部分 */ 
	for (uint8_t i = 0; i < 8; i++)
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			/* 显示上半部分内容 */ 

	/* 后输出字符的下半部分 */
	OLED_SetCursor((Column - 1) * 8, (Line - 1) * 2 + 1);	/* 设置光标位置在下半部分 */ 
	for (uint8_t i = 0; i < 8; i++)
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		/* 显示下半部分内容 */ 
}



/**
 * @brief       OLED显示字符串
 * @param       Line 起始行位置，范围：1~4
 * @param		Column 起始列位置，范围：1~16
 * @param		String 要显示的字符串，范围：ASCII可见字符
 * @retval		无
 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char* String)
{
	for (uint8_t i = 0; String[i] != '\0'; i++)
		OLED_ShowChar(Line, Column + i, String[i]);
}

/**
 * @brief       OLED显示数字（十进制，uint32_t类型数据）
 * @brief		当 Length > Number 时，在数据后面添0；当 Length < Number 时，将数据的低位截断，保留高位
 * @param       Line 起始行位置，范围：1~4
 * @param		Column 起始列位置，范围：1~16
 * @param		Number 要显示的数字，范围：0~4294967295
 * @param		Length 要显示数字的长度，范围：1~10
 * @retval		无
 */
void OLED_Show_u32(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	for (u8 i = 0; i < Length; i++)
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
}

/**
  * @brief  OLED显示数字（十进制，int32_t类型数据）（数值前面带正负号，占一个显示长度）
  * @brief  当 Length > Number 时，在数据后面添0
  * @brief  当 Length < Number 时，将数据的低位截断，保留高位
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
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
  * @brief  OLED显示数字（十转二进制，正数）
  * @brief 二进制正数显示范围 0~1111 1111 1111 1111
  * @brief 当 Length > 实际数据位数 时，在实际数据前面添0
  * @brief 当 Length < 实际数据位数 时，将实际数据的低位截断，保留高位
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要输入的十进制数字，范围：0~255
  * @param  Length 留给显示二进制数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	for (u8 i = 0; i < Length; i++)
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');		// 感觉将十转二进制的那个公式特别神奇，我也没深研究原理，拿来用即可
																								// 其实或许可以尝试用以下位运算来转换二进制，但是我懒
}

/**
  * @brief  OLED显示数字（十转十六进制，正数）
  * @brief  十六进制正数显示范围 0~0xFFFF FFFF
  * @brief  当 Length > 实际数据位数 时，在实际数据前面添0
  * @brief  当 Length < 实际数据位数 时，将实际数据的低位截断，保留高位
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要输入的十进制数字，范围：0~4294967295
  * @param  Length 留给显示十六进制数字的长度，范围：1~8
  * @retval 无
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
