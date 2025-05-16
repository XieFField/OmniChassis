/**
 * @file		MPU6050.c
 * @brief		MPU6050(I2CЭ�����ͨ��)��������Դ�ļ�
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��13��
 */


// ��ʵ��ʵ˵����Ҳ���Ǻܶ����6050оƬ��ʹ�������ʹ�÷���

// ����ʹ�õ��ǿ�©���ģʽ��

// �ڲ�����ʱ���Ż���O0�Ż��������
// ʵ��ͨ��ʱ��Ƶ��Լ500kHz
// ��һ�ε�������֮һ��������ҪԼ77us
// ��һ��ȫ������������ҪԼ0.97ms

// �ڿ���ʱ���Ż���O3�Ż��������
// ʵ��ͨ��ʱ��Ƶ��Լ1.30MHz
// ��һ�ε�������֮һ��������ҪԼ32us
// ��һ��ȫ������������ҪԼ0.40ms


#include <stdint.h>
#include "stm32f10x.h"
#include "Delay_API_z.h"
#include "MPU6050_I2C_API.h"
#include "MPU6050_Reg.h"
#include "MPU6050_API.h"


#define MPU6050_I2C_Address			0xD0		// д��ַ����AD0�õ͵�ƽʱΪ0xD0��11010000������AD0�øߵ�ƽʱΪ0xD2��11010010��

// ���������ǣ���λ����/s���ͼ��ٶȼƣ���λ��G��������
#define AccelerationRange		2
#define GyroscopeRange 			500


static MPU6050_DataTypeDef MPU6050_Data;


 /**
   * @brief	MPU6050���Ĵ���
   * @param  RegAddress �Ĵ�����ַ����Χ���ο�MPU6050�ֲ�ļĴ�������
   * @retval ��ȡ�Ĵ��������ݣ���Χ��0x00~0xFF
   */
static uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;

	I2C_Start();							// ���ɿ�ʼ����

	I2C_SendByte(MPU6050_I2C_Address);		// I2C����д�ӻ���ַ

	I2C_SendByte(RegAddress);				// I2C���ʹӻ��Ĵ�����ַ

	I2C_Start();							// �ظ����ɿ�ʼ����

	I2C_SendByte(MPU6050_I2C_Address + 1);	// I2C���Ͷ��ӻ���ַ

	Data = I2C_ReadByte(0);

	return Data;
}

/**
  * @brief	MPU6050д�Ĵ���
  * @param  RegAddress �Ĵ�����ַ����Χ���ο�MPU6050�ֲ�ļĴ�������
  * @retval Data Ҫд��Ĵ��������ݣ���Χ��0x00~0xFF
  */
static void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	I2C_Start();							// ���ɿ�ʼ����

	I2C_SendByte(MPU6050_I2C_Address);		// I2C����д�ӻ���ַ

	I2C_SendByte(RegAddress);				// I2C���ʹӻ��Ĵ�����ַ

	I2C_SendByte(Data);						// I2C������������

	I2C_Stop();								// ������ֹ����
}

/**
  * @brief	MPU6050Ӳ����ʼ��
  * @param  ��
  * @retval ��
  */
static void MPU6050_HW_Init(void)
{
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);				//	��Դ����Ĵ���1��ȡ��˯��ģʽ��ѡ��ʱ��ԴΪX��������
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);				//	��Դ����Ĵ���2������Ĭ��ֵ0���������������
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);				//	�����ʷ�Ƶ�Ĵ��������ò�����
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);					//	���üĴ���������DLPF
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x08);			//	���������üĴ�����ѡ��������Ϊ��500��/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x00);			//	���ٶȼ����üĴ�����ѡ��������Ϊ��2g
}

/**
  * @brief	MPU6050
  * @param  ��
  * @retval ��
  */
uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);		// ����WHO_AM_I�Ĵ�����ֵ
}

MPU6050_DataTypeDef* MPU6050_GetPoint_Data(void)
{
	return &MPU6050_Data;
}

/**
  * @brief	MPU6050��ȡ����
  * @param  
  * @retval ��
  */
MPU6050_DataTypeDef* MPU6050_ReadData(void)
{
	uint8_t DataH, DataL;																			// �������ݸ�8λ�͵�8λ�ı���
																									   
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);													// ��ȡ���ٶȼ�X��ĸ�8λ����
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);													// ��ȡ���ٶȼ�X��ĵ�8λ����
	MPU6050_Data.AccX = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * AccelerationRange;		// ����ƴ�ӣ�ͨ�������������
														   
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);													// ��ȡ���ٶȼ�Y��ĸ�8λ����
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);													// ��ȡ���ٶȼ�Y��ĵ�8λ����
	MPU6050_Data.AccY = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * AccelerationRange;		// ����ƴ�ӣ�ͨ�������������

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);													// ��ȡ���ٶȼ�Z��ĸ�8λ����
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);													// ��ȡ���ٶȼ�Z��ĵ�8λ����
	MPU6050_Data.AccZ = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * AccelerationRange;		// ����ƴ�ӣ�ͨ�������������
														   
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);													// ��ȡ������X��ĸ�8λ����
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);													// ��ȡ������X��ĵ�8λ����
	MPU6050_Data.GyroX = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * GyroscopeRange;			// ����ƴ�ӣ�ͨ�������������
														   
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);													// ��ȡ������Y��ĸ�8λ����
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);													// ��ȡ������Y��ĵ�8λ����
	MPU6050_Data.GyroY = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * GyroscopeRange;			// ����ƴ�ӣ�ͨ�������������
														   
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);													// ��ȡ������Z��ĸ�8λ����
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);													// ��ȡ������Z��ĵ�8λ����
	MPU6050_Data.GyroZ = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * GyroscopeRange;			// ����ƴ�ӣ�ͨ�������������

	return &MPU6050_Data;
}

/**
  * @brief	MPU6050��ʼ��
  * @param  ��
  * @retval ��
  */
void MPU6050_Init(void)
{
	MyI2C_Init();

	MPU6050_HW_Init();
}



