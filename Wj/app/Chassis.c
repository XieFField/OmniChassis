/**************************************
 * @file  ：Chassis.c
 * @author：XieFField
 * @date  ：2025/5/17
 * @brief ：底盘控制模块
 **************************************
 */
#include "stm32f10x.h"                  // Device header
#include "Chassis.h"
#include "Servo.h"
#include "math.h"
#include "PID.h"
#include "bsp_motorTIM.h"
#include "bsp_ChassisEncoder.h"
#include "Incl_z.h"
#include "Delay.h"
#include <stdio.h>


ChassisMotor_t          ChassisMotor;        //底盘参数结构体
ChassisMotor_t* Data;                        //底盘指针结构体
VL5300_DataSetTypeDef *Laser_Data;           //激光测距结构体

FSM_E chassis_ctrl;                          //底盘状态机
SERVO_E servo_ctrl;                          //舵机状态机
PATH_E path_ctrl;                            //路径状态机
PI_E   Red_Grean_ctrl;                       //红绿灯的状态判断

MorseCode_T code_read;                       //树莓派电码储存

pid_type_def Chassis_PID[5];
int myabs(int a);
ChassisMotor_t* Chassis_ReadData(void);     //用于将变量转换成指针的，除了调PID给上位机发送参数外，应该用不上

//位置式 R1
 float Motor_Kp[5] = {2.9,  2.9,  1.5,      2,   1};            /*R1是小轮底盘的参数*/
 float Motor_Ki[5] = {0.15, 0.15, 0.15,     0.1, 0.2};
 float Motor_Kd[5] = {0.1,  0.1,  0.1,     0.1, 0.5};

//位置式 R2
//float Motor_Kp[5] = {2.5,  2.4,  2.9,     1.9,   1};          /*R2是大轮底盘的参数*/
//float Motor_Ki[5] = {0.9,  0.1,  0.1,     0.1, 0.2};
//float Motor_Kd[5] = {0.5,  0.1,  0.1,     0.1, 0.5};

//比大小函数
float maxNum(float a, float b)
{
    float temp;
    if(a > b)
    {
        temp = a;
    }
    else if(a < b)
    {
        temp = b;
    }
    else if(a == b)
    {
        temp = a;
    }
    return temp;
}

/**************************************
 * 函数：全向轮底盘GPIO初始化函数
 * 参数：无
 * 返回值：无
 * 说明：之所以要在这里把GPIO和PIN赋值到底盘结构体变量里，
 *      是为了方便执行重复操作，如果遇到要执行四个电机的重复操作，
 *      那么用一个for循环，就可以执行完。
 **************************************
 */
 
void Chassis_GPIOInit(ChassisMotor_t *init)
{
    init->motor.motor_gpio[0].gpio = ChassisMotor1_GPIO;
    init->motor.motor_gpio[1].gpio = ChassisMotor2_GPIO;
    init->motor.motor_gpio[2].gpio = ChassisMotor3_GPIO;
    init->motor.motor_gpio[3].gpio = ChassisMotor4_GPIO;
    
    init->motor.motor_gpio[0].in1  = ChassisMotor1_GPIOLPin;
    init->motor.motor_gpio[1].in1  = ChassisMotor2_GPIOLPin;
    init->motor.motor_gpio[2].in1  = ChassisMotor3_GPIOLPin;
    init->motor.motor_gpio[3].in1  = ChassisMotor4_GPIOLPin;
    
    init->motor.motor_gpio[0].in2  = ChassisMotor1_GPIORPin;
    init->motor.motor_gpio[1].in2  = ChassisMotor2_GPIORPin;
    init->motor.motor_gpio[2].in2  = ChassisMotor3_GPIORPin;
    init->motor.motor_gpio[3].in2  = ChassisMotor4_GPIORPin;
    
    RCC_APB2PeriphClockCmd(ChassisMotor1_RCC | ChassisMotor2_RCC | ChassisMotor3_RCC | ChassisMotor4_RCC, ENABLE);
    
    //motor1
    GPIO_InitTypeDef Motor1_InitStructure;
    Motor1_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    Motor1_InitStructure.GPIO_Pin =  init->motor.motor_gpio[0].in1| init->motor.motor_gpio[0].in2;
    Motor1_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(init->motor.motor_gpio[0].gpio, &Motor1_InitStructure);
    GPIO_SetBits(init->motor.motor_gpio[0].gpio, init->motor.motor_gpio[0].in1| init->motor.motor_gpio[0].in2);
    
    //motor2
    GPIO_InitTypeDef Motor2_InitStructure;
    Motor2_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    Motor2_InitStructure.GPIO_Pin =  init->motor.motor_gpio[1].in1| init->motor.motor_gpio[1].in2;
    Motor2_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(init->motor.motor_gpio[1].gpio, &Motor2_InitStructure);
    GPIO_SetBits(init->motor.motor_gpio[1].gpio, init->motor.motor_gpio[1].in1| init->motor.motor_gpio[1].in2);
    
    //motor3
    GPIO_InitTypeDef Motor3_InitStructure;
    Motor3_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    Motor3_InitStructure.GPIO_Pin =  init->motor.motor_gpio[2].in1| init->motor.motor_gpio[2].in2;
    Motor3_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(init->motor.motor_gpio[2].gpio, &Motor3_InitStructure);
    GPIO_SetBits(init->motor.motor_gpio[2].gpio, init->motor.motor_gpio[2].in1| init->motor.motor_gpio[2].in2);
    
    //motor4  
    GPIO_InitTypeDef Motor4_InitStructure;
    Motor4_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    Motor4_InitStructure.GPIO_Pin =  init->motor.motor_gpio[3].in1| init->motor.motor_gpio[3].in2;
    Motor4_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(init->motor.motor_gpio[3].gpio, &Motor4_InitStructure);
    GPIO_SetBits(init->motor.motor_gpio[3].gpio, init->motor.motor_gpio[3].in1| init->motor.motor_gpio[3].in2);
}

