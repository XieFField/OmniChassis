/**
 * @file		Delay_API_z.h
 * @brief		ϵͳ�δ�ʱ��(SysTick)ʵ������ʽ��ʱ����ӿ�(API)ͷ�ļ�
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��14��
 */


#ifndef __Delay_API_z_h
#define __Delay_API_z_h


#include <stdint.h>


void Delay_ns(uint32_t ns);
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);


#endif
