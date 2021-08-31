#ifndef RM__MY__JUDGE
#define RM__MY__JUDGE

#include "sys.h"

#define JUDGE_FRAME_HEADER 0xa5

#define Client_mid_position_x 960
#define Client_mid_position_y 540

#define AIM_X 950

#define AIM_Y 540

//***********����ϵͳ�ķ�����Ϣ***************/
enum
{ 
	ID_game_state       						= 0x0001,//����״̬���ݣ�1Hz
	ID_game_result 	   							= 0x0002,//����������ݣ�������������
	ID_game_robot_HP       					= 0x0003,//����������Ѫ�����ݣ�1Hz����
	ID_dart_status									= 0x0004,//���ڷ���״̬�����ڷ���ʱ����
	ID_ICRA_buff_debuff_zone_status = 0x0005,//�˹�������ս���ӳ���ͷ���״̬��1Hz
	ID_event_data  									= 0x0101,//�����¼����ݣ��¼� �����ı�󡪡� ����
	ID_supply_projectile_action   	= 0x0102,//���ز���վ������ʶ���ݣ����� �����ı�󡪡� ����
	ID_supply_projectile_booking    = 0x0103,//���󲹸�վ�������� ���������ӷ��͡�����RM �Կ�����δ����10Hz
	ID_referee_warning					 		= 0x0104,//���о������ݣ���������󡪡�����
	ID_dart_remaining_time					= 0x0105,//���ڷ���ڵ���ʱ��1Hz
	ID_game_robot_state    					= 0x0201,//������״̬���ݣ�10Hz
	ID_power_heat_data    					= 0x0202,//ʵʱ�����������ݣ�50Hz
	ID_game_robot_pos        				= 0x0203,//������λ�����ݣ�10Hz
	ID_buff_musk										= 0x0204,//�������������ݣ�1Hz
	ID_aerial_robot_energy					= 0x0205,//���л���������״̬���ݣ�10Hz��ֻ�С������л��������ء�������
	ID_robot_hurt										= 0x0206,//�˺�״̬���ݣ��˺���������
	ID_shoot_data										= 0x0207,//ʵʱ������ݣ��ӵ��������
	ID_bullet_remaining							= 0x0208,//����ʣ�෢���������������л����ˣ��ڱ������ˡ����Լ�ICRA�����˷��ͣ�1Hz
	ID_rfid_status									= 0x0209,//������RFID״̬��1Hz
	
	ID_dart_client_directive        = 0x020A,//���ڻ����˿ͻ���ָ����, 10Hz
	
	ID_robot_interactive_header_data			= 0x0301,//�����˽������ݣ��������ͷ������������� 10Hz
	ID_controller_interactive_header_data = 0x0302,//�Զ���������������ݽӿڣ�ͨ�������ͻ��˴����������� 30Hz
	ID_map_interactive_header_data        = 0x0303,//�ͻ���С��ͼ�������ݣ������������͡���
	ID_keyboard_information               = 0x0304//���̡������Ϣ��ͨ������ͼ�����ڡ�������
};
//������ö��CMD_ID
//**************����ϵͳ������Ϣ�ĳ��ȣ���λ/�ֽڣ�**************************/
enum judge_data_length_t {
	/* Std */
	LEN_FRAME_HEAD 	                 = 5,	// ֡ͷ����
	LEN_CMD_ID 		                   = 2,	// �����볤��
	LEN_FRAME_TAIL 	                 = 2,	// ֡βCRC16
	/* Ext */
	// 0x000x
	LEN_GAME_STATUS 				         = 11,
	LEN_GAME_RESULT 				         = 1,
	LEN_GAME_ROBOT_HP 			         = 28,
	LEN_DART_STATUS					         = 3,
	LEN_ICRA_BUFF_DEBUFF_ZONE_STATUS = 11,//0x0005
	// 0x010x
	LEN_EVENT_DATA					         = 4,
	LEN_SUPPLY_PROJECTILE_ACTION	   = 4,//����������������������������������
	LEN_SUPPLY_PROJECTILE_BOOKING	   = 3,//�Կ���δ����
	LEN_REFEREE_WARNING				       = 2,
	LEN_DART_REMAINING_TIME		     	 = 1,//0x0105
	// 0x020x
	LEN_GAME_ROBOT_STATUS			       = 27,//15!!!!!!!!!!!!!!!!!!!!!!!!!!!
	LEN_POWER_HEAT_DATA 			       = 16,//��������������������
	LEN_GAME_ROBOT_POS				       = 16,
	LEN_BUFF_MASK		 				         = 1,
	LEN_AERIAL_ROBOT_ENERGY 	     	 = 1,//����������
	LEN_ROBOT_HURT				         	 = 1,
	LEN_SHOOT_DATA					         = 7,//��������
	LEN_BULLET_REMAINING	 		       = 6,//��������
	LEN_RFID_STATUS					         = 4,
	LEN_DART_CLIENT_DIRECTIVE        = 12,//0x020A
	// 0x030x
	//LEN_robot_interactive_header_data      = n,
	//LEN_controller_interactive_header_data = n,
	LEN_MAP_INTERACTIVE_HEADERDATA           = 15,
	LEN_KEYBOARD_INFORMATION                 = 12,//0x0304
};//��2-4
/*******************����ϵͳ��Ϣ��������**************************/


