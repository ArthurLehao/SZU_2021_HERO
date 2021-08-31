#ifndef __GIMBAL_H
#define __GIMBAL_H

/* Includes ------------------------------------------------------------------*/
#include "rp_config.h"
#include "rp_math.h"
#include "chassis_motor.h"
#include "gimbal_motor.h"
#include "imu_sensor.h"
#include "rc_sensor.h"
#include "drv_io.h"
#include "predict.h"

#define GIMBAL_MECH_ANGLE_POS_MID CO_MECH_ANGLE_POS_MID
#define GIMBAL_MECH_ANGLE_NEG_MID CO_MECH_ANGLE_NEG_MID

//ANTI_TOP_TEST_FLAG = 
//1 ʱĬ�Ϸ����ݣ�������ͨԤ��
//0 ʱĬ����ͨԤ�⣬����������
#define ANTI_TOP_TEST_FLAG 0


/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
  * @brief   ��̨����ģʽ���� 
  * @note    ����ϵͳģʽ�Ĳ�ͬ���������ò�ͬ����̨����ģʽ
  * @author  RobotPilots
  */
typedef enum {
	GIMBAL_MODE_NORMAL		 			= 0, // ����ģʽ
	GIMBAL_MODE_AUTO    				= 1,
	GIMBAL_MODE_LONGSHOOT   		= 2, // Զ��ģʽ
	GIMBAL_MODE_RELOAD_BULLET		= 3, // ���̵��ٲ���ģʽ
	GIMBAL_MODE_SZUPUP					= 4, // SZU����ģʽ
} gimbal_mode_t;


/**
  * @brief ��̨���pid�е�target����
  * @note 
  * @author  RobotPilots  
*/
typedef struct {
	pid_ctrl_t	speed;
	pid_ctrl_t	angle;
	float		out;
} gimbal_motor_pid_t;

typedef struct {
	pid_ctrl_t	speed;
	pid_ctrl_t	angle;
	float 		out;
} gimbal_imu_pid_t;

/**
  * @brief 		gimbal_ctrl_t ��̨��������
  * @note 		�ṹ������ָ����̵��������PID��ָ��
							�Լ������˶�ʱz������ת��PIDָ��
	* @program	gimbal_motor_pid_t  	ָ�����ٶȻ��ͽǶȻ��Լ�����������ָ��
							gimbal_imu_pid_t				ָ��Yaw����Ƕȵ�Yawָ��
  * @author	  RobotPilots  
*/
typedef struct {
	gimbal_motor_pid_t		(*motor)[GIMBAL_MOTOR_CNT];
} gimbal_ctrl_t;

typedef struct {
	float yaw;
	float pitch;
	float roll;
	short rate_yaw;
	short rate_pitch;
	short rate_roll;
} gimbal_imu_t;

/**
  * @brief 		gimbal_dev_t ������������
  * @note 		���Ի�ÿ���ʱ����Ҫ�ĸ��ഫ����Ϣ
	* @program	gim_motor  	ָ�򵥸�������ݵ�ָ��
							imu_sensor		ָ��IMU��������ָ��
							rc_sensor			ָ��ң������������ָ��	
							gimbal_imu_sensor		imu�����������任������ߵ�����
  * @author 	RobotPilots  
*/
typedef struct {
	gimbal_motor_t	*gimbal_motor[GIMBAL_MOTOR_CNT];
	imu_sensor_t	*imu_sensor;
	rc_sensor_t		*rc_sensor;
	gimbal_imu_t  *gimbal_imu_sensor;
} gimbal_dev_t;


/**
  * @brief		gimbal_info_t  ������Ϣ����
  * @note			��¼�˵�ǰ���̵Ĺ���ģʽ�Լ��߼���λ
  * @program	remote_mode			ң��ģʽ������ȷ������������Դ
							co_mode					����ģʽ������ȷ������̨���ϵ�ģʽ
							local_mode			����ģʽ������ȷ�������˶�ģʽ
							co_angle_logic	��̨�������־
							top_gyro				С����ģʽ��־λ
  * @author  RobotPilots  
*/
typedef struct {
	remote_mode_t		remote_mode;
	co_mode_t		co_mode;
	gimbal_mode_t		local_mode;
	co_angle_logic_t	co_angle_logic; //����ͷ�����λ��
	bool				top_gyro;
	bool 				if_fire_ready;
}gimbal_info_t;


/**
  * @brief		gimbal_t  ��������
  * @note			���˴�����Ϣ�Ϳ�����Ϣ�⣬�������˺���ָ������ִ����Ӧ����
  * @program	�����������֣���Ϣ���ֿ����棩
							.init 										��ʼ��
							.update 									���̸��º���
							.test											���Ժ���������test_open����ʱ����
							.ctrl											���ƺ���
							.output										����������
							.self_protect							ʧ������
							.if_back_to_mid_angle			�Ƿ���в�ѯ
							.if_top_gyro_open					�Ƿ��������ǲ�ѯ

	* @author  RobotPilots  
*/
typedef struct gimbal{
	gimbal_ctrl_t	*controller;		//ָ��chassis_ctrl_t ���̿��������� ���͵�ָ��
	gimbal_dev_t	*dev;							//���̿�����ص�������Ϣ
	gimbal_info_t	*info;					//��ϵͳģʽ���������ı���ģʽ
	bool			test_open;						//�����ú���
	void			(*init)(void);
	void 			(*reset)(void);
	void			(*update)(void);
	void			(*test)(void);
	void			(*ctrl)(void);
	void			(*output)(void);
	void			(*self_protect)(void);
	bool			(*gimbal_imu_if_back_mid)(void);
}gimbal_t;

extern gimbal_t gimbal;
extern gimbal_imu_t gimbal_imu;
extern uint8_t anti_top_flag;
/* Exported functions --------------------------------------------------------*/
void VISION_PREDICT(void);
float GIMBAL_GET_IMU_MID_ERR(void);
/* ��Ϣ�� --------------------------------------------------------------------*/
//bool CHASSIS_IfBackToMiddleAngle(void);


void gimbal_now_angle_ms_update(void);
/* provide functions --------------------------------------------------------*/
	

#endif
