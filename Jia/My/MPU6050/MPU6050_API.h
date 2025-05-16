/**
 * @platform	keil5平台STM32F103C8T6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月13日
 */


#ifndef __MPU6050_API_h
#define __MPU6050_API_h


#include <stdint.h>


/**
  * @brief  存储读取ATK_IMU901的数据的结构体
  */
typedef struct
{
	float AccX;		// 加速度计X轴数据，单位：G
	float AccY; 	// 加速度计Y轴数据，单位：G
	float AccZ;		// 加速度计Z轴数据，单位：G
	float GyroX;	// 陀螺仪X轴数据，单位：度/s
	float GyroY; 	// 陀螺仪Y轴数据，单位：度/s
	float GyroZ;	// 陀螺仪Z轴数据，单位：度/s
}MPU6050_DataTypeDef;


void MPU6050_Init(void);
uint8_t MPU6050_GetID(void);
MPU6050_DataTypeDef* MPU6050_GetPoint_Data(void);
MPU6050_DataTypeDef* MPU6050_ReadData(void);


void MPU6050_Demo(void);


#endif
