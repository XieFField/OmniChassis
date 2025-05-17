/**
 * @platform	keil5ƽ̨STM32F103C8T6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ����Ŀ��ƿε�5��6�鵥Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��5��16��
 */


#include <stdint.h>
#include "VL5300_API.h"


 // ����������ڴ�С��������������5��7��
#define MEDIAN_FILTER_WINDOW_SIZE 3


uint16_t MedianFilter1(void) 
{
    VL5300_DataSetTypeDef* VL5300;
    VL5300 = VL5300_GetDataSetPoint();


    uint16_t samples[MEDIAN_FILTER_WINDOW_SIZE];

    // 1. ��������N��
    for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE; i++) 
    {
        VL5300_ReadData();
        samples[i] = VL5300->Front.Correction_TOF;
    }

    // 2. �Բ���ֵ��������ð������
    for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE - 1; i++) {
        for (int j = 0; j < MEDIAN_FILTER_WINDOW_SIZE - i - 1; j++) {
            if (samples[j] > samples[j + 1]) {
                uint16_t temp = samples[j];
                samples[j] = samples[j + 1];
                samples[j + 1] = temp;
            }
        }
    }

    // 3. �����м�ֵ
    return samples[MEDIAN_FILTER_WINDOW_SIZE / 2];
}

uint16_t MedianFilter2(void)
{
    VL5300_DataSetTypeDef* VL5300;
    VL5300 = VL5300_GetDataSetPoint();


    uint16_t samples[MEDIAN_FILTER_WINDOW_SIZE];

    // 1. ��������N��
    for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE; i++)
    {
        VL5300_ReadData();
        samples[i] = VL5300->Left.Correction_TOF;
    }

    // 2. �Բ���ֵ��������ð������
    for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE - 1; i++) {
        for (int j = 0; j < MEDIAN_FILTER_WINDOW_SIZE - i - 1; j++) {
            if (samples[j] > samples[j + 1]) {
                uint16_t temp = samples[j];
                samples[j] = samples[j + 1];
                samples[j + 1] = temp;
            }
        }
    }

    // 3. �����м�ֵ
    return samples[MEDIAN_FILTER_WINDOW_SIZE / 2];
}

uint16_t MedianFilter3(void)
{
    VL5300_DataSetTypeDef* VL5300;
    VL5300 = VL5300_GetDataSetPoint();


    uint16_t samples[MEDIAN_FILTER_WINDOW_SIZE];

    // 1. ��������N��
    for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE; i++)
    {
        VL5300_ReadData();
        samples[i] = VL5300->Right.Correction_TOF;
    }

    // 2. �Բ���ֵ��������ð������
    for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE - 1; i++) {
        for (int j = 0; j < MEDIAN_FILTER_WINDOW_SIZE - i - 1; j++) {
            if (samples[j] > samples[j + 1]) {
                uint16_t temp = samples[j];
                samples[j] = samples[j + 1];
                samples[j + 1] = temp;
            }
        }
    }

    // 3. �����м�ֵ
    return samples[MEDIAN_FILTER_WINDOW_SIZE / 2];
}
