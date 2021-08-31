/**
 * @file        system_task.c
 * @author      RobotPilots@2020
 * @Version     V1.0
 * @date        27-October-2020
 * @brief       Decision Center.
 */

/* Includes ------------------------------------------------------------------*/
#include "system_task.h"
#include "launcher.h"
#include "cmsis_os.h"
#include "rc_sensor.h"

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static int8_t  prev_sw2 = RC_SW_MID;				//�������ñ�־
bool gyro2mech_pid_mode = false;
static uint8_t mouse_lock_R = false;				//����������־
static uint8_t rc_lock_SW1 = false;
static uint8_t key_lock_Ctrl = false;
static uint8_t relife_flag = 1;							//��ʼ�ϵ羭��һ����ʱ��Ž���λ��0 �����л�������ģʽ


/* Exported variables --------------------------------------------------------*/
flag_t flag = {
	.gimbal = {
		.gimbal_mode_lock = false,
		.relife_flag = false,
		.reset_start = false,
		.reset_ok = false,
	},
};

system_t sys = {
	.remote_mode = RC,
	.co_mode = CO_MECH,
	.state = SYS_STATE_RCLOST,
	.mode = SYS_MODE_NORMAL,
};

/* Private functions ---------------------------------------------------------*/
/**
 *	@brief	ң��ʧ�����Ĵ����Լ���ʧ��ʱһЩ��־λ������
 */
static void rc_update_info(void)
{
	if(sys.state != SYS_STATE_NORMAL) 
	{
		prev_sw2 = RC_SW_MID;	
		gyro2mech_pid_mode = false;
		
		mouse_lock_R = false;
		rc_lock_SW1 = false;
		key_lock_Ctrl = false;
	}
	else 
	{
		/* ң��ģʽ */
		if(sys.remote_mode == RC) 
		{
			// ���ü���ģʽ�µĲ���
			mouse_lock_R = false;
			rc_lock_SW1 = false;
			key_lock_Ctrl = false;
		}
		else if(sys.remote_mode == KEY)
		{
			//�ݲ���Ҫ����
			
		}
	}
}

/**
 *	@brief	����ң�����л����Ʒ�ʽ
 */
static void system_ctrl_mode_switch(void)
{
	uint8_t sw2 = RC_SW2_VALUE;
	
	if(sw2 == RC_SW_UP)	//����ģʽ
	{
		//����ת��е��Ҫ��������δ��
		
		if(prev_sw2 == RC_SW_MID) 
		{	
			gyro2mech_pid_mode = false;
			sys.remote_mode = KEY;
			sys.co_mode = CO_MECH;
			// ���л���ȥ��ʱ������Ϊ����ģʽ
			sys.mode = SYS_MODE_NORMAL;	
			
		}
		else
		{
			
		}
		prev_sw2 = RC_SW_UP;
	}
	else if(sw2 == RC_SW_MID || relife_flag == 1)	//��еģʽ
	{
		//����ת��е��Ҫ��������δ��
		
		sys.mode = SYS_MODE_NORMAL;	
		gyro2mech_pid_mode = false;
		sys.remote_mode = RC;
		sys.co_mode = CO_MECH;
		
		
		if(prev_sw2 != RC_SW_MID) //����̨�л�����ʱ
		{	
			
//			Gimbal.Yaw_Motor.angle_turns = Gimbal.Yaw_Motor.angle_turns%2;
//			Gimbal.Yaw_Motor.tg_angle_turns = Gimbal.Yaw_Motor.angle_turns;
//			Gimbal.tg_PAngle = Gimbal.Pitch_Motor.angle;
//			Gimbal.tg_PTurns = Gimbal.Pitch_Motor.angle_turns;

//			Gimbal.Pitch_Motor.tg_angle = Gimbal.Pitch_Motor.angle;
//			Gimbal.Pitch_Motor.tg_angle_turns = Gimbal.Pitch_Motor.angle_turns;
			
		}

		prev_sw2 = RC_SW_MID;
	}
	else if(sw2 == RC_SW_DOWN)	//������ģʽ
	{
		gyro2mech_pid_mode = false;
		sys.mode = SYS_MODE_NORMAL;
		sys.remote_mode = RC;
		sys.co_mode = CO_GYRO;
		
		if(prev_sw2 != RC_SW_MID) //�ӻ�е�л�����ʱ
		{	

		}
		
		prev_sw2 = RC_SW_DOWN;
		
	}
	
	
}

/**
 *	@brief	���ݼ����л�ϵͳ��Ϊ
 */
