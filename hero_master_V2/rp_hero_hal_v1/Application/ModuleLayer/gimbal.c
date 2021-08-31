/**
 * @file        gimbal.c
 * @author      MarkVimy
 * @Version     V1.0
 * @date        23-October-2020
 * @brief       gimbal Module.
 */
 
/* Includes ------------------------------------------------------------------*/
#include "gimbal.h"
#include "vision_sensor.h"
#include "cmsis_os.h"
#include "launcher.h"
#include "control.h"
#include "can_potocol.h"
#include "vision_sensor.h"
#include "rp_math.h"
#include "kalman.h"
#include "kalman_filter.h"

/* Private macro -------------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/


void GIMBAL_Init(void);
void GIMBAL_Ctrl(void);
void GIMBAL_Test(void);
void GIMBAL_Reset(void);
void GIMBAL_SelfProtect(void);
void GIMBAL_PidCtrl(void);
bool GIMBAL_IMU_IF_BACK_MID(void);
void VISION_PREDICT(void);
void vision_anti_top_analyze(void);

//��ȡϵͳ��Ϣ
void GIMBAL_GetSysInfo(void);
void GIMBAL_GetJudgeInfo(void);
void GIMBAL_GetRcInfo(void);
void GIMBAL_GetTopGyroInfo(void);
void GIMBAL_GetSelfAttitude(void);
void VISION_PREDICT(void);//��ȡ�Ӿ���Ϣ

//��ʼ��PID�ڲ�����
static void GIMBAL_PidParamsInit(gimbal_motor_pid_t *pid, uint8_t motor_cnt);

//ֹͣPID���
static void GIMBAL_Stop(gimbal_motor_pid_t *pid);

//pid�������
static void GIMBAL_PidOut(gimbal_motor_pid_t *pid);

//����ID����PID���
static void GIMBAL_Angle_PidCalc(gimbal_motor_pid_t *pid, gimbal_motor_cnt_t MOTORx);
static void GIMBAL_Speed_PidCalc(gimbal_motor_pid_t *pid, gimbal_motor_cnt_t MOTORx);


/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// ��̨�����������
drv_can_t				*gimb_drv[GIMBAL_MOTOR_CNT];
gimbal_motor_t			*gimb_motor[GIMBAL_MOTOR_CNT];
gimbal_motor_info_t	*gimb_motor_info[GIMBAL_MOTOR_CNT];


//��̨�˶�������
float TargetPitch = 0, TargetYaw = 0;

float gimbal_mouse_vy = 0.15f;
float gimbal_mouse_vp = 0.1f;

//��Ϊ��еװ�����̨��һ������б��������Ҫ��������������ݽǶȽ������ǲ���
float gimbal_top_offset = 0;

float auto_binary;
//RPAA RobotPilotsAutoAim
//RP������벹��
float RPAA_Distance_offset = 0;

// ����PID������ֵ
uint16_t GIMBAL_SPEED_MAX	= 8000;
uint16_t GIMBAL_PID_OUT_MAX = 9000;



// ң�ذ�����Ϣ
static uint8_t key_lock_G = false;
static uint8_t key_lock_Q = false;
static uint8_t key_lock_E = false;
static uint8_t key_lock_V = false;

static uint8_t key_lock_W = false;
static uint8_t key_lock_S = false;
static uint8_t key_lock_A = false;
static uint8_t key_lock_D = false;


static uint8_t key_lock_C = false;
/* б�� */


/* ����ģʽ�µ�ǰ������Լ���תб�±��� */


/* �����ٶ� */


/* ҡ�������� */
//��еģʽ����̨����ϵ��,����ҡ����Ӧ�ٶ�,�����СҲ���������ת��,max = ��ϵ�� *660
float kRc_Mech_Gimbal_Pitch, kRc_Mech_Gimbal_Yaw;//ƽ�ƣ���ת,

//������ģʽ����̨����ϵ��,����ҡ����Ӧ�ٶ�,�����СҲ���������ת��,max = ��ϵ�� *660
float kRc_Gyro_Gimbal_Pitch, kRc_Gyro_Gimbal_Yaw;//ƽ�ƣ���ת

//��еģʽ����̨����ϵ��,���Ƽ���б�±仯��
float kKey_Mech_Gimbal_Pitch, kKey_Mech_Gimbal_Yaw;//ƽ�ƣ���ת

//������ģʽ����̨����ϵ��,���Ƽ���б�±仯��
float kKey_Gyro_Gimbal_Pitch, kKey_Gyro_Gimbal_Yaw;//ƽ�ƣ���ת



/* Exported variables --------------------------------------------------------*/
gimbal_imu_t gimbal_imu = {
	.yaw = 0,
	.pitch = 0,
	.roll = 0,
	.rate_yaw = 0,
	.rate_pitch = 0,
	.rate_roll = 0,
};

// ��̨���PID������
gimbal_motor_pid_t 	gimbal_motor_pid[GIMBAL_MOTOR_CNT] = {
	[GIMBAL_YAW] = {
		.speed.kp = 40.0f, //35.0f,
		.speed.ki = 1.0f,
		.speed.kd = 0,
		.speed.integral_max = 20000,
		.speed.out_max = 26000,
		.angle.kp = 12.0f, //30.f,
		.angle.ki = 0.0f,
		.angle.kd = 0,
		.angle.integral_max = 1000,
		.angle.out_max = 8000,
	},
	[GIMBAL_M_PITCH] = {
		.speed.kp = -22.0f, //22.0f,
		.speed.ki = -0.4f,
		.speed.kd = 0,
		.speed.integral_max = 20000,
		.speed.integral_bias = 18000,
		.speed.out_max = 28000,
		.angle.kp = -15.0f,//22.0f,
		.angle.ki = 0.f,
		.angle.kd = 0.f,
		.angle.integral_max = 2000,
		.angle.out_max = 8000,
	},
	[GIMBAL_IMU_YAW] = {
		.speed.kp = 40.0f, 
		.speed.ki = 1.0f,
		.speed.kd = 0,
		.speed.integral_max = 16000,
		.speed.out_max = 26000,
		.angle.kp = 8.0f, 
		.angle.ki = 0.f,
		.angle.kd = 0,
		.angle.integral_max = 1000,
		.angle.out_max = 8000,
	},
	[GIMBAL_IMU_PITCH] = {
		.speed.kp = -22.0f, 
		.speed.ki = -0.4f,
		.speed.kd = 0,
		.speed.integral_max = 20000,
		.speed.integral_bias = 18000,
		.speed.out_max = 28000,
		.angle.kp = 0.0f, 
		.angle.ki = 0.f,
		.angle.kd = 0,
		.angle.integral_max = 1000,
		.angle.out_max = 6000,
	},
};

gimbal_motor_pid_t 	gimbal_reset_pid[GIMBAL_MOTOR_CNT] = {
	[GIMBAL_YAW] = {
		.speed.kp = 40.0f, //35.0f,
		.speed.ki = 1.0f,
		.speed.kd = 0,
		.speed.integral_max = 10000,
		.speed.out_max = 26000,
		.angle.kp = 12.0f, //30.f,
		.angle.ki = 0.0f,
		.angle.kd = 0,
		.angle.integral_max = 1000,
		.angle.out_max = 8000,
	},
	[GIMBAL_M_PITCH] = {
		.speed.kp = -22.0f, //22.0f,
		.speed.ki = -0.4f,
		.speed.kd = 0,
		.speed.integral_max = 15000,
		.speed.integral_bias = 0,
		.speed.out_max = 28000,
		.angle.kp = -15.0f,//22.0f,
		.angle.ki = 0.f,
		.angle.kd = 0.f,
		.angle.integral_max = 2000,
		.angle.out_max = 8000,
	},
	[GIMBAL_IMU_YAW] = {
		.speed.kp = 40.0f, 
		.speed.ki = 1.0f,
		.speed.kd = 0,
		.speed.integral_max = 16000,
		.speed.out_max = 26000,
		.angle.kp = 8.0f, 
		.angle.ki = 0.0f,
		.angle.kd = 0,
		.angle.integral_max = 1000,
		.angle.out_max = 8000,
	},
};


//// ���Բ���
//gimbal_motor_pid_t 	gimbal_motor_pid[GIMBAL_MOTOR_CNT] = {
//	[GIMBAL_YAW] = {
//		.speed.kp = 10.0f, //35.0f,
//		.speed.ki = 0.f,
//		.speed.kd = 0,
//		.speed.integral_max = 20000,
//		.speed.out_max = 26000,
//		.angle.kp = 5.0f, //30.f,
//		.angle.ki = 0.0f,
//		.angle.kd = 0,
//		.angle.integral_max = 1000,
//		.angle.out_max = 8000,
//	},
//	[GIMBAL_M_PITCH] = {
//		.speed.kp = -5.0f, //22.0f,
//		.speed.ki = -0.f,
//		.speed.kd = 0,
//		.speed.integral_max = 20000,
//		.speed.integral_bias = 18000,
//		.speed.out_max = 28000,
//		.angle.kp = -5.0f,//22.0f,
//		.angle.ki = 0.f,
//		.angle.kd = 0.f,
//		.angle.integral_max = 2000,
//		.angle.out_max = 8000,
//	},
//	[GIMBAL_IMU_YAW] = {
//		.speed.kp = 10.0f, 
//		.speed.ki = 0.0f,
//		.speed.kd = 0,
//		.speed.integral_max = 16000,
//		.speed.out_max = 26000,
//		.angle.kp = 6.0f, 
//		.angle.ki = 0.f,
//		.angle.kd = 0,
//		.angle.integral_max = 1000,
//		.angle.out_max = 8000,
//	},
//};

