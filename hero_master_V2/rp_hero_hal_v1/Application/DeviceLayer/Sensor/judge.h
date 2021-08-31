#ifndef RM__MY__JUDGE
#define RM__MY__JUDGE

#include "main.h"

#define Judge_SOF 0xa5;


/* �Զ���֡ͷ */
typedef __packed struct
{
	uint8_t  SOF;						
	uint16_t DataLength;
	uint8_t  Seq;
	uint8_t  CRC8;
	
}FrameHeader;

/* ID: 0x0001  Byte:  11    ����״̬���� */

typedef __packed struct 
{ 
	uint8_t game_type : 4;					//����ģʽ
	uint8_t game_progress : 4;			//��ǰ�����׶�
	uint16_t stage_remain_time;			//��ǰ�׶�ʣ��ʱ�� ��λs
	uint64_t SyncTimeStamp;
} ext_game_state_t; 

/* ID: 0x0002  Byte:  1    ����������� */
typedef __packed struct 
{ 
	uint8_t winner;
} ext_game_result_t; 

/* ID: 0x0003  Byte:  32    ����������Ѫ������ */
typedef __packed struct 
{ 
	uint16_t red_1_robot_HP;   
	uint16_t red_2_robot_HP;  
	uint16_t red_3_robot_HP;  
	uint16_t red_4_robot_HP;  
	uint16_t red_5_robot_HP;  
	uint16_t red_6_robot_HP;  
	uint16_t red_7_robot_HP;  
	uint16_t red_outpost_HP; 
  uint16_t red_base_HP; 
	
	uint16_t blue_1_robot_HP;   
	uint16_t blue_2_robot_HP;   
	uint16_t blue_3_robot_HP;   
	uint16_t blue_4_robot_HP;   
	uint16_t blue_5_robot_HP;   
	uint16_t blue_6_robot_HP;   
	uint16_t blue_7_robot_HP; 

	uint16_t blue_outpost_HP; 
  uint16_t blue_base_HP;   
}  ext_game_robot_HP_t; 

/* ID: 0x0004  Byte:  3    ���ڷ���״̬ */
typedef __packed struct 
{  
	uint8_t dart_belong; 							//������ڵĶ���
	uint16_t stage_remaining_time; 		//����ʱʣ��ı���ʱ��
} ext_dart_status_t;

/* ID: 0x0005  Byte:  11   �˹�������ս�� buff and debuff */
typedef __packed struct
{ 
	uint8_t F1_zone_status:1;  							//�����Ƿ����ı�־
	uint8_t F1_zone_buff_debuff_status:3;   //�췽��Ѫ
	
	uint8_t F2_zone_status:1;  
	uint8_t F2_zone_buff_debuff_status:3;   //�췽��ҩ
	
	uint8_t F3_zone_status:1;  
	uint8_t F3_zone_buff_debuff_status:3;		//������Ѫ  
	
	uint8_t F4_zone_status:1;  
	uint8_t F4_zone_buff_debuff_status:3;  	//������ҩ
	
	uint8_t F5_zone_status:1;  
	uint8_t F5_zone_buff_debuff_status:3;  	//��ֹ�����
	
	uint8_t F6_zone_status:1;  
	uint8_t F6_zone_buff_debuff_status:3;  	//��ֹ�ƶ���
	
	uint16_t red1_bullet_left;							//������ʣ�൯��
	uint16_t red2_bullet_left;
	uint16_t blue1_bullet_left;
	uint16_t blue2_bullet_left;
	
} ext_ICRA_buff_debuff_zone_status_t; 

/* ID: 0x0101  Byte:  4    �����¼����� */
typedef __packed struct 
{ 
	uint32_t event_type;										//0~4bit���ã�5~31����
	
} ext_event_data_t; 


/* ID: 0x0102  Byte:  4    ���ز���վ������ʶ���� */
typedef __packed struct 
{ 
	uint8_t supply_projectile_id;						//����վ��ID
	uint8_t supply_robot_id;								//��ǰ����������ID
	uint8_t supply_projectile_step;					//����վ������״̬
	
	uint8_t supply_projectile_num;					//������Ŀ
} ext_supply_projectile_action_t; 

