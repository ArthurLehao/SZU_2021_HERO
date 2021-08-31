#ifndef __TASK_H
#define __TASK_H

#include "link.h"
#include "supercap.h"

#define STACK_SIZE 9

//����ģ����LOOP������Ϊ��С��λ��������������չ
//��������ģ���������LOOP�Ķ�ʱʱ�䳤�̽�����Ӧ�ı�������

typedef enum Task_Status //����ṹ��ö���б�
{
	SUSPENDED,		//����״̬
	READY,				//׼��״̬
	RUNNING,			//����̬
	BLOCKED				//����̬�������������Ҫ�ⲿ�����²�ʹ��
}Task_Status;

typedef struct TASK_Module{
	Task_Status flag;			//�����־λ
	void (*function)(void);	//����ִ�к���
	u32 cycle;						//�������ڣ���С���ڵ�λΪ100us
	u32 cnt;							//��������������������ٸ���С��λ
}TASK_Module;

typedef struct TASK_Stack
{
	TASK_Module Stack[STACK_SIZE+1];
	u8 stack_pot;
	u8 stack_base;
}TASK_Stack;


//TASK_Init(void)
//ִ�������ܳ�ʼ��
void TASK_Init(void);

//xms ������
static void Task_1MS(void);
static void Task_2MS(void);
static void Task_10MS(void);
static void Task_20MS(void);

//Creat_Task
//�����µ�����
//����1�ɹ�������0��ʾ��ջ����������ʧ��
u8 Creat_Task(TASK_Stack *stack, u32 cycle, void (*function)(void));


//�����ʼ������
//��������������Լ���ر�־λ������
void Task_Module_init(TASK_Module *pTask, u32 cycle);

//�����ջ��ʼ������
void TASK_Stack_Init(TASK_Stack *stack);

void easy_task(void);//�ֲڵ�������
void Task_roop(void);//����ѭ������

void Task_disapart(void);//����ַ�����Ҫ����������
void Task_start(void);
void Task_stop(void);

#endif
