#include "chassis.h"

#define MOTOR_CODE 1

Chassis_Module chassis;//���̽ṹ��
Motor_speed speed2[4] = {0, 0, 0, 0};
Motor_speed output[4] = {0, 0, 0, 0};//�������

void Chassis_Config(void)
{
	Motor_Module *M_temp;
	u8 i;
	
	for(i = 0; i<4; i++)
	{
		M_temp = &(chassis.Motor_Num[i]);
		
		Motor_Module_Init(M_temp, 10000);
		
		PID_Init(&(M_temp->speed_PID));
		PID_Init(&(M_temp->angle_PID));
		
		PID_Set(&(M_temp->speed_PID), 5.1f, 0.35f, 10, 6000.0f, 10, 10000);//3508�ٶȻ�PID, �����޷�һ��
		PID_Set(&(M_temp->angle_PID), 0.25f, 0, 0, 100.0f, 20, 6000);//3508λ�û�PID
	}
	
}


//����Ŀ��ǶȺ��� ��Ӧ�Ƕȵ����ֵ8191 ��Ӧ0x1fff;
void Chassis_set_angle(u8 motor_code, int angle)
{
	
	Motor_set_angle(&(chassis.Motor_Num[motor_code]), angle);

}

//����Ŀ���ٶȺ���
void Chassis_set_speed(Motor_speed s1, Motor_speed s2, Motor_speed s3, Motor_speed s4)
{
	
	Motor_set_speed(&(chassis.Motor_Num[0]), s1);
	Motor_set_speed(&(chassis.Motor_Num[1]), s2);
	Motor_set_speed(&(chassis.Motor_Num[2]), s3);
	Motor_set_speed(&(chassis.Motor_Num[3]), s4);

}

u8 start_pid = 0;//PID��ʼ��־λ
u8 start_pos_pid = 0;//λ�û���ʼ��־λ
short int speed_limit = 8000;//�����ٶ���ֵ
Motor_Module motor_temp1;//JSCOPE�鿴����ṹ�����ݵ�temp
Motor_Module motor_temp2;//JSCOPE�鿴����ṹ�����ݵ�temp
Motor_Module motor_temp3;//JSCOPE�鿴����ṹ�����ݵ�temp
Motor_Module motor_temp4;//JSCOPE�鿴����ṹ�����ݵ�temp
u32 sentcnt = 0;//���㷢�ʹ���
void Chassis_Speed_Update(void)
{
	Motor_Module *M_temp;
	Motor_speed speed_temp;
	short int output_temp;
	u8 i;
	float err;
	
	//�����ĸ������λ�û�
	
	for(i = 0; i<4; i++)	//�����ĸ�����ĸ���PID��
	{
		M_temp = &(chassis.Motor_Num[i]);
		
		if(M_temp->state != M_CONNECTING)//������������ʹ��̬���Ͳ����
		{
			output[i] = 0;
			continue;
		}
		
		if(start_pos_pid)
		{
			//λ�û�
			err = 8192.0f*((float)(M_temp->tg_angle_turns-M_temp->angle_turns))+((float)((M_temp->tg_angle) - (M_temp->angle)));
			Pos_PID_CAL_OUT(&(M_temp->angle_PID), err);
			speed_temp = (short int)(M_temp->angle_PID.output);
		
			speed_temp = MIN(M_temp->tg_speed, speed_limit);//�޷�
			speed_temp = MAX(M_temp->tg_speed, -speed_limit);
			
			Motor_set_speed(M_temp, speed_temp);
		}
		
		//�ٶȻ�
		err = (float)((M_temp->tg_speed) - (M_temp->speed_real));
		Pos_PID_CAL_OUT(&(M_temp->speed_PID), err);
		
		output_temp = (short int)(M_temp->speed_PID.output);
		
		if((M_temp->output)>M_temp->output_limit)
			M_temp->output = M_temp->output_limit;
		else if(M_temp->output < -(M_temp->output_limit))
			M_temp->output = -M_temp->output_limit;

		
		M_temp->output = output_temp;
			
		output[i] = M_temp->output;
		
	}

	motor_temp1 = chassis.Motor_Num[0];//��ȡ���ڵ��Եĵ�������ݣ�JSCOPE�����˽ṹ������ĵ���ֵ
	motor_temp2 = chassis.Motor_Num[1];//��ȡ���ڵ��Եĵ�������ݣ�JSCOPE�����˽ṹ������ĵ���ֵ
	motor_temp3 = chassis.Motor_Num[2];//��ȡ���ڵ��Եĵ�������ݣ�JSCOPE�����˽ṹ������ĵ���ֵ
	motor_temp4 = chassis.Motor_Num[3];//��ȡ���ڵ��Եĵ�������ݣ�JSCOPE�����˽ṹ������ĵ���ֵ

	Chassis_Set_Output(output[0], output[1], output[2], output[3]);
	
	//	if(start_pid)
//	{	
//		sentcnt++;
//		Chassis_Set_Output(output[0], output[1], output[2], output[3]);
//	}
//	else
//	{
//		Chassis_Set_Output(0, 0, 0, 0);
//	}
}