/**************************************
 * 函数：底盘初始化集成函数
 * 参数：无
 * 返回值：无
 * 注意：无
 **************************************
 */
void Chassis_Init(void)
{  
    ChassisMotor.maxVx = 79000.2f ;
    ChassisMotor.maxVy = 79000.2f ;
    
    /*由于每个电机体质不一样，甚至左右转速不一，所以出此下策*/
    ChassisMotor.maxRpm_Left[0] = 307;  //已测试 307        
    ChassisMotor.maxRpm_Left[1] = 360;  //已测试
    ChassisMotor.maxRpm_Left[2] = 350;  //已测试
    ChassisMotor.maxRpm_Left[3] = 345;
    
    ChassisMotor.maxRpm_Right[0] = 330; //已测试 330
    ChassisMotor.maxRpm_Right[1] = 360; //已测试
    ChassisMotor.maxRpm_Right[2] = 350; //已测试
    ChassisMotor.maxRpm_Right[3] = 345;
    
    ChassisMotor.gear = 1;              //档位1, 2, 3
    
    ChassisMotor.speedtap[0] = 0.3;
    ChassisMotor.speedtap[1] = 0.5;
    ChassisMotor.speedtap[2] = 1;
    ChassisMotor.pidMode = PID_POSITION;

    ChassisMotor.chassisVx = 0;
    ChassisMotor.chassisVy = 0;

    ChassisMotor.chassisVx_calc = 0;    //速度规划的
    ChassisMotor.chassisVy_calc = 0;

    //初始化，舵机状态归零
    Servo1_Close;
    Servo2_Close;

    servo_ctrl = 0;
    MotorPID_Init();
    ChassisPWM_Init();
    bsp_ChassisEncoder_Init();
    VL5300_Init();
    TIM6_Init(T);
    Chassis_GPIOInit(&ChassisMotor);
}

int flag;

/**************************************
 * 函数：全向轮底盘PID初始化函数
 * 参数：void
 * 返回值：无
 * 注意：无
 **************************************
 */
void MotorPID_Init(void)
{
    //底盘PID初始化
    int i;
    for(i = 0; i < 4; i++)
    {
        Chassis_PID[i].Kp = Motor_Kp[i];
        Chassis_PID[i].Kd = Motor_Kd[i];
        Chassis_PID[i].Ki = Motor_Ki[i];

        Chassis_PID[i].max_iout = 250;
        Chassis_PID[i].max_out = 300;

        Chassis_PID[i].mode = ChassisMotor.pidMode;

        ChassisMotor.motor.motor_set[i].speed_set = 0;
    }
}