/* ID: 0x0104  Byte: 2   ����ϵͳ������Ϣ */
typedef __packed struct 
{ 
  uint8_t level; 													//����ȼ�
	uint8_t foul_robot_id;									//���������ID
}  ext_referee_warning_t;  

/* ID: 0x0105  Byte:1  ���ڷ���ڵ���ʱ */
typedef __packed struct 
{ 
	uint8_t dart_remaining_time; 						//15s����ʱ
}  ext_dart_remaining_time_t;  


/* ID: 0X0201  Byte: 27    ������״̬���� */
//0������17mmǹ�ܣ�1����42mmǹ��
typedef __packed struct 			
{ 
	uint8_t robot_id;   													//������ID��������У�鷢��
	uint8_t robot_level;  												//�����˵ȼ� 1һ����2������3����
	uint16_t remain_HP; 													//������ʣ��Ѫ��
	uint16_t max_HP; 															//����������Ѫ��
	uint16_t shooter_heat0_cooling_rate;  				//������ 17mm 1�ӵ�������ȴ�ٶ� ��λ /s
	uint16_t shooter_heat0_cooling_limit;   			// ������ 17mm 1�ӵ���������
	uint16_t shooter_heat0_speed_limit;						//������17mm 1ǹ�������ٶ�
	uint16_t shooter_heat1_cooling_rate;   				//������17mm 2ǹ��ÿ����ȴֵ
	uint16_t shooter_heat1_cooling_limit;  				//������17mm 2ǹ����������
	uint16_t shooter_heat1_speed_limit;						//������17mm 2ǹ�������ٶ�
	uint16_t shooter_42mm_cooling_rate;   				//������17mm 2ǹ��ÿ����ȴֵ
	uint16_t shooter_42mm_cooling_limit;  				//������17mm 2ǹ����������
	uint16_t shooter_42mm_speed_limit;						//������17mm 2ǹ�������ٶ�
	
	uint16_t max_chassis_power; 										//�����˵��̹�������
	uint8_t mains_power_gimbal_output : 1;  			//���ص�Դ��̨�����״̬
	uint8_t mains_power_chassis_output : 1;  			//���ص�Դ���̿����״̬
	uint8_t mains_power_shooter_output : 1; 			//���ص�Դǹ�ܿ����״̬
} ext_game_robot_state_t;   

/* ID: 0X0202  Byte: 16    ʵʱ������������ */
typedef __packed struct 
{ 
	uint16_t chassis_volt;   											//���������ѹ mV
	uint16_t chassis_current;    									//����������� mA
	float chassis_power;   												//����������� 
	uint16_t chassis_power_buffer;								//���̻��幦��ʣ��
	uint16_t shooter_heat0;												//17mmǹ��1Ŀǰ����
	uint16_t shooter_heat1;  											//17mmǹ��2Ŀǰ����
	uint16_t shooter_heat2_42mm; 								//42mmǹ��Ŀǰ����
} ext_power_heat_data_t; 

/* ID: 0x0203  Byte: 16    ������λ������ */
typedef __packed struct 
{   
	float x;   																		//������λ��x
	float y;   																		//������λ��y
	float z;   																		//������λ��z
	float yaw; 																		//������ƫ����
} ext_game_robot_pos_t; 

/* ID: 0x0204  Byte:  1    �������������� */
typedef __packed struct 
{ 
	uint8_t power_rune_buff;											// 
} ext_buff_musk_t; 

/* ID: 0x0205  Byte:  1    ���л���������״̬���� */
typedef __packed struct 
{ 
	uint8_t attack_time; 													//�ɹ���ʱ��
} aerial_robot_energy_t; 

