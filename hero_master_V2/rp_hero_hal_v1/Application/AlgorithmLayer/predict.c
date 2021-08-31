#include "predict.h"

Anti_top_Data TOP_Data;


moving_Average_Filter frams_MAF;



float absolute_yaw_kp = 34.0f, absolute_pitch_kp = 22.5f;
void PREDICT_Init()
{
	average_init(&frams_MAF, 2);
	
}


//��ʽ�����̧ͷ��

//���뵥λmm���Ƕȵ�λ��
//�ص�����λ�ȡ�Ƕȣ����뽻���ǻ�ȥ��
//������Ӧ���������ǽǶ�Ϊ��׼�����Ե���Ƕ�Ϊ��׼
//�����ǽǶȴ������⣬����Ӳ
//����Ƕȴ������⣬����ʵ������ĽǶȣ�����Ӧ�ǶȲ�׼ȷ
//
//��֪�Ӿ�����һ�������ǹ���->ת����Ŀ��Ե���Ƕ�->����Ƕ�-��ֵ��ɻ�е�ĸ���->/22.75�����Ƕ������������
//->����Ƕȵ�ƫ����->�ӵ���׼�Ƕ���

//����ͳһ����pitch��
float Fire_angle_cal(float distance, float angle)
{
	float tan1, tan2;
	float tempv = 0, temp_sqrt;
	float target_x, target_y;
	float theta_pitch = 0;//�������
	float delta_mach_pitch;
	
	if(distance<50 || angle >70 || angle<-70)
	{
		return 0;
	}
	
	target_x = distance*cos(angle/180.0f*PI);
	target_y = distance*sin(angle/180.0f*PI);
	tempv = Gravity_g*target_x/(2*SHOOT_SPEED*SHOOT_SPEED);
	
	if(target_x == 0 || tempv == 0)
	{
		return 0;
	}
	
	temp_sqrt = sqrt(1-4*tempv*(tempv+target_y/target_x));
	tan1 = (1+temp_sqrt)/2/tempv;
  tan2 = (1-temp_sqrt)/2/tempv;//һ�����ȡ��ֵ��С��tan2��tan1�Ǹߵ���
	
	theta_pitch = atan(tan2);//���㷢��ʱpitchӦ�ñ��ֵĽǶ�
	
	delta_mach_pitch = (theta_pitch-angle)*22.7555555f;
	
	return delta_mach_pitch;
}