/* �Զ���֡ͷ */
typedef __packed struct
{
	uint8_t  SOF;						
	uint16_t DataLength;
	uint8_t  Seq;
	uint8_t  CRC8;
	
}FrameHeader;


typedef __packed struct
{
	uint8_t  sof;
	uint16_t data_length;
	uint8_t  seq;
	uint8_t  crc8;
} std_frame_header_t;//LEN_FRAME_HEAD

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
	uint16_t red_7_robot_HP;  
	uint16_t red_outpost_HP; 
  uint16_t red_base_HP; 
	
	uint16_t blue_1_robot_HP;   
	uint16_t blue_2_robot_HP;   
	uint16_t blue_3_robot_HP;   
	uint16_t blue_4_robot_HP;   
	uint16_t blue_5_robot_HP;     
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
	uint16_t shooter_42mm_cooling_rate;   				//������42mm ǹ��ÿ����ȴֵ
	uint16_t shooter_42mm_cooling_limit;  				//������42mm ǹ����������
	uint16_t shooter_42mm_speed_limit;						//������42mm ǹ�������ٶ�
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
/* ID: 0x020a  Byte:  4    FRID״̬ */
typedef __packed struct {  
	uint8_t dart_launch_opening_status;  
	uint8_t dart_attack_target;  
	uint16_t target_change_time; 
	uint16_t operate_launch_cmd_time; 
} ext_dart_client_cmd_t; 
	
typedef __packed struct 
{ 
	uint16_t data_cmd_id;    
	uint16_t send_ID;    
	uint16_t receiver_ID; 
} ext_student_interactive_header_data_t; 






	/*******************************************************************************/
/*
	������ ID��
	1��Ӣ��(��)��
	2������(��)��
	3/4/5������(��)��
	6������(��)��
	7���ڱ�(��)��
	9���״�죩
	101��Ӣ��(��)��
	102������(��)��
	103/104/105������(��)��
	106������(��)��
	107���ڱ�(��)��
	109���״����

	�ͻ��� ID�� 
	0x0101 ΪӢ�۲����ֿͻ���(��) ��
	0x0102 Ϊ���̲����ֿͻ���( �� )��
	0x0103/0x0104/0x0105 Ϊ���������ֿͻ���(��)��
	0x0106 Ϊ���в����ֿͻ���((��)�� 

	0x0165��Ӣ�۲����ֿͻ���(��)��
	0x0166�����̲����ֿͻ���(��)��
	0x0167/0x0168/0x0169�����������ֿͻ���(��)��
	0x016A�����в����ֿͻ���(��)�� 
*/
enum judge_robot_ID{
	hero_red       = 1,
	engineer_red   = 2,
	infantry3_red  = 3,
	infantry4_red  = 4,
	infantry5_red  = 5,
	plane_red      = 6,
	
	hero_blue      = 101,
	engineer_blue  = 102,
	infantry3_blue = 103,
	infantry4_blue = 104,
	infantry5_blue = 105,
	plane_blue     = 106,
};
typedef __packed struct{
	uint16_t teammate_hero;
	uint16_t teammate_engineer;
	uint16_t teammate_infantry3;
	uint16_t teammate_infantry4;
	uint16_t teammate_infantry5;
	uint16_t teammate_plane;
	uint16_t teammate_sentry;
	
	uint16_t client_hero;
	uint16_t client_engineer;
	uint16_t client_infantry3;
	uint16_t client_infantry4;
	uint16_t client_infantry5;
	uint16_t client_plane;
} ext_interact_id_t;

