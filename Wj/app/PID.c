/**************************************
 * @file  ：PID.c
 * @author：吴嘉
 * @date  ：2025/2/25
 * @brief ：PID集成模块
 **************************************
 */

#include "stm32f10x.h"                  // Device header
#include "PID.h"
#include <stdio.h>

/**************************************
 * 函数：限幅函数
 * 参数：input 输入的参数
 * 参数：max   限幅
 * 注意：无
 **************************************
 */

void PID_Limit(pid_type_def *pid)
{
    if(pid->Iout > pid->max_iout)
    {
        pid->Iout = pid->max_iout;
    }
    else if(pid->Iout < -pid->max_iout)
    {
        pid->Iout = -pid->max_iout;
    }
    
    if(pid->out > pid->max_out)
    {
        pid->out = pid->max_out;
    }
    else if(pid->out < -pid->max_out)
    {
        pid->out = -pid->max_out;
    }
}

//#define LimitMax(input, max)   \
//    {                          \
//        if (input > max)       \
//        {                      \
//            input = max;       \
//        }                      \
//        else if (input < -max) \
//        {                      \
//            input = -max;      \
//        }                      \
//    }

/**************************************
 * 函数：PID初始化
 * 参数：pid_type_def *pid, const float PID[3],  float max_out, float max_iout
 * 注意：无
 **************************************
 */

const float PID[3] = {1, 1, 1};         //PID的 Kp Ki Kd 的值

void PID_Init(pid_type_def *pid, const float PID[3],  float max_out, float max_iout, uint8_t mode)
{
    if (pid == NULL || PID == NULL)
    {
        return;
    }
    pid->mode = mode;
    pid->Kp = PID[0];
    pid->Ki = PID[1];
    pid->Kd = PID[2];
    pid->max_out = max_out;
    pid->max_iout = max_iout;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->error[0] = pid->error[1] = pid->error[2] = pid->Pout = pid->Iout = pid->Dout = pid->out = 0.0f;
}

/**************************************
 * 函数：PID计算
 * 参数：PidTypeDef_t *pid PID结构体指针
 *       ref 当前回传的实际值
 *       set 期望值
 * 返回值：Output PID控制器的输出值
 * 注意：无
 **************************************
 */

float PID_Calc(pid_type_def *pid, float ref, float set)
{
    if (pid == NULL)
    {
        return 0.0f;
    }

    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];

    pid->error[0] = set - ref;
    
    /*      增量式PID       */    
    if(pid->mode == PID_DELTA)
    {
       pid->Pout = pid->Kp * (pid->error[0] - pid->error[1]);
       pid->Iout = pid->Ki * pid->error[0];
       pid->Dbuf[2] = pid->Dbuf[1];
       pid->Dbuf[1] = pid->Dbuf[0];
       pid->Dbuf[0] = (pid->error[0] - 2.0f * pid->error[1] + pid->error[2]);
       pid->Dout = pid->Kd * pid->Dbuf[0];
       PID_Limit(pid);
    }
    
    /*      位置式PID       */
    else if(pid->mode == PID_POSITION)
    {
        pid->Pout = pid->Kp * pid->error[0];
        pid->Iout += pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = pid->error[0] - pid->error[1];
        pid->Dout = pid->Kd * pid->Dbuf[0];
        PID_Limit(pid);
    }
    pid->out = pid->Pout + pid->Iout + pid->Dout;


    return pid->out;
}

/**************************************
 * 函数：PID输出清除
 * 参数：PidTypeDef_t *pid PID结构数据指针
 * 返回值：Output PID控制器的输出值
 * 注意：无
 **************************************
 */

void PID_Clear(pid_type_def *pid)
{
    if (pid == NULL)
    {
        return;
    }

    pid->error[0] = pid->error[1] = pid->error[2] = 0.0f;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->out = pid->Pout = pid->Iout = pid->Dout = 0.0f;
    pid->fdb = pid->set = 0.0f;
}

void PID_Change(pid_type_def *pid, float Kp, float Ki, float Kd)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
}
