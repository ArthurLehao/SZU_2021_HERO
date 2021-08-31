
#include "offline_judge.h"


void Judge_State_Flash(OffLine_Judge_Module *pJudge)
{
	
	if(pJudge->state != DISCONNECTING)//��������״̬��++
	{
		pJudge->cycle_cnt++;
		if(pJudge->cycle_cnt>pJudge->cycle)
		{
			pJudge->state = DISCONNECTING;
		}
	}
	
	if(pJudge->state == RECONNECTING)	
	{
		pJudge->reconnect_cnt++;
		if(pJudge->reconnect_cnt>(pJudge->cycle))//��������������ת���ɹ���״̬������
		{
			pJudge->state = WORKING;
			pJudge->reconnect_cnt = 0;
		}
	}
	else if(pJudge->state == DISCONNECTING)
	{
		if(pJudge->cycle_cnt<pJudge->cycle)
			Judge_State_ReConnect(pJudge);
	}
	
}

void Judge_State_ReConnect(OffLine_Judge_Module *pJudge)
{
	pJudge->state = RECONNECTING;
}

void Judge_State_Working(OffLine_Judge_Module *pJudge)
{
	pJudge->state = WORKING;
}

void Judge_State_DisConnect(OffLine_Judge_Module *pJudge)
{
	pJudge->cycle = 9999;
	pJudge->cycle_cnt = 0;
	pJudge->reconnect_cnt = 0;
	pJudge->state = DISCONNECTING;
}

void Judge_Init(OffLine_Judge_Module *pJudge)
{
	pJudge->cycle = 9999;
	pJudge->cycle_cnt = 0;
	pJudge->reconnect_cnt = 0;
	pJudge->state = DISCONNECTING;
	
	pJudge->func.deal_disconnecting = Judge_Func_Void;
	pJudge->func.deal_reconnecting 	= Judge_Func_Void;
	pJudge->func.deal_refreshing 		= Judge_Func_Void;
	pJudge->func.deal_working 			= Judge_Func_Void;
	
}

void Judge_Set(OffLine_Judge_Module *pJudge, u16 cycle)
{
	pJudge->cycle = cycle;
	pJudge->cycle_cnt = cycle+1;
	pJudge->reconnect_cnt = 0;
	pJudge->state = DISCONNECTING;
}

u8 Judge_Func_Void(void)
{
	return 1;
}




