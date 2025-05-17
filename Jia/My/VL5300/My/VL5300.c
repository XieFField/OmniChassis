/**
 * @platform	keil5平台STM32F103C8T6型号单片机
 * @mainpage	机器人学院十期项目设计课第5、6组单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年5月16日
 */


#include <stdint.h>
#include "stm32f10x.h"
#include "Delay_API_z.h"
#include "VL5300_I2C_API.h"
#include "VL5300_API.h"

#include "VI530x_API.h"


 /* ATK-TOF模块读取模式
  * 0：单次读取
  * 1：连续读取
  */
#define CONTINUOUS_READ           1


#define VL5300_1_I2C            0xD2

#define RCC_XSH_1				RCC_APB2Periph_GPIOA		    // XSH_1
#define GPIOx_XSH_1				GPIOA
#define GPIO_Pinx_XSH_1			GPIO_Pin_4

#define XSH_1_H			(GPIO_WriteBit(GPIOx_XSH_1, GPIO_Pinx_XSH_1, (BitAction)1))
#define XSH_1_L			(GPIO_WriteBit(GPIOx_XSH_1, GPIO_Pinx_XSH_1, (BitAction)0))


#define VL5300_2_I2C            0xD4

#define RCC_XSH_2				RCC_APB2Periph_GPIOA		    // XSH_2
#define GPIOx_XSH_2				GPIOA
#define GPIO_Pinx_XSH_2			GPIO_Pin_5

#define XSH_2_H			(GPIO_WriteBit(GPIOx_XSH_2, GPIO_Pinx_XSH_2, (BitAction)1))
#define XSH_2_L			(GPIO_WriteBit(GPIOx_XSH_2, GPIO_Pinx_XSH_2, (BitAction)0))


#define VL5300_3_I2C            0x70

#define RCC_XSH_3				RCC_APB2Periph_GPIOA		    // XSH_3
#define GPIOx_XSH_3				GPIOA
#define GPIO_Pinx_XSH_3			GPIO_Pin_6

#define XSH_3_H			(GPIO_WriteBit(GPIOx_XSH_3, GPIO_Pinx_XSH_3, (BitAction)1))
#define XSH_3_L			(GPIO_WriteBit(GPIOx_XSH_3, GPIO_Pinx_XSH_3, (BitAction)0))


// 不开启时间优化(Optimize for Time),O0级代码优化
// Program Size: Code=9246 RO-data=10102 RW-data=8 ZI-data=1216  
// 实测通信时钟频率约850kHz
// 读取一次两个激光测距的时间约为0.99ms

// 开启时间优化(Optimize for Time),O3级代码优化
// Program Size: Code=9728 RO-data=10100 RW-data=8 ZI-data=1216   
// 实测通信时钟频率约1.5MHz
// 读取一次两个激光测距的时间约为0.50ms


static VI530x_MEASURE_TypeDef VL5300_Data1;
static VI530x_MEASURE_TypeDef VL5300_Data2;
static VI530x_MEASURE_TypeDef VL5300_Data3;
static VL5300_DataSetTypeDef VL5300_DataSet;


