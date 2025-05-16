/**
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��14��
 */


#ifndef __ATK_IMU901_API_h
#define __ATK_IMU901_API_h


#include <stdint.h>


/**
  * @brief  �洢��ȡATK_IMU901�����ݵĽṹ��
  */
typedef struct
{
	float Roll;			// ��ת�ǣ�X�ᣩ��Roll������Χ��-180��-+180�ȣ���λ���ȣ�������λС������
	float Pitch;		// �����ǣ�Y�ᣩ��Pitch������Χ��-90��-+90�ȣ���λ���ȣ�������λС������
	float Yaw;			// ƫ���ǣ�Z�ᣩ��Yaw������Χ��-180��-+180�ȣ���λ���ȣ�������λС������
	float AccX;			// ���ٶȼ�X�����ݣ���λ��G
	float AccY; 		// ���ٶȼ�Y�����ݣ���λ��G
	float AccZ;			// ���ٶȼ�Z�����ݣ���λ��G
	float GyroX;		// ������X�����ݣ���λ����/s
	float GyroY; 		// ������Y�����ݣ���λ����/s
	float GyroZ;		// ������Z�����ݣ���λ����/s
}ATK_IMU901_DataTypeDef;


void ATK_IMU901_Init(void);
ATK_IMU901_DataTypeDef* ATK_IMU901_GetPoint_Data(void);
void ATK_IMU901_ReadData(void);


void ATK_IMU901_Demo(void);


#endif
