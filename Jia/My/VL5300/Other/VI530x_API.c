/**
 * @platform	keil5平台STM32F103C8T6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车从2单片机(型号F103C8T6)
 * @author		桑叁
 * @date		2025年4月15日
 */


#include "VI530x_API.h"


// VI530x GPIO中断信号
// 0-清零/没有中断信号
// 1-有中断信号
uint8_t VI530x_GPIO_Interrupt_status = 0;

//VI530x IIC的设备地址上电默认8位地址为0xD8
uint8_t VI530x_IIC_Dev_Addr_Now = VL5300_I2C_Default;

VI530x_Calibration_TypeDef VI530x_Cali_Data;
uint8_t VI530x_Chip_Version = 0x30;


/**************************************************************************************************
******************************************  Init API *********************************************
**************************************************************************************************/

/**
 * @brief 	VI530X 唤醒
 * @param 	[none] 
 * @return 	[uint8_t]	ret:0-操作成功（I2C读写无异常）;other-异常（I2C读写有异常）
 */
VI530x_Status VI530x_Set_Digital_Clock_Dutycycle(uint8_t VL5300_I2C_Address)
{
    VI530x_Status ret = VI530x_OK;
    I2C_SingleSend(VL5300_I2C_Address,VAN_REG_PW_CTRL, 0x0F);
    I2C_SingleSend(VL5300_I2C_Address, VAN_REG_PW_CTRL, 0x0E);
    Delay_ms(5);
    return ret;
}

/**
 * @brief 	查询VI530X芯片busy等状态
 * @param 	[none] 
 * @return 	[uint8_t]	ret:0-（空闲 & I2C读写无异常）;other-异常（busy | I2C读写有异常）
 */
VI530x_Status VI530x_Wait_For_CPU_Ready(uint8_t VL5300_I2C_Address)
{
    VI530x_Status Status = VI530x_OK;
    uint8_t stat;
    int retry = 0;

    do
    {
        Delay_ms(1); // delay 1ms
        I2C_ConsecutiveRead(VL5300_I2C_Address,0x02, &stat,1);
    }
    while ((retry++ < 20) && (stat & 0x01));
    if (retry >= 20)
    {
        //printf("CPU Busy stat = %d\n", stat);
        return VI530x_BUSY;
    }

    return Status;
}

/**
 * @brief 	VI530X中断清除
 * @param 	[none] 
 * @return 	[uint8_t]	ret:0-操作成功（I2C读写无异常）;other-异常（I2C读写有异常）
 */
VI530x_Status VI530x_Clear_Interrupt(uint8_t VL5300_I2C_Address)
{
    VI530x_Status ret = VI530x_OK;
    //硬件中断
    VI530x_GPIO_Interrupt_status = 0;
    //寄存器中断（软件中断）
    I2C_SingleRead(VL5300_I2C_Address,VAN_RET_INT_STATUS, &ret);
	return VI530x_OK;
}

/**
 * @brief 	VI530X中断 读取 & 清除
 * @param 	[uint8_t] *interrupt_status：获取中断状态
 * @return 	[uint8_t]	ret:0-操作成功（I2C读写无异常）;other-异常（I2C读写有异常）
 */
VI530x_Status VI530x_Get_And_Clear_Interrupt(uint8_t VL5300_I2C_Address,uint8_t *interrupt_status)
{
    VI530x_Status ret = VI530x_OK;
    uint8_t temp_status = 0;

    //使用寄存器中断（软件中断）
    if(!VI530x_Cali_Data.VI530x_Interrupt_Mode_Status)
    {
        I2C_ConsecutiveRead(VL5300_I2C_Address,VAN_RET_INT_STATUS, &temp_status,1);
    }
    if(VI530x_GPIO_Interrupt_status || (temp_status & 0x01))
    {
        *interrupt_status = 0x01;
        VI530x_GPIO_Interrupt_status = 0;
    }
    else
    {
        *interrupt_status = 0x00;
    }
    return ret;
}

/**
 * @brief 改IIC设备地址
* @param [uint8_t] addr_val	8位地址，注意不是7位地址，最低位为0
 * @return [type]
 */
