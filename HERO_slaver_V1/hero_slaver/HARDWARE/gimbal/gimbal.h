#ifndef RP__GIMBAL__H
#define RP__GIMBAL__H

#include "link.h"


typedef struct Gimbal_Module
{
	Motor_Module Yaw_Motor;
	Motor_Module Pitch_Motor;
	short gyrox;
  short	gyroy;
	short	gyroz;//���ٶ�ֵ x�Ǻ�����ٶȣ�y��pitch���ٶȣ�z��ƫ�����ٶ�
	float pitch;
	float roll;
	float yaw;		//�Ƕ�ֵ   pitch �Ǹ����Ƕ� ǰ��Ϊ���� roll�Ǻ���Ƕ� �ҵ�Ϊ����yaw��ƫ���Ƕ� ˳ʱ��Ϊ��
	
	int YTurns;
	u16 YAngle;
	u16 tg_YAngle;//Ŀ��Ƕ�
	int tg_YTurns;
	
	float tg_PAngle;
	
	
	Pid_Module Mpu_Yaw_SPID;
	Pid_Module Mpu_Yaw_APID;
	
	Pid_Module Mpu_Pitch_SPID;
	Pid_Module Mpu_Pitch_APID;
	
}Gimbal_Module;

extern Gimbal_Module Gimbal;
extern Gimbal_Module infantry_Gimbal;

//��̨������º���
void Gimbal_State_Update(void);
void Gimbal_Config(void);
void Gimbal_PID_Cal(u8 code);
void Gimbal_Set_Output(short int out_1, short int out_2);

void Gimbal_MpuPID_Cal(u8 Motor_code);
void Gimbal_Mpu_Analyze(void);//MPU6050����������
void Mpu_Yaw_tgAngle_add(int angle);
#endif

