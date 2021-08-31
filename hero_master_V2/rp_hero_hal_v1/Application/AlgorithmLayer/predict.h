#ifndef __PREDICT__H
#define __PREDICT__H

#include "vision_sensor.h"
#include "arm_math.h"
#include "cmsis_os.h"
#include "rp_math.h"
#include "filter.h"
#include "kalman.h"

#ifndef PI 
	#define PI 3.14159265358979f
#endif

#ifndef Gravity_g
	#define Gravity_g 9.788f
#endif

#define SHOOT_SPEED 15.0f 

#define ARMER_SWITCH_THRESHOLD 50.0f

//����������ʱ��
#define AUTO_ANTI_TOP_TICK 1500

//������װ�װ����ʱ��������ֵ���Ӿ�����������Խǿ����ֵ����ԽС
//��ʱ���Ϸ�ֹû�����ݵ�ʱ���Ի����л�װ�װ�������±߽粻�ϸ��£�Խ��ԽС
//��λ����
#define ANTI_TOP_TIME_THRESHOLD 30

//������װ�װ����ʱ�Ƕȼ������ֵ���Ӿ�����������Խǿ����ֵ����ԽС
//�ӿռ��Ϸ�ֹû�����ݵ�ʱ���Ի����л�װ�װ�������±߽粻�ϸ��£�Խ��ԽС
//��λ������Ƕ�
#define ANTI_TOP_SPACE_THRESHOLD 80

typedef enum
{
	NONE,
	STANDING_BY,
	READY
}Anti_top_State;

typedef enum
{
	AT_DISABLE,
	AT_ENABLE
}Anti_top_Switch;

typedef struct Anti_top_Data
{
	float binary_low;
	float binary_high;
	float top_speed;
	float top_mid;
	float top_circle;
	uint32_t cnt;
	uint32_t cnt_max;
	Anti_top_State state;
	Anti_top_Switch AT_witch;
}Anti_top_Data;

//��ʼ��
void PREDICT_Init(void);

//��ʽ����ⷢ���
float Fire_angle_cal(float distance, float angle);

//���鷨��ⷢ���
float predict_cal_shoot_angle(float distance, float angle, float Friction_speed);

uint32_t Vision_get_interval(uint8_t lost_flag);

//�ж�������
uint8_t Predict_Judge_JumpEdge(float yaw_angle);

//����������Խ���
void PREDICT_Cal_Base(uint8_t lost_flag, float Tg_y, float Tg_p, float Distance);
void PREDICT_Base_Reset(float Tg_y, float Tg_p, float Distance);

//���鷴�������ݼ���
void Predict_Anti_Top_Cal_all(float binary_l, float binary_h);
void Predict_Anti_Top_Data_Heart_beat(void);
void Predict_Anti_Top_Data_Clear(void);

//���ݱ߽��Ŀ��yaw����ѭ������
float Predict_Anti_Top_Judge_Yaw(float auto_yaw);

//�ж�����Ƕ��Ƿ�������������
int8_t Predict_Anti_Top_Binary_judge(float yaw_angle);

//���������ƶ��߽�
void Predict_Anti_Top_binary_update(float now_yaw);

void vision_anti_top(void);

extern moving_Average_Filter frams_MAF;

#endif