VI530x_Status VI530x_Set_ModelChangeAddr(uint8_t addr_val)
{
    VI530x_Status ret = VI530x_OK;
    uint8_t IIC_ID = 0;
    //0x0c进入standby,0x08退出standby	,0x88允许更改VI530x的IIC 地址
    I2C_SingleSend(VL5300_I2C_Default, VAN_REG_SYS_CFG,0x88);
    //更改设备地址，写入IIC寄存器
    I2C_SingleSend(VL5300_I2C_Default, VAN_REG_IIC_DEV_ADDR,addr_val);
    if(ret == VI530x_OK)
    {
        //新地址赋值IIC地址全局变量
        VI530x_IIC_Dev_Addr_Now = addr_val;
    }

    I2C_SingleSend(VL5300_I2C_Default, VAN_REG_SYS_CFG,0x08);
    Delay_ms(5);
    I2C_ConsecutiveRead(VL5300_I2C_Default,VAN_REG_IIC_DEV_ADDR, &IIC_ID,1);
    if(IIC_ID != addr_val)
    {
        VI530x_IIC_Dev_Addr_Now = VL5300_I2C_Default;
//        VI530x_ERROR;
    }
    return ret;
}

/**
 * @brief 	VI530X写命令
 * @param 	[none] 
 * @return 	[uint8_t]	ret:0-操作成功（I2C读写无异常）;other-异常（I2C读写有异常）
 */
VI530x_Status VI530xWriteCommand(uint8_t VL5300_I2C_Address,uint8_t cmd)
{
	I2C_SingleSend(VL5300_I2C_Address, VAN_REG_CMD, cmd);
    return VI530x_OK;
}


/**
 * @brief 	初始化VI530X寄存器
 * @param 	[none] 
 * @return 	[uint8_t]	ret:0-操作成功（I2C读写无异常）;other-异常（I2C读写有异常）
 */
VI530x_Status VI530x_Chip_Register_Init(uint8_t VL5300_I2C_Address, uint8_t *chip_version)
{
    VI530x_Status ret = VI530x_OK;
    uint8_t version_31_cnt = 5;
    uint8_t version_30_cnt = 5;

    //读取芯片版本
    do
    {
        I2C_ConsecutiveRead(VL5300_I2C_Address, 0x38, chip_version, 1);
        if (*chip_version == 0x30)
        {
            version_31_cnt--;
        }
        else
        {
            version_30_cnt--;
        }
    } 
    while (version_31_cnt && version_30_cnt); // 0x30 v3.1, 0x00 v3.0

    if (version_31_cnt == 0)
    {
        *chip_version = 0x31;
    }
    else
    {
        *chip_version = 0x30;
    }

    //寄存器初始化
    /*****************************************************************/
    /***********************for v3.1 test 20210304********************/
    //#define VI530x_POWER_MANAGE       1

    if(VI530x_Cali_Data.VI530x_Power_Manage_Status)
    {
        //开启电源管理模式
        I2C_SingleSend(VL5300_I2C_Address, VAN_REG_SYS_CFG, 0x0C);
    }
    else
    {
        //关闭启电源管理模式
        I2C_SingleSend(VL5300_I2C_Address, VAN_REG_SYS_CFG, 0x08);
    }

    I2C_SingleSend(VL5300_I2C_Address,0x07, 0x00); //閿熸枻鎷蜂綅PD閿熸枻鎷锋媷閿熸枻鎷烽敓锟?
    I2C_SingleSend(VL5300_I2C_Address,0x07, 0x01);
    I2C_SingleSend(VL5300_I2C_Address,0x07, 0x00); //閿熻?闈╂嫹A0閿熸枻鎷锋媷閿熸枻鎷烽敓琛楋拷
    I2C_SingleSend(VL5300_I2C_Address,0x04, 0x21);
    I2C_SingleSend(VL5300_I2C_Address,0x05, 0x0e);

    I2C_SingleSend(VL5300_I2C_Address,0x08, 0x00);
    I2C_SingleSend(VL5300_I2C_Address,0x37, 0x80);
    I2C_SingleSend(VL5300_I2C_Address,0x38, 0x30); // v3.0 0x00
    I2C_SingleSend(VL5300_I2C_Address,0x39, 0x00);
    I2C_SingleSend(VL5300_I2C_Address,0x3a, 0x30); // v3.0 0x00
    I2C_SingleSend(VL5300_I2C_Address,0x3b, 0x80);
    I2C_SingleSend(VL5300_I2C_Address,0x3c, 0x80);
    I2C_SingleSend(VL5300_I2C_Address,0x3d, 0x80);
    I2C_SingleSend(VL5300_I2C_Address,0x3e, 0x00);
    I2C_SingleSend(VL5300_I2C_Address,0x3f, 0x00);
    I2C_SingleSend(VL5300_I2C_Address,0x07, 0x0e);
    I2C_SingleSend(VL5300_I2C_Address,0x07, 0x0f);

    return ret;
}