static void VL5300_1_HW_Init(void)
{
    XSH_1_H;
    Delay_ms(1);    // 这里必须需要延时，来给硬件一点时间，不过具体可以到多小没严格测量和计算过，我也不会计算

    VI530x_Status ret = VI530x_OK;

    /* 软件中断，GPIO引脚直接上拉 */
    VI530x_Cali_Data.VI530x_Interrupt_Mode_Status = 0x00;

    /* 初始化VI5300 */
    ret |= VI530x_Chip_Init(VL5300_1_I2C);

    /* 写入VI5300固件 */
    ret |= VI530x_Download_Firmware(VL5300_1_I2C, (uint8_t*)VI5300_M31_firmware_buff, FirmwareSize());

    /* 配置标定值 */
    ret |= VI530x_Set_Californiation_Data(VL5300_1_I2C, VI530x_Cali_Data.VI530x_Calibration_Offset);

    /* 开启温度校准, 0x00:关，0x01:开 */
    ret |= VI530x_Set_Sys_Temperature_Enable(VL5300_1_I2C,0x01);

    /* 配置帧率，积分次数 */
    ret |= VI530x_Set_Integralcounts_Frame(VL5300_1_I2C,30, 131072);        // 其实老实说，我还没测试过帧率和积分次数对测量的影响
                                                                            // 甚至其实我对这个VL5300芯片的开发度也很低

    /* 配置数据读取模式 */
#if CONTINUOUS_READ == 1
    ret |= VI530x_Start_Continue_Ranging_Cmd(VL5300_1_I2C); /* 连续模式 */
#else
    ret = VI530x_Start_Single_Ranging_Cmd(VL5300_1_I2C);    /* 单次模式 */
#endif

    if (ret)
    {
        //printf("VI5300初始化失败!\r\n");
    }
    else
    {
        //printf("VI5300初始化成功!\r\n");
    }
}

static void VL5300_2_HW_Init(void)
{
    XSH_2_H;
    Delay_ms(1);    // 这里必须需要延时，来给硬件一点时间，不过具体可以到多小没严格测量和计算过，我也不会计算

    VI530x_Status ret = VI530x_OK;

    /* 软件中断，GPIO引脚直接上拉 */
    VI530x_Cali_Data.VI530x_Interrupt_Mode_Status = 0x00;

    /* 初始化VI5300 */
    ret |= VI530x_Chip_Init(VL5300_2_I2C);

    /* 写入VI5300固件 */
    ret |= VI530x_Download_Firmware(VL5300_2_I2C, (uint8_t*)VI5300_M31_firmware_buff, FirmwareSize());

    /* 配置标定值 */
    ret |= VI530x_Set_Californiation_Data(VL5300_2_I2C, VI530x_Cali_Data.VI530x_Calibration_Offset);

    /* 开启温度校准, 0x00:关，0x01:开 */
    ret |= VI530x_Set_Sys_Temperature_Enable(VL5300_2_I2C, 0x01);

    /* 配置帧率，积分次数 */
    ret |= VI530x_Set_Integralcounts_Frame(VL5300_2_I2C, 30, 131072);

    /* 配置数据读取模式 */
#if CONTINUOUS_READ == 1
    ret |= VI530x_Start_Continue_Ranging_Cmd(VL5300_2_I2C); /* 连续模式 */
#else
    ret = VI530x_Start_Single_Ranging_Cmd(VL5300_2_I2C);    /* 单次模式 */
#endif

    if (ret)
    {
        //printf("VI5300初始化失败!\r\n");
    }
    else
    {
        //printf("VI5300初始化成功!\r\n");
    }
}

static void VL5300_3_HW_Init(void)
{
    XSH_3_H;
    Delay_ms(1);    // 这里必须需要延时，来给硬件一点时间，不过具体可以到多小没严格测量和计算过，我也不会计算

    VI530x_Status ret = VI530x_OK;

    /* 软件中断，GPIO引脚直接上拉 */
    VI530x_Cali_Data.VI530x_Interrupt_Mode_Status = 0x00;

    /* 初始化VI5300 */
    ret |= VI530x_Chip_Init(VL5300_3_I2C);

    /* 写入VI5300固件 */
    ret |= VI530x_Download_Firmware(VL5300_3_I2C, (uint8_t*)VI5300_M31_firmware_buff, FirmwareSize());

    /* 配置标定值 */
    ret |= VI530x_Set_Californiation_Data(VL5300_3_I2C, VI530x_Cali_Data.VI530x_Calibration_Offset);

    /* 开启温度校准, 0x00:关，0x01:开 */
    ret |= VI530x_Set_Sys_Temperature_Enable(VL5300_3_I2C, 0x01);

    /* 配置帧率，积分次数 */
    ret |= VI530x_Set_Integralcounts_Frame(VL5300_3_I2C, 30, 131072);

    /* 配置数据读取模式 */
#if CONTINUOUS_READ == 1
    ret |= VI530x_Start_Continue_Ranging_Cmd(VL5300_3_I2C); /* 连续模式 */
#else
    ret = VI530x_Start_Single_Ranging_Cmd(VL5300_3_I2C);    /* 单次模式 */
#endif

    if (ret)
    {
        //printf("VI5300初始化失败!\r\n");
    }
    else
    {
        //printf("VI5300初始化成功!\r\n");
    }
}

