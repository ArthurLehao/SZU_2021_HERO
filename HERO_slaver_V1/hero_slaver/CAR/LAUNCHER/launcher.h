#ifndef RM__LAUNCHER__H
#define RM__LAUNCHER__H


#include "link.h"

typedef enum 
{
	OFF,
	ON,
	LOCK
}Binary_State;

typedef struct Friction_Module
{
	u32 PWM_Speed;
	u32 PWM_Max;
	u32 Target_Speed;
	Binary_State state;
}Friction_Module;

typedef struct Launcher_Module
{
	Motor_Module Magazine;	//���ֵ��
	Motor_Module Dial;			//���̵��
	Motor_Module Safety;		//��λ/����
	Friction_Module Friction;
	
}Launcher_Module;

extern Launcher_Module Hero_Launcher;


//���������ʼ��
void Launcher_Init(void);

//�������������
void Launcher_Task(Launcher_Module *Launcher, u8 mode_flag);

//�ܵ��ʧ���ж�
void Launcher_state_update(void);

//���������䷢������
void Launcher_set_output(u16 Magazine, u16 Safety, u16 Dial);


//����������չ�������
void Launcher_Magazine(void);

//Ħ���ֹ�������
void Launcher_Friction(void);

//Ħ����б�º���
void Friction_output(void);

//����һ�ŵ���
void Launcher_Dial(u8 flag);
	
#endif