/**
 * @brief 	初始化VI530X
 * @param 	[none] 
 * @return 	[uint8_t]	ret:0-操作成功（I2C读写无异常）;other-异常（I2C读写有异常）
 */
VI530x_Status VI530x_Chip_Init(uint8_t VL5300_I2C_NewAddress)
{
    VI530x_Status Status = VI530x_OK;
    uint8_t IIC_ID = 0;

    //配置电源管理模式：0----关闭电源管理模式，其他值----开启电源管理
    //开启后VI530x待机进入低功耗
    VI530x_Cali_Data.VI530x_Power_Manage_Status = 0x01; //开启电源管理

    //Xshut使能
    //atk_tof_hw_reset();

    I2C_SingleRead(VL5300_I2C_Default,VAN_REG_IIC_DEV_ADDR, &IIC_ID);

    if(IIC_ID != VL5300_I2C_Default)
    {
        Status = VI530x_ERROR;
        //Debug
#ifdef Debug_Mode
        /* VI530x Device ID 为 0xD8, ID不对时不可通讯，请检测 */
        printf("Check device ID is 0x%2x!\r\n",IIC_ID);
#endif
    }

#ifdef Change_IIC_Dev_Addr
    //更改IIC地址为新地址，调用前需要保证IIC总线上的器件释放IIC
    //多颗VI530x共IIC总线时需要分别使能对应Xshut引脚配置
    Status |= VI530x_Set_ModelChangeAddr(VL5300_I2C_NewAddress);
#endif
    
    Status |= VI530x_Chip_Register_Init(VL5300_I2C_NewAddress , &VI530x_Chip_Version);
    
#ifdef Debug_Mode
    printf("VI530x chip version is %x\r\n",VI530x_Chip_Version);
#endif

    Status |= VI530x_Wait_For_CPU_Ready(VL5300_I2C_NewAddress);
    return Status;
}


/**************************************************************************************************
******************************************  Range API *********************************************
**************************************************************************************************/

/**
 * @brief 	VI530X开始单次测距命令
 * @param 	[none] 
 * @return 	[uint8_t]	ret:0-操作成功（I2C读写无异常）;other-异常（I2C读写有异常）
 */
VI530x_Status VI530x_Start_Single_Ranging_Cmd(uint8_t VL5300_I2C_Address)
{
  VI530x_Status ret = VI530x_OK;
	ret |= VI530x_Clear_Interrupt(VL5300_I2C_Address);
	ret |= Get_VI530x_Download_Firmware_Status(VL5300_I2C_Address);
	ret |= VI530x_Set_Digital_Clock_Dutycycle(VL5300_I2C_Address);

	ret |= VI530xWriteCommand(VL5300_I2C_Address,VAN_START_RANG_CMD);

	return ret;
}

/**
 * @brief 	VI530X开始连续测距命令
 * @param 	[none] 
 * @return 	[uint8_t]	ret:0-操作成功（I2C读写无异常）;other-异常（I2C读写有异常）
 */
VI530x_Status VI530x_Start_Continue_Ranging_Cmd(uint8_t VL5300_I2C_Address )
{

  VI530x_Status ret = VI530x_OK;

	ret |= VI530x_Clear_Interrupt(VL5300_I2C_Address);
	ret |= Get_VI530x_Download_Firmware_Status(VL5300_I2C_Address);
	ret |= VI530x_Set_Digital_Clock_Dutycycle(VL5300_I2C_Address);

	ret |= VI530xWriteCommand(VL5300_I2C_Address,0x0F);

	return ret;
}

/**
 * @brief 	VI530X停止连续测距命令
 * @param 	[none] 
 * @return 	[uint8_t]	ret:0-操作成功（I2C读写无异常）;other-异常（I2C读写有异常）
 */