/**************************************
 * 函数：全向轮底盘逆解算
 * 参数：Vx Vy Omega 自己看
 * 参数：ChassisMotor_t *speedcalc
 * 返回值：无
 * 注意：无
 **************************************
 */
void ChassisCalculate(float Vx, float Vy, ChassisMotor_t *speedcalc)
{
        Vx = Vx * ChassisMotor.speedtap[ChassisMotor.gear - 1];
        Vy = Vy * ChassisMotor.speedtap[ChassisMotor.gear - 1];
        speedcalc->motor.motor_set[0].speed_set = ((sqrt(2) / 2) * Vx - (sqrt(2) / 2) * Vy ) * K;
        speedcalc->motor.motor_set[1].speed_set = (((-1) * sqrt(2) / 2) * Vx - (sqrt(2) / 2) * Vy) * K;
        speedcalc->motor.motor_set[2].speed_set = (-(sqrt(2) / 2) *Vx + (sqrt(2) / 2) * Vy ) * K;
        speedcalc->motor.motor_set[3].speed_set = ((sqrt(2) / 2) * Vx + (sqrt(2) / 2) * Vy ) * K;
        for(int i = 0; i < 4; i++)
        {
            /*输出限幅*/
            if(speedcalc->motor.motor_set[i].speed_set > 0)
            {
                if(speedcalc->motor.motor_set[i].speed_set > ChassisMotor.maxRpm_Left[i]){speedcalc->motor.motor_set[i].speed_set = ChassisMotor.maxRpm_Left[i];}
            }
            if(speedcalc->motor.motor_set[i].speed_set < 0)
            {
                if(speedcalc->motor.motor_set[i].speed_set < -ChassisMotor.maxRpm_Right[i]){speedcalc->motor.motor_set[i].speed_set = -ChassisMotor.maxRpm_Right[i];}
            }
        }
}


/**************************************
 * 函数：PID计算
 * 参数：ChassisMotor_t 底盘结构体指针
 * 返回值：无
 * 注意：无
 **************************************
 */
void Chassis_PIDCalc(ChassisMotor_t *pid)
 {   
    int i;
    for(i = 0; i<4; i++)
    {
        if(pid->motor.motor_set[i].speed_set != pid->motor.motor_fdb[i].speed_fdb)
        {
            pid->motor.motor_set[i].pid_speed = PID_Calc(&Chassis_PID[i], pid->motor.motor_fdb[i].speed_fdb, pid->motor.motor_set[i].speed_set);
            
            
            if(myabs(pid->motor.motor_set[i].speed_set) < 0.1)
            {
                pid->motor.motor_set[i].pid_speed = 0;
            }
            float temp;
            if(pid->motor.motor_set[i].pid_speed > 0)
            {
                temp = pid->motor.motor_set[i].pid_speed / ChassisMotor.maxRpm_Left[i];
                pid->motor.motor_set[i].current_set = (uint16_t)(temp * 100);
            }
            else if(pid->motor.motor_set[i].pid_speed < 0)
            {
                temp = pid->motor.motor_set[i].pid_speed / ChassisMotor.maxRpm_Right[i];
                pid->motor.motor_set[i].current_set = (uint16_t)(temp * 100);
            }
            if(pid->motor.motor_set[i].current_set > 100){pid->motor.motor_set[i].current_set = 100;}
            
            //Motor Turn 引脚翻转，控制电机方向
            //用上这个就可以利用PID做到刹车的功能
            if(pid->motor.motor_set[i].pid_speed > 0)
            {
                GPIO_SetBits(pid->motor.motor_gpio[i].gpio, pid->motor.motor_gpio[i].in1);
                GPIO_ResetBits(pid->motor.motor_gpio[i].gpio, pid->motor.motor_gpio[i].in2);
            }
            else if(pid->motor.motor_set[i].pid_speed < 0)
            {
                GPIO_SetBits(pid->motor.motor_gpio[i].gpio, pid->motor.motor_gpio[i].in2);
                GPIO_ResetBits(pid->motor.motor_gpio[i].gpio, pid->motor.motor_gpio[i].in1);
            }
            else if(pid->motor.motor_set[i].pid_speed == 0)
            {
                GPIO_SetBits(pid->motor.motor_gpio[i].gpio, pid->motor.motor_gpio[i].in1);
                GPIO_SetBits(pid->motor.motor_gpio[i].gpio, pid->motor.motor_gpio[i].in2);
            }

        }
        else if((pid->motor.motor_set[i].speed_set == 0) && (pid->motor.motor_fdb[i].speed_fdb == 0))
        {
            pid->motor.motor_set[i].pid_speed = 0;
        }
    }
}