uint8_t test_auto_aim = 0;
static void system_mode_act_switch(void)//����ר��
{
	/*����״̬������*/
	
	static uint32_t KeyCtrlTime_Cur = 0;
	
	
	
	if(test_auto_aim == 0)//����������
	{
		if(IF_MOUSE_PRESSED_RIGH)
		{
			if(mouse_lock_R == false && sys.mode == SYS_MODE_NORMAL) 
			{
				sys.mode = SYS_MODE_AUTO;
				sys.co_mode = CO_GYRO;
			}
			mouse_lock_R = true;	// �Ҽ�����
		}
		else
		{
			if(sys.mode == SYS_MODE_AUTO)
			{
				sys.mode = SYS_MODE_NORMAL;
				sys.co_mode = CO_GYRO;
				
			}
			mouse_lock_R = false;	// �Ҽ�����
		}
		
		if(rc_sensor.info->thumbwheel > 330)
		{
			sys.mode = SYS_MODE_AUTO;
			sys.co_mode = CO_GYRO;
		}
		else if(rc_sensor.info->thumbwheel < -330)
		{
			sys.mode = SYS_MODE_NORMAL;
			sys.co_mode = CO_GYRO;
		}
	}
	else
	{
//		if(IF_RC_SW1_MID) {
//			/* ���� -> ���� */
//			if(rc_lock_SW1 == false && sys.mode == SYS_MODE_NORMAL) 
//			{
//				sys.mode = SYS_MODE_AUTO;
//				sys.co_mode = CO_GYRO;
//			}
//			rc_lock_SW1 = true;// Sw1����
//		} 
//		/* Sw1 ������ */
//		else {
//			/* �˳�����ģʽ�ж�(���� -> ����)*/
//			if(sys.mode == SYS_MODE_AUTO) 
//			{
//				sys.mode = SYS_MODE_NORMAL;
//				sys.co_mode = CO_GYRO;
//			}
//			rc_lock_SW1 = false;// Sw1����
//		}
		
		
	}
	
	/*������������йص�һϵ��ģʽ�л�����*/
	if(IF_KEY_PRESSED_CTRL || relife_flag)
	{
		KeyCtrlTime_Cur = osKernelSysTick()+500;//��ȡ��ǰʱ��
		if(sys.co_mode == CO_GYRO)
		{
			if(sys.mode == SYS_MODE_AUTO)
			{
				sys.mode = SYS_MODE_NORMAL;
			}
			 
			sys.co_mode = CO_MECH;
		}
		else if(sys.co_mode == CO_MECH)//�������������������ж�
		{
			
		}
		
	}
	else if(KeyCtrlTime_Cur<osKernelSysTick())//û�а���ctrlʱ,�ҳ�ʱ
	{
		if(sys.co_mode == CO_GYRO)//��������
		{
			
		}
		else if(sys.co_mode == CO_MECH)
		{
			if(sys.mode == SYS_MODE_LONGSHOOT)
			{
				sys.mode = SYS_MODE_NORMAL;
			}
			
			if(relife_flag == 0)
			{
				sys.co_mode = CO_GYRO;
			}
		}
		
	}
	
	
}

static void system_state_machine(void)
{
	// ���Ʒ�ʽ�л�
	system_ctrl_mode_switch();
	// ���Ʒ�ʽ�л�(����ģʽ�²������л�)
	if(sys.remote_mode == KEY)
		system_mode_act_switch();
}

/* Exported functions --------------------------------------------------------*/
/**
 *	@brief	ϵͳ��������
 */
uint32_t relife_time;
uint32_t thotogate = 1;
void StartSystemTask(void const * argument)
{
	relife_time = osKernelSysTick()+3000;
	for(;;)
	{
		portENTER_CRITICAL();//�����ٽ���
		
		// ����ң����Ϣ����ȡ����
		rc_update_info();
		
		if(relife_time<osKernelSysTick())
		{
			relife_flag = 0;
			BMI_SET_Kp(0.1f);
		}

		//��Ϣ����
		/* ң������ */
		if(rc_sensor.work_state == DEV_OFFLINE) 
		{
			sys.state = SYS_STATE_RCLOST;
			RC_ResetData(&rc_sensor);
			sys.remote_mode = RC;
			sys.co_mode = CO_MECH;
			flag.gimbal.reset_start = false;
			flag.gimbal.reset_ok = false;
		} 
		/* ң������ */
		else if(rc_sensor.work_state == DEV_ONLINE)
		{
			/* ң������ */
			if(rc_sensor.errno == NONE_ERR) 
			{
				/* ʧ���ָ� */
				if(sys.state == SYS_STATE_RCLOST) 
				{
					// ���ڴ˴�ͬ����̨��λ��־λ					
					// ϵͳ������λ
					
					if(flag.gimbal.reset_ok == true)
					{
						sys.remote_mode = RC;
						sys.state = SYS_STATE_NORMAL;
						sys.mode = SYS_MODE_NORMAL;
					}
					else
					{
						
						flag.gimbal.reset_start = true;
						flag.gimbal.reset_ok = false;
						relife_flag = 1;
					}
					
				}
				else if(sys.state == SYS_STATE_NORMAL)
				{
					system_state_machine();
				}
				// ���ڴ˴��ȴ���̨��λ��������л�״̬
				//
				
			}
			/* ң�ش��� */
			else if(rc_sensor.errno == DEV_DATA_ERR) {
				sys.state = SYS_STATE_RCERR;
				//reset CPU
				__set_FAULTMASK(1);//�ر����ж�
				NVIC_SystemReset();
			} else {
				sys.state = SYS_STATE_WRONG;
				//reset CPU
				__set_FAULTMASK(1);//�ر����ж�
				NVIC_SystemReset();
			}
		}
		thotogate = PEin(11);
		portEXIT_CRITICAL();//�뿪�ٽ���
		
		osDelay(2);
	}
}