VI530x_Status VI530x_Stop_Continue_Ranging_Cmd(void)
{
  VI530x_Status ret = VI530x_OK;

	ret |= VI530x_Clear_Interrupt(VL5300_I2C_Default);
	ret |= VI530x_Set_Digital_Clock_Dutycycle(VL5300_I2C_Default);
	//VI530x_POWER_MANAGE == 1 => VI530x_STOP_MEASURE_SET == 1
	if(VI530x_Cali_Data.VI530x_Power_Manage_Status)
	{
		//开启电源管理模式
		ret |= VI530xWriteCommand(VL5300_I2C_Default,0x1F);
	}
	else
	{
		//关闭启电源管理模式
		ret |= VI530xWriteCommand(VL5300_I2C_Default,0X00);
	}	
	
	Delay_ms(10);
	return ret;
}

/**
 * @brief 	VI530X获取测距测距数据
 * @param 	[uint8_t] *data_buff:获取测距数据（包括校准的TOF、confidence）
 * @return 	[uint8_t]	ret:0-操作成功（I2C读写无异常）;other-异常（I2C读写有异常）
 */
VI530x_Status VI530x_Get_Measure_Data(uint8_t VL5300_I2C_Address,VI530x_MEASURE_TypeDef *measure_data)
{
	VI530x_Status ret = VI530x_OK;
	uint8_t Interrupt_status = 0;
	uint8_t data_buff[32] = {0};
	uint16_t time_out_cnt = 10;
	//参数
	int16_t raw_tof = 0;
	int16_t correction_tof = 0;	
	uint32_t intecounts = 0;
	uint32_t peak = 0;
	uint16_t noise = 0;	
	uint16_t xtalk_count = 0;
	int16_t ref_tof = 0;
	uint32_t ref_peak = 0;

	uint8_t confidence = 0;	
	float bias = 0;

	while (time_out_cnt--)
	{
		ret |= VI530x_Get_And_Clear_Interrupt(VL5300_I2C_Address ,&Interrupt_status);
		if (Interrupt_status)
		{
			I2C_ConsecutiveRead(VL5300_I2C_Address, 0x0C, data_buff, 32);
			
			memcpy(&ref_peak,&data_buff[8],4);
			memcpy(&ref_tof,&data_buff[20],2);
			memcpy(&raw_tof,&data_buff[12],2);
			memcpy(&intecounts,&data_buff[22],4);
			intecounts = intecounts & 0x00FFFFFF;
			memcpy(&noise,&data_buff[26],2);
			memcpy(&peak,&data_buff[28],4);
			memcpy(&xtalk_count,&data_buff[14],2);
			//校正
			bias = VI530x_V10_Calculate_Pileup_Bias(ref_peak, noise, intecounts);
			correction_tof  = raw_tof  + bias - VI530x_Cali_Data.VI530x_Calibration_Offset;
			if(correction_tof  < 0)
			{
				correction_tof  = 0;
			}
			//置信度
			confidence = VI530x_Calculate_Confidence(noise, peak, intecounts);
			if( (correction_tof  < 50)&& (peak < 800000)&& (confidence > 50) )
			{
				confidence = 50;
			}

			measure_data->intecounts = intecounts;
			measure_data->correction_tof = correction_tof;
			measure_data->confidence = confidence;
			measure_data->peak = peak;	
			measure_data->noise = noise;	
			measure_data->xtalk_count = xtalk_count;
			break;
		}
		if (time_out_cnt == 0)
		{
			ret |= VI530x_ERROR;
		}
		
		Delay_ms(1);
	}
	//Debug
#ifdef Debug_Mode
	printf("raw_tof = %2d, peak = %4d, ref_tof = %2d, ref_peak = %4d, noise = %4d, bias = %2f, cali_offset = %4f, intecounts = %4d, xtalk_count = %2d\r\n",
			  raw_tof,peak,ref_tof,ref_peak,noise,bias,
				 VI530x_Cali_Data.VI530x_Calibration_Offset,intecounts,xtalk_count);
#endif
	return ret;
}


/**************************************************************************************************
******************************************  Calibration *******************************************
**************************************************************************************************/

/**
 * @brief xTalk 标定
 * @param *pbuff xtalk_pos(1 Byte) + xtalk_peak(2 Bytes) + xtalk_tof(2 Bytes)
 * @return
 */
