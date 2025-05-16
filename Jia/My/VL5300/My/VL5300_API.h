/**
 * @platform	keil5平台STM32F103C8T6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车从2单片机(型号F103C8T6)
 * @author		桑叁
 * @date		2025年4月15日
 */


#ifndef __VL5300_API_h
#define __VL5300_API_h


#include <stdint.h>


typedef struct
{
    // 校正的tof
    int16_t Correction_TOF;
    // 置信度
    uint8_t Confidence;
    // 积分次数
    uint32_t Intecounts;
    // Peak
    uint32_t Peak;
    // Noise
    uint16_t Noise;
    // 好像是串扰计数器或串扰补偿参数
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
