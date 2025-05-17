/**
 * @platform	keil5平台STM32F103C8T6型号单片机
 * @mainpage	机器人学院十期项目设计课第5、6组单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年5月16日
 */


#include <stdint.h>
#include "VL5300_API.h"


 // 定义采样窗口大小（建议奇数，如5或7）
#define MEDIAN_FILTER_WINDOW_SIZE 3


uint16_t MedianFilter1(void) 
{
    VL5300_DataSetTypeDef* VL5300;
    VL5300 = VL5300_GetDataSetPoint();


    uint16_t samples[MEDIAN_FILTER_WINDOW_SIZE];

    // 1. 连续采样N次
    for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE; i++) 
    {
        VL5300_ReadData();
        samples[i] = VL5300->Front.Correction_TOF;
    }

    // 2. 对采样值进行排序（冒泡排序）
    for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE - 1; i++) {
        for (int j = 0; j < MEDIAN_FILTER_WINDOW_SIZE - i - 1; j++) {
            if (samples[j] > samples[j + 1]) {
                uint16_t temp = samples[j];
                samples[j] = samples[j + 1];
                samples[j + 1] = temp;
            }
        }
    }

    // 3. 返回中间值
    return samples[MEDIAN_FILTER_WINDOW_SIZE / 2];
}

uint16_t MedianFilter2(void)
{
    VL5300_DataSetTypeDef* VL5300;
    VL5300 = VL5300_GetDataSetPoint();


    uint16_t samples[MEDIAN_FILTER_WINDOW_SIZE];

    // 1. 连续采样N次
    for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE; i++)
    {
        VL5300_ReadData();
        samples[i] = VL5300->Left.Correction_TOF;
    }

    // 2. 对采样值进行排序（冒泡排序）
    for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE - 1; i++) {
        for (int j = 0; j < MEDIAN_FILTER_WINDOW_SIZE - i - 1; j++) {
            if (samples[j] > samples[j + 1]) {
                uint16_t temp = samples[j];
                samples[j] = samples[j + 1];
                samples[j + 1] = temp;
            }
        }
    }

    // 3. 返回中间值
    return samples[MEDIAN_FILTER_WINDOW_SIZE / 2];
}

uint16_t MedianFilter3(void)
{
    VL5300_DataSetTypeDef* VL5300;
    VL5300 = VL5300_GetDataSetPoint();


    uint16_t samples[MEDIAN_FILTER_WINDOW_SIZE];

    // 1. 连续采样N次
    for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE; i++)
    {
        VL5300_ReadData();
        samples[i] = VL5300->Right.Correction_TOF;
    }

    // 2. 对采样值进行排序（冒泡排序）
    for (int i = 0; i < MEDIAN_FILTER_WINDOW_SIZE - 1; i++) {
        for (int j = 0; j < MEDIAN_FILTER_WINDOW_SIZE - i - 1; j++) {
            if (samples[j] > samples[j + 1]) {
                uint16_t temp = samples[j];
                samples[j] = samples[j + 1];
                samples[j + 1] = temp;
            }
        }
    }

    // 3. 返回中间值
    return samples[MEDIAN_FILTER_WINDOW_SIZE / 2];
}