float pre_dis_to_pitch_offset;
float theta_machanic = 0;
//PITCH��������̧ͷ��Ҫ�Ӹ���
float K_theta_machanic = 156455.6775f;
float temp_dis_kp = 0.15f;
float temp_dis_kb = 5.0f;
float predict_cal_shoot_angle(float Target_distance, float Target_angle, float Friction_speed)
{
	
	//��һ������е ���-ǹ�ܱ궨
	if(Target_distance>300)
	{
		theta_machanic = 156455.6775f/Target_distance;
	}
	else 
	{
		theta_machanic = 0;
	}
	//��������ĺ�ǹ�����ĵó���̧ͷ��
	
	//�ڶ�����ͬ�����߱궨
	
	//��������ͬ�߱����궨
	//̧ͷ��
	//��������ѡ��ͬ�ľ��븩������������
	if(Friction_speed <4000)
	{
		if(Target_distance<1100)
		{
			pre_dis_to_pitch_offset = 0.087f*Target_distance-135.16;
		}
		else
		{
			pre_dis_to_pitch_offset = 0.00001*Target_distance*Target_distance + 0.0155*Target_distance - 49.68;
		}
		
	}
	else if(Friction_speed > 4000)
	{
		if(Target_distance<600)
		{
			pre_dis_to_pitch_offset = -60.0f;
		}
		else if(Target_distance<880)
		{
			pre_dis_to_pitch_offset = 0.2142854f*(Target_distance-600)-60.0f;
		}
		else if(Target_distance<1380)
		{
			pre_dis_to_pitch_offset = 0.08f*(Target_distance-880);
		}
		else if(Target_distance<2000)
		{
			pre_dis_to_pitch_offset = 0.032258065f*(Target_distance-2000)+40;
		}
		else if(Target_distance<2740)
		{
			pre_dis_to_pitch_offset = 60;
		}
		else if(Target_distance<3430)
		{
			pre_dis_to_pitch_offset = -0.014492754f*(Target_distance-2740)+60;
		}
		else if(Target_distance<4247)
		{
			pre_dis_to_pitch_offset = -0.012239902f*(Target_distance-3430)+50;
		}
		else if(Target_distance<5036)
		{
			pre_dis_to_pitch_offset = -0.019011407*(Target_distance-4247)+40;
		}
		else if(Target_distance<5911)
		{
			pre_dis_to_pitch_offset = -0.017142857f*(Target_distance-5036)+25;
		}
		else if(Target_distance<7000)
		{
			pre_dis_to_pitch_offset = -0.013774105*(Target_distance-5911)+10;

		}
		else if(Target_distance<7934)
		{
			if(Vision_attack_Color == VISION_RED_ATTACK)
			{
				pre_dis_to_pitch_offset = -0.037473233f*(Target_distance-7000)-5;
			}
			else 
			{
				//pre_dis_to_pitch_offset = -0.02141f*(Target_distance-7000)-5;
				pre_dis_to_pitch_offset = -0.037473233f*(Target_distance-7000)-5;
			}
		}
		else
		{
			if(Vision_attack_Color == VISION_RED_ATTACK)
			{
				pre_dis_to_pitch_offset = -40;
			}
			else 
			{
				pre_dis_to_pitch_offset = -40;
			}
			
		}
	}
	
	
	
	return pre_dis_to_pitch_offset;
}

float calibrate_Data[60] = {0};
float calibrate_Distance[30] = {0};
float calibrate_Angle[30] = {0};
float calibrate_Kp[30] = {0};
//�б���̧ͷ��
void List_shoot_angle_init()
{
	uint8_t i;
	float temp;
	
	for( i = 0; i<30; i++)
	{
		calibrate_Distance[i] = calibrate_Data[i*2];
		calibrate_Angle[i] = calibrate_Data[i*2+1];
	}
	
	for(i=0; i<(30-1); i++)//kp = ��angle/��distance
	{
		if(calibrate_Distance[i+1] == 0 || calibrate_Distance[i] == 0)//�㵽��β�������
		{
			break;
		}
		calibrate_Kp[i] = calibrate_Angle[i+1]-calibrate_Angle[i];
		
		temp = calibrate_Distance[i+1]-calibrate_Distance[i];
		if(temp<=0)
		{
			calibrate_Kp[i] = 0;
		}
		else
		{
			calibrate_Kp[i] = calibrate_Kp[i]/temp;
			calibrate_Kp[i+1] = calibrate_Kp[i]/temp;
		}
	}
	
}

float List_shoot_angle_Cal(float distance)
{
	uint8_t i = 0, base_i;
	float Base_Distance = 0;
	float predict_angle = 0;
	
	for(i = 0; i<30; i++)
	{
		if(calibrate_Distance[i]>distance)
		{
			break;
		}
		Base_Distance = calibrate_Distance[i];
		base_i = i;
	}
	
	predict_angle = calibrate_Kp[base_i]*(distance-Base_Distance);
	predict_angle += Base_Distance;
	
}


//��ȡ����������֡��ʱ��
uint32_t now_ms, last_ms;
uint32_t Vision_get_interval(uint8_t lost_flag)
{
	uint32_t time_ms;
	now_ms =	osKernelSysTick();//��ȡ����һ֡�ļ��ʱ��
	if(lost_flag >= 1)
	{
		last_ms = now_ms;
		time_ms = 7;
	}
	else
	{
		time_ms = now_ms-last_ms;
		last_ms = now_ms;
	}
	
	return time_ms;
}



//װ���л�����
uint32_t armer_switch_times = 0;
uint8_t armer_switch_flag = 0;
uint32_t armer_switch_tick = 0;
int armer_switch_circle = 0;