static void MCU_XSH_Pin_Init(void)
{
    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_XSH_1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_XSH_2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_XSH_3, ENABLE);

    // 初始化GPIO口
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_x;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_XSH_1;			// XSH_1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// 配置推挽输出模式
    GPIO_Init(GPIOx_XSH_1, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_XSH_2;			// XSH_2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// 配置推挽输出模式
    GPIO_Init(GPIOx_XSH_2, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_XSH_3;			// XSH_3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// 配置推挽输出模式
    GPIO_Init(GPIOx_XSH_3, &GPIO_InitStructure);

    // 设置GPIO初始状态
    XSH_1_L;
    XSH_2_L;
    XSH_3_L;
    Delay_ms(30);
}

void VL5300_Init(void)
{
	/* 初始化MCU的I2C接口引脚 */
	MyI2C_Init();

    /* 初始化MCU的XSH接口引脚 */
    MCU_XSH_Pin_Init();

	/* 初始化VL5300硬件 */
	VL5300_1_HW_Init();
	VL5300_2_HW_Init();
	VL5300_3_HW_Init();
}

VL5300_DataSetTypeDef* VL5300_GetDataSetPoint(void)
{
	return &VL5300_DataSet;
}

void VL5300_ReadData(void)
{
    VI530x_Get_Measure_Data(VL5300_1_I2C ,&VL5300_Data1);      /* 读取距离数据 */
    VI530x_Get_Measure_Data(VL5300_2_I2C ,&VL5300_Data2);      /* 读取距离数据 */
    VI530x_Get_Measure_Data(VL5300_3_I2C ,&VL5300_Data3);      /* 读取距离数据 */

    VL5300_DataSet.Front.Correction_TOF      = VL5300_Data1.correction_tof;
    VL5300_DataSet.Front.Confidence          = VL5300_Data1.confidence;
    VL5300_DataSet.Front.Intecounts          = VL5300_Data1.intecounts;
    VL5300_DataSet.Front.Peak                = VL5300_Data1.peak;
    VL5300_DataSet.Front.Noise               = VL5300_Data1.noise;
    VL5300_DataSet.Front.XTALK_Count         = VL5300_Data1.xtalk_count;

    VL5300_DataSet.Left.Correction_TOF     = VL5300_Data2.correction_tof;
    VL5300_DataSet.Left.Confidence         = VL5300_Data2.confidence;
    VL5300_DataSet.Left.Intecounts         = VL5300_Data2.intecounts;
    VL5300_DataSet.Left.Peak               = VL5300_Data2.peak;
    VL5300_DataSet.Left.Noise              = VL5300_Data2.noise;
    VL5300_DataSet.Left.XTALK_Count        = VL5300_Data2.xtalk_count;

    VL5300_DataSet.Right.Correction_TOF    = VL5300_Data3.correction_tof;
    VL5300_DataSet.Right.Confidence        = VL5300_Data3.confidence;
    VL5300_DataSet.Right.Intecounts        = VL5300_Data3.intecounts;
    VL5300_DataSet.Right.Peak              = VL5300_Data3.peak;
    VL5300_DataSet.Right.Noise             = VL5300_Data3.noise;
    VL5300_DataSet.Right.XTALK_Count       = VL5300_Data3.xtalk_count;
}