//gimbal_motor_pid_t 	gimbal_reset_pid[GIMBAL_MOTOR_CNT] = {
//	[GIMBAL_YAW] = {
//		.speed.kp = 10.0f, //35.0f,
//		.speed.ki = 0.f,
//		.speed.kd = 0,
//		.speed.integral_max = 20000,
//		.speed.out_max = 26000,
//		.angle.kp = 5.0f, //30.f,
//		.angle.ki = 0.0f,
//		.angle.kd = 0,
//		.angle.integral_max = 1000,
//		.angle.out_max = 8000,
//	},
//	[GIMBAL_M_PITCH] = {
//		.speed.kp = -5.0f, //22.0f,
//		.speed.ki = -0.f,
//		.speed.kd = 0,
//		.speed.integral_max = 20000,
//		.speed.integral_bias = 18000,
//		.speed.out_max = 28000,
//		.angle.kp = -5.0f,//22.0f,
//		.angle.ki = 0.f,
//		.angle.kd = 0.f,
//		.angle.integral_max = 2000,
//		.angle.out_max = 8000,
//	},
//	[GIMBAL_IMU_YAW] = {
//		.speed.kp = 40.0f, 
//		.speed.ki = 1.0f,
//		.speed.kd = 0,
//		.speed.integral_max = 16000,
//		.speed.out_max = 26000,
//		.angle.kp = 6.0f, 
//		.angle.ki = 0.f,
//		.angle.kd = 0,
//		.angle.integral_max = 1000,
//		.angle.out_max = 8000,
//	},
//};


// �������˲���
extKalman_t gimbal_speed_pid_Kal[GIMBAL_MOTOR_CNT];
extKalman_t gimbal_angle_pid_Kal[GIMBAL_MOTOR_CNT];

extKalman_t vision_dis_pitch_offset;
extKalman_t vision_absolute_Yaw_Kal, vision_absolute_Pitch_Kal;
extKalman_t vision_angleY_KF, vision_angleP_KF;
extKalman_t vision_speedY_KF, vision_speedP_KF;
extKalman_t vision_accelY_KF, vision_accelP_KF;

extKalman_t vision_distance_KF, vision_speedD_KF;

moving_Average_Filter vision_absolute_Y_MF, vision_absolute_P_MF;
moving_Average_Filter vision_angleY_MF, vision_angleP_MF;
moving_Average_Filter vision_speedY_MF, vision_speedP_MF;
moving_Average_Filter vision_accelY_MF, vision_accelP_MF;
moving_Average_Filter vision_distance_MF, vision_speedD_MF;

moving_Average_Filter zhen_time_ms;

LOW_Pass_Filter vision_speedY_LPF, vision_speedP_LPF;


//�����ݱ���
moving_Average_Filter Anti_top_yaw_MF;//�ͺ����ݵĻ���ƽ���˲���
moving_Average_Filter gimbal_now_yaw_raw, gimbal_now_pitch_raw;//��¼��99ms�ڵ������ǽǶ�,��������
moving_Average_Filter yaw_raw_ms, pitch_raw_ms; //��¼��99ms�ڵ������ǽǶ�,ms����

Median_Filter Anti_top_yaw_MedianF;
/*
��һ�淴���ݴ�����Ҫ��T1, T2���б궨
T1�궨���������ͻ��ˣ������飬�����װ�װ��ƶ��󼤹����ʱʱ�䡣
T2�궨�������趨�ٶȣ����ݲ�ͬ����⼸�������ʱ���Ӳ��¿��ص�����Ŀ���ʱ�䣬Ȼ�����һ������
T0 ����������װ�װ���ԽǶ������ʱ�����
һ�����
T3' = n*T0-T1-T2
T3'-(N-1)T0 = T0-T1-T2 = T3

while(T3<0)//����T3�ͺ󲻳���һ������
{
	T3+=T0;
}
Ȼ����T3��������Ӧ�ø���ǰ��ȥ�ڼ�֡�ľ��ԽǶ�����
*/
float Anti_top_Time0_enermy_circle = 0;//�з�С����ʱ��װ�װ���������ĵ�����ʱ�䣬��λ���룬תһȦ�����ĸ�����
float Anti_top_Time1_chase_delay = 200;//���������ʱ����λ����

//Anti_top_Time2_shoot_delay�ӷ�������ָ���������װ�װ��ʱ�䡣�ɻ�е�ṹ����������з���������������ӳ١�
//2021/03/30  3m,5m,8m ���ݲ���� T2 = 59.737f*distance+183.07; ���뵥λΪ��
float Anti_top_Time2_shoot_delay = 0;		

float Anti_top_Time3_chase_force_delay = 0;//������ʱ��Ϊ�˻�����һ��װ�װ壬ǿ������������ʱ��

moving_Average_Filter Anti_top_AimPos_MAF;


extKalman_t AntiTop_dis_pitch_offset;
// ����ģ�������
gimbal_ctrl_t		gimbal_ctrl = {
	.motor = &gimbal_motor_pid,
};

// ����ģ�鴫����
gimbal_dev_t		gimbal_dev = {
	.gimbal_motor[GIMBAL_YAW] = &gimbal_motor[GIMBAL_YAW],
	.gimbal_motor[GIMBAL_M_PITCH] = &gimbal_motor[GIMBAL_M_PITCH],
	.gimbal_motor[GIMBAL_S_PITCH] = &gimbal_motor[GIMBAL_S_PITCH],
	.gimbal_motor[GIMBAL_IMU_YAW] = &gimbal_motor[GIMBAL_IMU_YAW],
	.imu_sensor = &imu_sensor,
	.rc_sensor = &rc_sensor,
	.gimbal_imu_sensor = &gimbal_imu,
};

// ����ģ����Ϣ
gimbal_info_t 	gimbal_info = {
	.remote_mode = RC,
	.co_mode = CO_MECH,
	.local_mode = GIMBAL_MODE_NORMAL,
	.co_angle_logic = LOGIC_FRONT,
	.if_fire_ready = 0,
};

gimbal_t gimbal = {
	.controller = &gimbal_ctrl,
	.dev = &gimbal_dev,
	.info = &gimbal_info,
	.reset = GIMBAL_Reset,
	.init = GIMBAL_Init,
	.test = GIMBAL_Test,
	.ctrl = GIMBAL_Ctrl,
	.gimbal_imu_if_back_mid = GIMBAL_IMU_IF_BACK_MID,
	.self_protect = GIMBAL_SelfProtect,
	
};

/* Private functions ---------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/**
 *	@brief	��̨���PID������ʼ����������
 */
static void GIMBAL_PidParamsInit(gimbal_motor_pid_t *pid, uint8_t motor_cnt)
{
	for(uint8_t i = 0; i < motor_cnt; i++) {
		pid_val_init(&pid[i].angle);
		pid_val_init(&pid[i].speed);
		pid[i].out = 0;
		
		pid[i].angle.target = pid[i].angle.measure;
	}	
}

static void GIMBAL_Pidclear(gimbal_motor_pid_t *pid, uint8_t motor_cnt)
{
	for(uint8_t i = 0; i < motor_cnt; i++) {
		pid_val_init(&pid[i].angle);
		pid_val_init(&pid[i].speed);
		pid[i].out = 0;
	}	
}


static void GIMBAL_SinglePidParamsInit(gimbal_motor_pid_t *pid, uint8_t motor_cnt)
{
	
	
	pid_val_init(&pid[motor_cnt].angle);
	pid_val_init(&pid[motor_cnt].speed);
	pid[motor_cnt].out = 0;
	
	pid[motor_cnt].angle.target = pid[motor_cnt].angle.measure;

}

/**
 *	@brief	��̨���̵��ж����������
 */
static void GIMBAL_Stop(gimbal_motor_pid_t *pid)
{
	
	pid[GIMBAL_S_PITCH].out = 0;
	pid[GIMBAL_M_PITCH].out = 0;
	pid[GIMBAL_YAW].out = 0;
	pid[GIMBAL_IMU_YAW].out = 0;
	
	
	CAN1_0X1ff_BUF[3] = 0;
	CAN2_0X1ff_BUF[2] = 0;
	CAN2_0X1ff_BUF[3] = 0;
	
}

/**
 *	@brief	��̨���PID���
 */
static void GIMBAL_PidOut(gimbal_motor_pid_t *pid)
{
	
	if(gimb_motor[GIMBAL_YAW]->work_state == DEV_ONLINE) 
	{
		if(gimbal.info->co_mode == CO_MECH)
		{
			CAN1_0X1ff_BUF[3] = (int16_t)(pid[GIMBAL_YAW].out);
		}
		else if(gimbal.info->co_mode == CO_GYRO)
		{
			CAN1_0X1ff_BUF[3] = (int16_t)(pid[GIMBAL_IMU_YAW].out);
		}
	} 
	else 
	{
		CAN1_0X1ff_BUF[3] = 0;
	}

	if(gimb_motor[GIMBAL_M_PITCH]->work_state == DEV_ONLINE) 
	{
		CAN2_0X1ff_BUF[2] = (int16_t)(pid[GIMBAL_M_PITCH].out);
		CAN2_0X1ff_BUF[3] = (int16_t)(pid[GIMBAL_M_PITCH].out);
	} 
	else 
	{
		CAN2_0X1ff_BUF[2] = 0;
		CAN2_0X1ff_BUF[3] = 0;
	}

}

static void GIMBAL_Angle_PidCalc(gimbal_motor_pid_t *pid, gimbal_motor_cnt_t MOTORx)
{
	pid[MOTORx].angle.err = pid[MOTORx].angle.target - pid[MOTORx].angle.measure;
	
	if(MOTORx == GIMBAL_YAW || MOTORx == GIMBAL_IMU_YAW)
	{
		if(pid[MOTORx].angle.err>GIMBAL_YAW_CIRCULAR_STEP/2)
		{
			pid[MOTORx].angle.err -= GIMBAL_YAW_CIRCULAR_STEP;
		}
		else if(pid[MOTORx].angle.err<-GIMBAL_YAW_CIRCULAR_STEP/2)
		{
			pid[MOTORx].angle.err += GIMBAL_YAW_CIRCULAR_STEP;
		}
	}
	pid[MOTORx].angle.true_err = pid[MOTORx].angle.err;
	pid[MOTORx].angle.err = KalmanFilter(&gimbal_angle_pid_Kal[MOTORx], pid[MOTORx].angle.err);
	
	single_pid_ctrl(&pid[MOTORx].angle);
	pid[MOTORx].speed.target = pid[MOTORx].angle.out;
	
	pid[MOTORx].speed.err = pid[MOTORx].speed.target - pid[MOTORx].speed.measure;
	pid[MOTORx].speed.err = KalmanFilter(&gimbal_speed_pid_Kal[MOTORx], pid[MOTORx].speed.err);
	
	single_pid_ctrl(&pid[MOTORx].speed);
	pid[MOTORx].out = pid[MOTORx].speed.out;
}