typedef __packed struct JUDGE_MODULE_DATA
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
	
	
	ext_game_robot_state_t robot_status;						//������״̬
	ext_power_heat_data_t out_and_heat; 					//��������״̬
	ext_game_robot_pos_t  robot_pos;							//������λ��
	ext_buff_musk_t 			buff;										//������buff״̬
	ext_robot_hurt_t     robot_hurt;							//���������˾���
	ext_shoot_data_t			shoot_data;							//ʵʱ�������
	ext_bullet_remaining_t bullet_remain;					//�ӵ�����
	
	ext_rfid_status_t		rfid_status;							//rfid��״̬
	
	ext_dart_client_cmd_t           dart_client;        // 0x020A
	
	ext_interact_id_t								ids;								//�뱾�������Ļ�����id
	uint16_t                        self_client;        //�����ͻ���
	
	u8 state_flag;
	u8 out_heat_flag;
	u8 buff_flag;
	u8 hurt_flag;
	u8 shoot_flag;
	u8 bullet_flag;
	u8 rfid_flag;
	
	uint8_t	 		data_valid;	// ������Ч��
	uint8_t			err_cnt;
	uint8_t		offline_cnt;
	uint8_t		offline_max_cnt;	
	
} JUDGE_MODULE_DATA;
/* 
	ѧ�������˼�ͨ�� cmd_id 0x0301������ data_ID:0x0200~0x02FF
	�������� �����˼�ͨ�ţ�0x0301��
	����Ƶ�ʣ����������кϼƴ������� 5000 Byte�� �����з���Ƶ�ʷֱ𲻳���30Hz��
 * +------+------+-------------+------------------------------------+
 * | byte | size |    breif    |            note                    |
 * |offset|      |             |                                    |
 * +------+------+-------------+------------------------------------+
 * |  0   |  2   | 	 data_ID   | 0x0200~0x02FF,��������Щ ID ��ѡȡ |
 * |      |      |             | ����ID�����ɲ������Զ���           |
 * +------|------|-------------|------------------------------------|
 * |  2   |  2   | 	sender_ID  | ��ҪУ�鷢���ߵ� ID ��ȷ��					|
 * +------|------|-------------|------------------------------------|
 * |  4   |  2   | receiver_ID | ��ҪУ������ߵ� ID ��ȷ��					|
 * |      |      |             | ���粻�ܷ��͵��жԻ����˵�ID				| 
 * +------|------|-------------|------------------------------------|
 * |  6   |  n   | 		Data     | n ��ҪС�� 113 										|
 * +------+------+-------------+------------------------------------+
*/
/******************************�ͻ��˽�������**************************************/
#define INTERACT_DATA_LEN	113
typedef __packed struct //���ݶ����ݸ�ʽ
{ 
	uint16_t data_cmd_id;    
	uint16_t send_ID;    
	uint16_t receiver_ID; 
} ext_client_data_header_t; 
enum
{
	//0x200-0x02ff 	�����Զ������� ��ʽ  INTERACT_ID_XXXX
	INTERACT_ID_delete_graphic 			= 0x0100,	/*�ͻ���ɾ��ͼ��*/
	INTERACT_ID_draw_one_graphic 		= 0x0101,	/*�ͻ��˻���һ��ͼ��*/
	INTERACT_ID_draw_two_graphic 		= 0x0102,	/*�ͻ��˻���2��ͼ��*/
	INTERACT_ID_draw_five_graphic 	= 0x0103,	/*�ͻ��˻���5��ͼ��*/
	INTERACT_ID_draw_seven_graphic 	= 0x0104,	/*�ͻ��˻���7��ͼ��*/
	INTERACT_ID_draw_char_graphic 	= 0x0110,	/*�ͻ��˻����ַ�ͼ��*/
	INTERACT_ID_bigbome_num					= 0x02ff
};
typedef __packed struct 
{ 
	uint8_t data[INTERACT_DATA_LEN]; //���ݶ�,n��ҪС��113
} robot_interactive_data_t;
//��λ���ֽڣ�
enum
{
	LEN_INTERACT_delete_graphic     = 8,  //ɾ��ͼ�� 2(��������ID)+2(������ID)+2��������ID��+2���������ݣ�  
	LEN_INTERACT_draw_one_graphic   = 21, // ����2+2+2+15
	LEN_INTERACT_draw_two_graphic   = 36, //6+15*2
	LEN_INTERACT_draw_five_graphic  = 81, //6+15*5
	LEN_INTERACT_draw_seven_graphic = 111,//6+15*7
	LEN_INTERACT_draw_char_graphic  = 51, //6+15+30���ַ������ݣ�
};
//****************************��ͼ�����ݶ�����****************************/
typedef __packed struct//ͼ��
{                          
	uint8_t graphic_name[3]; 
	uint32_t operate_tpye:3; 
	uint32_t graphic_tpye:3; //ֱ��  ����  ��Բ  ��Բ  Բ��  ����  ����  �ַ�
	uint32_t layer:4;        
	uint32_t color:4;        
	uint32_t start_angle:9;  //��    ��    ��    ��    �Ƕ�  ��С  ��С  ��С
	uint32_t end_angle:9;    //��    ��    ��    ��          λ��  ��    ����
	uint32_t width:10;       
	uint32_t start_x:11;     //���  ���  Բ��  Բ��  Բ��  ���  ���  ���
	uint32_t start_y:11;     //
	uint32_t radius:10;      //��    ��    �뾶  ��    ��    ��    ��    ��
	uint32_t end_x:11;       //�յ�  �Զ�  ��    ����  ����  ��    ��    ��
	uint32_t end_y:11;       //                              ��    ��    ��
} graphic_data_struct_t;
typedef __packed struct//������
{                          
	uint8_t graphic_name[3]; 
	uint32_t operate_tpye:3; 
	uint32_t graphic_tpye:3; 
	uint32_t layer:4;        
	uint32_t color:4;        
	uint32_t start_angle:9;  
	uint32_t end_angle:9;    
	uint32_t width:10;       
	uint32_t start_x:11;    
	uint32_t start_y:11;     
  int number;       
} Float_data_struct_t;
typedef __packed struct//������
{                          
	uint8_t graphic_name[3]; 
	uint32_t operate_tpye:3; 
	uint32_t graphic_tpye:3; 
	uint32_t layer:4;        
	uint32_t color:4;        
	uint32_t start_angle:9;  
	uint32_t end_angle:9;    
	uint32_t width:10;       
	uint32_t start_x:11;    
	uint32_t start_y:11;     
  int number;       
} Int_data_struct_t;
/* data_ID: 0X0100  Byte:  2	    �ͻ���ɾ��ͼ��*/
typedef __packed struct
{
	uint8_t operate_type; 
	uint8_t layer;//ͼ������0~9
}ext_client_custom_graphic_delete_t;
typedef enum
{
	NONE_delete    = 0,
	GRAPHIC_delete = 1,
	ALL_delete     = 2
}delete_Graphic_Operate;//ext_client_custom_graphic_delete_t��uint8_t operate_type
/*ͼ��ɾ������*/

