/**
 * @platform	keil5ƽ̨STM32F103C8T6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��13��
 */


#ifndef __MPU6050_API_h
#define __MPU6050_API_h


#include <stdint.h>


/**
  * @brief  �洢��ȡATK_IMU901�����ݵĽṹ��
  */
typedef struct
{
	float AccX;		// ���ٶȼ�X�����ݣ���λ��G
	float AccY; 	// ���ٶȼ�Y�����ݣ���λ��G
	float AccZ;		// ���ٶȼ�Z�����ݣ���λ��G
	float GyroX;	// ������X�����ݣ���λ����/s
	float GyroY; 	// ������Y�����ݣ���λ����/s
	float GyroZ;	// ������Z�����ݣ���λ����/s
}MPU6050_DataTypeDef;


void MPU6050_Init(void);
uint8_t MPU6050_GetID(void);
MPU6050_DataTypeDef* MPU6050_GetPoint_Data(void);
MPU6050_DataTypeDef* MPU6050_ReadData(void);


void MPU6050_Demo(void);


#endif