/**************************************
 * 函数：获取底盘电机实际转速函数
 * 参数：ChassisMotor_t *fdb 底盘参数结构体
 * 返回值：无
 * 注意：无
 **************************************
 */
void Chassis_fdb(ChassisMotor_t *fdb)
{
    int i = 0;
    for(i = 0; i < 4; i++)
    {
        fdb->motor.motor_fdb[i].coder_fdb = Chassis_ReadEncoder(i + 1);
        fdb->motor.motor_fdb[i].speed_fdb = 60 * (fdb->motor.motor_fdb[i].coder_fdb * timeHz / 4) / (Motor_i * NPN);
        if(myabs(fdb->motor.motor_fdb[i].speed_fdb) < 1){fdb->motor.motor_fdb[i].speed_fdb = 0;}
    } 
    fdb->motor.motor_fdb[0].speed_fdb = - fdb->motor.motor_fdb[0].speed_fdb; //这个编码器体质不一样，需要取反
}

/**************************************
 * 函数：电机控制函数(PWM)
 * 参数：
 * 返回值：无 
 * 注意：无
 **************************************
 */

void Motor_Control(int16_t current_1,int16_t current_2,int16_t current_3,int16_t current_4)
{
    ChassisMotor1_SetCompare(current_1);
    ChassisMotor2_SetCompare(current_2);
    ChassisMotor3_SetCompare(current_3);
    ChassisMotor4_SetCompare(current_4);
}

/**************************************
 * 函数：转速转化为PWM
 * 参数：ChassisMotor_t *set 底盘参数结构体
 * 返回值：无
 * 注意：无
 **************************************
 */
void CurrentCalc(ChassisMotor_t *set)
{
   uint8_t i;
   for(i = 0; i < 4; i++)
   {
        if(set->motor.motor_set[i].speed_set > 0)
        {
            set->motor.motor_set[i].current_set = ((set->motor.motor_set[i].speed_set / ChassisMotor.maxRpm_Left[i]) * 100);
            if(set->motor.motor_set[i].current_set > 100){set->motor.motor_set[i].current_set = 100;}
        }
        else if(set->motor.motor_set[i].speed_set < 0)
        {
            set->motor.motor_set[i].current_set = ((set->motor.motor_set[i].speed_set / ChassisMotor.maxRpm_Right[i]) * 100);
            if(set->motor.motor_set[i].current_set > 100){set->motor.motor_set[i].current_set = 100;}
        }
        else if(set->motor.motor_set[i].speed_set == 0)
        {
            set->motor.motor_set[i].current_set = 0;
        }
   }
}

/**************************************
 * 函数：电机IN1 IN2电平设置
 * 参数：ChassisMotor_t *turn 底盘参数结构体
 * 返回值：无
 * 注意：无
 **************************************
 */
void Motor_Turn(ChassisMotor_t *turn)
{
    int i;
    for(i = 0; i < 4; i++)
    {
                if(turn->motor.motor_set[i].speed_set > 0)
                {
                    GPIO_SetBits(turn->motor.motor_gpio[i].gpio, turn->motor.motor_gpio[i].in1);
                    GPIO_ResetBits(turn->motor.motor_gpio[i].gpio, turn->motor.motor_gpio[i].in2);
                }
                else if(turn->motor.motor_set[i].speed_set < 0)
                {
                    GPIO_SetBits(turn->motor.motor_gpio[i].gpio, turn->motor.motor_gpio[i].in2);
                    GPIO_ResetBits(turn->motor.motor_gpio[i].gpio, turn->motor.motor_gpio[i].in1);
                }
                else if(turn->motor.motor_set[i].speed_set == 0)
                {
                    GPIO_SetBits(turn->motor.motor_gpio[i].gpio, turn->motor.motor_gpio[i].in1);
                    GPIO_SetBits(turn->motor.motor_gpio[i].gpio, turn->motor.motor_gpio[i].in2);
                }
    }
}

/**************************************
 * 函数：将底盘结构体变量转换为指针变量
 * 参数：无
 * 返回值：ChassisMotor_t
 * 注意：无
 **************************************
 */
