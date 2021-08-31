#include "launcher.h"


Launcher_Module Hero_Launcher;	//Ӣ�۵ķ������

u32 friction_speed[6] = {1000, 1200, 1300, 1400, 1500, 1600};

void Launcher_Task(Launcher_Module *Launcher, u8 mode_flag)
{
	static u8 mid_flag = 0;
	
	if(mode_flag == 0)
	{
		Launcher_set_output(0, 0, 0);
		return;
	}
	
	if(mid_flag == 1)
	{
		if(DJI_RC1.rece_Data.sw1 == 1)//Ħ���ֺ͵��ַ�֧
		{
			//��еģʽ	�򿪵���
			if(mode_flag == 3)
			{
				Launcher_Magazine();
				mid_flag = 0;
			}
			//������ģʽ	��Ħ����
			else if(mode_flag == 2)
			{
				Launcher_Friction();
				mid_flag = 0;
			}
			
		}
		else if(DJI_RC1.rece_Data.sw1 == 2)
		{
			//��еģʽ		����
			if(Hero_Launcher.Friction.state == ON)
			{
				Launcher_Dial(1);
				mid_flag = 0;
			}
			
			//������ģʽ		����
		}
		else if(DJI_RC1.rece_Data.sw1 == 3)
		{
			mid_flag = 1;
		}
		else
		{
			
		}
	}
	else	//mid_flag��Ϊ1,�����ڻ��е�ʱ������Ϊ1��
	{
		if(DJI_RC1.rece_Data.sw1 == 3)
		{
			mid_flag = 1;
		}
	}
	
	Launcher_Dial(0);
	
	Motor_PID_Update(&Hero_Launcher.Magazine);
	Motor_PID_Update(&Hero_Launcher.Safety);
	Motor_PID_Update(&Hero_Launcher.Dial);
	
	Friction_output();
	Launcher_set_output(Hero_Launcher.Magazine.output, Hero_Launcher.Safety.output, Hero_Launcher.Dial.output);
	
	
}


void Launcher_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	///////////////////////// ���ֲ���
	Motor_Module_Init(&(Hero_Launcher.Magazine), 1500);
	
	PID_Init(&(Hero_Launcher.Magazine.speed_PID));
	PID_Init(&(Hero_Launcher.Magazine.angle_PID));
	
	Hero_Launcher.Magazine.angle_PID.startfalg = PID_ENABLE;
	
	//PID����
	PID_Set(&(Hero_Launcher.Magazine.speed_PID), 6, 0, 0, 60.0f, 100, 1500);//�ٶȻ�PIDҪ����������ٶ�ʹ��
	PID_Set(&(Hero_Launcher.Magazine.angle_PID), 1, 0, 0, 100.0f, 100, 5000);
	

	///////////////////////// ���̲���
	Motor_Module_Init(&(Hero_Launcher.Dial), 8000);
	
	PID_Init(&(Hero_Launcher.Dial.speed_PID));
	PID_Init(&(Hero_Launcher.Dial.angle_PID));
	
	Hero_Launcher.Dial.angle_PID.startfalg = PID_ENABLE;
	
	//PID����
	PID_Set(&(Hero_Launcher.Dial.speed_PID), 5.1f, 0.25f, 0, 1000.0f, 10, 6000);//�ٶȻ�PIDҪ����������ٶ�ʹ��
	PID_Set(&(Hero_Launcher.Dial.angle_PID), 0.25f, 0, 0, 100.0f, 5, 1500);
	
	
	/////////////////////////	��λ���ղ���
	
	Motor_Module_Init(&(Hero_Launcher.Safety), 7000);
	
	PID_Init(&(Hero_Launcher.Safety.speed_PID));
	PID_Init(&(Hero_Launcher.Safety.angle_PID));
	
	//PID����
	PID_Set(&(Hero_Launcher.Safety.speed_PID), 5, 0, 0, 6000.0f, 10, 7000);//�ٶȻ�PIDҪ����������ٶ�ʹ��
	PID_Set(&(Hero_Launcher.Safety.angle_PID), 0, 0, 0, 100.0f, 10, 200);
	
	
	///////////////////////// Ħ���ֲ���
	
	Hero_Launcher.Friction.state = OFF;
	Hero_Launcher.Friction.PWM_Max = 1650;
	Hero_Launcher.Friction.Target_Speed = 750;
	Hero_Launcher.Friction.PWM_Speed = 750;
	
	TIM3_PWM_Init(84, 4000);//84��Ƶ+4000����, ��С����1΢��
	sys_delay_ms(50);
	
	TIM_SetCompare1(TIM3, 750);//��������ٶ�
	TIM_SetCompare2(TIM3, 750);
	
	

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOCʱ��

	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ���ģʽ
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//LED0��LED1��ӦIO��
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIO
	
}

