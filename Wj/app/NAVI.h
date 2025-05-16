#ifndef __NAVI_H
#define __NAVI_H

/*      宏 定 义       */
#define Measure_frequency   100
#define Acc_gravity         9.8066
#define min_to_s            60
#define meter_to_mm         1000

/*      结 构 体       */
typedef struct{
    float speed_x;
    float speed_y;
}SPEED_t;

typedef struct{
    float Loca_x;       //XY坐标
    float Loca_y;
    float Head_angle;   //航向角
    float speed_angle;  //角速度
    SPEED_t Chassis;
    SPEED_t Word;
}NAVI_t;

/*      函数声明       */
void NAVI_Init(NAVI_t *init);

NAVI_t * NAVI_DataCalc_Task(void);
#endif