ChassisMotor_t* Chassis_ReadData(void)
{
    static ChassisMotor_t Chassis_Data;
    Chassis_Data.chassisVx = ChassisMotor.chassisVx;
    Chassis_Data.chassisVy = ChassisMotor.chassisVy;
    Chassis_Data.anglespeed_set = ChassisMotor.anglespeed_set;
    Chassis_Data.gear = ChassisMotor.gear;
    Chassis_Data.ctrl_mode = ChassisMotor.ctrl_mode;
    Chassis_Data.anglespeed_fdb = ChassisMotor.anglespeed_fdb;
    Chassis_Data.anglespeed_set = ChassisMotor.anglespeed_set;
    Chassis_Data.pid_anglespeed = ChassisMotor.pid_anglespeed;
    uint8_t i;
    for(i = 0; i < 4; i++)
    {
        Chassis_Data.motor.motor_set[i].speed_set = ChassisMotor.motor.motor_set[i].speed_set;
    }

    for(i = 0; i < 4; i++)
    {
        Chassis_Data.motor.motor_fdb[i].speed_fdb = ChassisMotor.motor.motor_fdb[i].speed_fdb;
    }
    for(i = 0; i < 4; i++)
    {
        Chassis_Data.motor.motor_set[i].pid_speed = ChassisMotor.motor.motor_set[i].pid_speed;
    }
    for(i = 0; i < 4; i++)
    {
        Chassis_Data.motor.motor_set[i].current_set = ChassisMotor.motor.motor_set[i].current_set;
    }
    return  &Chassis_Data;
}


/**************************************************************************
功能：翻译树莓派的电码
参数：void
返回值: void
**************************************************************************/
void Code_Tranf(void)
{
    MorCode_Read(&code_read);
    if(code_read.code[0] == 1 && code_read.code[1] == 1 && code_read.code[2] == 1)
    {
        servo_ctrl = Servo1_OPEN_S;
        path_ctrl = THE_PATH_ONE;
    }
    else if(code_read.code[0] == 1 && code_read.code[1] == 1 && code_read.code[2] == 0)
    {
        servo_ctrl = Servo1_OPEN_S;
        path_ctrl = THE_PATH_TWO;
    }
    else if(code_read.code[0] == 1 && code_read.code[1] == 0 && code_read.code[2] == 1)
    {
        servo_ctrl = Servo2_OPEN_S;
        path_ctrl = THE_PATH_ONE;
    }
    else if(code_read.code[0] == 1 && code_read.code[1] == 0 && code_read.code[2] == 0)
    {
        servo_ctrl = Servo2_OPEN_S;
        path_ctrl = THE_PATH_TWO;
    }

    if(code_read.code[0] == 0 && code_read.code[1] == 1 && code_read.code[2] == 1)
    {
        Red_Grean_ctrl = Read_Green_Light;
    }
    else
    {
        Red_Grean_ctrl = Read_RED_Light;
        servo_ctrl = 0;
    }
}

/**************************************************************************
功能：取绝对值
参数：需要取绝对值的数  int a
返回值: temp
**************************************************************************/
int myabs(int a)
{ 		   
	int temp;
	if(a<0)  
	  temp=-a;  
	else 
	  temp=a;
	return temp;
}

int textnum = 0;

/**************************************************************************
功能：总状态机
参数：void
返回值: void
**************************************************************************/
int auto_num = 0;

