#ifndef RP__OFFLINE__JUDGE
#define RP__OFFLINE__JUDGE

#include "sys.h"

typedef enum
{
	WORKING,								//WORKING    		��Ӧ���ǹ���̬����������¿���ֱ�Ӳ���
	REFRESHING,							//REFRESHING	 	��Ӧ����ˢ��̬�����������һ�㲻����ֱ�Ӳ���
	DISCONNECTING,					//DISCONNECTING ��Ӧ��������̬�������������Ҫ��һЩӦ����ʩ
	RECONNECTING						//RECONNECTING  �������ӻ���̬
}Module_STATE;

typedef struct judge_function
{
	u8 (*deal_working)(void);
	u8 (*deal_refreshing)(void);
	u8 (*deal_reconnecting)(void);
	u8 (*deal_disconnecting)(void);
	
}judge_function;

typedef struct OffLine_Judge_Module
{
	Module_STATE state;
	u16 cycle;
	u16 cycle_cnt;
	u16 reconnect_cnt;
	
	judge_function func;
	
}OffLine_Judge_Module;

//�������Ӧ�÷ŵ�1ms�ж�����
//ÿ1ms�Ե�ǰ״̬�����ж�
void Judge_State_Flash(OffLine_Judge_Module *pJudge);

//��ʼ���жϽṹ��
void Judge_Init(OffLine_Judge_Module *pJudge);

//�жϽṹ�����ò���
void Judge_Set(OffLine_Judge_Module *pJudge, u16 cycle);

//�������Ӻ��������ı�״̬
void Judge_State_ReConnect(OffLine_Judge_Module *pJudge);

//�պ�����������
u8 Judge_Func_Void(void);

#endif

