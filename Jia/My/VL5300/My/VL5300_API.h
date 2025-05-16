/**
 * @platform	keil5ƽ̨STM32F103C8T6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ����Ŀ��ƿε�5��6�鵥Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��5��16��
 */


#ifndef __VL5300_API_h
#define __VL5300_API_h


#include <stdint.h>


typedef struct
{
    // У����tof
    uint16_t Correction_TOF;
    // ���Ŷ�
    uint8_t Confidence;
    // ���ִ���
    uint32_t Intecounts;
    // Peak
    uint32_t Peak;
    // Noise
    uint16_t Noise;
    // �����Ǵ��ż��������Ų�������
    uint16_t XTALK_Count;
}VL5300_DataTypeDef;

typedef struct
{
    VL5300_DataTypeDef Front;
    VL5300_DataTypeDef Left;
    VL5300_DataTypeDef Right;
}VL5300_DataSetTypeDef;


void VL5300_Init(void);
VL5300_DataSetTypeDef* VL5300_GetDataSetPoint(void);
void VL5300_ReadData(void);


void VL5300_Demo(void);



#endif
