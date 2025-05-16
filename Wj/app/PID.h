#ifndef __PID_H
#define __PID_H

/*		 PID ģ ʽ ö ��	 */

enum PID_MODE
{
	PID_POSITION = 0,
	PID_DELTA	 = 1
};

/*       �� �� �� �� ��      */
typedef struct 
{
	//PIDģʽ
	uint8_t mode;

	//PID ������
	float Kp;
	float Ki;
	float Kd;

	float max_out;  //������
	float max_iout; //���������

	float set;
	float fdb;

	float out;
	float Pout;
	float Iout;
	float Dout;
	float Dbuf[3];  //΢���� 0���� 1��һ�� 2���ϴ�
	float error[3]; //����� 0���� 1��һ�� 2���ϴ�

} pid_type_def;


/*          �� �� ��         */



/*          ��������         */

void PID_Init(pid_type_def *pid, const float PID[3],  float max_out, float max_iout, uint8_t mode);
float PID_Calc(pid_type_def *pid, float ref, float set);
void PID_Clear(pid_type_def *pid);
void PID_Change(pid_type_def *pid, float Kp, float Ki, float Kd);

void LimitMax_Int16(int16_t input, int16_t max);
void LimitMaxfloat(float input, float max);
#endif
