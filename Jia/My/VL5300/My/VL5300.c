/**
 * @platform	keil5ƽ̨STM32F103C8T6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ����Ŀ��ƿε�5��6�鵥Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��5��16��
 */


#include <stdint.h>
#include "stm32f10x.h"
#include "Delay_API_z.h"
#include "VL5300_I2C_API.h"
#include "VL5300_API.h"

#include "VI530x_API.h"


 /* ATK-TOFģ���ȡģʽ
  * 0�����ζ�ȡ
  * 1��������ȡ
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


// ������ʱ���Ż�(Optimize for Time),O0�������Ż�
// Program Size: Code=9246 RO-data=10102 RW-data=8 ZI-data=1216  
// ʵ��ͨ��ʱ��Ƶ��Լ850kHz
// ��ȡһ�������������ʱ��ԼΪ0.99ms

// ����ʱ���Ż�(Optimize for Time),O3�������Ż�
// Program Size: Code=9728 RO-data=10100 RW-data=8 ZI-data=1216   
// ʵ��ͨ��ʱ��Ƶ��Լ1.5MHz
// ��ȡһ�������������ʱ��ԼΪ0.50ms


static VI530x_MEASURE_TypeDef VL5300_Data1;
static VI530x_MEASURE_TypeDef VL5300_Data2;
static VI530x_MEASURE_TypeDef VL5300_Data3;
static VL5300_DataSetTypeDef VL5300_DataSet;


static void VL5300_1_HW_Init(void)
{
    XSH_1_H;
    Delay_ms(1);    // ���������Ҫ��ʱ������Ӳ��һ��ʱ�䣬����������Ե���Сû�ϸ�����ͼ��������Ҳ�������

    VI530x_Status ret = VI530x_OK;

    /* ����жϣ�GPIO����ֱ������ */
    VI530x_Cali_Data.VI530x_Interrupt_Mode_Status = 0x00;

    /* ��ʼ��VI5300 */
    ret |= VI530x_Chip_Init(VL5300_1_I2C);

    /* д��VI5300�̼� */
    ret |= VI530x_Download_Firmware(VL5300_1_I2C, (uint8_t*)VI5300_M31_firmware_buff, FirmwareSize());

    /* ���ñ궨ֵ */
    ret |= VI530x_Set_Californiation_Data(VL5300_1_I2C, VI530x_Cali_Data.VI530x_Calibration_Offset);

    /* �����¶�У׼, 0x00:�أ�0x01:�� */
    ret |= VI530x_Set_Sys_Temperature_Enable(VL5300_1_I2C,0x01);

    /* ����֡�ʣ����ִ��� */
    ret |= VI530x_Set_Integralcounts_Frame(VL5300_1_I2C,30, 131072);        // ��ʵ��ʵ˵���һ�û���Թ�֡�ʺͻ��ִ����Բ�����Ӱ��
                                                                            // ������ʵ�Ҷ����VL5300оƬ�Ŀ�����Ҳ�ܵ�

    /* �������ݶ�ȡģʽ */
#if CONTINUOUS_READ == 1
    ret |= VI530x_Start_Continue_Ranging_Cmd(VL5300_1_I2C); /* ����ģʽ */
#else
    ret = VI530x_Start_Single_Ranging_Cmd(VL5300_1_I2C);    /* ����ģʽ */
#endif

    if (ret)
    {
        //printf("VI5300��ʼ��ʧ��!\r\n");
    }
    else
    {
        //printf("VI5300��ʼ���ɹ�!\r\n");
    }
}

