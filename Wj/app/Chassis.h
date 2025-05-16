#ifndef __CHASSIS_H
#define __CHASSIS_H

#include "stm32f10x.h"                  // Device header
#include "Incl_z.h"

/*       �� �� �� �� ��      */

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


/*          �� �� ��         */

#define timeHz    100          //10ms, ����Ƶ��
#define NULL    0
#define Wheel_R 30.0f       //���Ӱ뾶
#define PI      3.1415926f  //PI
#define Motor_i 30.0f       //���ٱ�
#define NPN     13.0f       //תһȦ��������
#define T   100             //������arr
#define r   188.14  
#define MotorMAXspeed   56520 //���ٶ�
#define MAX_AngleSpeed  2.00f     //��λΪ rad/s
#define MAXVx           79580.4  
#define MAXVy           79580.4
#define K              1 / (2 * PI * Wheel_R)          //���̽���ת��ϵ�� 188.4
#define AnalogyMode 2
#define KeyMode     1
#define NotConnect  0
// ҡ��ģ����ģʽʱ��2��ҡ��������ģʽʱ��1��δ����ʱ��0

#define Left    1
#define Right   2
#define Center  0

#define Sensor_ON   1
#define Sensor_OFF  0
        
/*          ��������         */
void Chassis_Init(void);

void ChassisCalculate(float Vx, float Vy, float Omega, ChassisMotor_t *speedcalc);
void MotorPID_Init(void);
void Chassis_Task(void);
void MotorPID_Init(void);
void GyroCail_PIDInit(void);

void Chassis_PIDCalc(ChassisMotor_t *pid);
void Chassis_fdb(ChassisMotor_t *fdb);
void Motor_Control(int16_t current_1,int16_t current_2,int16_t current_3,int16_t current_4);
void Chassis_ModeSet(void);
void CurrentCalc(ChassisMotor_t *set);
void Motor_Turn(ChassisMotor_t *turn);

void Chassis_Task(void);                            //���񼯳�ģ��

#endif
