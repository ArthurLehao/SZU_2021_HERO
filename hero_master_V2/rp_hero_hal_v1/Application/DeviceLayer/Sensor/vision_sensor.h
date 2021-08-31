#ifndef RM__VISION__H
#define RM__VISION__H

#include "rp_config.h"

//��ʼ�ֽڣ�Э��̶�β0xA5
#define 	VISION_SOF					(0xA5)

//���ȸ���Э�鶨��
#define		VISION_LEN_HEADER		3			//֡ͷ��
#define 	VISION_LEN_DATA 		18    //���ݶγ��ȣ����Զ���
#define   VISION_LEN_TAIL			2			//ƵβCRC16
#define		VISION_LEN_PACKED		23		//���ݰ����ȣ����Զ���

#define 	VISION_LEN_TX_PACKED 8

#define    VISION_OFF           (0x00)
#define    VISION_RED_ATTACK    (0x01)
#define    VISION_BLUE_ATTACK   (0x02)
#define    VISION_RED_BASE      (0x03)
#define    VISION_BLUE_BASE     (0x04)
#define    VISION_RED_GETBOMB   (0x05)
#define    VISION_BLUE_GETBOMB  (0x06)


//�������뷢��ָ������бȽϣ����շ�һ��ʱ���ݿ���

//֡ͷ��CRC8У��,��֤���͵�ָ������ȷ��

//PC�շ���STM32�շ��ɾ����ϵ,���½ṹ��������STM32,PC�������޸�

typedef __packed struct
{
    //ͷ
    uint8_t		SOF;			//֡ͷ��ʼλ,�ݶ�0xA5
    uint8_t 	CmdID;		//ָ��
    uint8_t 	CRC8;			//֡ͷCRCУ��,��֤���͵�ָ������ȷ��
} extVisionSendHeader_t;


/*****************�Ӿ����հ���ʽ********************/
typedef __packed struct
{
    /*ͷ*/
    uint8_t 	SOF;			//֡ͷ��ʼλ,0xA5
    uint8_t 	CmdID;		//ָ��
    uint8_t 	CRC8;			//֡ͷCRCУ��,��֤���͵�ָ������ȷ��

    /*����*/
    float pitch_angle;
    float yaw_angle;
    float distance;
    uint8_t identify_target;// �Ƿ�ʶ��Ŀ��	��λ��0/1
    uint8_t buff_change_armor_four;	// �Ƿ��л������Ŀ�װ�װ�		��λ��0/1
    uint8_t identify_buff;	// �Ƿ�ʶ��Buff	��λ��0/1
    uint8_t identify_too_close;	// Ŀ��������	��λ��0/1
    uint8_t anti_gyro;	// �Ƿ�ʶ��С����	��λ��0/1
    uint8_t	anti_gyro_change_armor;	// �Ƿ��ڷ�����״̬���л�װ�װ�	��λ��0/1

    /*β*/
    uint16_t CRC16;
    uint8_t flag;
} extVisionRecvData_t;

typedef __packed struct vision_sensor_info_struct {
	/*ͷ*/
    uint8_t 	SOF;			//֡ͷ��ʼλ,0xA5
    uint8_t 	CmdID;		//ָ��
    uint8_t 	CRC8;			//֡ͷCRCУ��,��֤���͵�ָ������ȷ��

    /*����*/
    float pitch_angle;
    float yaw_angle;
    float distance;
    uint8_t identify_target;// �Ƿ�ʶ��Ŀ��	��λ��0/1
    uint8_t buff_change_armor_four;	// �Ƿ��л������Ŀ�װ�װ�		��λ��0/1
    uint8_t identify_buff;	// �Ƿ�ʶ��Buff	��λ��0/1
    uint8_t identify_too_close;	// Ŀ��������	��λ��0/1
    uint8_t anti_gyro;	// �Ƿ�ʶ��С����	��λ��0/1
    uint8_t	anti_gyro_change_armor;	// �Ƿ��ڷ�����״̬���л�װ�װ�	��λ��0/1

    /*β*/
    uint16_t CRC16;
    uint8_t flag;
		uint16_t zhen;
	
		int16_t		offline_cnt;
		int16_t		offline_max_cnt;
} vision_sensor_info_t;


/**
  * @brief  �����Ӿ�ָ��
  * @param  CmdID
  * @retval void
  * @attention  ��Э���������ݷ���
  *				CmdID   0x00   �ر��Ӿ�
  *				CmdID   0x01   ʶ���ɫװ��
  *				CmdID   0x02   ʶ����ɫװ��
  *				CmdID   0x03   С��
  *				CmdID   0x04   ���
  */