static void VL5300_2_HW_Init(void)
{
    XSH_2_H;
    Delay_ms(1);    // ���������Ҫ��ʱ������Ӳ��һ��ʱ�䣬����������Ե���Сû�ϸ�����ͼ��������Ҳ�������

    VI530x_Status ret = VI530x_OK;

    /* ����жϣ�GPIO����ֱ������ */
    VI530x_Cali_Data.VI530x_Interrupt_Mode_Status = 0x00;

    /* ��ʼ��VI5300 */
    ret |= VI530x_Chip_Init(VL5300_2_I2C);

    /* д��VI5300�̼� */
    ret |= VI530x_Download_Firmware(VL5300_2_I2C, (uint8_t*)VI5300_M31_firmware_buff, FirmwareSize());

    /* ���ñ궨ֵ */
    ret |= VI530x_Set_Californiation_Data(VL5300_2_I2C, VI530x_Cali_Data.VI530x_Calibration_Offset);

    /* �����¶�У׼, 0x00:�أ�0x01:�� */
    ret |= VI530x_Set_Sys_Temperature_Enable(VL5300_2_I2C, 0x01);

    /* ����֡�ʣ����ִ��� */
    ret |= VI530x_Set_Integralcounts_Frame(VL5300_2_I2C, 30, 131072);

    /* �������ݶ�ȡģʽ */
#if CONTINUOUS_READ == 1
    ret |= VI530x_Start_Continue_Ranging_Cmd(VL5300_2_I2C); /* ����ģʽ */
#else
    ret = VI530x_Start_Single_Ranging_Cmd(VL5300_2_I2C);    /* ����ģʽ */
#endif

    if (ret)
    {
        //printf("VI5300��ʼ��ʧ��!\r\n");
    }
    else
    {
        //printf("VI5300��ʼ���ɹ�!\r\n");
    }
}

static void VL5300_3_HW_Init(void)
{
    XSH_3_H;
    Delay_ms(1);    // ���������Ҫ��ʱ������Ӳ��һ��ʱ�䣬����������Ե���Сû�ϸ�����ͼ��������Ҳ�������

    VI530x_Status ret = VI530x_OK;

    /* ����жϣ�GPIO����ֱ������ */
    VI530x_Cali_Data.VI530x_Interrupt_Mode_Status = 0x00;

    /* ��ʼ��VI5300 */
    ret |= VI530x_Chip_Init(VL5300_3_I2C);

    /* д��VI5300�̼� */
    ret |= VI530x_Download_Firmware(VL5300_3_I2C, (uint8_t*)VI5300_M31_firmware_buff, FirmwareSize());

    /* ���ñ궨ֵ */
    ret |= VI530x_Set_Californiation_Data(VL5300_3_I2C, VI530x_Cali_Data.VI530x_Calibration_Offset);

    /* �����¶�У׼, 0x00:�أ�0x01:�� */
    ret |= VI530x_Set_Sys_Temperature_Enable(VL5300_3_I2C, 0x01);

    /* ����֡�ʣ����ִ��� */
    ret |= VI530x_Set_Integralcounts_Frame(VL5300_3_I2C, 30, 131072);

    /* �������ݶ�ȡģʽ */
#if CONTINUOUS_READ == 1
    ret |= VI530x_Start_Continue_Ranging_Cmd(VL5300_3_I2C); /* ����ģʽ */
#else
    ret = VI530x_Start_Single_Ranging_Cmd(VL5300_3_I2C);    /* ����ģʽ */
#endif

    if (ret)
    {
        //printf("VI5300��ʼ��ʧ��!\r\n");
    }
    else
    {
        //printf("VI5300��ʼ���ɹ�!\r\n");
    }
}

static void MCU_XSH_Pin_Init(void)
{
    // ʹ��GPIOʱ��
    RCC_APB2PeriphClockCmd(RCC_XSH_1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_XSH_2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_XSH_3, ENABLE);

    // ��ʼ��GPIO��
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_x;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_XSH_1;			// XSH_1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// �����������ģʽ
    GPIO_Init(GPIOx_XSH_1, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_XSH_2;			// XSH_2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// �����������ģʽ
    GPIO_Init(GPIOx_XSH_2, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_XSH_3;			// XSH_3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// �����������ģʽ
    GPIO_Init(GPIOx_XSH_3, &GPIO_InitStructure);

    // ����GPIO��ʼ״̬
    XSH_1_L;
    XSH_2_L;
    XSH_3_L;
    Delay_ms(30);
}

void VL5300_Init(void)
{
	/* ��ʼ��MCU��I2C�ӿ����� */
	MyI2C_Init();

    /* ��ʼ��MCU��XSH�ӿ����� */
    MCU_XSH_Pin_Init();

	/* ��ʼ��VL5300Ӳ�� */
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
    VI530x_Get_Measure_Data(VL5300_1_I2C ,&VL5300_Data1);      /* ��ȡ�������� */
    VI530x_Get_Measure_Data(VL5300_2_I2C ,&VL5300_Data2);      /* ��ȡ�������� */
    VI530x_Get_Measure_Data(VL5300_3_I2C ,&VL5300_Data3);      /* ��ȡ�������� */

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