u32 send_fail = 0;
//ԭ���ķ��ʹ������⣬������ʽ��������ʧ
u8 tx_current_data[8] = {0};
u8 Chassis_Set_Output(Motor_speed s1, Motor_speed s2, Motor_speed s3, Motor_speed s4)
{
	CAN1_0X200_BUF[0] = ((s1&0xff00)>>8)&0xff;
	CAN1_0X200_BUF[1] = (s1)&0xff;
	CAN1_0X200_BUF[2] = ((s2&0xff00)>>8)&0xff;
	CAN1_0X200_BUF[3] = (s2)&0xff;
	CAN1_0X200_BUF[4] = ((s3&0xff00)>>8)&0xff;
	CAN1_0X200_BUF[5] = (s3)&0xff;
	CAN1_0X200_BUF[6] = ((s4&0xff00)>>8)&0xff;
	CAN1_0X200_BUF[7] = (s4)&0xff;
	
	return 0;
}


int temp = 0;
int angle_t = 0;
u8 speed_analyze(u16 id, u8 *buf)
{
	//Motor_speed text_speed[4];
	
	switch(id)		//IDɸѡ�����������Ҫ��id����������
	{
		case 0x201: 
		case 0x202: 
		case 0x203: 
		case 0x204:	
		case 0x205:
		case 0x206:
		case 0x207:
		case 0x208:
		case 0x209:
		case 0x20a:
		case 0x20b:
		case 0x20c:id = id&0xf; break;
		default: return 1;
	}

	if(id<=4)
	{
		Motor_analyze(&(chassis.Motor_Num[id-1]), buf);
	}
	else if(id == 5)
	{
		Motor_analyze(&(infantry_Gimbal.Yaw_Motor), buf);
	}
	else if(id == 6)
	{
		Motor_analyze(&(infantry_Gimbal.Pitch_Motor), buf);
	}
	else if(id == 9)
	{
		Motor_analyze(&(Gimbal.Yaw_Motor), buf);
		//Gimbal.Yaw_Motor.speed_real = -MPU6050_Gimbal.gyroz-32;
	}
	else if(id == 10)
	{
		Motor_analyze(&(Gimbal.Pitch_Motor), buf);
		//Gimbal.Pitch_Motor.speed_real = -MPU6050_Gimbal.gyrox-50;
	}
	if(id>4 && id<12)
	{
		Motor_analyze(&(UNKONW.ID5_F[id-5]), buf);
	}
	
	return 0;
}

u8 speed_analyze_Can2(u16 id, u8 *buf)
{
	
	switch(id)		//IDɸѡ�����������Ҫ��id����������
	{
		case 0x201: 
		case 0x202: 
		case 0x203: 
		case 0x204:	
		case 0x205:
		case 0x206:
		case 0x207:
		case 0x208:
		case 0x209:
		case 0x20a:
		case 0x20b:
		case 0x20c:id = id&0xf; break;
		default: return 1;
	}

	if(id<=4)
	{
		Motor_analyze(&(chassis.Motor_Num[id-1]), buf);
	}
	else if(id == 5)
	{
		Motor_analyze(&(infantry_Gimbal.Yaw_Motor), buf);
	}
	else if(id == 6)
	{
		Motor_analyze(&(infantry_Gimbal.Pitch_Motor), buf);
	}
	else if(id == 9)
	{
		Motor_analyze(&(Gimbal.Yaw_Motor), buf);
		//Gimbal.Yaw_Motor.speed_real = -MPU6050_Gimbal.gyroz-32;
	}
	else if(id == 10)
	{
		Motor_analyze(&(Gimbal.Pitch_Motor), buf);
		//Gimbal.Pitch_Motor.speed_real = -MPU6050_Gimbal.gyrox-50;
	}
	if(id>4 && id<12)
	{
		Motor_analyze(&(UNKONW.ID5_F[id-5]), buf);
	}
	
	return 0;
}


//���̵����Ӧ���º���
void Chassis_State_Update(void)
{
	u8 i = 0;
	
	for(i = 0; i<4; i++)
	{
		Motor_State_Update(&(chassis.Motor_Num[i]));
	}
	
	for(i = 0; i<16; i++)
	{
		Motor_State_Update(&(UNKONW.ID5_F[i]));
	}
	
	for(i = 0; i<16; i++)
	{
		Motor_State_Update(&(UNKONW2.ID5_F[i]));
	}
}






