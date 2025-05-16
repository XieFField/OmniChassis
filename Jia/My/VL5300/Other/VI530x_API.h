/**
 * @platform	keil5平台STM32F103C8T6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车从2单片机(型号F103C8T6)
 * @author		桑叁
 * @date		2025年4月15日
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VI530x_API_H
#define __VI530x_API_H

/* Includes ------------------------------------------------------------------*/
#include "Delay_API_z.h"
#include "VL5300_I2C_API.h"

#include "atk_tof.h"
#include "VI530x_Firmware.h"
#include "VI530x_System_Data.h"
#include "VI530x_Algorithm.h"
#include <string.h>


//API 版本
//VI5300_MCU_Gemeral_M31_V203

//Debug时开启
//#define Debug_Mode

#define Change_IIC_Dev_Addr

#ifdef Change_IIC_Dev_Addr	
#define VI530x_IIC_DEV_ADDR2    0xD0        //配置IIC地址，8位地址，注意不是7位地址，最低位为0
#endif
extern uint8_t VI530x_IIC_Dev_Addr_Now;

#define APIversion               0x0203
#define VAN_REG_CMD              0x0A
#define VAN_RET_INT_STATUS       0x03
#define VAN_REG_PW_CTRL          0x07

#define VAN_REG_MCU_CFG          0x00
#define VAN_REG_SCRATCH_PAD_BASE 0x0c
#define VAN_REG_SYS_CFG          0x01
#define VAN_REG_AO_DOMAIN        0x3b
#define VAN_REG_SIZE             0x0b
#define VAN_REG_IIC_DEV_ADDR     0x06

#define REG_MCU_CFG          0x00
#define REG_SYS_CFG          0x01
#define REG_PW_CTRL          0x07
#define REG_CMD              0x0a
#define REG_SIZE             0x0b
#define REG_SCRATCH_PAD_BASE 0x0c 

//#define VAN_START_RANG_CMD       0x0e
//2022-3-25测试
#define VAN_START_RANG_CMD       0x0e
#define VAN_WRITEFW_CMD          0x03

//VI530x状态
typedef enum Result_Status
{
    Result_OK       = 0x00,
    Result_ERROR    = 0x90
}Result_Status;

typedef struct
{
    //530xID值
    uint16_t VI530x_Calibration_ID;	
    //VI530x电源管理设置
    //0x00：不开启电源管理，其它：开启电源管理
    uint8_t VI530x_Power_Manage_Status;
    //VI530x中断状态设置
    //0x88：使用硬件中断,不使用寄存器中断，0x00：不使用硬件中断,使用寄存器查询
    uint8_t VI530x_Interrupt_Mode_Status;
    //VI530x CG_Pos
    int8_t VI530x_Calibration_CG_Pos;
    //VI530x CG_Peak
    uint16_t VI530x_Calibration_CG_peak;
    //VI530x CG_Maxratio
    uint8_t VI530x_Calibration_CG_Maxratio;
    // VI530x CG Xtalk Ratio计算值
    uint8_t VI530x_Xtalk_Ratio;
    //VI530x CK
    uint8_t VI530x_Calibration_CK;
    //VI530x MP
    uint8_t VI530x_Calibration_MP;
    //VI530x reftof标定值
    uint16_t VI530x_Calibration_Reftof;
    //VI530x 距离限制值
    uint16_t VI530x_Set_Limit;
    //VI530x offset标定值
    float VI530x_Calibration_Offset;
    //斜率
    float VI530x_Slope_K;

}VI530x_Calibration_TypeDef;



typedef struct
{
    //校正的tof
    int16_t correction_tof;
    //置信度
    uint8_t confidence;
    //积分次数
    uint32_t intecounts;
    //Peak
    uint32_t peak;
    //Noise
    uint16_t noise;
    //xtalk_count
    uint16_t xtalk_count;

}VI530x_MEASURE_TypeDef;



VI530x_Status VI530x_Chip_Init(uint8_t VL5300_I2C_NewAddress);
VI530x_Status VI530x_Set_Californiation_Data(uint8_t VL5300_I2C_Address, float cali_offset);
extern VI530x_Calibration_TypeDef VI530x_Cali_Data;
extern uint8_t VI530x_GPIO_Interrupt_status;


/**
 * @brief 清除中断
 * @return [type]
 */
VI530x_Status VI530x_Clear_Interrupt(uint8_t VL5300_I2C_Address);
/**
 * @brief 获取并清除中断信号
 * @param [uint8_t] *interrupt_status	1-有中断，0-无中断
 * @return [type]
 */
VI530x_Status VI530x_Get_And_Clear_Interrupt(uint8_t VL5300_I2C_Address,uint8_t *interrupt_status);
/**
 * @brief 开启单次测距
 * @return [type]
 */
VI530x_Status VI530x_Start_Single_Ranging_Cmd(uint8_t VL5300_I2C_Address);
/**
 * @brief 开启连续测距
 * @return [type]
 */
VI530x_Status VI530x_Start_Continue_Ranging_Cmd(uint8_t VL5300_I2C_Address);
/**
 * @brief 停止连续测距
 * @return [type]
 */
VI530x_Status VI530x_Stop_Continue_Ranging_Cmd(void);
/**
 * @brief 获取测距值
 * @param *data_buff
 * @return 
 */
VI530x_Status VI530x_Get_Measure_Data(uint8_t VL5300_I2C_Address,VI530x_MEASURE_TypeDef *measure_data);
uint8_t VI530x_Set_Digital_Clock_Dutycycle(uint8_t VL5300_I2C_Address);


uint8_t VI530x_Chip_Register_Init(uint8_t VL5300_I2C_Address, uint8_t* chip_version);
uint8_t VI530x_Xtalk_Calibration(void);
uint8_t VI530x_Get_CG_Maxratio(uint16_t cg_peak,uint8_t *maxratio);
uint8_t VI530x_Get_Histogram_Data(uint8_t tdc,uint8_t *histogram_buff);
uint8_t VI530x_Offset_Calibration(uint16_t mili);
uint8_t VI530x_Reftof_Calibration(void);



/**改IIC设备地址 **/
VI530x_Status VI530x_Set_ModelChangeAddr(uint8_t addr_val);

#endif  
	 
