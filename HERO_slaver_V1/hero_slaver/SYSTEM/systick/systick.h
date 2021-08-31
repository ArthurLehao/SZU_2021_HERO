#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "sys.h"
#include "core_cm4.h"


typedef struct timestamp
{
	u16 day;
	u16 hour;
	u16 min;
	u16 sec;
	u16 msec;
	u32 msec_cnt;
	u16 usec;
}timestamp;

extern timestamp SysTimeStamp;

//�δ�ʱ����ʼ���������δ�ʱ��������
void systick_init(u32 nus);
//�δ�ʱ�������жϺ���
void systick_interrupt(void);

//systick ר����ʱ����
void sys_delay_us(u32 nus);
void sys_delay_ms(u32 nms);
u32 timestamp_getms(void);

//sys ʱ�����������
//����һ��ϵͳ�ò���̫��ʱ��������Բ��Խṹ��ָ����Ϊ������
void timestamp_step_100us(void);
void timestamp_step_ms(void);
void timestamp_init(void);

#endif
