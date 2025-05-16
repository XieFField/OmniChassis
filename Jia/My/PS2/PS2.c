/**
 * @file		PS2.c
 * @brief		PS2手柄(软件SPI协议通信)操作函数源文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月10日
 */


// 读取一次手柄数据需要交换9个字节，加上开始和终止时序一共220个阻塞式Delay

// 开启时间优化(Optimize for Time),O3级代码优化
// Program Size: Code=2736 RO-data=1856 RW-data=4 ZI-data=1148  
// Delay3us，一次读取时间0.87ms
// Delay4us，一次读取时间1.08ms
// Delay5us，一次读取时间1.30ms
// Delay10us，一次读取时间2.39ms
// Delay50us，一次读取时间11.07ms
// 线性回归(好像是最小二乘法)：读取时间(ms) = 延时时间(us) * 0.217 + 0.216，拟合误差0.999999

// 不开启时间优化(Optimize for Time),O3级代码优化
// Program Size: Code=2302 RO-data=1858 RW-data=4 ZI-data=1148  
// Delay10us，一次读取时间2.41ms

// 不开启时间优化(Optimize for Time),O2级代码优化
// Program Size: Code=2294 RO-data=1858 RW-data=4 ZI-data=1148  
// Delay10us，一次读取时间2.41ms

// 不开启时间优化(Optimize for Time),O1级代码优化
// Program Size: Code=2334 RO-data=1858 RW-data=4 ZI-data=1148  
// Delay10us，一次读取时间2.41ms

// 不开启时间优化(Optimize for Time),O0级代码优化
// Program Size: Code=2722 RO-data=1858 RW-data=4 ZI-data=1148  
// Delay4us，一次读取时间1.18ms
// Delay10us，一次读取时间2.48ms


#include <stdint.h>
#include "PS2_Def.h"
#include "PS2_SPI_API.h"


static PS2_DataTypeDef PS2_Data;


/* 手柄接口初始化 */
void PS2_Init(void)
{
	MySPI_Init();
}

PS2_DataTypeDef* PS2_GetDataPoint(void)
{
	return &PS2_Data;
}

/**
  * @brief  读取手柄数据
  * @param  无
  * @retval 无
  */
void PS2_ReadData(void)
{
	u8 tem = 0x00;

	/* 手柄数据读取 */ 
	SPI_Start();			// 开始通信

	/* 第1个字节 */
	SPI_SwapByte(0x01);		// 0x01 和 0x42 是手柄命令，详见手柄使用说明书
	
	/* 第2个字节 */
	PS2_Data.PS2_ID = SPI_SwapByte(0x42);		// 0x01 和 0x42 是手柄命令，详见手柄使用说明书

	/* 第3个字节 */
	SPI_SwapByte(0x00);		// 没测试过传入字节的功能

	/* 第4个字节 */
	tem = SPI_SwapByte(0x00);		// 转入数据用于控制右侧的小震动电机，0x00关，其他值为开，没测试过，不包对

	PS2_Data.PS2_SELECT		=	tem & 0x01;
	PS2_Data.PS2_L3			=	(tem >> 1) & 0x01;
	PS2_Data.PS2_R3			=	(tem >> 2) & 0x01;
	PS2_Data.PS2_START		=	(tem >> 3) & 0x01;
	PS2_Data.PS2_UP			=	(tem >> 4) & 0x01;
	PS2_Data.PS2_RIGHT		=	(tem >> 5) & 0x01;
	PS2_Data.PS2_DOWN		=	(tem >> 6) & 0x01;
	PS2_Data.PS2_LEFT		=	(tem >> 7) & 0x01;

	/* 第5个字节 */
	tem = SPI_SwapByte(0x00);		// 转入数据用于控制左侧的大震动电机，0x40~0xFF电机开，值越大，电机转动越快，震动越明显，没测试过，不包对

	PS2_Data.PS2_L2 = tem & 0x01;
	PS2_Data.PS2_R2 = (tem >> 1) & 0x01;
	PS2_Data.PS2_L1 = (tem >> 2) & 0x01;
	PS2_Data.PS2_R1 = (tem >> 3) & 0x01;
	PS2_Data.PS2_Triangle = (tem >> 4) & 0x01;
	PS2_Data.PS2_Circle = (tem >> 5) & 0x01;
	PS2_Data.PS2_Fork = (tem >> 6) & 0x01;
	PS2_Data.PS2_Rectangle = (tem >> 7) & 0x01;

	/* 第6个字节 */
	PS2_Data.PS2_RX = SPI_SwapByte(0x00);		// 随意传入字节，用于交换数据

	/* 第7个字节 */
	PS2_Data.PS2_RY = SPI_SwapByte(0x00);		// 随意传入字节，用于交换数据

	/* 第8个字节 */
	PS2_Data.PS2_LX = SPI_SwapByte(0x00);		// 随意传入字节，用于交换数据

	/* 第9个字节 */
	PS2_Data.PS2_LY = SPI_SwapByte(0x00);		// 随意传入字节，用于交换数据

	SPI_Stop();			// 结束通信


	/* 判断手柄模式 */
	if (PS2_Data.PS2_ID == 0x41)
		PS2_Data.PS2_Mode = 1;				// 摇杆数字量模式
	else if (PS2_Data.PS2_ID == 0x73)
		PS2_Data.PS2_Mode = 2;				// 摇杆模拟量模式
	else
		PS2_Data.PS2_Mode = 0;				// 手柄未连接
}



