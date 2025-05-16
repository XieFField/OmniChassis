#ifndef __BSP_MOTORTIM_H
#define __BSP_MOTORTIM_H

/*       结 构 体 定 义       */



/*          宏 定 义         */

//PWM输出TIM
#define PWM_TIM                 TIM8
#define PWM_GPIO                GPIOC
#define PWM_RCCGPIO             RCC_APB2Periph_GPIOC
#define PWM_RCCTIM              RCC_APB2Periph_TIM8


//底盘电机1GPIO口
#define ChassisMotor1_PWM       GPIO_Pin_6  //PC6   T8C1

#define ChassisMotor1_GPIO      GPIOC
#define ChassisMotor1_RCC       RCC_APB2Periph_GPIOC
#define ChassisMotor1_GPIORPin  GPIO_Pin_2  //PC2   in2
#define ChassisMotor1_GPIOLPin  GPIO_Pin_3  //PC3   in1

//底盘电机2GPIO口
#define ChassisMotor2_PWM       GPIO_Pin_7  //PC7   T8C2

#define ChassisMotor2_GPIO      GPIOC
#define ChassisMotor2_RCC       RCC_APB2Periph_GPIOC
#define ChassisMotor2_GPIORPin  GPIO_Pin_4   //PC4
#define ChassisMotor2_GPIOLPin  GPIO_Pin_5   //PC5  in2

//底盘电机3GPIO口
#define ChassisMotor3_PWM       GPIO_Pin_8  //PC8   T8C3  

#define ChassisMotor3_GPIO      GPIOC
#define ChassisMotor3_RCC       RCC_APB2Periph_GPIOC
#define ChassisMotor3_GPIORPin  GPIO_Pin_1  //PC0
#define ChassisMotor3_GPIOLPin  GPIO_Pin_0  //PC1   in2

//底盘电机4GPIO口
#define ChassisMotor4_PWM       GPIO_Pin_9 	//PC9   T8C4

#define ChassisMotor4_GPIO      GPIOA
#define ChassisMotor4_RCC       RCC_APB2Periph_GPIOA
#define ChassisMotor4_GPIORPin  GPIO_Pin_10 //PA10
#define ChassisMotor4_GPIOLPin  GPIO_Pin_12 //PA12  in2

/*      函 数 声 明        */
void ChassisPWM_Init(void);

void ChassisMotor1_SetCompare(uint16_t Compare);
void ChassisMotor2_SetCompare(uint16_t Compare);
void ChassisMotor3_SetCompare(uint16_t Compare);
void ChassisMotor4_SetCompare(uint16_t Compare);
void PA12_Init(void);

#define Raspberry_PI_ON GPIO_SetBits(GPIOA, GPIO_Pin_11)
#define Raspberry_PI_OFF GPIO_ResetBits(GPIOA, GPIO_Pin_11)

#define BIBION GPIO_SetBits(GPIOB, GPIO_Pin_2)
#define BIBIOFF GPIO_ResetBits(GPIOB, GPIO_Pin_2)
#endif