/**
 *	@brief	��̨����ٶȻ�
 */
static void GIMBAL_Speed_PidCalc(gimbal_motor_pid_t *pid, gimbal_motor_cnt_t MOTORx)
{
	pid[MOTORx].speed.err = pid[MOTORx].speed.target - pid[MOTORx].speed.measure;
	pid[MOTORx].speed.err = KalmanFilter(&gimbal_speed_pid_Kal[MOTORx], pid[MOTORx].speed.err);
	
	single_pid_ctrl(&pid[MOTORx].speed);
	pid[MOTORx].out = pid[MOTORx].speed.out;
}



/* Exported functions --------------------------------------------------------*/
/* ��Ϣ�� --------------------------------------------------------------------*/

/**
 *	@brief	���̻�ȡϵͳ��Ϣ
 */
void GIMBAL_GetSysInfo(void)//ֻҪȷ����ʱ�л������⼴�ɾ���
{
	static uint16_t sw_outtime = 0;
	
	/*----���Ʒ�ʽ�޸�----*/
	if(sys.remote_mode == RC) {
		gimbal_info.remote_mode = RC;
	}
	else if(sys.remote_mode == KEY) {
		gimbal_info.remote_mode = KEY;
	}
	gimbal.info->if_fire_ready = 1;
	/*----����ģʽ�޸�----*/
	switch(sys.mode)	//�л�ģʽ
	{
		case SYS_MODE_LONGSHOOT: 
			if(gimbal_info.local_mode!=GIMBAL_MODE_SZUPUP)
			{
				gimbal_info.local_mode = GIMBAL_MODE_LONGSHOOT;	
			}
			break;

		case SYS_MODE_AUTO://����ģʽ
			gimbal_info.local_mode = GIMBAL_MODE_AUTO;
			break;
		case SYS_MODE_NORMAL:
			gimbal.info->if_fire_ready = 0;
			gimbal_info.local_mode = GIMBAL_MODE_NORMAL;
			break;
		case SYS_MODE_PARK: break;
		
		default: break;
	}
	
	if(sys.state == SYS_STATE_RCLOST)
	{
		gimbal_info.co_mode = sys.co_mode;
	}
	
	if(sys.co_mode != gimbal_info.co_mode)
	{
		if(gimbal_info.co_mode == CO_MECH)
		{
			gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target = gimbal_motor_pid[GIMBAL_IMU_YAW].angle.measure;
			GIMBAL_SinglePidParamsInit(gimbal_motor_pid, GIMBAL_YAW);
			gimbal_motor_pid[GIMBAL_YAW].angle.target = 0;
			gimbal_info.co_mode = CO_GYRO;
			sw_outtime = 0;
		}
		else if((abs(gimbal_motor[GIMBAL_YAW].info->angle_sum))<50 || sw_outtime>1000)
		{
			
			GIMBAL_SinglePidParamsInit(gimbal_motor_pid, GIMBAL_IMU_YAW);
			gimbal_motor_pid[GIMBAL_YAW].angle.target = 0;
			gimbal_info.co_mode = CO_MECH;
			
			sw_outtime = 0;
		}
		sw_outtime++;
	}
	else
	{
		gimbal_info.co_mode = sys.co_mode;
		sw_outtime = 0;
	}
	
}

void GIMBAL_GetJudgeInfo(void)
{
	//�ޣ� �����ǲ���ϵͳ�����ݻ�ȡ
}

void GIMBAL_GetRcInfo(void)
{
	/* ϵͳ���� */
	if(sys.state == SYS_STATE_NORMAL)
	{
		if(sys.remote_mode == RC) 
		{
			key_lock_G = false;
			key_lock_Q = false;
			key_lock_E = false;
			key_lock_V = false;
		}
		else if(sys.remote_mode == KEY) 
		{
		}
	}
	/* ϵͳ�쳣 ʧ�� ���� ���� */
	else 
	{
		key_lock_G = false;
		key_lock_Q = false;
		key_lock_E = false;
		key_lock_V = false;
	}
}

void GIMBAL_GetTopGyroInfo(void)
{
	
}

void GIMBAL_GetSelfAttitude(void)
{
	
}

gimbal_motor_pid_t temp_yaw, temp_pitch, temp_mpu_yaw;
gimbal_motor_t motor_yaw, motor_pitch, motor_mpu_yaw;
float gimbal_imu_yaw_angle_sum = 0;
void GIMBAL_UpdateController(void)
{
	float err;
	static float prev_angle = 0;
	
	imu_sensor_info_t *imu = gimbal_dev.imu_sensor->info;
	
	temp_yaw = gimbal_motor_pid[GIMBAL_YAW];
	temp_pitch = gimbal_motor_pid[GIMBAL_M_PITCH];
	temp_mpu_yaw = gimbal_motor_pid[GIMBAL_IMU_YAW];
	
	motor_yaw = gimbal_motor[GIMBAL_YAW];
	motor_pitch = gimbal_motor[GIMBAL_M_PITCH];
	motor_mpu_yaw = gimbal_motor[GIMBAL_IMU_YAW];
	
	//gimbal_imu.rate_yaw = (imu->rate_yaw*cos(imu->pitch*PI/180.0f))+(imu->rate_pitch*sin(imu->pitch*PI/180.0f))
	//																					-	imu->rate_yaw*cos(imu->roll*PI/180.0f) + imu->rate_roll*sin(imu->roll*PI/180.0f);
	gimbal_imu.rate_yaw = (imu->rate_yaw*cos(imu->pitch*PI/180.0f)*cos(imu->roll*PI/180.0f))
												+	(imu->rate_pitch*sin(imu->pitch*PI/180.0f)) 
												+ imu->rate_roll*sin(imu->roll*PI/180.0f)*cos(imu->pitch*PI/180.0f);
	
	gimbal_imu.rate_roll = -imu->rate_pitch;
	
	//gimbal_imu.rate_pitch = imu->rate_roll;//
	gimbal_imu.rate_pitch = imu->rate_roll*cos(imu->pitch*PI/180.0f) + imu->rate_yaw*sin(imu->pitch*PI/180.0f);
	
	gimbal.dev->gimbal_imu_sensor->yaw = -imu->yaw;
	gimbal.dev->gimbal_imu_sensor->roll = -imu->roll;
	gimbal.dev->gimbal_imu_sensor->pitch = -imu->pitch;
	
	gimbal_motor_pid[GIMBAL_IMU_YAW].angle.measure = imu->yaw/360.0f*GIMBAL_YAW_CIRCULAR_STEP;
	gimbal_motor_pid[GIMBAL_IMU_YAW].speed.measure = -gimbal.dev->imu_sensor->info->rate_yaw;
	
	err = gimbal_motor_pid[GIMBAL_IMU_YAW].angle.measure - prev_angle;
	
	/* ����� ���������ǽǶ�*/
	if(abs(err) > GIMBAL_YAW_CIRCULAR_STEP/2)
	{
		/* 0�� -> 12288 */
		if(err >= 0)
			gimbal_imu_yaw_angle_sum += -GIMBAL_YAW_CIRCULAR_STEP + err;
		/* 12288�� -> 0 */
		else
			gimbal_imu_yaw_angle_sum += GIMBAL_YAW_CIRCULAR_STEP + err;
	}
	/* δ����� */
	else
	{
		gimbal_imu_yaw_angle_sum += err;
	}
	prev_angle = gimbal_motor_pid[GIMBAL_IMU_YAW].angle.measure;
	
	gimbal_motor_pid[GIMBAL_YAW].angle.measure = gimbal_motor[GIMBAL_YAW].info->angle_sum;
	gimbal_motor_pid[GIMBAL_YAW].speed.measure = -gimbal.dev->imu_sensor->info->rate_yaw;
	
	gimbal_motor_pid[GIMBAL_M_PITCH].angle.measure = gimbal_motor[GIMBAL_M_PITCH].info->angle;
	gimbal_motor_pid[GIMBAL_M_PITCH].speed.measure = -gimbal.dev->imu_sensor->info->rate_roll;
	
	gimbal_motor_pid[GIMBAL_IMU_PITCH].angle.measure = -imu->pitch*22.75556f;
	gimbal_motor_pid[GIMBAL_IMU_PITCH].speed.measure = -gimbal.dev->imu_sensor->info->rate_roll;
}

/* Ӧ�ò� --------------------------------------------------------------------*/
uint32_t binary = 30;
bool GIMBAL_IMU_IF_BACK_MID(void)
{
	float gimbal_angle_err;
	
	gimbal_angle_err = gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target - gimbal_motor_pid[GIMBAL_IMU_YAW].angle.measure;
	
	if(gimbal_angle_err>GIMBAL_YAW_CIRCULAR_STEP/2)
	{
		gimbal_angle_err -= GIMBAL_YAW_CIRCULAR_STEP;
	}
	else if(gimbal_angle_err<-GIMBAL_YAW_CIRCULAR_STEP/2)
	{
		gimbal_angle_err += GIMBAL_YAW_CIRCULAR_STEP;
	}
	
	return ((abs(gimbal_angle_err)) < (binary))?true:false;
}

float GIMBAL_GET_IMU_MID_ERR(void)
{
	float gimbal_angle_err;
	
	gimbal_angle_err = gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target - gimbal_motor_pid[GIMBAL_IMU_YAW].angle.measure;
	
	if(gimbal_angle_err>GIMBAL_YAW_CIRCULAR_STEP/2)
	{
		gimbal_angle_err -= GIMBAL_YAW_CIRCULAR_STEP;
	}
	else if(gimbal_angle_err<-GIMBAL_YAW_CIRCULAR_STEP/2)
	{
		gimbal_angle_err += GIMBAL_YAW_CIRCULAR_STEP;
	}
	
	return gimbal_angle_err;
}

