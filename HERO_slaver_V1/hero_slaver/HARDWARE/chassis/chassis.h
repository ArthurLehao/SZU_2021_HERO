#ifndef RP__CLASSIS__H
#define RP__CLASSIS__H

#include "link.h"
#include "motor.h"



typedef struct Chassis_Module
{
	Motor_Module Motor_Num[4];
	
}Chassis_Module;


typedef struct unknow_Module
{
	Motor_Module ID5_F[16];
	
}unknow_Module;


extern u8 start_pid;
extern u8 start_pos_pid;
extern Chassis_Module chassis;//���̽ṹ��
extern Motor_speed speed2[4];//���̵���ٶȻ�����
extern unknow_Module UNKONW;
extern unknow_Module UNKONW2;

//��CAN�����ĸ����ӵ��ٶ�
u8 Chassis_Set_Output(Motor_speed s1, Motor_speed s2, Motor_speed s3, Motor_speed s4);

//��CAN�����ж�ȡ�ĸ����ӵ�״̬��Ϣ
u8 speed_analyze(u16 id, u8 *buf);
u8 Motor_analyze2(u16 id, u8 *buf);


//���µ��̵����
void Chassis_Speed_Update(void);
//���õ���Ŀ���ٶ�
void Chassis_set_speed(Motor_speed s1, Motor_speed s2, Motor_speed s3, Motor_speed s4);
//���õ���Ŀ��Ƕ�
void Chassis_set_angle(u8 motor_code, int angle);
//���̵�����ó�ʼ��
void Chassis_Config(void);
//���̵��״̬���º���
void Chassis_State_Update(void);//1ms����








#endif
