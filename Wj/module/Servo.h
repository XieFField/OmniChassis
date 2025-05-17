#ifndef __SERVO_H
#define __SERVO_H
#include "stm32f10x.h"

// 定义PWM参数
#define PWM_PERIOD   20000  // 20ms周期(72MHz/(72*20)=50Hz)
#define PWM_PRESCALER  72   // 预分频值

// 脉宽限制(0.5ms-2.5ms)
#define SERVO_MIN  500    // 0.5ms = 0°
#define SERVO_MAX  2500   // 2.5ms = 180°

typedef struct 
{
    int code[3];
}MorseCode_T;

/*      函数声明        */

void TIM5_PWM_Init(void);

// 设置舵机角度
// servo_num: 1-舵机1(PA0), 2-舵机2(PA1)
// angle: 0-180度
void Set_Servo_Angle(uint8_t servo_num, uint8_t angle);

// 舵机开关控制
// state: 0-关闭(0°), 1-打开(180°)
void Servo_Switch(uint8_t servo_num, uint8_t state);


void MorCode_Read(MorseCode_T * read);

#define Servo1_Open Servo_Switch(1, 100)
#define Servo2_Open Servo_Switch(2, 100)

#define Servo1_Close Servo_Switch(1, 0)
#define Servo2_Close Servo_Switch(2, 0)

#endif