//bit 0-2
typedef enum
{
	NONE   = 0,/*�ղ���*/
	ADD    = 1,/*����ͼ��*/
	MODIFY = 2,/*�޸�ͼ��*/
	DELETE = 3,/*ɾ��ͼ��*/
}Graphic_Operate;//graphic_data_struct_t��uint32_t operate_tpye
/*ͼ�����*/
//bit3-5
typedef enum
{
	LINE      = 0,//ֱ��
	RECTANGLE = 1,//����
	CIRCLE    = 2,//��Բ
	OVAL      = 3,//��Բ
	ARC       = 4,//Բ��
	INT    	  = 5,//������
	FLOAT     = 6,//������
	CHAR      = 7 //�ַ�
}Graphic_Type;
/*ͼ������*/
//bit 6-9ͼ���� ���Ϊ9����С0
//bit 10-13��ɫ
typedef enum
{
	RED_BLUE  = 0,//������ɫ	
	YELLOW    = 1,
	GREEN     = 2,
	ORANGE    = 3,
	FUCHSIA   = 4,	/*�Ϻ�ɫ*/
	PINK      = 5,
	CYAN_BLUE = 6,	/*��ɫ*/
	BLACK     = 7,
	WHITE     = 8
}Graphic_Color;
/*ͼ����ɫ����*/
//bit 14-31 �Ƕ� [0,360]
/**********************************�ͻ��˻�ͼ************************************************/
//ɾ��ͼ��
typedef __packed struct
{
	std_frame_header_t txFrameHeader;			
	uint16_t  CmdID;										
	ext_client_data_header_t   dataFrameHeader;
	ext_client_custom_graphic_delete_t clientData;		
	uint16_t	FrameTail;								
}ext_deleteLayer_data_t;

