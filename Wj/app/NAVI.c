#include "stm32f10x.h"                  // Device header
#include "Incl_z.h"
#include <math.h>
#include "NAVI.h"

NAVI_t NAVI;
ATK_IMU901_DataTypeDef *NAVI_Data;
void NAVI_Init(NAVI_t *init)
{
    ATK_IMU901_Init();
    init->Chassis.speed_x = 0;
    init->Chassis.speed_y = 0;
    init->Loca_x = 0;
    init->Loca_y = 0;
    init->Head_angle = 0;    //��λΪrad��������
    init->speed_angle = 0;
    init->Word.speed_x = 0;
    init->Word.speed_y = 0;
}

ATK_IMU901_DataTypeDef * NAVI_DataRead(void)
{
    ATK_IMU901_DataTypeDef *read;
    read = ATK_IMU901_GetPoint_Data();
    ATK_IMU901_ReadData();
    return read;
}

//����ǲⶨ
//����һ��ֱ�Ӷ�ȡIMU901�ĺ����Yaw������(�����ʵ������)
//����������Z����ٶȻ��ֵõ�Yaw�ẽ���(���㾫��)
void HeadAngle_Calc_1(NAVI_t *Head, ATK_IMU901_DataTypeDef *read)
{
    Head->Head_angle = read->Yaw;
}

//�����ǲ��ٵ�λ ��/s
//����ǵ�λ    rad
void HeadAngle_Calc_2(NAVI_t *Head, ATK_IMU901_DataTypeDef *read)
{
    float last_Head_angle = Head->Head_angle;
    Head->Head_angle = last_Head_angle + read->GyroZ / Measure_frequency;
    
    Head->Head_angle = -Head->Head_angle * 3.14 / 180;
}

//���ٶȵ�λΪ rad/s
void Angle_SpeedRead(NAVI_t *angle, ATK_IMU901_DataTypeDef *read)
{
    angle->speed_angle = -read->GyroZ * 3.14 / 180;
}

//���ٶȼƲ��ٵ�λ g/s^2
//�ٶȵ�λ mm/min
void ChassisSpeed_Calc(NAVI_t *chassis, ATK_IMU901_DataTypeDef *read)
{
    float last_speed_x, last_speed_y;
    last_speed_x = chassis->Chassis.speed_x;
    last_speed_y = chassis->Chassis.speed_y;

    chassis->Chassis.speed_x = last_speed_x + ((read->AccX / Acc_gravity) / Measure_frequency) * min_to_s * meter_to_mm;
    chassis->Chassis.speed_y = last_speed_y + ((read->AccY / Acc_gravity) / Measure_frequency) * min_to_s * meter_to_mm;
}

void WordSpeed_Calc(NAVI_t *word, ATK_IMU901_DataTypeDef *read)
{
    float last_speed_x, last_speed_y;
    last_speed_x = word->Word.speed_x;
    last_speed_y = word->Word.speed_y;
    word->Word.speed_x = last_speed_x + word->Chassis.speed_x * cos(word->Head_angle);
    word->Word.speed_y = last_speed_y + word->Chassis.speed_y * cos(word->Head_angle);
}

//��λ���뵥λΪ mm
void Location_Calc(NAVI_t * loca)
{
    float last_location_x, last_location_y;
    last_location_x = loca->Loca_x;
    last_location_y = loca->Loca_y;

    loca->Loca_x = last_location_x + loca->Word.speed_x / Measure_frequency;
    loca->Loca_y = last_location_y + loca->Word.speed_y / Measure_frequency;
}

//����ϼ�����
NAVI_t * NAVI_DataCalc_Task(void)
{
    NAVI_t * navi;
    HeadAngle_Calc_1(navi, NAVI_Data);
    Angle_SpeedRead(navi, NAVI_Data);
    ChassisSpeed_Calc(navi, NAVI_Data);
    WordSpeed_Calc(navi, NAVI_Data);
    Location_Calc(navi);
    return navi;
}



