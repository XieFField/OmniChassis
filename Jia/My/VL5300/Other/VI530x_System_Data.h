/**
 * @platform	keil5平台STM32F103C8T6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车从2单片机(型号F103C8T6)
 * @author		桑叁
 * @date		2025年4月15日
 */


#ifndef __VI530x_SYSTEM_DATA_H
#define __VI530x_SYSTEM_DATA_H

/* Includes ------------------------------------------------------------------*/
#include "atk_tof.h"
#include "VI530x_API.h"
#include "VI530x_Firmware.h"
#include "VI530x_Algorithm.h"


//设置积分次数
uint8_t VI530x_Set_Sys_Integral_Time(uint8_t VL5300_I2C_Address, uint32_t integral_time);
//积分次数读取
uint8_t VI530x_Get_Sys_Integral_Time(uint32_t *integral_time);

//设置帧率
uint8_t VI530x_Set_FPS(uint8_t data_fps);
//读取帧率
uint8_t VI530x_Get_FPS(uint8_t *data_fps);

//设置delaytime
uint8_t VI530x_Set_Sys_DelayTime(uint8_t VL5300_I2C_Address,uint16_t delay_time);
//读取delaytime
uint8_t VI530x_Get_Sys_DelayTime(uint16_t *delay_time);
//设置帧率和积分次数
uint8_t VI530x_Set_Integralcounts_Frame(uint8_t VL5300_I2C_Address, uint8_t fps, uint32_t intecoutns);
uint8_t VI530x_Set_Sys_Reftof(uint8_t VL5300_I2C_Address, uint16_t reftof);
uint8_t VI530x_Get_Sys_Reftof(uint16_t *reftof);
uint8_t VI530x_Set_Sys_Histogram_MA_Window_Data(uint8_t *setting_buff);
uint8_t VI530x_Set_Sys_Temperature_Enable(uint8_t VL5300_I2C_Address,uint8_t status);
uint8_t VI530x_Get_Sys_Temperature_Enable(uint8_t *status);
uint8_t VI530x_Set_Sys_Limit_Distance(uint16_t distance);
uint8_t VI530x_Get_Sys_Limit_Distance(uint16_t *distance);
uint8_t VI530x_Set_Sys_CG_Maxratio(uint8_t VL5300_I2C_Address, uint8_t maxratio);
uint8_t VI530x_Get_Sys_CG_Maxratio(uint8_t *maxratio);
uint8_t VI530x_Set_Sys_CK(uint8_t ck);
uint8_t VI530x_Set_Sys_Xtalk_Position(uint8_t VL5300_I2C_Address,uint8_t xtalk_position);
uint8_t VI530x_Get_Sys_Xtalk_Position(uint8_t *xtalk_position);
uint8_t VI530x_Set_Sys_Histogram_MA_Window_Data(uint8_t *setting_buff);
uint8_t VI530x_Get_Sys_Histogram_MA_Window_Data(uint8_t *getting_buff);
uint8_t VI530x_Write_System_Data(uint8_t offset_addr, uint8_t *buff, uint8_t len);
uint8_t VI530x_Read_System_Data(uint8_t offset_addr, uint8_t *buff, uint8_t len);
uint8_t VI530x_Bvd_Add(uint8_t bvd_add);

#endif



