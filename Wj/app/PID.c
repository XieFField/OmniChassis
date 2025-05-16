/**************************************
 * @file  ��PID.c
 * @author�����
 * @date  ��2025/2/25
 * @brief ��PID����ģ��
 **************************************
 */

#include "stm32f10x.h"                  // Device header
#include "PID.h"
#include <stdio.h>

/**************************************
 * �������޷�����
 * ������input ����Ĳ���
 * ������max   �޷�
 * ע�⣺��
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
 * ������PID��ʼ��
 * ������pid_type_def *pid, const float PID[3],  float max_out, float max_iout
 * ע�⣺��
 **************************************
 */

const float PID[3] = {1, 1, 1};         //PID�� Kp Ki Kd ��ֵ

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
 * ������PID����
 * ������PidTypeDef_t *pid PID�ṹ��ָ��
 *       ref ��ǰ�ش���ʵ��ֵ
 *       set ����ֵ
 * ����ֵ��Output PID�����������ֵ
 * ע�⣺��
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
    
    /*      ����ʽPID       */    
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
    
    /*      λ��ʽPID       */
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
 * ������PID������
 * ������PidTypeDef_t *pid PID�ṹ����ָ��
 * ����ֵ��Output PID�����������ֵ
 * ע�⣺��
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