//���2s�ڷ���װ�װ��л�С��һ�����������˳�����ģʽ
uint32_t switch_min_ticks = AUTO_ANTI_TOP_TICK;
uint32_t min_switch_times = 3;

uint8_t anti_top_flag;//�����ݱ�־λ
void vision_anti_top(void)
{
	static uint32_t anti_top_tick;
	
	//ÿ2s����һ�������ڵĻװ�װ��л�����������л�����С������Ϊû�н�������
	if(anti_top_tick<osKernelSysTick())
	{
		if(armer_switch_times<min_switch_times)
		{
			//anti_top_flag = 0;
		}
		else 
		{
			//anti_top_flag = 1;
		}
		anti_top_tick = osKernelSysTick()+switch_min_ticks;
		armer_switch_times = 0;
	}
	
}




float base_yaw_speed, base_yaw_accel;
float base_pitch_speed, base_pitch_accel;
float base_dis_speed, base_dis_accel;
//����Ԥ����������ٶȣ����ٶ�
void PREDICT_Cal_Base(uint8_t lost_flag, float Tg_y, float Tg_p, float Distance)
{
	static float L_Tg_y, L_Tg_p, L_Dis;
	float dY, dP, dD;
	float dSY, dSP, dSD;
	float dAY, dAP, dAD;
	
	if(lost_flag)//���
	{
		base_yaw_speed = 0;
		base_yaw_accel = 0;
		base_pitch_speed = 0;
		base_pitch_accel = 0;
		base_dis_speed = 0;
		base_dis_accel = 0;
		
		L_Tg_y = Tg_y;
		L_Tg_p = Tg_p;
		L_Dis = Distance;
		return;
	}
	
	//΢��
	dY = Tg_y-L_Tg_y;
	dP = Tg_p-L_Tg_p;
	dD = Distance-L_Dis;
	
	//�˲�
	
	
	//��΢��
	
	
	//���˲�
	
	
	//����ֵ
	base_yaw_speed = 0;
	base_yaw_accel = 0;
	base_pitch_speed = 0;
	base_pitch_accel = 0;
	base_dis_speed = 0;
	base_dis_accel = 0;
	
	L_Tg_y = Tg_y;
	L_Tg_p = Tg_p;
	L_Dis = Distance;
}

void PREDICT_Base_Reset(float Tg_y, float Tg_p, float Distance)
{
	uint8_t i;
	


}




/*Ŀǰֻ�ܱ�֤�ٶ������ʱ���ܽ����������,���ҵ�һ�ν����ʱ���������*/
void Predict_anti_top_get_circle(void)
{
	static uint32_t Anti_top_tick;;
	uint32_t tick_ms, temp;

	temp = osKernelSysTick();
	//TOP_Data.top_circle = 1500;
	
	if(TOP_Data.state == READY)
	{
		tick_ms = (float)((int)(temp - Anti_top_tick));
		
		if(tick_ms<ANTI_TOP_TIME_THRESHOLD)//ms��С���ݴ��忴�������ڣ�ȥ��
		{
			return;
		}
		
		TOP_Data.top_circle = tick_ms;
	}
	
	
	Anti_top_tick = temp;
}

void Predict_anti_top_get_binary(float binary_l, float binary_h)
{
	TOP_Data.binary_low = binary_l;
	TOP_Data.binary_high = binary_h;
	TOP_Data.top_mid = (binary_l+binary_h)/2;
}

void Predict_anti_top_get_speed(void)
{
	TOP_Data.top_speed = (TOP_Data.binary_low+TOP_Data.binary_high)/TOP_Data.top_circle;//�������ٶȼ���
}

