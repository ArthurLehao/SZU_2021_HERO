#include "motor.h"

//���ģ���ʼ������
void Motor_Module_Init(Motor_Module *motor, short int output_limit)
{
	motor->output_limit = output_limit;
	motor->output = 0;
	motor->tg_speed = 0;
	motor->speed_real =0;
	motor->angle_turns=0;
	motor->tg_angle = 0;
	motor->angle_turns = 0;
	motor->tg_angle_turns = 0;
	motor->offline_cnt = 0;
	
	motor->state = M_DISABLE;
}


//���������ķ���ֵ
u8 Motor_analyze(Motor_Module *motor, u8 *buf)
{
	Motor_angle angle_temp;
	Motor_speed speed_temp;
	short int current_temp;
	int temp;
	
	angle_temp = buf[0];//ȡ����λ
	speed_temp = buf[2];
	current_temp = buf[4];
	angle_temp = (angle_temp<<8) | (buf[1]&0x00ff);//��λ���ƻ��ϵ�λ
	speed_temp = (speed_temp<<8) | (buf[3]&0x00ff);
	current_temp = (current_temp<<8) | (buf[5]&0x00ff);

	if(angle_temp >= motor->angle)
	{
		temp = (int)(8191.0f-angle_temp + motor->angle);
		if(-4000<temp && temp<4000)//angle�仯����ֵС��4000
		{
			(motor->angle_turns)--;
		}
	}
	else if(angle_temp < motor->angle)
	{
		temp = (int)(8191.0f+angle_temp - motor->angle);
		if(-4000<temp && temp<4000)//angle�仯����ֵС��4000
		{
			motor->angle_turns++;
		}
	}
	
	if(motor->state == M_DISCONNECT)
	{
		motor->tg_angle = angle_temp;
		motor->tg_angle_turns = 0;
	}
	
	Motor_State_Frash(motor);	//ÿ��һ���յ����ݾͽ������״̬����Ϊ����
	motor->angle = angle_temp;//����ID������Ӧ�Ľṹ��
	motor->speed_real = speed_temp;
	motor->current = current_temp;
	motor->temperature = buf[6];
	
	return 0;
}


//���Ŀ���ٶ����ú���
void Motor_set_speed(Motor_Module *motor, Motor_speed speed)
{
	motor->tg_speed = speed;
}

//���Ŀ��Ƕ����ú���
void Motor_set_angle_turns(Motor_Module *motor, int turn, short int angle)
{
	Motor_angle temp;
	
	temp = motor->angle;
	temp += angle;
	
	if(angle>=8192)
	{
		angle -= 8192;
		turn++;
	}
	else if(angle < 0)
	{
		angle += 8192;
		turn--;
	}
	
	motor->tg_angle_turns = turn;
	motor->tg_angle = angle;	
}
float a_temp;
void Motor_angle_add(Motor_Module *motor, int angle)
{
	float angle_temp;
	
	angle_temp = 8192.0f*((float)(motor->tg_angle_turns))+((float)((motor->tg_angle)));
	a_temp = angle_temp;
	angle_temp += angle;
	
	Motor_set_angle(motor, (int)angle_temp);
}

void Motor_set_angle(Motor_Module *motor, int angle)
{
	Motor_set_angle_turns(motor, angle/8192, angle%8192);
}

//���������������
void Motor_State_Update(Motor_Module *motor)
{
	motor->offline_cnt++;
	
	if(motor->offline_cnt>40)
	{
		motor->offline_cnt = 41;
		motor->state = M_DISCONNECT;
		PID_Clear(&(motor->speed_PID));
		PID_Clear(&(motor->angle_PID));
	}
}

//�����������ˢ�º���
void Motor_State_Frash(Motor_Module *motor)
{
	motor->offline_cnt = 0;
	motor->state = M_CONNECTING;
}


//���õ���������ĵط���Ҫ����PID�ṹ������startflag
void Motor_PID_Update(Motor_Module *Motor)
{
	Motor_Module *M_temp;
	Motor_speed speed_temp;
	short int output_temp;
	float err;

	M_temp = Motor;
	//�����ĸ������λ�û�

	
	if(M_temp->state != M_CONNECTING)//������������ʹ��̬���Ͳ����
	{
		M_temp->output = 0;
		return;
	}
	
	if(M_temp->angle_PID.startfalg)
	{
		//λ�û�
		err = 8192.0f*((float)(M_temp->tg_angle_turns-M_temp->angle_turns))+((float)((M_temp->tg_angle) - (M_temp->angle)));
		Pos_PID_CAL_OUT(&(M_temp->angle_PID), err);
		speed_temp = (short int)(M_temp->angle_PID.output);
		
		
		if((speed_temp)>M_temp->angle_PID.output_limit)
			speed_temp = M_temp->angle_PID.output_limit;
		else if((speed_temp)<-(M_temp->angle_PID.output_limit))
			speed_temp = -M_temp->angle_PID.output_limit;
		
		Motor_set_speed(M_temp, speed_temp);
	}
	

	//�ٶȻ�
	err = (float)((M_temp->tg_speed) - (M_temp->speed_real));
	Pos_PID_CAL_OUT(&(M_temp->speed_PID), err);
	output_temp = (short int)(M_temp->speed_PID.output);

	if((output_temp)>M_temp->output_limit)
			output_temp = M_temp->output_limit;
	else if(output_temp < -(M_temp->output_limit))
			output_temp = -M_temp->output_limit;

	M_temp->output = output_temp;

}


void Motor_PID_Clear(Motor_Module *Motor)
{
	PID_Clear(&(Motor->angle_PID));
	PID_Clear(&(Motor->speed_PID));
}