//���˳����ϵ�����ĳ�ʼ��
uint8_t first_flag = 0;
void GIMBAL_Reset(void)
{
	float err_y, err_p;
	float err_to_pos, err_to_neg;
	
	gimbal.info->co_mode = CO_MECH;
	
	gimbal_motor_pid[GIMBAL_YAW].angle.out_max = 6000;//������̨�����ٶ�
	gimbal_motor_pid[GIMBAL_M_PITCH].angle.out_max = 2000;
	REBOOT_RELAY_SET_TOGGLE();//���濪���������̵���
	GIMBAL_UpdateController();//������̨��Ϣ
	
	gimbal_motor_pid[GIMBAL_M_PITCH].angle.target = GIMBAL_PITCH_MID;
	
	
	gimbal_motor_pid[GIMBAL_YAW].angle.target = 0;
	gimbal_motor_pid[GIMBAL_YAW].angle.measure = gimbal_motor[GIMBAL_YAW].info->angle_sum;
	
	//ԭ�����ǿ���ѡ��ֻ���ϵ�ʱ����һ�飬��������ÿ��ң�ض�ʧ�������ϵ�һ��
	if(first_flag == 0)
	{
		gimbal_motor_pid[GIMBAL_YAW].angle.measure = 2000; //�ϵ縴λ,��һ���Ľ��ٶ���ת
		if(REBOOT_READ() == 0)//���ܵ������ź�
		{
			//��Ϊ1��1.5����̨���������Ƕȿ��Գ�ǰ�������Ծͽ�ԭ����źŴ���
			err_to_pos = abs(gimbal_motor[GIMBAL_YAW].info->angle - (float)(GIMBAL_MECH_ANGLE_POS_MID));
			err_to_neg = abs(gimbal_motor[GIMBAL_YAW].info->angle - (float)(GIMBAL_MECH_ANGLE_NEG_MID));
			
			//�Ծͽ�ԭ��ѡȡʱ����Ҫ�Ե��Խ�����������жϣ��ٽ�ֵ�Ƽ�Ϊ������Ƕȵ�һ��
			if(err_to_pos>4096)
			{
				err_to_pos = err_to_pos-4096;
			}
			if(err_to_neg > 4096)
			{
				err_to_neg = err_to_neg-4096;
			}
			
			if(err_to_neg>err_to_pos)
			{
				first_flag = 1;
				gimbal_motor[GIMBAL_YAW].info->angle_sum = -GIMBAL_MECH_ANGLE_POS_MID + (gimbal_motor[GIMBAL_YAW].info->angle);
			}
			else if(err_to_neg<err_to_pos)
			{
				first_flag = 1;
				gimbal_motor[GIMBAL_YAW].info->angle_sum = -GIMBAL_MECH_ANGLE_NEG_MID + (gimbal_motor[GIMBAL_YAW].info->angle);
			}
			//��Ϊ������errʱ͵�����˾���ֵ�����Դ˴���Ҫ�ټ���һ�νǶȵ�����
			if((gimbal_motor[GIMBAL_YAW].info->angle_sum) > 4096)
			{
				gimbal_motor[GIMBAL_YAW].info->angle_sum = gimbal_motor[GIMBAL_YAW].info->angle_sum - 8192;
			}
			else if((gimbal_motor[GIMBAL_YAW].info->angle_sum) < -4096)
			{
				gimbal_motor[GIMBAL_YAW].info->angle_sum = 8192 + gimbal_motor[GIMBAL_YAW].info->angle_sum;
			}
			
		}
	}
	
	
	GIMBAL_PidCtrl();
	
	
	err_y = abs(gimbal_motor_pid[GIMBAL_YAW].angle.target - gimbal_motor_pid[GIMBAL_YAW].angle.measure);
	err_p = abs(gimbal_motor_pid[GIMBAL_M_PITCH].angle.target - gimbal_motor_pid[GIMBAL_M_PITCH].angle.measure);

	if(err_y <= 50 && err_p <= 50 && first_flag == 1)//�Ѿ�����
	{
		gimbal_motor_pid[GIMBAL_YAW].angle.target = 0;
		gimbal_motor_pid[GIMBAL_M_PITCH].angle.target = GIMBAL_PITCH_MID;
		GIMBAL_Pidclear(gimbal_motor_pid, GIMBAL_MOTOR_CNT);

		gimbal_motor_pid[GIMBAL_YAW].angle.out_max = 8000;
		gimbal_motor_pid[GIMBAL_M_PITCH].angle.out_max = 8000;
		REBOOT_RELAY_SET_ON();	//�رռ̵���
		flag.gimbal.reset_ok = true;
		flag.gimbal.reset_start = false;
	}
	
}


/* ����� --------------------------------------------------------------------*/
void GIMBAL_Init(void)
{
	
	
	gimb_drv[GIMBAL_YAW] = gimbal_dev.gimbal_motor[GIMBAL_YAW]->driver;
	gimb_drv[GIMBAL_M_PITCH] = gimbal_dev.gimbal_motor[GIMBAL_M_PITCH]->driver;
	gimb_drv[GIMBAL_S_PITCH] = gimbal_dev.gimbal_motor[GIMBAL_S_PITCH]->driver;

	gimb_motor[GIMBAL_YAW] = gimbal_dev.gimbal_motor[GIMBAL_YAW];
	gimb_motor[GIMBAL_M_PITCH] = gimbal_dev.gimbal_motor[GIMBAL_M_PITCH];
	gimb_motor[GIMBAL_S_PITCH] = gimbal_dev.gimbal_motor[GIMBAL_S_PITCH];
	
	gimb_motor_info[GIMBAL_YAW] = gimbal_dev.gimbal_motor[GIMBAL_YAW]->info;
	gimb_motor_info[GIMBAL_M_PITCH] = gimbal_dev.gimbal_motor[GIMBAL_M_PITCH]->info;
	gimb_motor_info[GIMBAL_S_PITCH] = gimbal_dev.gimbal_motor[GIMBAL_S_PITCH]->info;
	
	KalmanCreate(&gimbal_speed_pid_Kal[GIMBAL_YAW], 1, 5);
	KalmanCreate(&gimbal_speed_pid_Kal[GIMBAL_M_PITCH], 1, 10);
	KalmanCreate(&gimbal_speed_pid_Kal[GIMBAL_IMU_YAW], 1, 0);
	
	KalmanCreate(&gimbal_angle_pid_Kal[GIMBAL_YAW], 1, 30);
	KalmanCreate(&gimbal_angle_pid_Kal[GIMBAL_M_PITCH], 1, 200);
	KalmanCreate(&gimbal_angle_pid_Kal[GIMBAL_IMU_YAW], 1, 20);
	
	//��������˲���
	KalmanCreate(&vision_absolute_Yaw_Kal, 1, 20);//10);	//���ԽǶȵĿ������˲���
	KalmanCreate(&vision_absolute_Pitch_Kal, 1, 20);
	KalmanCreate(&vision_dis_pitch_offset, 1, 30);
	
	KalmanCreate(&vision_angleY_KF, 1, 0);
	KalmanCreate(&vision_angleP_KF, 1, 0);
	KalmanCreate(&vision_speedY_KF, 1, 40);//5);
	KalmanCreate(&vision_speedP_KF, 1, 40);
	KalmanCreate(&vision_accelY_KF, 1, 10);
	KalmanCreate(&vision_accelP_KF, 1, 0);
	
	average_init(&vision_absolute_Y_MF, 5);//���ԽǶȵĻ���ƽ���˲���
	average_init(&vision_absolute_P_MF, 10);
	
	average_init(&vision_angleY_MF, 1);
	average_init(&vision_angleP_MF, 1);
	average_init(&vision_speedY_MF, 5);//5);
	average_init(&vision_speedP_MF, 10);
	average_init(&vision_accelY_MF, 30);
	average_init(&vision_accelP_MF, 10);
	
	average_init(&Anti_top_yaw_MF, 3);
	
	KalmanCreate(&vision_distance_KF, 1, 30);
	KalmanCreate(&vision_speedD_KF, 1, 100);
	average_init(&vision_distance_MF, 30);
	average_init(&vision_speedD_MF, 80);
	
	
	
	median_init(&Anti_top_yaw_MedianF, 80);
	
	MAF_ANTI_TOP_init(&Absolute_yaw_angle_raw, AUTO_ANTI_TOP_TICK/10);//�Խ�AUTO_ANTI_TOP_TICK/10֡��Ϊ�����ݵ��ж�
	MAF_ANTI_TOP_init(&Absolute_pitch_angle_raw, AUTO_ANTI_TOP_TICK/10);
	MAF_ANTI_TOP_init(&Absolute_distance_raw, 3);
	KalmanCreate(&AntiTop_dis_pitch_offset, 1, 10);

	average_init(&yaw_raw_ms, 99);
	average_init(&pitch_raw_ms, 99);
	average_init(&gimbal_now_yaw_raw, 99);
	average_init(&gimbal_now_pitch_raw, 99);
	average_init(&zhen_time_ms, 99);
	
	average_init(&Anti_top_AimPos_MAF, 10);
	
	LPF_Init(&vision_speedY_LPF, 5);
	LPF_Init(&vision_speedP_LPF, 1);
	
}

void GIMBAL_GetInfo(void)	//���±��ز���ģʽ
{
	GIMBAL_GetSysInfo();								//��ȡϵͳ����ģʽ״̬
	GIMBAL_GetJudgeInfo();							//�ж���̨��Ч��
	GIMBAL_GetRcInfo();								//��ȡң������Ϣ
	//GIMBAL_GetTopGyroInfo();						//�ж��Ƿ�����ת״̬
	//GIMBAL_GetSelfAttitude();					//��ȡ��������	
	GIMBAL_UpdateController();					//���¿�������Ϣ����
	VISION_PREDICT();
}

void GIMBAL_SelfProtect(void)
{
	GIMBAL_Stop(gimbal_motor_pid);
	GIMBAL_PidParamsInit(gimbal_motor_pid, GIMBAL_MOTOR_CNT);
	first_flag = 0;
	GIMBAL_GetInfo();
}



