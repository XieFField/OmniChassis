/**
 * @file		PS2ToSlave1_API.h
 * @brief		��������PS2���ݸ��ӻ�1����ӿ�(API)ͷ�ļ�
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��11��
 */


#ifndef __PS2ToSlave1_API_h
#define __PS2ToSlave1_API_h


#include <stdio.h>
#include "PS2ToSlave1_Demo.h"


void PS2ToSlave1_Init(void);
void PS2ToSlave1_SetPoint_PS2Data(uint8_t* DataPoint);
void PS2ToSlave1_SendData(void);


// ������λ��"PS2ToSlave1.c�ļ�"�ĺ���δ������"�ļ��ھ�̬����"
// �ж����覴ã������Ҳ����޸���
//PS2_DataTypeDef* PS2FromMaster_GetPoint_PS2Data(void);
//int8_t PS2FromMaster_ReadData(void);


#endif
