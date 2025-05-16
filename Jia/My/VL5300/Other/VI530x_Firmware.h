/**
 * @platform	keil5平台STM32F103C8T6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车从2单片机(型号F103C8T6)
 * @author		桑叁
 * @date		2025年4月15日
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VI530x_FIRMWARE_H
#define __VI530x_FIRMWARE_H			 

/* Includes ------------------------------------------------------------------*/
#include "atk_tof.h"
#include "VI530x_API.h"
#include "VI530x_System_Data.h"
#include "VI530x_Algorithm.h"

/**
 * @brief 	VI530X 固件存放的数据
 */
extern const uint8_t VI5300_M31_firmware_buff[8192];

/**
 * @brief 	VI530X 获取固件运行状态
 * @param 	[none] 
 * @return 	[uint8_t]	ret:0-固件成功运行 && I2C读写无异常;other-异常（固件没有成功运行 || I2C读写有异常）
 */
VI530x_Status Get_VI530x_Download_Firmware_Status(uint8_t VL5300_I2C_Address);
//uint8_t VI530x_Write_Firmware_PreConfig(void);
//uint8_t VI530x_Write_Firmware_Post_Config(void);
/**
 * @brief 	VI530X 写固件
 * @param 	[uint8_t] *Firmware_buff：固件数据地址
 * @param 	[uint16_t] size：固件长度
 * @return 	[uint8_t]	ret：0-操作成功（I2C读写无异常）;other-异常（I2C读写有异常）
 */
VI530x_Status VI530x_Download_Firmware(uint8_t VL5300_I2C_Address, uint8_t* Firmware_buff, uint16_t size);

/**
 * @brief 	固件大小
 * @param 	
 * @return 	[uint16_t]	固件数据数组大小
 */
uint16_t FirmwareSize(void);

#endif  