void GIMBAL_PidCtrl(void)
{
	//����pid
	if(gimbal.info->co_mode == CO_MECH)
	{
		GIMBAL_Angle_PidCalc(gimbal_motor_pid, GIMBAL_YAW);
	}
	else if(gimbal.info->co_mode == CO_GYRO)
	{
		GIMBAL_Angle_PidCalc(gimbal_motor_pid, GIMBAL_IMU_YAW);
	}
	
	GIMBAL_Angle_PidCalc(gimbal_motor_pid, GIMBAL_M_PITCH);
	
	GIMBAL_Angle_PidCalc(gimbal_motor_pid, GIMBAL_IMU_PITCH);
	
	// ��̨����ٶȻ�
	//GIMBAL_Speed_PidCalc(gimbal_motor_pid, GIMBAL_YAW);
	//GIMBAL_Speed_PidCalc(gimbal_motor_pid, GIMBAL_M_PITCH);
	
	// ��̨��������Ӧ
	GIMBAL_PidOut(gimbal_motor_pid);
}

void GIMBAL_KEY_GET_TARGET(void)
{
	
}

void GIMBAL_NormalCtrl(void)
{
	float pre_target_pitch;
	static uint32_t key_press_tick;
	launcher.info->fire_permit_flag = 1;
	
	if(gimbal.info->co_mode == CO_MECH)
	{
		
		if(IF_KEY_PRESSED_C)
		{
			TargetYaw = -0.0001f*(rc_move_info.key_d-rc_move_info.key_a);
			TargetPitch = 0.0001f*(rc_move_info.key_w-rc_move_info.key_s);
		}
		else
		{
			TargetPitch = -(float)(rc_move_info.mouse_vy)*gimbal_mouse_vp;
			TargetYaw = 0;
		}
		
		
		gimbal_motor_pid[GIMBAL_YAW].angle.target = gimbal_motor_pid[GIMBAL_YAW].angle.target + TargetYaw;
		
		TargetPitch = gimbal_motor_pid[GIMBAL_M_PITCH].angle.target - TargetPitch;
		gimbal_motor_pid[GIMBAL_M_PITCH].angle.target = constrain(TargetPitch, GIMBAL_PITCH_MIN, GIMBAL_PITCH_MAX);
		
		
	}
	else if(gimbal.info->co_mode == CO_GYRO)//
	{
		TargetPitch = -(float)(rc_move_info.mouse_vy)*gimbal_mouse_vp;
		TargetYaw = -(float)(rc_move_info.mouse_vx)*gimbal_mouse_vy;
		
		
		TargetYaw = TargetYaw + gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target;
		if(TargetYaw>(GIMBAL_YAW_CIRCULAR_STEP/2))
		{
			TargetYaw -= GIMBAL_YAW_CIRCULAR_STEP;
		}
		else if(TargetYaw<-(GIMBAL_YAW_CIRCULAR_STEP/2))
		{
			TargetYaw += GIMBAL_YAW_CIRCULAR_STEP;
		}
		gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target = constrain(TargetYaw, -GIMBAL_YAW_CIRCULAR_STEP/2, GIMBAL_YAW_CIRCULAR_STEP/2);
		
		if(IF_KEY_PRESSED_G)//������pitchĿ��
		{

		}
		else//����pitchĿ��
		{
			pre_target_pitch = gimbal_motor_pid[GIMBAL_M_PITCH].angle.target;
			pre_target_pitch = pre_target_pitch - TargetPitch;
			gimbal_motor_pid[GIMBAL_M_PITCH].angle.target = constrain(pre_target_pitch, GIMBAL_PITCH_MIN, GIMBAL_PITCH_MAX);
		}
		
		//��������
		if(gimbal.gimbal_imu_if_back_mid())
		{
			
//			if(IF_KEY_PRESSED_G && key_lock_G == false)//�½���
//			{
//				//gimbal_motor_pid[GIMBAL_M_PITCH].angle.target = constrain(gimbal_motor_pid[GIMBAL_M_PITCH].angle.target + 500, GIMBAL_PITCH_MIN, GIMBAL_PITCH_MAX);
//				gimbal_motor_pid[GIMBAL_M_PITCH].angle.target = 2689;
//				key_lock_G = true;
//			}
//			else if(IF_KEY_PRESSED_G == 0)
//			{
//				key_lock_G = false;
//			}
			
			
			if(IF_KEY_PRESSED_Q && key_lock_Q == false)//�½���
			{
				TargetYaw = gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target + GIMBAL_YAW_CIRCULAR_STEP/4;
				
				key_lock_Q = true;
			}
			else if(IF_KEY_PRESSED_Q == 0)
			{
				key_lock_Q = false;
			}
			
			
			if(IF_KEY_PRESSED_E && key_lock_E == false)//�½���
			{
				TargetYaw = gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target - GIMBAL_YAW_CIRCULAR_STEP/4;//���Ŀ���
				key_lock_E = true;
			}
			else if(IF_KEY_PRESSED_E == 0)
			{
				key_lock_E = false;
			}
			
			if(IF_KEY_PRESSED_V && key_lock_V == false)//�½���
			{
				if(IF_KEY_PRESSED_A)
				{
					TargetYaw = gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target + GIMBAL_YAW_CIRCULAR_STEP/2;
				}
				else 
				{
					TargetYaw = gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target - GIMBAL_YAW_CIRCULAR_STEP/2;
				}
				
				key_lock_V = true;
			}
			else if(IF_KEY_PRESSED_V == 0)
			{
				key_lock_V = false;
			}
			
			
		}
	}
	
	
	if(TargetYaw>(GIMBAL_YAW_CIRCULAR_STEP/2))
	{
		TargetYaw -= GIMBAL_YAW_CIRCULAR_STEP;
	}
	else if(TargetYaw<-(GIMBAL_YAW_CIRCULAR_STEP/2))
	{
		TargetYaw += GIMBAL_YAW_CIRCULAR_STEP;
	}
	gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target = constrain(TargetYaw, -GIMBAL_YAW_CIRCULAR_STEP/2, GIMBAL_YAW_CIRCULAR_STEP/2);

	
	TargetYaw = 0;
	TargetPitch = 0;
}

float get_auto_speed(moving_Average_Filter *AUTO_MF, float angle)
{
	float speed;

	if((angle-(AUTO_MF->aver_num))>GIMBAL_YAW_CIRCULAR_STEP/2 )
	{
			angle-=GIMBAL_YAW_CIRCULAR_STEP;
	}
	else if((angle-(AUTO_MF->aver_num))<-(GIMBAL_YAW_CIRCULAR_STEP/2))
	{
			angle+=GIMBAL_YAW_CIRCULAR_STEP;
	}

	//last_angle = Vision_y_angle.aver_num;
	//average_add(AUTO_MF, angle);

	speed = (-(AUTO_MF->aver_num) + angle);


    return speed;
}

float get_auto_speed_V2(float angle)
{
	float speed;
	static float last_raw_angle;
	
	if((angle-last_raw_angle)>GIMBAL_YAW_CIRCULAR_STEP/2 )
	{
			angle-=GIMBAL_YAW_CIRCULAR_STEP;
	}
	else if((angle-last_raw_angle)<-(GIMBAL_YAW_CIRCULAR_STEP/2))
	{
			angle+=GIMBAL_YAW_CIRCULAR_STEP;
	}

	//last_angle = Vision_y_angle.aver_num;
	//average_add(AUTO_MF, angle);

	speed = (-last_raw_angle + angle);


    return speed;
}


float get_auto_accel(moving_Average_Filter *AUTO_MF ,float speed)
{
	 float accel;

   // average_add(AUTO_MF, speed);

    accel = (-(AUTO_MF->aver_num) + speed);

    return accel;
}


float now_yaw, now_pitch;							//���㿪ʼʱ����̨�Ƕ�ֵ
float now_imu_pitch, target_imu_pitch;

float last_yaw_angle_raw = 0;
float yaw_angle_raw, pitch_angle_raw;//���ԽǶ�ԭʼֵ���˲�ֵ
float auto_yaw_angle, auto_pitch_angle, auto_distance;

float yaw_speed_raw, pitch_speed_raw;//�����ٶ�ԭʼֵ���˲�ֵ
float auto_yaw_speed, auto_pitch_speed, auto_distance_speed;

float yaw_accel_raw, pitch_accel_raw;			//���Լ��ٶ�ԭʼֵ���˲�ֵ
float auto_yaw_accel, auto_pitch_accel, auto_distance_accel;

float distance_raw, distance_speed_raw, distance_accel_raw;

//dis��speed kp�����ڿ�����ʱ����80�͹��ˣ�Զ��Ӧ��Ҫ��һ��
float auto_distance_speed_kp = 80, auto_distance_accel_kp = 0;
float auto_pitch_speed_kp = 0, auto_pitch_accel_kp = 0;
float auto_yaw_speed_kp = -0, auto_yaw_accel_kp = 0;	//�ٶ�����ٶȵ�Ԥ�����

float predict_err_yaw = 0, predict_err_pitch = -240;//�Ƕ�ƫ����

float feed_pre_angle_yaw, predict_angle_yaw;	//Ԥ��ƫ����
float feed_pre_angle_pitch, predict_angle_pitch;
float feed_pre_distance, predict_distance;

//������kp�ú�ʵ�ʵĽǶȱ���һ��������pitch ����ᳬ�������������о������У������������Ԥ����ܾͲ�׼��
float vision_y_kp = 33.5f, vision_p_kp = 22.755f;

uint32_t times_before_pre = 0;//δ�������飬����Ŀ�궪ʧ����0
uint32_t times_before = 5;	//130 //times_before_pre����times_beforeֵʱ����Ԥ��
float last_angle;
int32_t time_ms;
uint32_t target_lost_flag = 1;

float Auto_yaw_speed_offset, Auto_pitch_speed_offset;
float Auto_yaw_accel_offset, Auto_pitch_accel_offset;
float distance_offset = 0;
float dis_to_pitch_offset = 0;
int32_t vision_lost_flag = 0;
uint8_t temp_i = 12;//��ͼ15������ͼ18
int temp_i2 = 0;//����ͼ+ǰ��֡ȡ15�� +ǰһ֡ȡ22
int temp_ms_zhen = 0;
float temp_yaw_zhen;
int temp_i3 = 0;