VI530x_Status VI530x_Xtalk_Calibration(void)
{
	VI530x_Status ret = VI530x_OK;
	uint8_t status = 0;
	uint16_t time_out_cnt = 1000;
	uint8_t xtalk_buff[10] = {0};

	//关闭温度校准:0x00-关
	ret |= VI530x_Set_Sys_Temperature_Enable(VL5300_I2C_Default,0x00);
	ret |= VI530x_Clear_Interrupt(VL5300_I2C_Default);
	ret |= VI530x_Set_Digital_Clock_Dutycycle(VL5300_I2C_Default);
	//Xtalk命令
	I2C_SingleSend(VL5300_I2C_Default, 0x0A, 0x0D);

	while (time_out_cnt--)
	{
		Delay_ms(10);
		ret |= VI530x_Get_And_Clear_Interrupt(VL5300_I2C_Default,&status);
		if (status)
		{
			Delay_ms(10);
			I2C_SingleRead(VL5300_I2C_Default,0x08, &status);
			if (status == 0xAA)
			{
				I2C_ConsecutiveRead(VL5300_I2C_Default, 0x0C, xtalk_buff, 5);
				//将xtalk_pos/xtalk_maxratio参数写到固件
        VI530x_Cali_Data.VI530x_Calibration_CG_Pos = xtalk_buff[0];
        VI530x_Cali_Data.VI530x_Calibration_CG_Maxratio = xtalk_buff[3];
				VI530x_Cali_Data.VI530x_Calibration_CG_peak = (uint16_t)((((uint16_t)xtalk_buff[2])<<8) |(( (uint16_t)xtalk_buff[1])));
        ret |= VI530x_Set_Sys_Xtalk_Position(VL5300_I2C_Default,VI530x_Cali_Data.VI530x_Calibration_CG_Pos);
        ret |= VI530x_Set_Sys_CG_Maxratio(VL5300_I2C_Default,VI530x_Cali_Data.VI530x_Calibration_CG_Maxratio);
				break;
			}
			else
			{
				ret |= VI530x_ERROR_XTALK_CALIB;
				return ret;
			}
		}
		if (time_out_cnt == 0)
		{
			ret |= VI530x_ERROR_XTALK_CALIB;
			return ret;
		}		
	}
	return ret;
}


/**
 * @brief RefToF 标定
 * @param 
 * @return
 */
uint8_t VI530x_Reftof_Calibration(void)
{
	VI530x_Status ret = VI530x_OK;
	uint8_t interrupt_status = 0;
  // 采集数据计数
  uint8_t get_data_cnt = 0;
  uint8_t databuff[2] = {0};
	uint16_t time_out_cnt = 0;
	//采样上下限
  uint8_t get_data_total_times = 20;
  uint8_t start_get_data_times = 10;
	
  //参数
	uint16_t ref_tof = 0;
	int32_t sum_reftof = 0;
	
	//关闭温度校准:0x00-关
	ret |= VI530x_Set_Sys_Temperature_Enable(VL5300_I2C_Default,0x00);
	ret |= VI530x_Clear_Interrupt(VL5300_I2C_Default);
	ret |= VI530x_Start_Continue_Ranging_Cmd(VL5300_I2C_Default);

  while(1)
  {
    Delay_ms(5);
    ret |= VI530x_Get_And_Clear_Interrupt(VL5300_I2C_Default,&interrupt_status);
    if (interrupt_status)
    {
      // 获取Reftof
      I2C_ConsecutiveRead(VL5300_I2C_Default, 0x20, databuff, 2);
			time_out_cnt = 0;
			memcpy(&ref_tof,&databuff[0],2);		
			
			if(get_data_cnt > start_get_data_times)
			{
				sum_reftof += ref_tof;
			}			
      get_data_cnt++;
			
		//Debug_Mode
		#ifdef Debug_Mode
			printf("RefTof:ref_tof = %2d, cnt = %d\r\n",
					ref_tof,get_data_cnt);
		#endif
    }
    if(get_data_cnt > get_data_total_times)
    {
			VI530x_Cali_Data.VI530x_Calibration_Reftof = sum_reftof / (get_data_total_times-start_get_data_times);
			//设置RefToF标定值启用
			ret |= VI530x_Set_Sys_Reftof(VL5300_I2C_Default,VI530x_Cali_Data.VI530x_Calibration_Reftof);
      break;
    }
		time_out_cnt++;
		if(time_out_cnt > 200)
		{
			//超时异常
			ret |= VI530x_ERROR;
			break;
		}
  }
	ret |= VI530x_Stop_Continue_Ranging_Cmd();
	return ret;
}


