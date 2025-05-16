#include "stm32f10x.h"                  // Device header
#include "bsp_motorTIM.h"

/**************************************
 * 函数：底盘电机PWM初始化
 * 参数：无
 * 返回值：无
 * 注意：无
 **************************************
 */

void ChassisPWM_Init(void)
{
    RCC_APB2PeriphClockCmd(PWM_RCCTIM, ENABLE);
    RCC_APB2PeriphClockCmd(PWM_RCCGPIO, ENABLE);

    GPIO_InitTypeDef PWM_InitStructure;
    PWM_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    PWM_InitStructure.GPIO_Pin = ChassisMotor1_PWM | ChassisMotor2_PWM | ChassisMotor3_PWM | ChassisMotor4_PWM;
    PWM_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(PWM_GPIO, &PWM_InitStructure);
    
    
    /*时基单元*/
    TIM_TimeBaseInitTypeDef PWM_TimeBaseInitStructure;
    PWM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    PWM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    PWM_TimeBaseInitStructure.TIM_Period = 100 - 1;                 //ARR
    PWM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;               //PSC
    PWM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(PWM_TIM, &PWM_TimeBaseInitStructure);
    
    /*输出比较单元*/
    TIM_OCInitTypeDef PWM_OCInitStructure;
    TIM_OCStructInit(&PWM_OCInitStructure);
    
    PWM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    PWM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    PWM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    
    PWM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC1Init(PWM_TIM, &PWM_OCInitStructure);
    
    PWM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC2Init(PWM_TIM, &PWM_OCInitStructure);
    
    PWM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC3Init(PWM_TIM, &PWM_OCInitStructure);
    
    PWM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC4Init(PWM_TIM, &PWM_OCInitStructure);
    
    TIM_BDTRInitTypeDef  TIM8_BDTRInitStructure;
    TIM_BDTRStructInit(&TIM8_BDTRInitStructure);
    
    TIM8_BDTRInitStructure.TIM_DeadTime = 0;
    TIM8_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
    TIM8_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(PWM_TIM, &TIM8_BDTRInitStructure);
    
    TIM_CtrlPWMOutputs(PWM_TIM, ENABLE);    //高级定时器专用设置函数
    
    TIM_Cmd(PWM_TIM, ENABLE);
    
}

/**************************************
 * 函数：底盘Motor1 PWM设置
 * 参数：uint16_t Compare
 * 返回值：无
 * 注意：无
 **************************************
 */

void ChassisMotor1_SetCompare(uint16_t Compare)
{
    TIM_SetCompare1(PWM_TIM, Compare);
}

/**************************************
 * 函数：底盘Motor2 PWM设置
 * 参数：uint16_t Compare
 * 返回值：无
 * 注意：无
 **************************************
 */

void ChassisMotor2_SetCompare(uint16_t Compare)
{
    TIM_SetCompare2(PWM_TIM, Compare);
}

/**************************************
 * 函数：底盘Motor3 PWM设置
 * 参数：uint16_t Compare
 * 返回值：无
 * 注意：无
 **************************************
 */

void ChassisMotor3_SetCompare(uint16_t Compare)
{
    TIM_SetCompare3(PWM_TIM, Compare);
}

/**************************************
 * 函数：底盘Motor4 PWM设置
 * 参数：uint16_t Compare
 * 返回值：无
 * 注意：无
 **************************************
 */

void ChassisMotor4_SetCompare(uint16_t Compare)
{
    TIM_SetCompare4(PWM_TIM, Compare);
}

void PA12_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef PWM_InitStructure;
    PWM_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    PWM_InitStructure.GPIO_Pin = GPIO_Pin_11;
    PWM_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOA, &PWM_InitStructure);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    PWM_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    PWM_InitStructure.GPIO_Pin = GPIO_Pin_2;
    PWM_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &PWM_InitStructure);
}


