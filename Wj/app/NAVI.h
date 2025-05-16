#ifndef __NAVI_H
#define __NAVI_H

/*      �� �� ��       */
#define Measure_frequency   100
#define Acc_gravity         9.8066
#define min_to_s            60
#define meter_to_mm         1000

/*      �� �� ��       */
typedef struct{
    float speed_x;
    float speed_y;
}SPEED_t;

typedef struct{
    float Loca_x;       //XY����
    float Loca_y;
    float Head_angle;   //�����
    float speed_angle;  //���ٶ�
    SPEED_t Chassis;
    SPEED_t Word;
}NAVI_t;

/*      ��������       */
void NAVI_Init(NAVI_t *init);

NAVI_t * NAVI_DataCalc_Task(void);
#endif