/**
 * @brief Offset标定
 * @param mili	标定的位置
 * @param *pbuff	标定结果：(int32) 单位0.1mm
 * @return
 */
VI530x_Status VI530x_Offset_Calibration(uint16_t mili)
{
    VI530x_Status ret = VI530x_OK;
    uint8_t interrupt_status = 0;
    // 采集数据计数
    uint8_t get_data_cnt = 0;
    uint8_t databuff[32] = {0};
    uint16_t time_out_cnt = 0;
    //采样上下限
    uint8_t get_data_total_times = 40;
    uint8_t start_get_data_times = 10;

    //参数
    uint16_t noise = 0;
    int16_t raw_tof = 0;
    uint32_t ref_peak = 0, peak1 = 0;
    uint32_t intecounts = 0;

    uint8_t confidence = 0;
    float bias = 0;
    float offset_mili = 0.0;
    int32_t sum_tof = 0, sum_bias = 0;

    //关闭温度校准:0x00-关
    ret |= VI530x_Set_Sys_Temperature_Enable(VL5300_I2C_Default,0x00);
    ret |= VI530x_Clear_Interrupt(VL5300_I2C_Default);
    ret |= VI530x_Start_Continue_Ranging_Cmd(VL5300_I2C_Default);

    while(1)
    {
        Delay_ms(5);
        ret |= VI530x_Get_And_Clear_Interrupt(VL5300_I2C_Default,&interrupt_status);
        if (interrupt_status)
        {
            // 获取tof
            I2C_ConsecutiveRead(VL5300_I2C_Default, 0x0C, databuff, 32);
            time_out_cnt = 0;
            memcpy(&intecounts,&databuff[22],4);
            intecounts = intecounts & 0x00FFFFFF;

            memcpy(&noise,&databuff[26],2);
            memcpy(&peak1,&databuff[28],4);
            memcpy(&ref_peak,&databuff[8],4);
            memcpy(&raw_tof,&databuff[12],2);
            bias = VI530x_V10_Calculate_Pileup_Bias(ref_peak,noise,intecounts);
            confidence = VI530x_Calculate_Confidence(noise, peak1, intecounts);
            if( confidence != 100)
            {
                //输出异常
                ret |= VI5300_ERROR_OFFSET_CALIB;
                break;
            }
            
            if(get_data_cnt > start_get_data_times)
            {
                sum_tof += raw_tof;
                sum_bias += bias;
            }
            get_data_cnt++;
            
            //Debug_Mode
            #ifdef Debug_Mode
                printf("Offset:raw_tof = %2d, ref_peak = %4d, peak = %4d, noise = %4d, bias = %2f, intecounts = %4d, cnt = %d\r\n",
                    raw_tof,ref_peak,peak1,noise,bias,intecounts,get_data_cnt);
            #endif
        }
        if(get_data_cnt > get_data_total_times)
        {
            offset_mili = (float)(sum_tof+sum_bias ) / (get_data_total_times-start_get_data_times) - mili;
            break;
        }
        time_out_cnt++;
        if(time_out_cnt > 200)
        {
            //超时异常
            ret |= VI5300_ERROR_OFFSET_CALIB;
            break;
        }
    }
    ret |= VI530x_Stop_Continue_Ranging_Cmd();
    //Offset标定值赋值
    VI530x_Cali_Data.VI530x_Calibration_Offset = offset_mili;
    return ret;
}


/**
 * @brief 	VI530X 下载完固件后设置标定数据
 * @param 	[none] 
 * @return 	[uint8_t]	ret:0-操作成功（I2C读写无异常）;other-异常（I2C读写有异常）
 */
