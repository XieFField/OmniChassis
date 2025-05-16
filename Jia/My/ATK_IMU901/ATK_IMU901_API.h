/**
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月14日
 */


#ifndef __ATK_IMU901_API_h
#define __ATK_IMU901_API_h


#include <stdint.h>


/**
  * @brief  存储读取ATK_IMU901的数据的结构体
  */
typedef struct
{
	float Roll;			// 滚转角（X轴）（Roll），范围：-180度-+180度，单位：度，建议两位小数精度
	float Pitch;		// 俯仰角（Y轴）（Pitch），范围：-90度-+90度，单位：度，建议两位小数精度
	float Yaw;			// 偏航角（Z轴）（Yaw），范围：-180度-+180度，单位：度，建议两位小数精度
	float AccX;			// 加速度计X轴数据，单位：G
	float AccY; 		// 加速度计Y轴数据，单位：G
	float AccZ;			// 加速度计Z轴数据，单位：G
	float GyroX;		// 陀螺仪X轴数据，单位：度/s
	float GyroY; 		// 陀螺仪Y轴数据，单位：度/s
	float GyroZ;		// 陀螺仪Z轴数据，单位：度/s
}ATK_IMU901_DataTypeDef;


void ATK_IMU901_Init(void);
ATK_IMU901_DataTypeDef* ATK_IMU901_GetPoint_Data(void);
void ATK_IMU901_ReadData(void);


void ATK_IMU901_Demo(void);


#endif