void Predict_Anti_Top_binary_update(float now_yaw)
{
	int8_t flag;
	float binary_err;
	
	flag = Predict_Anti_Top_Binary_judge(now_yaw);
	if(flag == 1)
	{
		binary_err = now_yaw-TOP_Data.binary_high;
		TOP_Data.binary_high =  TOP_Data.binary_high+binary_err;
		TOP_Data.binary_low = TOP_Data.binary_low+binary_err;
		TOP_Data.top_mid = (TOP_Data.binary_high+TOP_Data.binary_low)/2;
	}
	else if(flag == -1)
	{
		binary_err = now_yaw-TOP_Data.binary_low;
		TOP_Data.binary_high =  TOP_Data.binary_high+binary_err;
		TOP_Data.binary_low = TOP_Data.binary_low+binary_err;
		TOP_Data.top_mid = (TOP_Data.binary_high+TOP_Data.binary_low)/2;
	}
}

void Predict_Anti_Top_Cal_all(float binary_first, float binary_second)
{
	float binary_l, binary_h;
	TOP_Data.cnt_max = 1500;
	
	if(abs(binary_first-binary_second)<ANTI_TOP_SPACE_THRESHOLD)//����߽����̫���������𵴴���
	{
		return;
	}
	
	//��ȡ��������
	Predict_anti_top_get_circle();
	
	if(TOP_Data.state == NONE)
	{
		TOP_Data.state = STANDING_BY;
	}
	else if(TOP_Data.state == STANDING_BY)
	{
		TOP_Data.state = READY;
	}
	else if(TOP_Data.state == READY)
	{
		//TOP_Data.state = STANDING_BY;
//		if(TOP_Data.top_circle>1000 && TOP_Data.AT_witch == AT_ENABLE)
//		{
//			TOP_Data.AT_witch = AT_DISABLE;
//		}
//		else if(TOP_Data.top_circle<650 && TOP_Data.AT_witch == AT_DISABLE)
//		{
//			TOP_Data.AT_witch = AT_ENABLE;
//		}
		TOP_Data.AT_witch = AT_ENABLE;
	}
	
	binary_h = RP_MAX(binary_first, binary_second);
	binary_l = RP_MIN(binary_first, binary_second);
	

	Predict_anti_top_get_binary(binary_l, binary_h);
	Predict_anti_top_get_speed();
	
	TOP_Data.cnt = 0;
}

int8_t Predict_Anti_Top_Binary_judge(float yaw_angle)
{
	if(yaw_angle>TOP_Data.binary_high)
	{
		return 1;
	}
	else if(yaw_angle>=TOP_Data.binary_low && yaw_angle<=TOP_Data.binary_high)
	{
		return 0;
	}
	else if(yaw_angle<TOP_Data.binary_low)
	{
		return -1;
	}
	else 
	{
		return 0;
	}
	
	return 0;
}

float Predict_Anti_Top_Judge_Yaw(float auto_yaw)
{
	float final_yaw;
	
	if(TOP_Data.state == NONE || TOP_Data.AT_witch == AT_DISABLE)
	{
		return auto_yaw;
	}
	
	if(auto_yaw>TOP_Data.binary_high)
	{
		final_yaw = auto_yaw-TOP_Data.binary_high+TOP_Data.binary_low;
	}
	else if(auto_yaw>=TOP_Data.binary_low && auto_yaw<=TOP_Data.binary_high)
	{
		final_yaw = auto_yaw;
	}
	else if(auto_yaw<TOP_Data.binary_low)
	{
		final_yaw = auto_yaw - TOP_Data.binary_low + TOP_Data.binary_high;
	}
	else 
	{
		final_yaw = auto_yaw;
	}
	
	//final_yaw = TOP_Data.top_mid;
	
	return final_yaw;
}

void Predict_Anti_Top_Data_Clear(void)
{
	TOP_Data.state = NONE;
	TOP_Data.binary_high = 0;
	TOP_Data.binary_low = 0;
	TOP_Data.top_circle = 1500;
	TOP_Data.top_speed = 0;
	TOP_Data.top_mid = 0;
	TOP_Data.AT_witch = AT_DISABLE;
}

void Predict_Anti_Top_Data_Heart_beat(void)
{
	TOP_Data.cnt++;
	if(TOP_Data.cnt>TOP_Data.cnt_max)
	{
		TOP_Data.state = NONE;
		Predict_Anti_Top_Data_Clear();
	}
	
}