VI530x_Status VI530x_Set_Californiation_Data(uint8_t VL5300_I2C_Address, float cali_offset)
{
    VI530x_Status ret = VI530x_OK;
    //标定的pos写入固件
    ret |= VI530x_Set_Sys_Xtalk_Position(VL5300_I2C_Address,VI530x_Cali_Data.VI530x_Calibration_CG_Pos);
    //标定的maxratio写入固件
    ret |= VI530x_Set_Sys_CG_Maxratio(VL5300_I2C_Address, VI530x_Cali_Data.VI530x_Calibration_CG_Maxratio);
    //标定的Reftof写入固件
    ret |= VI530x_Set_Sys_Reftof(VL5300_I2C_Address, VI530x_Cali_Data.VI530x_Calibration_Reftof);
    //Offset标定值用于驱动层，不需要写入芯片
    VI530x_Cali_Data.VI530x_Calibration_Offset = cali_offset;
    return ret;
}


/**************************************************************************************************
******************************************  Other *******************************************
**************************************************************************************************/

VI530x_Status VI530x_Get_Histogram_Data(uint8_t tdc, uint8_t *histogram_buff)
{
    VI530x_Status ret = VI530x_OK;

    uint8_t i = 0;
    uint8_t reg_pw_ctrl = 0;
    uint8_t reg_sys_cfg = 0;
    uint16_t ram_addr_base = 0;

    if (tdc <= 9)
    {
        ram_addr_base = 0x1000 + 0x0400 * tdc;
        // 0-9直方图，9-ref直方图
    }
    else if (tdc >= 0xF1 && tdc <= 0xF4)
    {
        // CG(MA)直方图
        ram_addr_base = (tdc - 0xF1) * 0x0400;
    }

    ret |= VI530x_Set_Digital_Clock_Dutycycle(VL5300_I2C_Default);

    I2C_SingleRead(VL5300_I2C_Default,REG_SYS_CFG, &reg_sys_cfg);
    I2C_SingleSend(VL5300_I2C_Default, REG_SYS_CFG, reg_sys_cfg | (0x01 << 0));
    I2C_SingleRead(VL5300_I2C_Default,REG_PW_CTRL, &reg_pw_ctrl);
    I2C_SingleSend(VL5300_I2C_Default,REG_PW_CTRL, reg_pw_ctrl | (0x01 << 3));               // set otp_ld_done
    I2C_SingleSend(VL5300_I2C_Default,REG_PW_CTRL, reg_pw_ctrl | (0x01 << 3) | (0x01 << 1)); // set otp_ld_done, pw_ctrl_lp
    I2C_SingleSend(VL5300_I2C_Default,REG_MCU_CFG, 0x03);                                    // 0x01 FOR VI530x V3.0
    I2C_SingleSend(VL5300_I2C_Default,REG_CMD, 0x01);
    I2C_SingleSend(VL5300_I2C_Default,REG_SIZE, 0x02);
    I2C_SingleSend(VL5300_I2C_Default,REG_SCRATCH_PAD_BASE + 0x00, *((uint8_t *)(&ram_addr_base)));
    I2C_SingleSend(VL5300_I2C_Default,REG_SCRATCH_PAD_BASE + 0x01, *((uint8_t *)(&ram_addr_base) + 1));

    Delay_ms(1);

    for (i = 0; i < 32; i++)
    {
        I2C_SingleSend(VL5300_I2C_Default,REG_CMD, 0x05);  // Issue RAM read command
        I2C_SingleSend(VL5300_I2C_Default,REG_SIZE, 0x20); // Issue RAM read command
        Delay_ms(5);

        I2C_ConsecutiveRead(VL5300_I2C_Default, 0x0c, &histogram_buff[32 * i], 32);
//      for(j = 0;j < 32;j++)
//      {
//          histogram_buff[32 * i + j] = j;
//      }
    }
    I2C_SingleSend(VL5300_I2C_Default,REG_MCU_CFG, 0x07);                       // 0X03  FOR VI530x V3.0
    I2C_SingleSend(VL5300_I2C_Default,REG_SYS_CFG, reg_sys_cfg & ~(0x01 << 0)); // clear sc_en
    I2C_SingleSend(VL5300_I2C_Default,REG_PW_CTRL, reg_pw_ctrl | (0x01 << 1));  // restore power         control register
    I2C_SingleSend(VL5300_I2C_Default,REG_PW_CTRL, reg_pw_ctrl);                // clear pw_ctrl_lp

    return ret;
}










