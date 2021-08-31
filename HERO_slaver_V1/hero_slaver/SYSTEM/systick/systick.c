#include "systick.h"
#include "core_cm4.h"
#include "task.h"

u32 sys_us_cnt = 0;
u32 sys_ms_cnt = 0;

timestamp SysTimeStamp;

RCC_ClocksTypeDef RCC_Clocks;//������ʵ��
void systick_init(u32 nus)//���ո�����΢����������ʱ��������ϵͳʱ��Ƶ�ʴ���1M���ϵ�ʱ��
{
	u32 reload_val;
	//RCC_ClocksTypeDef* RCC_Clocks;//û����ʵ��
	
	
	RCC_GetClocksFreq(&RCC_Clocks);//��ȡϵͳʱ����Դ
	
	reload_val = (u32)(RCC_Clocks.HCLK_Frequency /1000000);//1us ����ֵ
	//reload_val = 168;//1us ����ֵ
	//reload_val = (u32)(reload_val/8);//8��Ƶ
	sys_us_cnt = reload_val;
	sys_ms_cnt = reload_val*1000;
	reload_val = reload_val*nus;     //���ո���ֵʵ��systick�ж�ʱ��
	
	SysTick_Config(reload_val);
	
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Pos   |
                   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
	
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	
}

//����systick��ʼ���趨�ĺ�����ִ�е��ж�
void systick_interrupt()
{

	//u16 i = 0; //����дstatic
	static u32 i = 0;
	i++;

	timestamp_step_ms();
	//timestamp_step_100us();
	Task_roop();
	
	if(i>=10000)
		i -= 10000;
}



//ϵͳ΢�뼶��ʱ
void sys_delay_us(u32 nus)
{
	u32 last_temp = 0, now_temp = 0;//
	u32 nus_cnt = 0, temp = 0;
	
	last_temp = SysTick->VAL;
	
	while(nus_cnt<nus)
	{
		now_temp = SysTick->VAL;	//��ȡ��ǰ��ʱ���ļ�����ֵ
		
		if(now_temp >= last_temp)  //������ȡ������ʱ�䣬����ж�, ��Ϊ�Ǽ�ʱ���ǵ����ģ�������Ҫ��last��ȥnow��ֵ����ʱ���ܹ���ֵ
			temp += last_temp+(SysTick->LOAD) - now_temp;
		else 
			temp += last_temp-now_temp;
		
		last_temp = now_temp;
		if(temp>sys_us_cnt)				//��ǰ����ֵ������С���׵�λ
		{

			nus_cnt += temp/sys_us_cnt;
			temp = temp%sys_us_cnt;
		}
	}
}


//ϵͳ���뼶��ʱ
void sys_delay_ms(u32 nms)
{
	u32 nms_cnt = 0;
	
	for(nms_cnt = 0; nms_cnt<nms; nms_cnt++)
	{
		sys_delay_us(1000);
	}
}

//��΢������
void timestamp_step_100us(void)
{
	SysTimeStamp.usec += 100;
	if(SysTimeStamp.usec>=1000)
	{
		SysTimeStamp.usec -= 1000;
		timestamp_step_ms();
	}
}

//ʱ�������
//�ŵ�1ms��������
void timestamp_step_ms(void)
{
	SysTimeStamp.msec++;
	SysTimeStamp.msec_cnt++;//�����������ܺ�����
	if(SysTimeStamp.msec >= 1000)
	{
		SysTimeStamp.msec -= 1000;
		SysTimeStamp.sec += 1;
		if(SysTimeStamp.sec >= 60)
		{
			SysTimeStamp.sec -= 60;
			SysTimeStamp.min += 1;
			if(SysTimeStamp.min >= 60)
			{
				SysTimeStamp.min -= 60;
				SysTimeStamp.hour += 1;
				if(SysTimeStamp.hour >= 24)
				{
					SysTimeStamp.hour -= 24;
					SysTimeStamp.day += 1;
					if(SysTimeStamp.day >= 65530)
					{
						SysTimeStamp.day = 0;
					}
				}
			}
		}
	}
}

//TIMESTAMP��ʼ��
void timestamp_init(void)
{
	SysTimeStamp.day = 0;
	SysTimeStamp.hour = 0;
	SysTimeStamp.min = 0;
	SysTimeStamp.sec = 0;
	SysTimeStamp.msec = 0;
	SysTimeStamp.usec = 0;
}


u32 timestamp_getms(void)
{
	return SysTimeStamp.msec_cnt;
}


