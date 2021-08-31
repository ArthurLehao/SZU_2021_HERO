#ifndef RM__FILTER__H
#define RM__FILTER__H

#include "rp_config.h"
#include "rp_math.h"


#define MAF_MaxSize 100
#define Median_F_MaxSize 100
#define MAF_anti_top_MaxSize 1000


typedef struct moving_Average_Filter
{
	float num[MAF_MaxSize];
	uint8_t lenth;
	uint8_t pot;//��ǰλ��
	float total;
	float aver_num;
	
	
}moving_Average_Filter;	//�������MAF_MaxSize��


typedef struct MAF_Anti_top
{
	float num[MAF_anti_top_MaxSize];
	uint16_t lenth;
	uint16_t pot;//��ǰλ��
	float total;
	float aver_num;
	float max;
	float min;
}MAF_Anti_top;	//�������MAF_MaxSize��


//�������鸺���¼���ݽ������ݴ���ʱpot�ǵڼ���
//�ṩɾ�����ݵ�����
typedef struct Median_Filter
{
	float data_num[Median_F_MaxSize];//��ֵ����
	int data_index_num[Median_F_MaxSize];
	uint8_t lenth;

	uint8_t index_pot;//ʼ��ָ����һ��Ҫɾ����pot
	float median_data;
	
}Median_Filter;	//�������MAF_MaxSize��


typedef struct LOW_Pass_Filter
{
	float last;
	float now;
	float threshold;
	float output;
	uint8_t High_flag;//����һ������ʱָʾ��flag
}LOW_Pass_Filter;


//�����˲�����Ӧ�Ĳ�������
void average_add(moving_Average_Filter *Aver, float add_data);
float average_get(moving_Average_Filter *Aver, uint16_t pre);//��ȡǰn�ε�����
void average_init(moving_Average_Filter *Aver, uint8_t lenth);
void average_clear(moving_Average_Filter *Aver);
void average_fill(moving_Average_Filter *Aver, float temp);//�������˲����ĳ��ֵ

void MAF_ANTI_TOP_add(MAF_Anti_top *Aver, float add_data);
float MAF_ANTI_TOP_get(MAF_Anti_top *Aver, uint16_t pre);//��ȡǰn�ε�����
void MAF_ANTI_TOP_init(MAF_Anti_top *Aver, uint16_t lenth);
void MAF_ANTI_TOP_clear(MAF_Anti_top *Aver);


//��ֵ�˲�����Ӧ�Ĳ�������
void median_add(Median_Filter *Median, float add_data);
float median_get(Median_Filter *Median, uint16_t pre);//��ȡǰn�ε�����
void median_init(Median_Filter *Median, uint8_t lenth);
void median_clear(Median_Filter *Median);

//��ͨ�˲�����Ӧ�Ĳ�������
void LPF_Init(LOW_Pass_Filter *LPF, float threshold);
float LPF_add(LOW_Pass_Filter *LPF, float input_data);
void LPF_Clear(LOW_Pass_Filter *LPF);


////////////////////////////////�ⲿ�����˲���/////////////////

/////////////������ֵ�˲���
extern moving_Average_Filter KEY_W, KEY_A, KEY_S, KEY_D;
extern moving_Average_Filter MOUSE_X, MOUSE_Y;

extern MAF_Anti_top Absolute_yaw_angle_raw, Absolute_pitch_angle_raw, Absolute_distance_raw;


#endif

