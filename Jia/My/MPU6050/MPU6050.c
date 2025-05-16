/**
 * @file		MPU6050.c
 * @brief		MPU6050(I2C协议软件通信)操作函数源文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月13日
 */


// 其实老实说，我也不是很懂这个6050芯片的使用情况和使用方法

// 引脚使用的是开漏输出模式！

// 在不开启时间优化和O0优化的情况下
// 实测通信时钟频率约500kHz
// 读一次单个六轴之一的数据需要约77us
// 读一次全部六轴数据需要约0.97ms

// 在开启时间优化和O3优化的情况下
// 实测通信时钟频率约1.30MHz
// 读一次单个六轴之一的数据需要约32us
// 读一次全部六轴数据需要约0.40ms


#include <stdint.h>
#include "stm32f10x.h"
#include "Delay_API_z.h"
#include "MPU6050_I2C_API.h"
#include "MPU6050_Reg.h"
#include "MPU6050_API.h"


#define MPU6050_I2C_Address			0xD0		// 写地址，当AD0置低电平时为0xD0（11010000），当AD0置高电平时为0xD2（11010010）

// 设置陀螺仪（单位：度/s）和加速度计（单位：G）的量程
#define AccelerationRange		2
#define GyroscopeRange 			500


static MPU6050_DataTypeDef MPU6050_Data;


 /**
   * @brief	MPU6050读寄存器
   * @param  RegAddress 寄存器地址，范围：参考MPU6050手册的寄存器描述
   * @retval 读取寄存器的数据，范围：0x00~0xFF
   */
static uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;

	I2C_Start();							// 生成开始条件

	I2C_SendByte(MPU6050_I2C_Address);		// I2C发送写从机地址

	I2C_SendByte(RegAddress);				// I2C发送从机寄存器地址

	I2C_Start();							// 重复生成开始条件

	I2C_SendByte(MPU6050_I2C_Address + 1);	// I2C发送读从机地址

	Data = I2C_ReadByte(0);

	return Data;
}

/**
  * @brief	MPU6050写寄存器
  * @param  RegAddress 寄存器地址，范围：参考MPU6050手册的寄存器描述
  * @retval Data 要写入寄存器的数据，范围：0x00~0xFF
  */
static void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	I2C_Start();							// 生成开始条件

	I2C_SendByte(MPU6050_I2C_Address);		// I2C发送写从机地址

	I2C_SendByte(RegAddress);				// I2C发送从机寄存器地址

	I2C_SendByte(Data);						// I2C主机发送数据

	I2C_Stop();								// 生成终止条件
}

/**
  * @brief	MPU6050硬件初始化
  * @param  无
  * @retval 无
  */
static void MPU6050_HW_Init(void)
{
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);				//	电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);				//	电源管理寄存器2，保持默认值0，所有轴均不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);				//	采样率分频寄存器，配置采样率
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);					//	配置寄存器，配置DLPF
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x08);			//	陀螺仪配置寄存器，选择满量程为±500°/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x00);			//	加速度计配置寄存器，选择满量程为±2g
}

/**
  * @brief	MPU6050
  * @param  无
  * @retval 无
  */
uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);		// 返回WHO_AM_I寄存器的值
}

MPU6050_DataTypeDef* MPU6050_GetPoint_Data(void)
{
	return &MPU6050_Data;
}

/**
  * @brief	MPU6050获取数据
  * @param  
  * @retval 无
  */
MPU6050_DataTypeDef* MPU6050_ReadData(void)
{
	uint8_t DataH, DataL;																			// 定义数据高8位和低8位的变量
																									   
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);													// 读取加速度计X轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);													// 读取加速度计X轴的低8位数据
	MPU6050_Data.AccX = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * AccelerationRange;		// 数据拼接，通过输出参数返回
														   
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);													// 读取加速度计Y轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);													// 读取加速度计Y轴的低8位数据
	MPU6050_Data.AccY = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * AccelerationRange;		// 数据拼接，通过输出参数返回

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);													// 读取加速度计Z轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);													// 读取加速度计Z轴的低8位数据
	MPU6050_Data.AccZ = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * AccelerationRange;		// 数据拼接，通过输出参数返回
														   
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);													// 读取陀螺仪X轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);													// 读取陀螺仪X轴的低8位数据
	MPU6050_Data.GyroX = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * GyroscopeRange;			// 数据拼接，通过输出参数返回
														   
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);													// 读取陀螺仪Y轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);													// 读取陀螺仪Y轴的低8位数据
	MPU6050_Data.GyroY = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * GyroscopeRange;			// 数据拼接，通过输出参数返回
														   
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);													// 读取陀螺仪Z轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);													// 读取陀螺仪Z轴的低8位数据
	MPU6050_Data.GyroZ = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * GyroscopeRange;			// 数据拼接，通过输出参数返回

	return &MPU6050_Data;
}

/**
  * @brief	MPU6050初始化
  * @param  无
  * @retval 无
  */
void MPU6050_Init(void)
{
	MyI2C_Init();

	MPU6050_HW_Init();
}



