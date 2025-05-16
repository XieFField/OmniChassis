/**
 * @platform	keil5平台STM32F103C8T6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车从2单片机(型号F103C8T6)
 * @author		桑叁
 * @date		2025年4月15日
 */


#include "VI530x_Algorithm.h"

/**
 * @brief 	通用方案pileup校正
 * @param 	[uint32_t] peak2 ：测距输出的ref_peak
 * @param 	[uint32_t] noise ：测距输出的noise
 * @param 	[uint32_t] integral_times ：测距输出的积分次数
 * @return 	[uint32_t] bias:校正值,校正TOF = 原始TOF + bias - offset
 */
float VI530x_V10_Calculate_Pileup_Bias(uint32_t peak2, uint32_t noise, uint32_t integral_times)
{
    //********pileup
    float peak_tmp;
    float bias = 0;

    if (integral_times == 0)
    {
        return bias;
    }

    // peak2归一化
    if (peak2 > 65536)
    {
        peak_tmp = (peak2 * 256) / integral_times * 256;
    }
    else
    {
        peak_tmp = peak2 * 65536 / integral_times;
    }

    peak_tmp = peak_tmp / 4096;

    bias = (float)(PILEUP_A / (PILEUP_B - peak_tmp * PILEUP_D) - PILEUP_C) / PILEUP_D;
    if (bias < 0)
    {
        bias = 0;
    }
    return bias;

}


/**
 * @brief 	通用方案confidence计算
 * @param 	[uint32_t] noise ：测距输出的noise
 * @param 	[uint32_t] peak1 ：测距输出的peak1
 * @param 	[uint32_t] integral_times ：测距输出的积分次数
 * @return 	[uint8_t] confidence
 */
uint8_t VI530x_Calculate_Confidence(uint16_t noise, uint32_t peak1, uint32_t integral_times)
{
    uint8_t len = 0;
    uint8_t i = 0, confidence = 0;
    double Noise_regu = 0;
    double Peak_regu = 0;
    double Lower = 0;
    double Upper = 0;
    const uint32_t xth[10] = {19, 83, 163, 691, 1243, 2539, 5091, 10395, 20427, 33235};
    const uint32_t ylower[10] = {16, 25, 45, 145, 258, 501, 1004, 1980, 3804, 6063};
    const uint32_t yupper[10] = {26, 35, 61, 173, 303, 574, 1148, 2220, 4230, 6705};

    Noise_regu = (uint32_t)noise * 65536 / integral_times * 2;

    if (peak1 > 8000000)
    {
        Peak_regu = peak1 * 256 / integral_times * 4;
    }
    else if (peak1 > 4000000)
    {
        Peak_regu = peak1 * 512 / integral_times * 2;
    }
    else
    {
        Peak_regu = peak1 * 1024 / integral_times;
    }
    len = sizeof(xth) / sizeof(xth[0]);
    for (i = 0; i < (len - 1); i++)
    {
        if (Noise_regu < xth[i + 1])
        {
            Lower = (ylower[i + 1] - ylower[i]) * abs((int16_t)(Noise_regu - xth[i])) / (xth[i + 1] - xth[i]) + ylower[i];
            Upper = (yupper[i + 1] - yupper[i]) * abs((int16_t)(Noise_regu - xth[i])) / (xth[i + 1] - xth[i]) + yupper[i];
            break;
        }
        else if (Noise_regu >= xth[9])
        {
            Lower = (ylower[9] - ylower[8]) * abs((int16_t)(Noise_regu - xth[8])) / (xth[9] - xth[8]) + ylower[8];
            Upper = (yupper[9] - yupper[8]) * abs((int16_t)(Noise_regu - xth[8])) / (xth[9] - xth[8]) + yupper[8];
            break;
        }
    }

    if (Peak_regu > Upper)
    {
        confidence = 100;
    }
    else if (Peak_regu < Lower)
    {
        confidence = 0;
    }
    else
    {
        confidence = 100 * (Peak_regu - Lower) / (Upper - Lower);
    }

    return confidence;
}


/**
 * @brief  Xtalk比值计算
 * @param  measure_data：测距输出
* @retval ratio：0为无效值，不可以使用
 */
//可用于参考评估cg的Xtalk强度，容易受环境影响
uint8_t VI530x_Calculate_Xtalk_Ratio(int16_t tof, uint16_t count, uint32_t noise, uint8_t confidence, uint32_t peak)
{
    int16_t ratio=0;

    if( (confidence==0) | ((tof<20)&&(peak<500000)&&(confidence==100)) | ((tof>500)&&(confidence==100)) )
    {
        ratio = ( count + 81/2 - noise/8 )/81 + 3;
        if(ratio<0)
        {
            ratio = 0;
        }
    }
    return ratio;
}


/**
 * @brief  Xtalk比值校正
 * @param  xtalk_ratio：比值计算值
* @retval ret：0为OK
 */
//xtalk_ratio以应用环境评估，建议不超过15
uint8_t VI530x_Calibration_Xtalk_Ratio(uint8_t xtalk_ratio)
{
    uint8_t ret = 0;
    uint8_t ratio;
    if(xtalk_ratio<VI530x_Cali_Data.VI530x_Calibration_CG_Maxratio)
    {
        //小于出厂标定值不建议使用
        xtalk_ratio = VI530x_Cali_Data.VI530x_Calibration_CG_Maxratio;
    }
    else
    {
        VI530x_Cali_Data.VI530x_Xtalk_Ratio = xtalk_ratio;
        ret |= VI530x_Stop_Continue_Ranging_Cmd();	
        ret |= VI530x_Set_Sys_CG_Maxratio( VL5300_I2C_Default,xtalk_ratio );
        ret |= VI530x_Get_Sys_CG_Maxratio(&ratio);
        ret |= VI530x_Start_Continue_Ranging_Cmd(VL5300_I2C_Default);	
        if( ratio != xtalk_ratio )
        {
            return Result_ERROR;
        }
    }
    return ret;
}