void Chassis_FSM(void)
{
    if(auto_num == 0)
    {
        auto_num = 1;
        chassis_ctrl = STOP_WAIT;
        do{
            Code_Tranf();
            Chassis_Task();
        }while(servo_ctrl = 0);

        if(servo_ctrl == Servo1_OPEN_S)
        {
            Servo1_Open;
            servo_ctrl = Servo1_CLOSE_S;
        }
        else if(servo_ctrl == Servo2_OPEN_S)
        {
            Servo2_Open;
            servo_ctrl = Servo2_CLOSE_S;
        }
        Delay_s(3);
        Servo1_Close;
        Servo2_Close;
        
        
        chassis_ctrl = Forward_PATH;
        time = 0;
        while(time  <= 920){ Chassis_Task();}
        time = 0;
        chassis_ctrl = STOP_WAIT;
        while(Red_Grean_ctrl != Read_Green_Light)
        {
            Code_Tranf();
            Chassis_Task();
        }
        time = 0;
        chassis_ctrl = Forward_PATH;
        while(time  <= 200){ Chassis_Task();}
        time = 0;
        /*判断左右*/
        if(path_ctrl == Right_PATH)
        {
            chassis_ctrl = STOP_WAIT;
            while(time  <= 50){ Chassis_Task();}
            time = 0;
            chassis_ctrl = Right_PATH;
            while(time  <= 650){ Chassis_Task();}
            time = 0;
            chassis_ctrl = STOP_WAIT;
            while(time <= 200){Chassis_Task();}
            time = 0;
            if(servo_ctrl == Servo1_CLOSE_S)
            {
                Servo1_Open;
                while(time <= 200) {Chassis_Task();}
                Servo1_Close;
            }
            else if(servo_ctrl == Servo2_CLOSE_S)
            {
                Servo2_Open;
                while(time <= 200) {Chassis_Task();}
                Servo2_Close;
            }
            time = 0;
            
            chassis_ctrl = Left_PATH;
            while(time  <= 640){ Chassis_Task();}
            time = 0;
            chassis_ctrl = Backward_PATH;
            while(time  <= 920 + 230){ Chassis_Task();}
            time = 0;
        }
        /*判断左右*/
        else if(path_ctrl == Left_PATH)
        {
            chassis_ctrl = STOP_WAIT;
            while(time <= 200){Chassis_Task();}
            time = 0;
            if(servo_ctrl == Servo1_CLOSE_S)
            {
                Servo1_Open;
                while(time <= 200){Chassis_Task();}
                Servo1_Close;
            }
            else if(servo_ctrl == Servo2_CLOSE_S)
            {
                Servo2_Open;
                while(time <= 200){Chassis_Task();}
                Servo2_Close;
            }
            time = 0;

            chassis_ctrl = Left_PATH;
            while(time  <= 580){ Chassis_Task();}
            time = 0;
            chassis_ctrl = Right_PATH;
            while(time  <= 570){ Chassis_Task();}
            time = 0;
            chassis_ctrl = Backward_PATH;
            while(time  <= 920 + 230){ Chassis_Task();}
            time = 0;
        }
        
        
    }
}

/**************************************************************************
功能：用于路径测试的状态机
参数：void
返回值: void
**************************************************************************/
int time = 0;
void Chassis_FSM_TEST(void)
{
//    Laser_Data = VL5300_GetDataSetPoint();
    if(auto_num == 0)
    {    
        auto_num++;
        chassis_ctrl = STOP_WAIT;
        Chassis_Task();
        Delay_s(2);
        chassis_ctrl = Forward_PATH;
        time = 0;
        while(time  <= 920){ Chassis_Task();}
        time = 0;
        chassis_ctrl = STOP_WAIT;
        while(time  <= 100){ Chassis_Task();}
        time = 0;
        chassis_ctrl = Forward_PATH;
        while(time  <= 200){ Chassis_Task();}
        time = 0;
       /* chassis_ctrl = Left_PATH;
        while(time  <= 580){ Chassis_Task();}
        time = 0;*/
        chassis_ctrl = STOP_WAIT;
        while(time  <= 50){ Chassis_Task();}
        time = 0;
        chassis_ctrl = Right_PATH;
        while(time  <= 650){ Chassis_Task();}
        time = 0;
        chassis_ctrl = Left_PATH;
        while(time  <= 640){ Chassis_Task();}
        time = 0;
        chassis_ctrl = Backward_PATH;
        while(time  <= 920 + 230){ Chassis_Task();}
        time = 0;
    }
    chassis_ctrl = STOP_WAIT;
    Chassis_Task();
}