float Anti_top_pre_kp = 1.0f;
void VISION_PREDICT(void)
{
	
	static uint32_t blank_times = 0;
	uint8_t i = 0;
	int n_num = 0;
	
	now_yaw = gimbal_imu_yaw_angle_sum;
	now_pitch = gimbal_motor_pid[GIMBAL_M_PITCH].angle.measure;
	now_imu_pitch = (gimbal.dev->gimbal_imu_sensor->pitch);

	average_add(&gimbal_now_yaw_raw, now_yaw);
	average_add(&gimbal_now_pitch_raw, now_pitch);
	
	if(gimbal.info->local_mode != GIMBAL_MODE_AUTO)//����������ʱ��Ҳ����Ԥ��
	{
		//times_before_pre = 0;
		//predict_angle_yaw = now_yaw;
		//predict_angle_pitch = now_pitch;
	}
	
	if(vision_sensor.info->flag == 1)//ʶ�𵽵����
	{
		
		vision_sensor.info->flag = 0;//������ձ�־������û�н��յ�ʱ��������
		
		if(vision_sensor.info->zhen<40 && 0)//֡��С��40������ʧ
		{
			target_lost_flag = 1;
		}
		else
		{
			times_before_pre++;
		}
		
		//һ�����޸�time_ms
		time_ms = (int)Vision_get_interval(target_lost_flag);

		temp_ms_zhen = average_get(&zhen_time_ms, 2) + average_get(&zhen_time_ms, 3) + average_get(&zhen_time_ms, 1);
		
		yaw_angle_raw = (-vision_sensor.info->yaw_angle)*vision_y_kp + average_get(&yaw_raw_ms, temp_ms_zhen+temp_i2);//��ȡǰһ��֡����yaw��
		pitch_angle_raw = (-vision_sensor.info->pitch_angle)*vision_p_kp + average_get(&pitch_raw_ms, temp_ms_zhen+5);
		target_imu_pitch = now_imu_pitch + (-vision_sensor.info->pitch_angle);
		//����ʱ��Ҫ�����ԽǶ��Ƿ��б仯����������£�Ŀ�겻����������Ľ��Ӧ���ǲ����
		//���ԽǶ��˲�
		
		
		if(target_lost_flag >= 1)//��ʧ����װ��ֵ������Ƕ�ͻ��
		{
				for(i = 0; i<vision_angleY_MF.lenth; i++)
				{
						average_add(&vision_angleY_MF, yaw_angle_raw);
						
				}
				for(i = 0; i<vision_angleP_MF.lenth; i++)
				{
					average_add(&vision_angleP_MF, pitch_angle_raw);
				}
				last_angle = yaw_angle_raw;
				
				for(i = 0; i<25; i++)//�������˲�
				{
					KalmanFilter(&vision_angleY_KF, vision_angleY_MF.aver_num);
				}
		}
		average_add(&vision_angleY_MF, yaw_angle_raw);
		average_add(&vision_angleP_MF, pitch_angle_raw);
		
		
		auto_pitch_angle = vision_angleP_MF.aver_num;
		
		yaw_speed_raw =  get_auto_speed(&vision_angleY_MF, last_angle)/time_ms;//���˲������ٶ�
		//yaw_speed_raw =  get_auto_speed_V2(yaw_angle_raw)/time_ms;//�����ٶ����˲��������˲��ӳٵ���
		pitch_speed_raw = get_auto_speed(&vision_angleP_MF, auto_pitch_angle)/time_ms;
		
		auto_yaw_angle = KalmanFilter(&vision_angleY_KF, vision_angleY_MF.aver_num);
		
		//����ٶ�ֵ����Ҫ��һ����ͨ�˲�����ֹС��������
		yaw_speed_raw = LPF_add(&vision_speedY_LPF, yaw_speed_raw);
		pitch_speed_raw = LPF_add(&vision_speedP_LPF, pitch_speed_raw);
		
		
		Predict_Anti_Top_binary_update(auto_yaw_angle);
		//�ж��Ƿ�����, ��������ˣ��͸����������±߽�
		if(vision_speedY_LPF.High_flag)
		{
			Predict_Anti_Top_Cal_all(last_angle, yaw_angle_raw);
		}
		last_angle = auto_yaw_angle;
		
		if(target_lost_flag >= 1)//��ʧ����װ��0�������ٶ�ͻ��
		{
				for(i = 0; i<vision_speedY_MF.lenth; i++)
				{
						average_add(&vision_speedY_MF, 0);
				}
				for(i = 0; i<vision_speedP_MF.lenth; i++)
				{
						average_add(&vision_speedP_MF, 0);
				}
		}
		
		average_add(&vision_speedY_MF, yaw_speed_raw);
		average_add(&vision_speedP_MF, pitch_speed_raw);
		
		auto_yaw_speed = KalmanFilter(&vision_speedY_KF, vision_speedY_MF.aver_num);
		auto_pitch_speed = KalmanFilter(&vision_speedP_KF, vision_speedP_MF.aver_num);
		
		//���ٶ���̫С�ˣ���100���Ŵ�һ��
		yaw_accel_raw = 100*get_auto_accel(&vision_speedY_MF, auto_yaw_speed)/time_ms;
		pitch_accel_raw = 100*get_auto_accel(&vision_speedP_MF, auto_pitch_speed)/time_ms;
		
		if(target_lost_flag >= 1)//��ʧ����װ��ֵ�������ٶ�ͻ��
		{
				for(i = 0; i<vision_accelY_MF.lenth; i++)
				{
						average_add(&vision_accelY_MF, 0);
				}
				for(i = 0; i<vision_accelY_MF.lenth; i++)
				{
						average_add(&vision_accelP_MF, 0);
				}
		}
		average_add(&vision_accelY_MF, yaw_accel_raw);
		average_add(&vision_accelP_MF, pitch_accel_raw);
		
		auto_yaw_accel = KalmanFilter(&vision_accelY_KF, vision_accelY_MF.aver_num);
		auto_pitch_accel = KalmanFilter(&vision_accelP_KF, vision_accelP_MF.aver_num);
		
		target_lost_flag = 0;
		vision_lost_flag = 0;
		
		
		//���������õ�ʵ�ʾ���
		distance_raw = vision_sensor.info->distance;//����
		if(target_lost_flag >= 1)//��ʧ����װ��ֵ�������ٶ�ͻ��
		{
				for(i = 0; i<vision_distance_MF.lenth; i++)
				{
						average_add(&vision_distance_MF, distance_raw);
				}
		}
		average_add(&vision_distance_MF, distance_raw);
		auto_distance = KalmanFilter(&vision_distance_KF, vision_distance_MF.aver_num);
		distance_speed_raw = get_auto_speed(&vision_distance_MF, distance_raw)/time_ms;//����仯�ٶ�
		if(target_lost_flag >= 1)//��ʧ����װ��ֵ��������ٶ�ͻ��
		{
				for(i = 0; i<vision_speedY_MF.lenth; i++)
				{
						average_add(&vision_speedD_MF, 0);
				}
		}
		average_add(&vision_speedD_MF, distance_speed_raw);
		
		auto_distance_speed = KalmanFilter(&vision_speedD_KF, vision_speedD_MF.aver_num);
		distance_accel_raw = get_auto_accel(&vision_speedD_MF, distance_speed_raw)/time_ms;//Զ����ٶ�
		
		predict_distance = auto_distance;
		

	}
	else if(vision_sensor.info->flag == 2 || (vision_sensor.info->flag == 0 && vision_sensor.info->identify_target == 1))//ʶ����¼��
	{
		
			vision_lost_flag++;
		
			if(vision_lost_flag>200)//ʧ��
			{
				vision_sensor.info->identify_target = 0;
			}
			average_add(&vision_distance_MF, distance_raw);
		
	}
	else if(vision_sensor.info->flag == 0 && vision_sensor.info->identify_target == 0)//Ŀ�궪ʧ
	{
		average_clear(&vision_accelY_MF);//���
		average_clear(&vision_speedY_MF);
		average_clear(&vision_accelP_MF);
		average_clear(&vision_speedP_MF);
		//KalmanFilter(&vision_angleY_KF, now_yaw);//��ʧ��һֱ���ڸ��浱ǰ�Ƕ�
		//KalmanFilter(&vision_angleP_KF, now_pitch);
		KalmanClear(&vision_speedY_KF);//��ʧ������ٶȿ������˲�������
		LPF_Clear(&vision_speedY_LPF);//��ͨ�˲�����
		LPF_Clear(&vision_speedP_LPF);
		//KalmanFilter(&vision_dis_pitch_offset, 60.0f);//�����»������һ��
		//auto_pitch_angle = KalmanFilter(&vision_angleP_KF, now_pitch);//��ʧ��һֱ���ڸ��浱ǰ�Ƕ�
		
		times_before_pre = 0;
		target_lost_flag += 1;
	}
	
	predict_angle_yaw = auto_yaw_angle;//ÿʱ���£����ԽǶ�,�������൱��ÿ��ֻ����һ�ξ���
	predict_angle_pitch = auto_pitch_angle;//ÿʱ���£����ԽǶ�
	
	
	
	//���ݵ����ٶȣ�Ҳ����˵�ǵ������ʱ�䣩�޸�����Ԥ�����Ĵ�С
	if(Friction_speed <4000)
	{
		
		if(auto_distance>4000)
		{
			auto_distance = 4000;
		}
		
		auto_yaw_speed_kp = (auto_distance * -0.1124f-244.67f)*1.5f;//9m/s ��ʽ
		auto_yaw_speed_kp *= 0.4f;
		
		
	}
	else if(Friction_speed >4000)
	{
		auto_yaw_speed_kp = auto_distance * -0.1124f-244.67f;//14m/s ��ʽ
		auto_yaw_speed_kp *= 0.5f;
		
		//auto_yaw_speed_kp = (auto_distance * -0.1124f-244.67f)*0.6f;//���ĸ�׼һ��
		//auto_yaw_speed_kp *= 0.4f;
	}
	
	Auto_yaw_speed_offset = auto_yaw_speed_kp *auto_yaw_speed;
	Auto_yaw_accel_offset = auto_yaw_accel*auto_yaw_accel_kp;
	Auto_pitch_speed_offset = auto_pitch_speed*auto_pitch_speed_kp;
	Auto_pitch_accel_offset = auto_pitch_accel*auto_pitch_accel_kp;
	
	//ֻ������Ƚ��ȶ���ʱ��ż�Ԥ��
	if(times_before_pre>times_before)
	{
		//feed_pre_distance = auto_distance_speed_kp*auto_distance_speed; 
		feed_pre_distance = 0;
		
		//Ԥ��б��
		feed_pre_angle_yaw = RampFloat(feed_pre_angle_yaw, Auto_yaw_speed_offset + Auto_yaw_accel_offset, 3);
		feed_pre_angle_pitch =  RampFloat(feed_pre_angle_pitch, Auto_pitch_speed_offset + Auto_pitch_accel_offset,3);//��ǰԤ���
		//Ԥ�ⲻб��
		//feed_pre_angle_yaw = Auto_yaw_speed_offset + Auto_yaw_accel_offset;
		//feed_pre_angle_pitch = Auto_pitch_speed_offset + Auto_pitch_accel_offset;//��ǰԤ���
		
		feed_pre_angle_yaw = Auto_yaw_speed_offset + Auto_yaw_accel_offset;
		feed_pre_angle_pitch = Auto_pitch_speed_offset + Auto_pitch_accel_offset;
		
		if(feed_pre_angle_yaw > 300)//Ԥ�����޷�
		{
			feed_pre_angle_yaw = 300;
		}
		else if(feed_pre_angle_yaw < -300)
		{
			feed_pre_angle_yaw = -300;
		}
	}
	else 
	{
		feed_pre_angle_yaw = 0.0f*(Auto_yaw_speed_offset + Auto_yaw_accel_offset);
		feed_pre_angle_pitch = 0.0f*(Auto_pitch_speed_offset + Auto_pitch_accel_offset);
		feed_pre_distance = 0;
	}
	
	//���ݶԷ�Զ�����ǵ��ٶȽ���һ������Ԥ��
	
	if(predict_distance+feed_pre_distance<1500)
	{
		predict_distance = predict_distance;
	}
	else 
	{
		predict_distance+=feed_pre_distance;
	}		
	
	//̧ͷ��
	//��������ѡ��ͬ�ľ��븩������������
	//�����ٵ�ʱ����ĸ����Ǳ仯�ϴ󣬸����ٵ�ʱ�����Ǳ仯��С
	//���ݶ���Ҫ��������ʣ�10m/s�������ޣ�ʵ������ٶ�9.2��0.5m/s������������²�Ӧ�ô���4m. 
	//�ڴ���4m��ʱ�����ٶԸ����ǵ�Ӱ��ǳ���ĿǰĦ�����Լ�ǹ�����������ڡ�0.5m/s�ľ��ȣ�ϣ���д����
	//�������������ڵ����ߵ�ԭ�򣬸õ������ڴ���5m�����ˮƽ����Ŀ�꣬װ�װ��Ѿ������Ӿ���Ұ���·���
	
	//���ݶ���Ҫ��������ʣ�16m/s�������ޣ�ʵ������ٶ�15.0��0.5m/s��
	//����������¿��Դﵽ�Ӿ�ʶ��������ޣ���Ϊ����̫С��ʶ�𲻵�����������Ϊ10m��ʵ�ʾ���Ϊ9m������������Ϊ80%
	
	//�ر�ע����ǣ��ڹ���������ʱ�򣬸����Ǳ���Ӵ󣬷���ᱻ���������Լ�
	dis_to_pitch_offset = predict_cal_shoot_angle(predict_distance, predict_angle_pitch, Friction_speed);
	KalmanFilter(&vision_dis_pitch_offset, dis_to_pitch_offset);
	
	if(vision_dis_pitch_offset.X_now>180)//̧ͷ���������180
	{
			predict_angle_pitch = KalmanFilter(&vision_absolute_Pitch_Kal, predict_angle_pitch+feed_pre_angle_pitch) + 180 + predict_err_pitch;
	}
	else
	{
			predict_angle_pitch = KalmanFilter(&vision_absolute_Pitch_Kal, predict_angle_pitch+feed_pre_angle_pitch) + vision_dis_pitch_offset.X_now + predict_err_pitch;
	}
	
	
	
	
	//predict_angle_yaw = Predict_Anti_Top_Judge_Yaw(predict_angle_yaw);
	if(IF_KEY_PRESSED_C)
	{
		if(ANTI_TOP_TEST_FLAG == 1)
		{
			//��ͨԤ��
			predict_angle_yaw = predict_angle_yaw+feed_pre_angle_yaw;
			predict_angle_yaw =  KalmanFilter(&vision_absolute_Yaw_Kal, predict_angle_yaw) + predict_err_yaw;	
	
		}
		else
		{
			//������
			predict_angle_yaw = predict_angle_yaw+feed_pre_angle_yaw*Anti_top_pre_kp;
			predict_angle_yaw =  KalmanFilter(&vision_absolute_Yaw_Kal, predict_angle_yaw) + predict_err_yaw;	
	
			predict_angle_yaw = Predict_Anti_Top_Judge_Yaw(predict_angle_yaw);
		}
			
	}
	else 
	{
		if(ANTI_TOP_TEST_FLAG == 1)
		{
			//������
			predict_angle_yaw = predict_angle_yaw+feed_pre_angle_yaw*Anti_top_pre_kp;
			predict_angle_yaw =  KalmanFilter(&vision_absolute_Yaw_Kal, predict_angle_yaw) + predict_err_yaw;	
	
			predict_angle_yaw = Predict_Anti_Top_Judge_Yaw(predict_angle_yaw);
		}
		else
		{
			//��ͨԤ��
			predict_angle_yaw = predict_angle_yaw+feed_pre_angle_yaw;
			predict_angle_yaw =  KalmanFilter(&vision_absolute_Yaw_Kal, predict_angle_yaw) + predict_err_yaw;	
	
		}
		//predict_angle_yaw = Predict_Anti_Top_Judge_Yaw(predict_angle_yaw);
	}
	
	
	
	//predict_angle_yaw = constrain(predict_angle_yaw, yaw_angle_raw-300, yaw_angle_raw+300);
	//ȡ��predict��С�����ܳ��Ĳ���
	if(predict_angle_yaw>(GIMBAL_YAW_CIRCULAR_STEP/2))
	{
		n_num =(int)(predict_angle_yaw/(GIMBAL_YAW_CIRCULAR_STEP));//nȦ
		
		predict_angle_yaw = predict_angle_yaw-n_num*(GIMBAL_YAW_CIRCULAR_STEP);
		if(predict_angle_yaw>GIMBAL_YAW_CIRCULAR_STEP/2)
		{
			predict_angle_yaw -= GIMBAL_YAW_CIRCULAR_STEP;
		}
	}
	else if(predict_angle_yaw<-(GIMBAL_YAW_CIRCULAR_STEP/2))
	{
		n_num = (int)(predict_angle_yaw/(GIMBAL_YAW_CIRCULAR_STEP));
		predict_angle_yaw = predict_angle_yaw - n_num*(GIMBAL_YAW_CIRCULAR_STEP);
		if(predict_angle_yaw<-(GIMBAL_YAW_CIRCULAR_STEP/2))
		{
			predict_angle_yaw += GIMBAL_YAW_CIRCULAR_STEP;
		}
	}
	
}



