#ifndef RM__MOTOR__H
#define RM__MOTOR__H

#include "sys.h"
#include "pid.h"

//LIMIT������ʹ��ʱע��������������Ҫ��ͬ
#define MIN(x, y) ((x)>(y)?(y):(x))
#define MAX(x, y) ((x)>(y)?(x):(y))

typedef short int Motor_speed;
typedef short int Motor_angle;
typedef short int Motor_current;

typedef enum MOTOR_STATE
{
	M_DISABLE = 0,
	M_ENABLE,
	M_DISCONNECT,
	M_CONNECTING
	
}MOTOR_STATE;


typedef struct Motor_Module
{
	Motor_speed tg_speed;
	Motor_speed speed_real;
	Motor_angle angle;
	Motor_angle tg_angle;
	Motor_current current;
	u8 temperature;
	int angle_turns;
	int tg_angle_turns;
	short int output;
	short int output_limit;
	u8 offline_cnt;
	MOTOR_STATE state;									//�����ǰ״̬������
	
	Pid_Module speed_PID;
	Pid_Module angle_PID;
}Motor_Module;

//�����ʼ��
void Motor_Module_Init(Motor_Module *motor, short int output_limit);


//�������״̬����
u8 Motor_analyze(Motor_Module *motor, u8 *buf);
void Motor_PID_Update(Motor_Module *Motor);

//////////////////////////////////////////////////////////////////////////////////
//����ӿ��ຯ��

//���Ŀ���ٶ����ú���
void Motor_set_speed(Motor_Module *motor, Motor_speed speed);

//���Ŀ��Ƕ����ú���
void Motor_set_angle_turns(Motor_Module *motor, int turn, short int angle);
void Motor_set_angle(Motor_Module *motor, int angle);
void Motor_angle_add(Motor_Module *motor, int angle);

//���״̬���º���
void Motor_State_Update(Motor_Module *M_temp);
void Motor_State_Frash(Motor_Module *M_temp);
void Motor_PID_Clear(Motor_Module *Motor);

#endif
