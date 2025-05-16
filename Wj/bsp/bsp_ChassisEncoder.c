#include "stm32f10x.h"                  // Device header
#include "bsp_ChassisEncoder.h"

/**************************************
 * 函数：底盘编码器读数初始化
 * 参数：无
 * 返回值：无
 * 注意：无
 **************************************
 */
 
void bsp_ChassisEncoder1_Init(void)
{
    //RCC
    
    RCC_APB2PeriphClockCmd(ChassisEC1_TIMRCC, ENABLE);
    RCC_APB2PeriphClockCmd(ChassisEC1_GPIORCC, ENABLE);
    
  
    //GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = ChassisEC1_IN1 |ChassisEC1_IN2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(ChassisEC1_GPIO, &GPIO_InitStructure);
    
    
    //定时器配置
    
    TIM_TimeBaseInitTypeDef TIM1_TimebaseInitStructure;
    TIM1_TimebaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM1_TimebaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM1_TimebaseInitStructure.TIM_Period = 0xffff - 1;                        //ARR
    TIM1_TimebaseInitStructure.TIM_Prescaler = 1 - 1;                          //PSC
    TIM1_TimebaseInitStructure.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(ChassisEC1_TIM, &TIM1_TimebaseInitStructure);
    
    //输入捕获
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(& TIM_ICInitStructure);
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;      //滤波器
    TIM_ICInit(ChassisEC1_TIM, &TIM_ICInitStructure);
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;      //滤波器
    TIM_ICInit(ChassisEC1_TIM, &TIM_ICInitStructure);
    
    //配置编码器模式
    TIM_EncoderInterfaceConfig(ChassisEC1_TIM, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    TIM_SetCounter(ChassisEC1_TIM, 0);          //计数器置0

    ChassisEC1_TIM->CNT = 0x7fff;
    TIM_Cmd(ChassisEC1_TIM,ENABLE);
}

void bsp_ChassisEncoder2_Init(void)
{
    //RCC

    RCC_APB1PeriphClockCmd(ChassisEC2_TIMRCC, ENABLE);
    RCC_APB2PeriphClockCmd(ChassisEC2_GPIORCC1, ENABLE);
    RCC_APB2PeriphClockCmd(ChassisEC2_GPIORCC2, ENABLE);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
    
    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
  
    //GPIO
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = ChassisEC2_IN1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(ChassisEC2_GPIO1, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = ChassisEC2_IN2;
    GPIO_Init(ChassisEC2_GPIO2, &GPIO_InitStructure);
    
    //定时器配置
    
    TIM_TimeBaseInitTypeDef TIM2_TimebaseInitStructure;
    TIM2_TimebaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM2_TimebaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM2_TimebaseInitStructure.TIM_Period = 0xffff;                        //ARR
    TIM2_TimebaseInitStructure.TIM_Prescaler = 0;                          //PSC
    TIM2_TimebaseInitStructure.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(ChassisEC2_TIM, &TIM2_TimebaseInitStructure);
    
    
    //配置编码器模式
    
    TIM_EncoderInterfaceConfig(ChassisEC2_TIM, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    //输入捕获
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    
    TIM_ICInitStructure.TIM_ICFilter = 10;      //滤波器
    TIM_ICInit(ChassisEC2_TIM, &TIM_ICInitStructure);
    
    TIM_SetCounter(ChassisEC2_TIM, 0);          //计数器置0

    ChassisEC2_TIM->CNT = 0x7fff;
    TIM_Cmd(ChassisEC2_TIM,ENABLE);
}

void bsp_ChassisEncoder3_Init(void)
{
    //RCC
    
    RCC_APB1PeriphClockCmd(ChassisEC3_TIMRCC, ENABLE);
    RCC_APB2PeriphClockCmd(ChassisEC3_GPIORCC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
  
    //GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = ChassisEC3_IN1 |ChassisEC3_IN2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(ChassisEC3_GPIO, &GPIO_InitStructure);
    
    
    //定时器配置
    
    TIM_TimeBaseInitTypeDef TIM3_TimebaseInitStructure;
    TIM3_TimebaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM3_TimebaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM3_TimebaseInitStructure.TIM_Period = 0xffff;                        //ARR
    TIM3_TimebaseInitStructure.TIM_Prescaler = 0;                          //PSC
    TIM3_TimebaseInitStructure.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(ChassisEC3_TIM, &TIM3_TimebaseInitStructure);
    
    
    //配置编码器模式
    
    TIM_EncoderInterfaceConfig(ChassisEC3_TIM, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    //输入捕获
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(& TIM_ICInitStructure);
    
    TIM_ICInitStructure.TIM_ICFilter = 0;      //滤波器
    TIM_ICInit(ChassisEC3_TIM, &TIM_ICInitStructure);
    
    TIM_SetCounter(ChassisEC3_TIM, 0);          //计数器置0

    ChassisEC3_TIM->CNT = 20;
    TIM_Cmd(ChassisEC3_TIM,ENABLE);
}

void bsp_ChassisEncoder4_Init(void)
{
    //RCC
    
    RCC_APB1PeriphClockCmd(ChassisEC4_TIMRCC, ENABLE);
    RCC_APB2PeriphClockCmd(ChassisEC4_GPIORCC, ENABLE);
    
  
    //GPIO
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = ChassisEC4_IN1 |ChassisEC4_IN2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(ChassisEC4_GPIO, &GPIO_InitStructure);
    
    
    //定时器配置
    
    TIM_TimeBaseInitTypeDef TIM4_TimebaseInitStructure;
    TIM4_TimebaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM4_TimebaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM4_TimebaseInitStructure.TIM_Period = 0xffff;                        //ARR
    TIM4_TimebaseInitStructure.TIM_Prescaler = 0;                          //PSC
    TIM4_TimebaseInitStructure.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(ChassisEC4_TIM, &TIM4_TimebaseInitStructure);
    
    
    //配置编码器模式
    
    TIM_EncoderInterfaceConfig(ChassisEC4_TIM, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    //输入捕获
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(& TIM_ICInitStructure);
    
    TIM_ICInitStructure.TIM_ICFilter = 6;      //滤波器
    TIM_ICInit(ChassisEC4_TIM, &TIM_ICInitStructure);
    
    TIM_SetCounter(ChassisEC4_TIM, 0);          //计数器置0

    ChassisEC4_TIM->CNT = 0x7fff;
    TIM_Cmd(ChassisEC4_TIM,ENABLE);
}

void bsp_ChassisEncoder_Init(void)
{
    bsp_ChassisEncoder1_Init();
    bsp_ChassisEncoder2_Init();
    bsp_ChassisEncoder3_Init();
    bsp_ChassisEncoder4_Init();
}

int textcoder;
/**************************************
 * 函数：底盘编码器读数
 * 参数：uint8_t TIMX
 * 返回值：uint16_t EncoderData
 * 注意：无
 **************************************
 */
int16_t Chassis_ReadEncoder(uint8_t TIMX) 
{
    int16_t EncoderData;
    int16_t ReadData;
    switch(TIMX)
    {
        case 1:
            textcoder = TIM_GetCounter(ChassisEC1_TIM);
            ReadData = TIM_GetCounter(ChassisEC1_TIM);
            EncoderData = ReadData - 0x7fff;
            ChassisEC1_TIM->CNT = 0x7fff;
            break;
        case 2:
            ReadData = TIM_GetCounter(ChassisEC2_TIM);
            EncoderData = ReadData - 0x7fff;
            ChassisEC2_TIM->CNT = 0x7fff;
            break;
        case 3:
            ReadData = TIM_GetCounter(ChassisEC3_TIM);
            EncoderData = ReadData - 0x7fff;
            ChassisEC3_TIM->CNT = 0x7fff;
            break;
        case 4:
            ReadData = TIM_GetCounter(ChassisEC4_TIM);
            EncoderData = ReadData - 0x7fff;
            ChassisEC4_TIM->CNT = 0x7fff;
            break;
        default:
            EncoderData = 0;
            break;
    }
    return EncoderData;
}

//void TIM6_Init(uint8_t arr) 
//{
//    // 使能TIM6时钟
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

//    // 配置时基参数（示例：1kHz中断）
//    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
//    TIM_TimeBaseStruct.TIM_Prescaler = 7200 - 1;     // 72MHz → 10kHz
//    TIM_TimeBaseStruct.TIM_Period = arr - 1;      // 1kHz中断
//    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
//    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
//    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStruct);


//	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

//    // 配置NVIC
//    NVIC_InitTypeDef NVIC_InitStruct;
//    NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;
//    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; // 根据需求调整优先级
//    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStruct);
//	
//	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);

//    // 启动TIM6
//    TIM_Cmd(TIM6, ENABLE);
//	
//	    // 使能更新中断
//    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
//}

void TIM5_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 7200-1;        //预装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 100-1;     //预分频系数
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStruct); //初始化定时器，配置ARR,PSC
    
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStruct;
    
    NVIC_InitStruct.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	
	NVIC_Init(&NVIC_InitStruct);//配置NVIC
    
    TIM_Cmd(TIM5,ENABLE);
}
