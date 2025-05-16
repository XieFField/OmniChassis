#ifndef __CHASSIS_H
#define __CHASSIS_H

#include "stm32f10x.h"                  // Device header
#include "Incl_z.h"

#define START_TO_LIGHT_DISTANCE      1550//单位为毫米
#define LIGHT_TO_ENDLINE_DISTANCE    410//单位为毫米
#define ENDLINE_TO_FINAL             1020//单位为毫米

/*       结 构 体 定 义      */

typedef struct
{ 
    float speed_set;
    float pid_speed;
    uint16_t current_set;
}MOTOR_SET;

typedef struct 
{
    float speed_fdb;
    int16_t coder_fdb;
    uint16_t current_fdb;
}MOTOR_FDB;

typedef struct 
{
    GPIO_TypeDef *gpio;
    uint16_t in1;
    uint16_t in2;
}MOTOR_GPIO;


typedef struct
{
    MOTOR_SET   motor_set[4];
    MOTOR_FDB   motor_fdb[4];
    MOTOR_GPIO  motor_gpio[4];
}MOTOR_T;

typedef struct
{
    MOTOR_T motor;
    float chassisVx;
    float chassisVy;

    float chassisVx_calc;
    float chassisVy_calc;

    float chassisVx_last;
    float chassisVy_last;

    float anglespeed_set;
    float pid_anglespeed;
    float maxRpm_Left[4];
    float maxRpm_Right[4];
    float maxVx;
    float maxVy;
    float pidMode;
    int8_t gear;
    float speedtap[3];
    float anglespeed_fdb;
    int ctrl_mode;
}ChassisMotor_t;


/*      状   态   机      */

typedef enum FSM_E
{
    GAME_START = 1,
    GAME_OVER,
    STOP_WAIT,          //等待状态

    LED1_ON,            //识别物料的亮灯
    LED1_OFF,
    LED2_ON,
    LED2_OFF,

    PATH1,              //对应不同物料的路径
    PATH2,          
    Forward_PATH,
    Left_PATH,
    Right_PATH,
    Backward_PATH,
}FSM_E;

typedef enum SERVO_E
{
    Servo1_OPEN,        //舵机状态
    Servo1_CLOSE,
    Servo2_OPEN,
    Servo2_CLOSE,
    ALL_CLOSE,
}SERVO_E;

typedef enum PI_E
{
    FOOD1,
    FOOD2,
    PI_ISABLE,
    
    Read_RED_Light,     //红绿灯
    Read_Green_Light,
}PI_E;



/*          宏 定 义         */

#define timeHz    100          //10ms, 采样频率
#define dt        10           //10ms
#define acc_vel   237          
#define NULL    0
#define Wheel_R 30.0f       //轮子半径
#define PI      3.1415926f  //PI
#define Motor_i 30.0f       //减速比
#define NPN     13.0f       //转一圈的脉冲数
#define T   100             //编码器arr
#define r   188.14  
#define MotorMAXspeed   56520 //线速度
#define MAX_AngleSpeed  2.00f     //单位为 rad/s
#define MAXVx           79580.4  // mm/min
#define MAXVy           79580.4
#define K              1 / (2 * PI * Wheel_R)          //底盘解算转速系数 188.4
#define AnalogyMode 2
#define KeyMode     1
#define NotConnect  0
// 摇杆模拟量模式时是2，摇杆数字量模式时是1，未连接时是0


#define Sensor_ON   1
#define Sensor_OFF  0
        
/*          函数声明         */
void Chassis_Init(void);

void ChassisCalculate(float Vx, float Vy, ChassisMotor_t *speedcalc);
void MotorPID_Init(void);
void Chassis_Task(void);
void MotorPID_Init(void);

void Chassis_PIDCalc(ChassisMotor_t *pid);
void Chassis_fdb(ChassisMotor_t *fdb);
void Motor_Control(int16_t current_1,int16_t current_2,int16_t current_3,int16_t current_4);

void CurrentCalc(ChassisMotor_t *set);
void Motor_Turn(ChassisMotor_t *turn);

void Chassis_Task(void);                            //任务集成模块

#endif
