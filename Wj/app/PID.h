#ifndef __PID_H
#define __PID_H

/*		 PID 模 式 枚 举	 */

enum PID_MODE
{
	PID_POSITION = 0,
	PID_DELTA	 = 1
};

/*       结 构 体 定 义      */
typedef struct 
{
	//PID模式
	uint8_t mode;

	//PID 三参数
	float Kp;
	float Ki;
	float Kd;

	float max_out;  //最大输出
	float max_iout; //最大积分输出

	float set;
	float fdb;

	float out;
	float Pout;
	float Iout;
	float Dout;
	float Dbuf[3];  //微分项 0最新 1上一次 2上上次
	float error[3]; //误差项 0最新 1上一次 2上上次

} pid_type_def;


/*          宏 定 义         */



/*          函数声明         */

void PID_Init(pid_type_def *pid, const float PID[3],  float max_out, float max_iout, uint8_t mode);
float PID_Calc(pid_type_def *pid, float ref, float set);
void PID_Clear(pid_type_def *pid);
void PID_Change(pid_type_def *pid, float Kp, float Ki, float Kd);

void LimitMax_Int16(int16_t input, int16_t max);
void LimitMaxfloat(float input, float max);
#endif