/* ID: 0x0206  Byte:  1    �˺�״̬���� */
typedef __packed struct 
{ 
	uint8_t armor_id : 4; 												//����װ�װ�ID
	uint8_t hurt_type : 4; 												//Ѫ���۳�����
} ext_robot_hurt_t; 

/* ID: 0x0207  Byte:  7    ʵʱ������� */
typedef __packed struct 
{ 
	uint8_t bullet_type;   												//�ӵ�����
	uint8_t shooter_id;   												//�������id
	uint8_t bullet_freq;   												//�ӵ���Ƶ HZ
	float bullet_speed;  													//�ӵ����� m/s
} ext_shoot_data_t; 


/* ID: 0x0208  Byte:  6    �ӵ�ʣ������ */
typedef __packed struct 
{ 
	uint16_t bullet_remaining_num_17mm;   							//�ӵ�ʣ�෢����
	uint16_t bullet_remaining_num_42mm;   							//�ӵ�ʣ�෢����
	uint16_t coin_remain;   														//ʣ����
}  ext_bullet_remaining_t; 

/* ID: 0x0209  Byte:  4    FRID״̬ */
typedef __packed struct 
{ 
	uint32_t rfid_status ;												//RFID��״̬
}  ext_rfid_status_t; 


/*���ڻ����˿ͻ���ָ������*/
typedef __packed struct {  
	uint8_t dart_launch_opening_status;  
	uint8_t dart_attack_target;  
	uint16_t target_change_time; 
  uint8_t first_dart_speed;   
	uint8_t second_dart_speed;  
  uint8_t third_dart_speed;   
	uint8_t fourth_dart_speed; 
	uint16_t last_dart_launch_time; 
	uint16_t operate_launch_cmd_time; 
} ext_dart_client_cmd_t; 
	
typedef __packed struct 
{ 
	uint16_t data_cmd_id;    
	uint16_t send_ID;    
	uint16_t receiver_ID; 
} ext_student_interactive_header_data_t; 




typedef __packed struct 
{ 
	uint8_t data[10]; //���ݶ�,n��ҪС��113
} robot_interactive_data_t;

typedef struct JUDGE_MODULE_DATA
{
	FrameHeader all_head;							//֡ͷ
	
	//////////////////////// ����
	ext_game_state_t game_state;							//��Ϸ״̬
	ext_game_robot_HP_t robot_HP;							//������Ѫ��
	ext_dart_status_t dart_state;							//����״̬
	
	ext_ICRA_buff_debuff_zone_status_t zone;	//��ͼ״̬
	ext_event_data_t event;										//�¼�״̬
	
	ext_supply_projectile_action_t supply_status;	//����վ״̬
	ext_referee_warning_t offline_warning;				//���澯��
	ext_dart_remaining_time_t dart_cnt;						//���ڷ��䵹��ʱ
	
	
	ext_game_robot_state_t robot_state;						//������״̬
	ext_power_heat_data_t out_and_heat; 					//��������״̬
	ext_game_robot_pos_t  robot_pos;							//������λ��
	ext_buff_musk_t 			buff;										//������buff״̬
	ext_robot_hurt_t     robot_hurt;							//���������˾���
	ext_shoot_data_t			shoot_data;							//ʵʱ�������
	ext_bullet_remaining_t bullet_remain;					//�ӵ�����
	
	ext_rfid_status_t		rfid_status;							//rfid��״̬
	
	uint8_t state_flag;
	uint8_t out_heat_flag;
	uint8_t buff_flag;
	uint8_t hurt_flag;
	uint8_t shoot_flag;
	uint8_t rfid_flag;
	
} JUDGE_MODULE_DATA;

extern JUDGE_MODULE_DATA Judge_Hero;
extern int shoot_big_num;
extern uint32_t judge_offline_cnt;
extern uint8_t Judge_Offline_flag;



void DJI_Judge_Init(void);

// ����ϵͳ�ķ�������
void JUDGE_Analyze(volatile uint8_t *databuff, uint8_t lenth);
void CAN_Judge_Analyze(uint16_t id, uint8_t *databuff);


#endif