//���ַ���
typedef __packed struct
{
	graphic_data_struct_t grapic_data_struct;
	uint8_t data[30];
} ext_client_string_t;

typedef __packed struct
{
	std_frame_header_t txFrameHeader;			//֡ͷ
	uint16_t  CmdID;										//������
	ext_client_data_header_t   dataFrameHeader;//���ݶ�ͷ�ṹ
	ext_client_string_t clientData;//���ݶ�
	uint16_t	FrameTail;								//֡β
}ext_charstring_data_t;
//������ͼ
typedef __packed struct
{
	std_frame_header_t txFrameHeader;			//֡ͷ
	uint16_t  CmdID;										//������
	ext_client_data_header_t   dataFrameHeader;//���ݶ�ͷ�ṹ
	graphic_data_struct_t clientData;		//���ݶ�
	uint16_t	FrameTail;								//֡β
}ext_graphic_one_data_t;
typedef __packed struct
{
	std_frame_header_t txFrameHeader;			
	uint16_t  CmdID;										
	ext_client_data_header_t   dataFrameHeader;
	graphic_data_struct_t clientData[2];		
	uint16_t	FrameTail;
	
}ext_graphic_two_data_t;
typedef __packed struct
{
	std_frame_header_t txFrameHeader;			
	uint16_t  CmdID;										
	ext_client_data_header_t   dataFrameHeader;
	graphic_data_struct_t clientData[5];		
	uint16_t	FrameTail;								
}ext_graphic_five_data_t;
typedef __packed struct
{
	std_frame_header_t txFrameHeader;			
	uint16_t  CmdID;										
	ext_client_data_header_t   dataFrameHeader;
	graphic_data_struct_t clientData[7];		
	uint16_t	FrameTail;								
}ext_graphic_seven_data_t;
//���Ƹ�����
typedef __packed struct
{
	std_frame_header_t txFrameHeader;			
	uint16_t  CmdID;										
	ext_client_data_header_t   dataFrameHeader;
	Float_data_struct_t clientData[2];		
	uint16_t	FrameTail;								
}ext_float_two_data_t;

typedef __packed struct
{
	std_frame_header_t txFrameHeader;			
	uint16_t  CmdID;										
	ext_client_data_header_t   dataFrameHeader;
	Float_data_struct_t clientData[7];		
	uint16_t	FrameTail;								
}ext_float_seven_data_t;
//��������
typedef __packed struct
{
	std_frame_header_t txFrameHeader;			
	uint16_t  CmdID;										
	ext_client_data_header_t   dataFrameHeader;
	Int_data_struct_t clientData[2];		
	uint16_t	FrameTail;								
}ext_int_two_data_t;
typedef __packed struct
{
	std_frame_header_t txFrameHeader;			
	uint16_t  CmdID;										
	ext_client_data_header_t   dataFrameHeader;
	Int_data_struct_t clientData[7];		
	uint16_t	FrameTail;								
}ext_int_seven_data_t;
 
typedef __packed struct Client_Slave_Flag
{
	uint8_t global_fiction;
	uint8_t global_clip;
	uint8_t global_spin;
	uint8_t global_auto_aim;
	uint8_t global_twist;
	uint8_t global_anti_top;
	uint8_t shift_rush;
	uint8_t user1;
}Client_Slave_Flag;



extern uint8_t update_figure_flag,update_aim_flag,update_float_flag,update_supercapacitor_flag,update_int_flag;//�ͻ��˸���flag
extern JUDGE_MODULE_DATA Judge_Hero;
extern Client_Slave_Flag Slaver_flag;

void Determine_ID(void);

void DJI_Judge_Init(void);
void Client_graphic_Init(void);
void Client_graphic_delete_update(uint8_t delete_layer);//ɾ��ͼ����Ϣ
void Client_graphic_Info_update(void);
void Client_aim_update(void);//׼��
void Client_supercapacitor_update(void);//��������
void Client_gimbal_angle_update(void);//����Ƕ�
void Client_bullet_int_update(void);//������Ϣ
void Client_aim_line(void);//Ӣ��

void Client_flag_update(void);//��ʶ������
void PITCH_YAW_Analyze(u8 *buf);//pitch��yaw����
// ����ϵͳ�ķ�������
void JUDGE_Analyze(volatile u8 *databuff, u8 lenth);
void Client_task(void);


#endif