float AntiTop_angle_yaw;	//Ԥ��ƫ����
float AntiTop_angle_pitch;
float AntiTop_distance;
float Anti_dis_to_pitch_offset;
uint16_t pre_temp = 0;
float last_yaw_angle_raw_temp;
float top_moving_speed = 0;
float top_moving_speed_kp = 1;
void vision_anti_top_analyze(void)
{
	int n_num = 0;
	
	
	last_yaw_angle_raw_temp = Absolute_yaw_angle_raw.aver_num;
	MAF_ANTI_TOP_add(&Absolute_yaw_angle_raw, auto_yaw_angle);//���������д���õ�yaw�����ݣ�����һ����¼��ȥyaw����Ϣ������
	MAF_ANTI_TOP_add(&Absolute_pitch_angle_raw, auto_pitch_angle);
	MAF_ANTI_TOP_add(&Absolute_distance_raw, auto_distance);
	median_add(&Anti_top_yaw_MedianF, yaw_angle_raw);
	
	//����ֵ
	//AntiTop_angle_yaw = (Anti_top_yaw_MedianF.data_num[Anti_top_yaw_MedianF.lenth-1] + Anti_top_yaw_MedianF.data_num[0])/2;
	
	//���ֵ
	//AntiTop_angle_yaw = Absolute_yaw_angle_raw.aver_num;
	//AntiTop_angle_pitch = predict_angle_pitch;
	//AntiTop_distance = Absolute_distance_raw.aver_num;
	
	if(AntiTop_angle_yaw>(GIMBAL_YAW_CIRCULAR_STEP/2))
	{
		n_num =(int)(AntiTop_angle_yaw/(GIMBAL_YAW_CIRCULAR_STEP));//nȦ
		
		AntiTop_angle_yaw = AntiTop_angle_yaw-n_num*(GIMBAL_YAW_CIRCULAR_STEP);
		if(AntiTop_angle_yaw>GIMBAL_YAW_CIRCULAR_STEP/2)
		{
			AntiTop_angle_yaw -= GIMBAL_YAW_CIRCULAR_STEP;
		}
	}
	else if(AntiTop_angle_yaw<-(GIMBAL_YAW_CIRCULAR_STEP/2))
	{
		n_num = (int)(AntiTop_angle_yaw/(GIMBAL_YAW_CIRCULAR_STEP));
		AntiTop_angle_yaw = AntiTop_angle_yaw - n_num*(GIMBAL_YAW_CIRCULAR_STEP);
		if(AntiTop_angle_yaw<-(GIMBAL_YAW_CIRCULAR_STEP/2))
		{
			AntiTop_angle_yaw += GIMBAL_YAW_CIRCULAR_STEP;
		}
	}
	
	//̧ͷ��
	if(Friction_speed <5200)
	{
		if(AntiTop_distance>2000)
		{
			Anti_dis_to_pitch_offset = 0.0316f*(AntiTop_distance)+50.719f;
		}
		else if(AntiTop_distance>1000)
		{
			if(abs(Anti_dis_to_pitch_offset-170)<0.1f)
			{
				if(AntiTop_distance<1000+400)
				{
					Anti_dis_to_pitch_offset = 110;
				}
				else
				{
					Anti_dis_to_pitch_offset = 170;
				}
			}
			else 
			{
				Anti_dis_to_pitch_offset = 110;
			}
			
		}
		else if(AntiTop_distance<1000)
		{
			if(abs(Anti_dis_to_pitch_offset-110)<0.1f)//110��ʱ��
			{
				if(AntiTop_distance<1000-400)//ֻ�е���800�Ž���170
				{
					Anti_dis_to_pitch_offset = 170;
				}
				else
				{
					Anti_dis_to_pitch_offset = 110;
				}
			}
			else
			{
				Anti_dis_to_pitch_offset = 170;
			}
		}
	
		KalmanFilter(&AntiTop_dis_pitch_offset, Anti_dis_to_pitch_offset);//���빫ʽ9.2m/s����
	}
	else if(Friction_speed > 5200)
	{
		
		if(AntiTop_distance>800)
		{
			if(abs(Anti_dis_to_pitch_offset-200)<0.1f)//200�����
			{
				if(AntiTop_distance>800)
				{
					Anti_dis_to_pitch_offset = (7*AntiTop_distance*AntiTop_distance + 43000*AntiTop_distance)/10000000.0f;//���빫ʽ14.0m/s
					Anti_dis_to_pitch_offset += 58.904f;
				}
				else
				{
					Anti_dis_to_pitch_offset = 200;
				}
			}
			else 
			{
				Anti_dis_to_pitch_offset = (7*AntiTop_distance*AntiTop_distance + 43000*AntiTop_distance)/10000000.0f;//���빫ʽ14.0m/s
				Anti_dis_to_pitch_offset += 58.904f;
			}
		}
		else if(AntiTop_distance<800)
		{
			if(AntiTop_distance<800-300)
			{
				Anti_dis_to_pitch_offset = 200;
			}
		}
		Anti_dis_to_pitch_offset-=6;
		KalmanFilter(&AntiTop_dis_pitch_offset, Anti_dis_to_pitch_offset);
	}
	
	
	AntiTop_angle_pitch+=AntiTop_dis_pitch_offset.X_now;
}

