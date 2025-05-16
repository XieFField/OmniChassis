#include "stm32f10x.h"                  // Device header
#include "Servo.h"

void TIM5_PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // 1. 使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 2. 配置GPIO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // PA0,PA1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 3. 配置定时器基础
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD - 1;     // 自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    
    // 4. 配置PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;                     // 初始占空比0
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // 通道1配置
    TIM_OC1Init(TIM5, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
    
    // 通道2配置
    TIM_OC2Init(TIM5, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
    
    // 5. 启动定时器
    TIM_Cmd(TIM5, ENABLE);
    TIM_CtrlPWMOutputs(TIM5, ENABLE);
}

// 设置舵机角度
// servo_num: 1-舵机1(PA0), 2-舵机2(PA1)
// angle: 0-180度
void Set_Servo_Angle(uint8_t servo_num, uint8_t angle)
{
    uint16_t pulse_width;
    
    // 限制角度范围
    if(angle > 180) angle = 180;
    
    // 计算脉宽(线性映射)
    pulse_width = SERVO_MIN + (SERVO_MAX - SERVO_MIN) * angle / 180;
    
    // 设置对应通道的CCR值
    if(servo_num == 1) {
        TIM_SetCompare1(TIM5, pulse_width);
    } 
    else if(servo_num == 2) {
        TIM_SetCompare2(TIM5, pulse_width);
    }
}

// 舵机开关控制
// state: 0-关闭(0°), 1-打开(180°)
void Servo_Switch(uint8_t servo_num, uint8_t state)
{
    Set_Servo_Angle(servo_num, state ? 180 : 0);
}

void MorCode_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB || RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef PWM_InitStructure;
    PWM_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    PWM_InitStructure.GPIO_Pin = GPIO_Pin_11;
    PWM_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOA, &PWM_InitStructure);
    
    PWM_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    PWM_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    PWM_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &PWM_InitStructure);
}




void MorCode_Read(MorseCode_T * read)
{
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == SET)
    {
        read->code[0] = 1;
    }
    else
    {
        read->code[0] = 0;
    }

    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == SET)
    {
        read->code[1] = 1;
    }
    else
    {
        read->code[1] = 0;
    }

    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == SET)
    {
        read->code[2] = 1;
    }
    else
    {
        read->code[2] = 0;
    }
}