//ˢ�µ������
void Launcher_state_update(void)
{
	Motor_State_Update(&Hero_Launcher.Magazine);
	Motor_State_Update(&Hero_Launcher.Dial);
	Motor_State_Update(&Hero_Launcher.Safety);
}

void Launcher_set_output(u16 Magazine, u16 Safety, u16 Dial)
{
	CAN2_0X1ff_BUF[2] = (Magazine>>8) & 0xff;
	CAN2_0X1ff_BUF[3] = (Magazine) & 0xff;
	
	CAN2_0X200_BUF[2] = ((Safety)>>8) & 0xff;
	CAN2_0X200_BUF[3] = (Safety) & 0xff;
	
	CAN2_0X200_BUF[4] = ((Dial)>>8) & 0xff;
	CAN2_0X200_BUF[5] = (Dial) & 0xff;
	
}


void Launcher_Magazine(void)
{
	static u8 mode = 0;
	
	if(mode == 0)
	{
		mode = 1;
		Motor_angle_add(&Hero_Launcher.Magazine,-130000);
		
	}
	else if(mode == 1)
	{
		mode = 0;
		Motor_angle_add(&Hero_Launcher.Magazine,130000);
	}
	
}

void Launcher_Dial(u8 flag)
{
	static u32 timeout_cnt;
	
	if(flag == 1)
	{
		timeout_cnt = SysTimeStamp.msec_cnt+300;
		Motor_angle_add(&Hero_Launcher.Dial, 22235);
	}
	
	if(timeout_cnt>SysTimeStamp.msec_cnt)
	{
		Hero_Launcher.Safety.tg_speed = 10000;
	}
	else
	{
		Hero_Launcher.Safety.tg_speed = -3000;
	}
	
}


void Launcher_Friction(void)
{
	
	if(Hero_Launcher.Friction.state == OFF)//��Ħ����
	{
		Hero_Launcher.Friction.state = ON;
		Hero_Launcher.Friction.Target_Speed = friction_speed[1];
	}
	else if(Hero_Launcher.Friction.state == ON)//ֹͣĦ����
	{
		Hero_Launcher.Friction.state = OFF;
		Hero_Launcher.Friction.Target_Speed = 1000;
	}
	
	
}

//Ħ�����ٶ�б�º���
void Friction_output(void)
{
	if(Hero_Launcher.Friction.Target_Speed>Hero_Launcher.Friction.PWM_Max)
	{
		Hero_Launcher.Friction.Target_Speed = Hero_Launcher.Friction.PWM_Max;
	}
	
	if(Hero_Launcher.Friction.PWM_Speed < Hero_Launcher.Friction.Target_Speed)
	{
		Hero_Launcher.Friction.PWM_Speed++;
	}
	else if(Hero_Launcher.Friction.PWM_Speed > Hero_Launcher.Friction.Target_Speed)
	{
		Hero_Launcher.Friction.PWM_Speed--;
	}
	
	
	TIM_SetCompare1(TIM3, Hero_Launcher.Friction.PWM_Speed);
	TIM_SetCompare2(TIM3, Hero_Launcher.Friction.PWM_Speed);
}