typedef __packed struct vision_sent_info_struct {
	/*ͷ*/
    uint8_t 	SOF;			//֡ͷ��ʼλ,0xA5
    uint8_t 	CmdID;		//ָ��
    uint8_t 	CRC8;			//֡ͷCRCУ��,��֤���͵�ָ������ȷ��

    /*����*/
    uint8_t    sentry_mode;   		//�����ڱ�ģʽ
    uint8_t		 base_far_mode;     //Զ�̵������ģʽ
    uint8_t    base_near_mode;    //���̵������ģʽ(Ҳ������ǰ��վ)
    uint8_t    fric_speed; 				//�ӵ�����(ֱ�ӷּ���)
		uint8_t    user5;   		//�����ڱ�ģʽ
    uint8_t		 user6;     //Զ�̵������ģʽ
    uint8_t    user7;    //���̵������ģʽ(Ҳ������ǰ��վ)
    uint8_t    user8; 				//�ӵ�����(ֱ�ӷּ���)
		uint8_t    user9;   		//�����ڱ�ģʽ
    uint8_t		 user10;     //Զ�̵������ģʽ
    uint8_t    user11;    //���̵������ģʽ(Ҳ������ǰ��վ)
    uint8_t    user12; 				//�ӵ�����(ֱ�ӷּ���)
		uint8_t    user13;   		//�����ڱ�ģʽ
    uint8_t		 user14;     //Զ�̵������ģʽ
    uint8_t    user15;    //���̵������ģʽ(Ҳ������ǰ��վ)
    uint8_t    user16; 				//�ӵ�����(ֱ�ӷּ���)
		uint8_t    user17;   		//�����ڱ�ģʽ
    uint8_t		 user18;     //Զ�̵������ģʽ



    /*β*/
    uint16_t CRC16;

} vision_sent_info_t;


typedef struct vision_sensor_struct {
	vision_sensor_info_t	*info;
	vision_sent_info_t		*sent_info;
	uint8_t  sent[VISION_LEN_PACKED];
	drv_uart_t			*driver;
	void				(*init)(struct vision_sensor_struct *self);
	void				(*update)(struct vision_sensor_struct *self, uint8_t *rxBuf);
	void				(*check)(struct vision_sensor_struct *self);	
	void				(*heart_beat)(struct vision_sensor_struct *self);
	void 				(*sent_self)(struct vision_sensor_struct *self, uint8_t *txBuf);
	dev_work_state_t	work_state;
	dev_errno_t			errno;
	dev_id_t			id;
} vision_sensor_t;
/*****************�Ӿ����Ͱ���ʽ********************/


extern float theta_absolute;
extern vision_sensor_t vision_sensor;

//�������д��CRCУ��ֵ
//���ǿ���ֱ�����ùٷ�����CRC����

//ע��,CRC8��CRC16��ռ�ֽڲ�һ��,8Ϊһ���ֽ�,16Ϊ2���ֽ�

//д��    CRC8 ����    Append_CRC8_Check_Sum( param1, param2)
//���� param1����д����֡ͷ���ݵ�����(֡ͷ������ݻ�ûдû�й�ϵ),
//     param2����CRC8д������ݳ���,���Ƕ������ͷ�����һλ,Ҳ����3

//д��    CRC16 ����   Append_CRC16_Check_Sum( param3, param4)
//���� param3����д����   ֡ͷ + ����  ������(��������ͬһ������)
//     param4����CRC16д������ݳ���,���Ƕ�����������ݳ�����22,������22

/*----------------------------------------------------------*/


extern uint8_t Vision_SentNum[50];
extern uint8_t Vision_attack_Color;

void Vision_Read_Data(uint8_t *ReadFormUsart);
void Vision_Send_Data(uint8_t CmdID);

void Usart1_Sent_Byte(uint8_t ch);
void Vision_Sent(void);
void Vision_Init(void);

/******************���ָ���С����*******************/
void Vision_Ctrl(void);
void Vision_Auto_Attack_Off(void);
void Vision_Auto_Attack_Ctrl(void);

void Vision_Error_Yaw(float *error);
void Vision_Error_Pitch(float *error);
void Vision_Error_Angle_Yaw(float *error);
void Vision_Error_Angle_Pitch(float *error);
void Vision_Get_Distance(float *distance);

bool Vision_IF_Updata(void);
void Vision_Clean_Updata_Flag(void);
int16_t Get_Attack_attention_Mode(void);

float auto_aim(void);
float auto_get_speed(float angle);
float auto_get_accel(float speed);

#endif