float yaw_speed_offset_limit = 100, yaw_accel_offset_limit = 20;
float anti_top_limit = 50;
float anti_top_err = 0;
int anti_top_aim_pos = 0;
void GIMBAL_AUTO(void)
{
	//anti_top_flag = 0;
	
	int distance_dm = 0;
	float aim_yaw, aim_pitch;

	
	if(IF_KEY_PRESSED_X)
	{
		launcher.info->fire_permit_flag = 1;
		//TargetYaw = AntiTop_angle_yaw;
		//TargetPitch = Absolute_pitch_angle_raw.aver_num;//��ֵ�߶�
		TargetPitch = predict_angle_pitch;//���ڸ߶�
		
		//�������Լ���׼����
		TargetYaw = -(float)(rc_move_info.mouse_vx)*gimbal_mouse_vy;
		TargetYaw = TargetYaw + gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target;
		
		launcher.info->fire_permit_flag = 1;
		
		if(TargetYaw>(GIMBAL_YAW_CIRCULAR_STEP/2))
		{
			TargetYaw -= GIMBAL_YAW_CIRCULAR_STEP;
		}
		else if(TargetYaw<=-(GIMBAL_YAW_CIRCULAR_STEP/2))
		{
			TargetYaw += GIMBAL_YAW_CIRCULAR_STEP;
		}

		if(target_lost_flag<1)
		{
			
			aim_pitch = constrain(TargetPitch, GIMBAL_PITCH_MIN, GIMBAL_PITCH_MAX);
			//gimbal_motor_pid[GIMBAL_M_PITCH].angle.target = RampFloat(aim_pitch, gimbal_motor_pid[GIMBAL_M_PITCH].angle.target, 3);
			gimbal_motor_pid[GIMBAL_M_PITCH].angle.target = aim_pitch;
		}
		aim_yaw = constrain(TargetYaw, -GIMBAL_YAW_CIRCULAR_STEP/2, GIMBAL_YAW_CIRCULAR_STEP/2);
		gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target =	aim_yaw;

	}
	else 
	{
		launcher.info->fire_permit_flag = 1;
		
		TargetYaw = predict_angle_yaw;
		TargetPitch = predict_angle_pitch;
		
		
		launcher.info->fire_permit_flag = 1;
		
		if(TargetYaw>(GIMBAL_YAW_CIRCULAR_STEP/2))
		{
			TargetYaw -= GIMBAL_YAW_CIRCULAR_STEP;
		}
		else if(TargetYaw<=-(GIMBAL_YAW_CIRCULAR_STEP/2))
		{
			TargetYaw += GIMBAL_YAW_CIRCULAR_STEP;
		}
		
		if(target_lost_flag<1)
		{
			aim_yaw = constrain(TargetYaw, -GIMBAL_YAW_CIRCULAR_STEP/2, GIMBAL_YAW_CIRCULAR_STEP/2);
			aim_pitch = constrain(TargetPitch, GIMBAL_PITCH_MIN, GIMBAL_PITCH_MAX);
			
//			gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target =	RampFloat(aim_yaw, gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target, 3);
//			gimbal_motor_pid[GIMBAL_M_PITCH].angle.target = RampFloat(aim_pitch, gimbal_motor_pid[GIMBAL_M_PITCH].angle.target, 3);	
			gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target = aim_yaw;
			
			if(IF_KEY_PRESSED_G)//������pitchĿ��
			{

			}
			else//����pitchĿ��
			{
				gimbal_motor_pid[GIMBAL_M_PITCH].angle.target = aim_pitch;
			}
		}
	}
	
	TargetPitch	= gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target;
	TargetYaw = gimbal_motor_pid[GIMBAL_M_PITCH].angle.target;
}


void GIMBAL_ReloadBulletCtrl(void)
{
}

void GIMBAL_SzuPupCtrl(void)
{
}

void GIMBAL_LONGSHOTCtrl(void)
{
}

void GIMBAL_RcCtrl(void)
{
	float pre_target_pitch;
	launcher.info->fire_permit_flag = 1;
	
	if(gimbal.info->co_mode == CO_MECH)
	{
		
		TargetPitch = (float)(gimbal.dev->rc_sensor->info->ch1)*0.017f;
		TargetYaw = 0;
		
		gimbal_motor_pid[GIMBAL_YAW].angle.target = TargetYaw + gimbal_motor_pid[GIMBAL_YAW].angle.target;
		
		TargetPitch = gimbal_motor_pid[GIMBAL_M_PITCH].angle.target - TargetPitch;
		gimbal_motor_pid[GIMBAL_M_PITCH].angle.target = constrain(TargetPitch, GIMBAL_PITCH_MIN, GIMBAL_PITCH_MAX);
		
	}
	else if(gimbal.info->co_mode == CO_GYRO)//
	{
		if(sys.mode != SYS_MODE_AUTO)
		{
			TargetPitch = (float)(gimbal.dev->rc_sensor->info->ch1)*0.017f;
			TargetYaw = -(float)(gimbal.dev->rc_sensor->info->ch0)*0.034f;
			
			
			TargetYaw = TargetYaw + gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target;
			if(TargetYaw>(GIMBAL_YAW_CIRCULAR_STEP/2))
			{
				TargetYaw -= GIMBAL_YAW_CIRCULAR_STEP;
			}
			else if(TargetYaw<-(GIMBAL_YAW_CIRCULAR_STEP/2))
			{
				TargetYaw += GIMBAL_YAW_CIRCULAR_STEP;
			}
			//gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target = constrain(TargetYaw, -GIMBAL_YAW_CIRCULAR_STEP/2, GIMBAL_YAW_CIRCULAR_STEP/2);
			gimbal_motor_pid[GIMBAL_IMU_YAW].angle.target = TargetYaw;
			pre_target_pitch = gimbal_motor_pid[GIMBAL_M_PITCH].angle.target;
			pre_target_pitch = pre_target_pitch - TargetPitch;
			gimbal_motor_pid[GIMBAL_M_PITCH].angle.target = constrain(pre_target_pitch, GIMBAL_PITCH_MIN, GIMBAL_PITCH_MAX);
		}

	}
	
	TargetYaw = 0;
	TargetPitch = 0;
}

void GIMBAL_KeyCtrl(void)
{
	
	switch(gimbal_info.local_mode)
	{
		case GIMBAL_MODE_NORMAL:
			GIMBAL_NormalCtrl();
			break;
		case GIMBAL_MODE_AUTO:
			GIMBAL_AUTO();
			break;
		case GIMBAL_MODE_LONGSHOOT:
			GIMBAL_LONGSHOTCtrl();
			break;
		case GIMBAL_MODE_RELOAD_BULLET:
			GIMBAL_ReloadBulletCtrl();
			break;
		case GIMBAL_MODE_SZUPUP:
			GIMBAL_SzuPupCtrl();
			break;
		default:
			break;
	}
}



void GIMBAL_Angle_deliver(void)
{
	
	
	
	
	
	
}

void GIMBAL_Ctrl(void)
{
	/*----��Ϣ����----*/
	GIMBAL_GetInfo();
	/*----�����޸�----*/ 
	if(gimbal_info.remote_mode == RC) {
		GIMBAL_RcCtrl();
	}
	else if(gimbal_info.remote_mode == KEY) {
		GIMBAL_KeyCtrl();
	}
	GIMBAL_Angle_deliver();
	/*----�������----*/
	GIMBAL_PidCtrl();	
}

void GIMBAL_Test(void)
{
	
	/*----��Ϣ����----*/
	GIMBAL_GetInfo();
	/*----�����޸�----*/ 
	if(gimbal_info.remote_mode == RC) {
		GIMBAL_RcCtrl();
	}
	else if(gimbal_info.remote_mode == KEY) {
		GIMBAL_AUTO();
	}
	GIMBAL_Angle_deliver();
	/*----�������----*/
	GIMBAL_PidCtrl();	
	
}


void gimbal_now_angle_ms_update()
{	
	float now_ms_yaw, now_ms_pitch;
	now_ms_yaw = gimbal_imu_yaw_angle_sum;
	now_ms_pitch = gimbal_motor_pid[GIMBAL_M_PITCH].angle.measure;
	
	average_add(&yaw_raw_ms, now_yaw);
	average_add(&pitch_raw_ms, now_pitch);

}