/**************************************************************************
功能：裁定
参数：void
返回值: void
**************************************************************************/
void Chassis_Task(void)
{
    if(chassis_ctrl == STOP_WAIT)
    {
        ChassisMotor.chassisVx = 0;
        ChassisMotor.chassisVy = 0;
    }
    else if(chassis_ctrl == Forward_PATH)
    {
        ChassisMotor.chassisVx = 0;
        ChassisMotor.chassisVy = -ChassisMotor.maxVy * 0.45;
    }
    else if(chassis_ctrl == Left_PATH)
    {
        ChassisMotor.chassisVx = -ChassisMotor.maxVx * 0.45;
        ChassisMotor.chassisVy = 0;
    }
    else if(chassis_ctrl == Right_PATH)
    {
        ChassisMotor.chassisVx = ChassisMotor.maxVx * 0.45;
        ChassisMotor.chassisVy = 0;
    }
    else if(chassis_ctrl == Backward_PATH)
    {
        ChassisMotor.chassisVx = 0;
        ChassisMotor.chassisVy = ChassisMotor.maxVy * 0.45;
    }
    else
    {
        ChassisMotor.chassisVx = 0;
        ChassisMotor.chassisVy = 0;
    }
    ChassisCalculate(ChassisMotor.chassisVx_calc, ChassisMotor.chassisVy_calc, &ChassisMotor);
    Motor_Control(ChassisMotor.motor.motor_set[0].current_set, ChassisMotor.motor.motor_set[1].current_set, 
                        ChassisMotor.motor.motor_set[2].current_set, ChassisMotor.motor.motor_set[3].current_set);  
    Motor_Turn(&ChassisMotor);
}

/**************************************************************************
功能：速度规划
参数：void
返回值: void
说明：最简陋的一种梯形规划，基于v = v0 + a * dt的公式，只有在加速时候会规划
    若是在减速时候加上规划，那么就不能急停了。
**************************************************************************/
void Speed_Plan(void)
{
    if(ChassisMotor.chassisVx != 0)
        {
            //加速
            if(ChassisMotor.chassisVx >= ChassisMotor.chassisVx_calc && ChassisMotor.chassisVx > 0)
            {
                ChassisMotor.chassisVx_calc = ChassisMotor.chassisVx_last + acc_vel *dt;
            }
            else if(ChassisMotor.chassisVx <= ChassisMotor.chassisVx_calc && ChassisMotor.chassisVx < 0)
            {
                ChassisMotor.chassisVx_calc = ChassisMotor.chassisVx_last - acc_vel *dt;
            }

            //限幅
            if(ChassisMotor.chassisVx_calc >= (ChassisMotor.maxVx * 0.45))
            {
                ChassisMotor.chassisVx_calc = ChassisMotor.maxVx * 0.45;
            }
            else if(ChassisMotor.chassisVx_calc <= -(ChassisMotor.maxVx * 0.45))
            {
                ChassisMotor.chassisVx_calc = -ChassisMotor.maxVx *0.45;
            }

            ChassisMotor.chassisVx_last = ChassisMotor.chassisVx_calc;
        }
        else
        {
            ChassisMotor.chassisVx_calc = 0;
        }

        if(ChassisMotor.chassisVy != 0)
        {
            //加速
            if(ChassisMotor.chassisVy >= ChassisMotor.chassisVy_calc && ChassisMotor.chassisVy > 0)
            {
                ChassisMotor.chassisVy_calc = ChassisMotor.chassisVy_last + acc_vel *dt;
            }
            else if(ChassisMotor.chassisVy <= ChassisMotor.chassisVy_calc && ChassisMotor.chassisVy < 0)
            {
                ChassisMotor.chassisVy_calc = ChassisMotor.chassisVy_last - acc_vel *dt;
            }

            //限幅
            if(ChassisMotor.chassisVy_calc >= (ChassisMotor.maxVy * 0.45))
            {
                ChassisMotor.chassisVy_calc = ChassisMotor.maxVy * 0.45;
            }
            
            else if(ChassisMotor.chassisVy_calc <= -(ChassisMotor.maxVy * 0.45))
            {
                ChassisMotor.chassisVy_calc = -ChassisMotor.maxVy * 0.45;
            }

            ChassisMotor.chassisVy_last = ChassisMotor.chassisVy_calc;
        }
        else
        {
            ChassisMotor.chassisVy_calc = 0;
        }
}


/**************************************
 * 函数：中断读取编码器
 * 参数：自己看
 * 返回值：无
 * 注意：无
 **************************************
 */
int cout = 0;
void TIM6_IRQHandler(void)
{    
    if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        time += 1;
        cout +=1;
        
        Chassis_fdb(&ChassisMotor);
        /*启动100ms后再进入PID，目的是保证初始化前不进入该函数，
        有更好的解决方法，即每段Init都加上判断，若未初始化返回Init_Falut标志位
        */
        if(cout >= 10)              
        {    
            Speed_Plan();
            Chassis_PIDCalc(&ChassisMotor);  //PID
        }
    }
    TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
}
