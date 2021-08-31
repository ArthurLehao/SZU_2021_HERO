#ifndef RP__PID__H
#define RP__PID__H

#include "sys.h"

typedef enum
{
	PID_DISABLE,
	PID_ENABLE
}PID_STATE;

typedef struct Pid_Module
{
	float err;	//errֵ��ؼ���ֵ
	float err_dz;
	float err_limit;
	
	float d_err;
	float i_err;
	float i_err_limit;
	
	float kp;	//PID����
	float ki;
	float kd;

	float output;				//������
	float output_limit;//����޷�
	float p_output;
	float i_output;
	float d_output;
	
	PID_STATE startfalg;
	
}Pid_Module;

//���������
//PID ģ��ṹ��
//err ����PID�����еõ���errֵ
float Pos_PID_CAL_OUT(Pid_Module *PID, float err);

//PID�Ĳ���
void PID_Set(Pid_Module *PID, float kp, float ki, float kd, float i_err_limit, float err_dz, float output_limit);

//PID��ʼ��
void PID_Init(Pid_Module *PID);

//PID�������
void PID_Clear(Pid_Module *PID);

#endif

