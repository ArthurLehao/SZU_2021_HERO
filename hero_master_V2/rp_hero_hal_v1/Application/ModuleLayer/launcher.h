#ifndef __LAUNCHER_H
#define __LAUNCHER_H

/* Includes ------------------------------------------------------------------*/
#include "rp_config.h"
#include "launcher_motor.h"
#include "rc_sensor.h"
#include "driver.h"

#define re_dial_times 0 
#define MAGEZINE_OPEM_TIME 1000

#define SAFE_SPIN_TIME (120)
#define	SAFE_SPIN_SPEED (-12000)
#define DIAL_WARNING_OUTPUT (8000)

#define FIRE_FASTER_ABILITY 0
 
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
  * @brief   ���䱾��ģʽ���� 
  * @note    ����ϵͳģʽ�Ĳ�ͬ���������ò�ͬ�ķ��䱾��ģʽ
  * @author  RobotPilots
  */
typedef enum {
	LAUNCHER_MODE_NORMAL 		= 0, // ����ģʽ
	LAUNCHER_MODE_COUNT   		= 1, // ģʽ����
} launcher_mode_t;

typedef enum{
	MGZ_OFF,
	MGZ_ON,
} launcher_mgz_flag_t;

typedef enum{
	Friction_OFF,
	Friction_ON,
} launcher_friction_flag_t;

/**
  * @brief ������pid�е�target����
  * @note 
  * @author  RobotPilots  
*/
typedef struct {
	pid_ctrl_t	speed;
	pid_ctrl_t	angle;
	float		out;
} launcher_motor_pid_t;


/**
  * @brief 		launcher_ctrl_t ���̿�������
  * @note 		�ṹ������ָ����̵��������PID��ָ��
							�Լ������˶�ʱz������ת��PIDָ��
	* @program	launcher_motor_pid_t  	ָ�����ٶȻ��ͽǶȻ��Լ�����������ָ��
  * @author	  RobotPilots  
*/
typedef struct {
	launcher_motor_pid_t		(*motor)[LAUNCHER_MOTOR_CNT];
} launcher_ctrl_t;

/**
  * @brief 		launcher_dev_t ������������
  * @note 		���Ի�ÿ���ʱ����Ҫ�ĸ��ഫ����Ϣ
	* @program	lucher_motor  	ָ�򵥸�������ݵ�ָ��
							yaw_motor			ָ����̨������ݵ�ָ��
							imu_sensor		ָ��IMU��������ָ��
							rc_sensor			ָ��ң������������ָ��	
  * @author 	RobotPilots  
*/
typedef struct {
	launcher_motor_t	*launcher_motor[LAUNCHER_MOTOR_CNT];
	rc_sensor_t		*rc_sensor;
} launcher_dev_t;


/**
  * @brief		launcher_info_t  ������Ϣ����
  * @note			��¼�˵�ǰ���̵Ĺ���ģʽ�Լ��߼���λ
  * @program	remote_mode			ң��ģʽ������ȷ������������Դ
							local_mode			����ģʽ������ȷ�������˶�ģʽ
							co_angle_logic	��̨�������־
							top_gyro				С����ģʽ��־λ
  * @author  RobotPilots  
*/
typedef struct {
	remote_mode_t		remote_mode;
	co_mode_t		co_mode;
	launcher_mode_t		local_mode;
	launcher_mgz_flag_t launcher_mgz_flag;
	launcher_friction_flag_t launcher_friction_flag;
	bool fire_permit_flag;
}launcher_info_t;


/**
  * @brief		launcher_t  ��������
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
typedef struct launcher{
	launcher_ctrl_t	*controller;		//ָ��chassis_ctrl_t ���̿��������� ���͵�ָ��
	launcher_dev_t	*dev;							//���̿�����ص�������Ϣ
	launcher_info_t	*info;					//��ϵͳģʽ���������ı���ģʽ
	bool			test_open;						//�����ú���
	void			(*init)(void);
	void			(*update)(void);
	void			(*test)(void);
	void			(*ctrl)(void);
	void			(*output)(void);
	void			(*self_protect)(void);
	bool			(*if_friction_on)(void);
	bool			(*if_dud)(void);
}launcher_t;

extern launcher_t launcher;
extern uint8_t slaver_photogate, slaver_photogate_flag;
extern int Friction_speed;
extern int Fric_3508_speed[6];
extern uint8_t dial_empty_flag;

/* Exported functions --------------------------------------------------------*/
/* ��Ϣ�� --------------------------------------------------------------------*/
bool LAUNCHER_IfFriction_On(void);

#endif
