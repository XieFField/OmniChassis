/**
 * @platform	keil5平台STM32F103C8T6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车从2单片机(型号F103C8T6)
 * @author		桑叁
 * @date		2025年4月15日
 */


/**
 ****************************************************************************************************
 * @file        atk_tof.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-11-01
 * @brief       ATK_TOF模块 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 M48Z-M3最小系统板STM32F103版
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __ATK_TOF_H
#define __ATK_TOF_H

#include <stdint.h>


/**************************************************************************************************/

/* ATK-TOF模块上电默认IIC通讯地址 */
#define VL5300_I2C_Default          0xD8

/* ATK-TOF模块状态值 */
typedef enum 
{
    VI530x_OK                   = 0x00,
    VI530x_RANGING              = 0x01,
    VI530x_BUSY                 = 0x02,
    VI530x_SLEEP                = 0x04,
    VI530x_IIC_ERROR            = 0x08,
    VI530x_ERROR                = 0x10,
    VI530x_ERROR_XTALK_CALIB    = 0x20,
    VI5300_ERROR_OFFSET_CALIB   = 0x40,
    VI530x_ERROR_FW_FAILURE     = 0x80
} VI530x_Status;

/**************************************************************************************************/

#endif





