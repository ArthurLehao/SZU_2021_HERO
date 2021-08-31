
#include "link.h"
#include "my_judge.h"
#include "crc.h"


JUDGE_MODULE_DATA Judge_Hero;
volatile u8 JUDGE_NUM[200] = {0};
uint8_t CliendTxBuffer[200];
Client_Slave_Flag Slaver_flag;

//!!!!!!!!!!!!!!!!!!!ȫ�ֱ�������������
bool global_fiction,global_clip,global_spin,global_auto_aim,global_twist,global_anti_top;
uint32_t global_sight_bead_x = 960,global_sight_bead_y = 720,global_supercapacitor_point = 20;//[0,100]
float global_supercapacitor_remain = 77.3,//[0,100]
	    global_gimbal_pitch,global_gimbal_yaw;
int   global_bullet_speed,global_bullet_sum;
//////////////////////////////////////

void DJI_Judge_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOCʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIODʱ��
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE); //ʹ��DMA1ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);//ʹ��UART5ʱ��
 
	//����5��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); //GPIOC 12����ΪUART5
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); //GPIOD 2����ΪUART5
	
	//UART5�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //GPIO D2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //������
	GPIO_Init(GPIOD,&GPIO_InitStructure); //��ʼ��PD2
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //GPIO C12
	GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��PC12

   //UART5 ��ʼ������
	USART_InitStructure.USART_BaudRate = 115200;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//����ģʽ
  USART_Init(UART5, &USART_InitStructure); //��ʼ������5


	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//UART5 �ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);	//ʹ�ܴ���5�����ж�
	
  
	USART_DMACmd(UART5, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE); //ʹ�ܴ��ڵ�DMAģʽ
	
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;													//ѡ��ͨ��4
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(UART5->DR);				//DMA�����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&JUDGE_NUM[0];			//�洢����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;									//���赽�洢��ģʽ
	DMA_InitStructure.DMA_BufferSize = 200;																	//���ν��յ���֡����Ŀ
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//���������ģʽ							
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									//�洢������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//���跢�͵������ݳ���
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//�洢�������������ݳ���
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;													//�洢��ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;									//���ȼ��ǳ���
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;									//FIFOģʽ��ֹ
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;						//FIFOģʽ��ֵ
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;							//�洢��ͻ����������
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;			//����ͻ����������
	
	DMA_Init(DMA1_Stream0, &DMA_InitStructure);															//DMA1��ʼ��
	
	DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_FEIF0);			//�������IT��־λ
	DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_DMEIF0);
	DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TEIF0);
	DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_HTIF0);
	DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0);

	DMA_Cmd(DMA1_Stream0, ENABLE);
	
	USART_Cmd(UART5, ENABLE);  //ʹ�ܴ���5
}


u32 lenth = 0;
void UART5_IRQHandler(void)
{
	u8 receive_lenth;

	
	if(USART_GetFlagStatus(UART5, USART_FLAG_IDLE) != RESET)
	{
		DMA_Cmd(DMA1_Stream0, DISABLE);
		
		receive_lenth = UART5->SR;//�ȶ�SR���ٶ�DR
		receive_lenth = UART5->DR;
		
		USART_ClearFlag(UART5, USART_FLAG_IDLE);
		
		
		lenth = 200 - DMA_GetCurrDataCounter(DMA1_Stream0);
		
		JUDGE_Analyze(JUDGE_NUM, lenth);
		
		DMA1_Stream0->NDTR = 200;
		DMA1_Stream0->M0AR = (u32)&JUDGE_NUM[0];
		DMA_ClearFlag(DMA1_Stream0, DMA_IT_TCIF0);
		
		DMA_Cmd(DMA1_Stream0, ENABLE);
		
	}
	
}


void DMA1_Stream0_IRQHandler(void)
{
	DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_FEIF0);			//�������IT��־λ
	DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_DMEIF0);
	DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TEIF0);
	DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_HTIF0);
	DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0);
}


u8 Analyze_buffer[200] = {0};
u32 data_lenth = 0;
u32 CMD_ID = 0;
u32 CMD_ID2 = 0;

void JUDGE_Analyze(volatile u8 *databuff, u8 lenth)
{
	u8 pot = 0;
	
	memcpy((void*)Analyze_buffer, (const void*)databuff, 200);
	
	//
	
	while(pot<lenth)
	{
		if(Analyze_buffer[pot] == 0xa5)
		{
			if(Verify_CRC8_Check_Sum((u8*)(& Analyze_buffer[pot]), 5))//У��ɹ�
			{
				data_lenth = 0;
				data_lenth = ((Analyze_buffer[pot+1])&0xff) | ((Analyze_buffer[pot+2]<<8)&0xff00);
				
				if(pot+data_lenth+9>lenth)
				{
					pot++;
					continue;
				}
				
				if(Verify_CRC16_Check_Sum((u8*)(& Analyze_buffer[pot]), data_lenth+9))
				{
					CMD_ID = ((Analyze_buffer[pot+5])&0xff) | ((Analyze_buffer[pot+6]<<8)&0xff00);
					
					switch(CMD_ID)
					{
						
						case 0x0001: 
								data_lenth = 11;
								memcpy((void*)(&Judge_Hero.game_state), (const void*)(&Analyze_buffer[pot+7]), data_lenth);		
								break;
		//				case 0x0002: memcpy((void*)(&Judge_Hero.), (const void*)(&Analyze_buffer[pot+7]), data_lenth);
		//						break;
						case 0x0003: 
								data_lenth = 32;
								memcpy((void*)(&Judge_Hero.robot_HP), (const void*)(&Analyze_buffer[pot+7]), data_lenth);
								
								break;
						case 0x0004: 
								data_lenth = 3;
								memcpy((void*)(&Judge_Hero.dart_state), (const void*)(&Analyze_buffer[pot+7]), data_lenth);
								break;
						case 0x0005: 
								data_lenth = 11;
								memcpy((void*)(&Judge_Hero.zone), (const void*)(&Analyze_buffer[pot+7]), data_lenth);
								break;
						
						case 0x0101: 
								data_lenth = 4;
								memcpy((void*)(&Judge_Hero.event), (const void*)(&Analyze_buffer[pot+7]), data_lenth);
								break;
						case 0x0102:
								data_lenth = 4;
								memcpy((void*)(&Judge_Hero.supply_status), (const void*)(&Analyze_buffer[pot+7]), data_lenth);
								break;
						case 0x0104: 
								data_lenth = 2;
								memcpy((void*)(&Judge_Hero.offline_warning), (const void*)(&Analyze_buffer[pot+7]), data_lenth);
								break;
						case 0x0105: memcpy((void*)(&Judge_Hero.dart_cnt), (const void*)(&Analyze_buffer[pot+7]), 1);
								data_lenth = 1;
								break;
						
						case 0x0201: 
								data_lenth = 27;
								memcpy((void*)(&Judge_Hero.robot_status), (const void*)(&Analyze_buffer[pot+7]), 27);
								Determine_ID();
								Judge_Hero.state_flag = 1;
								break;
						case 0x0202: 
									data_lenth = 16;
									memcpy((void*)(&Judge_Hero.out_and_heat), (const void*)(&Analyze_buffer[pot+7]), data_lenth);
									Judge_Hero.out_heat_flag = 1;
								break;
						
						case 0x0203: memcpy((void*)(&Judge_Hero.robot_pos), (const void*)(&Analyze_buffer[pot+7]), 16);
								data_lenth = 16;
								break;
						case 0x0204: memcpy((void*)(&Judge_Hero.buff), (const void*)(&Analyze_buffer[pot+7]), 1);
								data_lenth = 1;
								Judge_Hero.buff_flag = 1;
								break;
		//				case 0x0205: memcpy((void*)(&Judge_Hero), (const void*)(&Analyze_buffer[pot+7]), data_lenth);
		//					data_lenth = 3;
		//						break;
						case 0x0206: memcpy((void*)(&Judge_Hero.robot_hurt), (const void*)(&Analyze_buffer[pot+7]), 1);
								data_lenth = 1;
								Judge_Hero.hurt_flag = 1;
								break;
						case 0x0207: 
							data_lenth = 7;
							memcpy((void*)(&Judge_Hero.shoot_data), (const void*)(&Analyze_buffer[pot+7]), data_lenth);
							Judge_Hero.shoot_flag = 1;
								break;
		//				case 0x0208: memcpy((void*)(&Judge_Hero.out_and_heat), (const void*)(&Analyze_buffer[pot+7]), data_lenth);
		//						break;
						case 0x0209: 
								data_lenth = 4;	
								memcpy((void*)(&Judge_Hero.rfid_status), (const void*)(&Analyze_buffer[pot+7]), data_lenth);
								Judge_Hero.rfid_flag = 1;
								break;
		//				case 0x020a: memcpy((void*)(&Judge_Hero.out_and_heat), (const void*)(&Analyze_buffer[pot+7]), data_lenth);
		//						break;
							
//						case ID_dart_client_directive:
//						memcpy(&Judge_Hero->dart_client,(rxBuf+DATA_SEG),LEN_DART_CLIENT_DIRECTIVE);
//						break;
//						
//						case ID_COMMUNICATION: 
//						JUDGE_ReadFromCom();
//						break;
						
						default: break;
					}
					pot += data_lenth+9;
					continue;
					
				}
			}
		}
		
		pot++;

	}
	memset(Analyze_buffer, 0, 200);
	
	
}

void Usart5_Sent_Byte(u8 ch)
{
	UART5->DR = ch;
	while((UART5->SR&0x40)==0);
}

void Usart5_Sent_string(u8 *string, uint16_t length)
{
	uint16_t i;
	
	for(i = 0; i<length; i++)
	{
		Usart5_Sent_Byte(string[i]);
	}
}

void Client_Sent_String(u8 *string, uint16_t length)
{
	Usart5_Sent_string(string, length);
}

void Determine_ID(void)//�ж��Լ����ĸ�����
{		
	if(Judge_Hero.robot_status.robot_id < 10)//�������˵�ID���췽
	{ 
		Judge_Hero.ids.teammate_hero 		 	= 1;
		Judge_Hero.ids.teammate_engineer  = 2;
		Judge_Hero.ids.teammate_infantry3 = 3;
		Judge_Hero.ids.teammate_infantry4 = 4;
		Judge_Hero.ids.teammate_infantry5 = 5;
		Judge_Hero.ids.teammate_plane		 	= 6;
		Judge_Hero.ids.teammate_sentry		= 7;
		
		Judge_Hero.ids.client_hero 		 	= 0x0101;
		Judge_Hero.ids.client_engineer  = 0x0102;
		Judge_Hero.ids.client_infantry3 = 0x0103;
		Judge_Hero.ids.client_infantry4 = 0x0104;
		Judge_Hero.ids.client_infantry5 = 0x0105;
		Judge_Hero.ids.client_plane			= 0x0106;
		
		if     (Judge_Hero.robot_status.robot_id == hero_red)//����ˢ�·����ڱ����и�����ɫ
			Judge_Hero.self_client = Judge_Hero.ids.client_hero;
		else if(Judge_Hero.robot_status.robot_id == engineer_red)
			Judge_Hero.self_client = Judge_Hero.ids.client_engineer;
		else if(Judge_Hero.robot_status.robot_id == infantry3_red)
			Judge_Hero.self_client = Judge_Hero.ids.client_infantry3;
		else if(Judge_Hero.robot_status.robot_id == infantry4_red)
			Judge_Hero.self_client = Judge_Hero.ids.client_infantry4;
		else if(Judge_Hero.robot_status.robot_id == infantry5_red)
			Judge_Hero.self_client = Judge_Hero.ids.client_infantry5;
		else if(Judge_Hero.robot_status.robot_id == plane_red)
			Judge_Hero.self_client = Judge_Hero.ids.client_plane;
	}
	else //����
	{
		Judge_Hero.ids.teammate_hero 		 	= 101;
		Judge_Hero.ids.teammate_engineer  = 102;
		Judge_Hero.ids.teammate_infantry3 = 103;
		Judge_Hero.ids.teammate_infantry4 = 104;
		Judge_Hero.ids.teammate_infantry5 = 105;
		Judge_Hero.ids.teammate_plane		 	= 106;
		Judge_Hero.ids.teammate_sentry		= 107;
		
		Judge_Hero.ids.client_hero 		 	= 0x0165;
		Judge_Hero.ids.client_engineer  = 0x0166;
		Judge_Hero.ids.client_infantry3 = 0x0167;
		Judge_Hero.ids.client_infantry4 = 0x0168;
		Judge_Hero.ids.client_infantry5 = 0x0169;
		Judge_Hero.ids.client_plane			= 0x016A;
		
		if     (Judge_Hero.robot_status.robot_id == hero_blue)
			Judge_Hero.self_client = Judge_Hero.ids.client_hero;
		else if(Judge_Hero.robot_status.robot_id == engineer_blue)
			Judge_Hero.self_client = Judge_Hero.ids.client_engineer;
		else if(Judge_Hero.robot_status.robot_id == infantry3_blue)
			Judge_Hero.self_client = Judge_Hero.ids.client_infantry3;
		else if(Judge_Hero.robot_status.robot_id == infantry4_blue)
			Judge_Hero.self_client = Judge_Hero.ids.client_infantry4;
		else if(Judge_Hero.robot_status.robot_id == infantry5_blue)
			Judge_Hero.self_client = Judge_Hero.ids.client_infantry5;
		else if(Judge_Hero.robot_status.robot_id == plane_blue)
			Judge_Hero.self_client = Judge_Hero.ids.client_plane;
		
	}

}
/*********************************************/
uint8_t CliendTxBuffer[200];
uint8_t TeammateTxBuffer[200];
char first_line[30]  = {"readyfire:"};//�Ƿ�������,����30���ַ�����bool
char second_line[30] = {"      top:"};//С����
char third_line[30]  = {" auto_aim:"};//����
char fourth_line[30] = {"    twist:"};//Ť��
char fifth_line[30]  = {":bullet_sum"};//������int
char sixth_line[30]  = {":bullet_speed"};//����int
char seventh_line[30]= {":supercapacitor"};//��������ʣ����,float
char empty_line[30] = {"                            "};
//*******************************���ַ���******************************/
ext_charstring_data_t tx_client_char;
uint8_t state_first_graphic;//0~7ѭ��
void Char_Graphic(ext_client_string_t* graphic,//����Ҫ����ȥ�������е����ݶ�����
									const char* name,
									uint32_t operate_tpye,
									
									uint32_t layer,
									uint32_t color,
									uint32_t size,
									uint32_t length,
									uint32_t width,
									uint32_t start_x,
									uint32_t start_y,

									
									
									const char *character)//�ⲿ���������
{
	graphic_data_struct_t *data_struct = &graphic->grapic_data_struct;
	for(char i=0;i<3;i++)
		data_struct->graphic_name[i] = name[i];	//�ַ�����
	data_struct->operate_tpye = operate_tpye; //ͼ�����
	data_struct->graphic_tpye = CHAR;         //Char��
	data_struct->layer = layer;//���ڵ����
	data_struct->color = color;//���ǰ�ɫ
	data_struct->start_angle = size;
	data_struct->end_angle = length;	
	data_struct->width = width;
	data_struct->start_x = start_x;
	data_struct->start_y = start_y;	
	
	data_struct->radius = 0;
	data_struct->end_x = 0;
	data_struct->end_y = 0;
	
	memcpy(graphic->data,empty_line,28);
	memcpy(graphic->data,character,length);
}
static void Draw_char()
{
	if(state_first_graphic == 0)//��֪��ʲôʱ�����ͻ�������Ҫ���ϸ���
	{
		Char_Graphic(&tx_client_char.clientData,"CL1",ADD,0,WHITE,10,strlen(first_line),1,(50),(1080*9/12),first_line);//x1920/18
		state_first_graphic = 1;
	}
	else if(state_first_graphic == 1)
	{
		Char_Graphic(&tx_client_char.clientData,"CL2",ADD,0,WHITE,10,strlen(second_line),1,(50),(1080*8/12),second_line);
		state_first_graphic = 2;
	}
	else if(state_first_graphic == 2)
	{
		Char_Graphic(&tx_client_char.clientData,"CL3",ADD,0,WHITE,10,strlen(third_line),1,(50),(1080*7/12),third_line);
		state_first_graphic = 3;
	}
	else if(state_first_graphic == 3)
	{
		Char_Graphic(&tx_client_char.clientData,"CL4",ADD,0,WHITE,10,strlen(fourth_line),1,(50),(1080*6/12),fourth_line);
		state_first_graphic = 4;
	}
	else if(state_first_graphic == 4)
	{
		//Char_Graphic(&tx_client_char.clientData,"CL5",ADD,0,WHITE,10,strlen(fifth_line),1,(1920-200),(1080*9/12),fifth_line);
		state_first_graphic = 5;
	}
	else if(state_first_graphic == 5)
	{
		//Char_Graphic(&tx_client_char.clientData,"CL6",ADD,0,WHITE,10,strlen(sixth_line),1,(1920-200),(1080*8/12),sixth_line);
		state_first_graphic = 6;
	}
	else if(state_first_graphic == 6)
	{
		Char_Graphic(&tx_client_char.clientData,"CL7",ADD,0,WHITE,10,strlen(seventh_line),1,(1920-200),(1080*7/12),seventh_line);
		state_first_graphic = 7;
	}
}
void Client_graphic_Init()
{
	if(state_first_graphic>=7)
	{
		state_first_graphic = 0;
	}
		//֡ͷ
		tx_client_char.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		tx_client_char.txFrameHeader.data_length = sizeof(ext_client_data_header_t) + sizeof(ext_client_string_t);
		tx_client_char.txFrameHeader.seq = 0;//�����
		memcpy(CliendTxBuffer,&tx_client_char.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//ͷУ��
	
		//������
		tx_client_char.CmdID = ID_robot_interactive_header_data;
		
		//���ݶ�ͷ�ṹ
		tx_client_char.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_char_graphic;
		tx_client_char.dataFrameHeader.send_ID     = Judge_Hero.robot_status.robot_id;
		tx_client_char.dataFrameHeader.receiver_ID = Judge_Hero.self_client;
		
		//���ݶ�
		Draw_char();
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_client_char.CmdID, LEN_CMD_ID+tx_client_char.txFrameHeader.data_length);//���������볤��2
		
		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(tx_client_char));
		
		Client_Sent_String(CliendTxBuffer, sizeof(tx_client_char));
}
//************************************��������*******************************/
ext_graphic_five_data_t tx_client_graphic_figure;
void Figure_Graphic(graphic_data_struct_t* graphic,//����Ҫ����ȥ����������ݶ�����
									const char* name,
									uint32_t operate_tpye,
									uint32_t graphic_tpye,//����ʲôͼ��
									uint32_t layer,
									uint32_t color,
									uint32_t start_angle,
									uint32_t end_angle,
									uint32_t width,
									uint32_t start_x,
									uint32_t start_y,
									uint32_t radius,
									uint32_t end_x,
									uint32_t end_y)							
{
	for(char i=0;i<3;i++)
		graphic->graphic_name[i] = name[i];	//�ַ�����
	graphic->operate_tpye = operate_tpye; //ͼ�����
	graphic->graphic_tpye = graphic_tpye;         //Char��
	graphic->layer        = layer;//���ڵ�һ��
	graphic->color        = color;//��ɫ
	graphic->start_angle  = start_angle;
	graphic->end_angle    = end_angle;	
	graphic->width        = width;
	graphic->start_x      = start_x;
	graphic->start_y      = start_y;	
	graphic->radius = radius;
	graphic->end_x  = end_x;
	graphic->end_y  = end_y;
}

uint8_t update_figure_flag;
static void readyfire_first_figure(bool fiction,bool clip)//Ħ���ִ�Ϊtrue���������Ϊtrue��
{
	if(fiction == true && clip == true)//��׼�����Ϊ��ɫ
		Figure_Graphic(&tx_client_graphic_figure.clientData[0],"GL1",update_figure_flag,CIRCLE,1,GREEN,0,0,5,  200,1080*9/12, 20,0,0);
	else if(fiction == true && clip == false)//������ˣ�����Ħ�����ѿ�������ɫΣ��
		Figure_Graphic(&tx_client_graphic_figure.clientData[0],"GL1",update_figure_flag,CIRCLE,1,ORANGE,0,0,5,  200,1080*9/12, 20,0,0);
	else if(fiction == false && clip == true)//��������ˣ�����Ħ����û�򿪣���ɫ�޷�����
		Figure_Graphic(&tx_client_graphic_figure.clientData[0],"GL1",update_figure_flag,CIRCLE,1,YELLOW,0,0,5,  200,1080*9/12, 20,0,0);
	else if(fiction == false && clip == false)//����򿪣�Ħ����û�п������Ϻ�ɫװ��
		Figure_Graphic(&tx_client_graphic_figure.clientData[0],"GL1",update_figure_flag,CIRCLE,1,FUCHSIA,0,0,5,  200,1080*9/12, 20,0,0);
}

static void spin_second_figure(bool spin)//С���ݴ�Ϊtrue
{
	if(spin == true)//��С����Ϊ��ɫ
		Figure_Graphic(&tx_client_graphic_figure.clientData[1],"GL2",update_figure_flag,CIRCLE,1,GREEN,0,0,5,  200,1080*8/12, 20,0,0);
	else if(spin == false)//û��С����Ϊ�Ϻ�ɫ
		Figure_Graphic(&tx_client_graphic_figure.clientData[1],"GL2",update_figure_flag,CIRCLE,1,FUCHSIA,0,0,5,  200,1080*8/12, 20,0,0);
}
static void auto_aim_third_figure(bool auto_aim)//�����Ϊtrue
{
	if(auto_aim == true)//������Ϊ��ɫ
		Figure_Graphic(&tx_client_graphic_figure.clientData[2],"GL3",update_figure_flag,CIRCLE,1,GREEN,0,0,5,  200,1080*7/12, 20,0,0);
	else if(auto_aim == false)//û������Ϊ�Ϻ�ɫ
		Figure_Graphic(&tx_client_graphic_figure.clientData[2],"GL3",update_figure_flag,CIRCLE,1,FUCHSIA,0,0,5,  200,1080*7/12, 20,0,0);
}
static void twist_fourth_figure(bool twist)//��Ť��Ϊtrue
{
	if(twist == true)
		Figure_Graphic(&tx_client_graphic_figure.clientData[3],"GL4",update_figure_flag,CIRCLE,1,GREEN,0,0,5,  200,1080*6/12, 20,0,0);
	else if(twist == false)
		Figure_Graphic(&tx_client_graphic_figure.clientData[3],"GL4",update_figure_flag,CIRCLE,1,FUCHSIA,0,0,5,  200,1080*6/12, 20,0,0);	
}
//static void sight_bead_fifsix_figrue(uint32_t x,uint32_t y)//���ƶ�׼�ģ���ǿ��ת����uint32_t1920*1080�в��ֵ����޷�����
//{
//	Figure_Graphic(&tx_client_graphic_figure.clientData[4],"GL5",update_figure_flag,LINE,1,FUCHSIA,0,0,3,  x-20,y+20  ,0,  x+20,y-20);
//	Figure_Graphic(&tx_client_graphic_figure.clientData[5],"GL6",update_figure_flag,LINE,1,FUCHSIA,0,0,3,  x-20,y-20  ,0,  x+20,y+20);
//}

//static void supercapacitor_seventh_figure(float remain_energy,uint32_t turning_point)//ʣ�೬�����ݣ���λ�ٷֱȣ�������ĳ�ٷֱȱ��ɫ
//{
//	uint32_t remaining = (uint32_t)remain_energy;//ǿ��ת��
//	if(remaining >= turning_point)//ֱ�߳���Ϊ3
//		Figure_Graphic(&tx_client_graphic_figure.clientData[6],"GL7",update_figure_flag,LINE,1,GREEN,0,0,10,(1920-350),585  ,0,  (1920-350)+remaining*3,585);
//	else if(remaining < turning_point)
//		Figure_Graphic(&tx_client_graphic_figure.clientData[6],"GL7",update_figure_flag,LINE,1,FUCHSIA,0,0,10,(1920-350),585  ,0,  (1920-350)+remaining*3,585);		
//}

static void anti_top_fifth_figure(bool anti_top)//������
{
	if(anti_top == true)
		Figure_Graphic(&tx_client_graphic_figure.clientData[4],"GL5",update_figure_flag,RECTANGLE,1,GREEN,0,0,5,  (1920-580),540+70, 0,1920-450,540-70);
	else if(anti_top == false)
		Figure_Graphic(&tx_client_graphic_figure.clientData[4],"GL5",update_figure_flag,RECTANGLE,1,FUCHSIA,0,0,5,  (1920-580),540+70, 0,1920-450,540-70);
}


static void Draw_Figure_bool()
{
	readyfire_first_figure(global_fiction,global_clip);
	spin_second_figure    (global_spin);
	auto_aim_third_figure (global_auto_aim);
	twist_fourth_figure   (global_twist);
	anti_top_fifth_figure (global_anti_top);
//	sight_bead_fifsix_figrue(global_sight_bead_x,global_sight_bead_y);
//	supercapacitor_seventh_figure(global_supercapacitor_remain,global_supercapacitor_point);
}
//ɾ��ͼ����Ϣ
ext_deleteLayer_data_t tx_client_delete;
void Client_graphic_delete_update(uint8_t delete_layer)//ɾ��ͼ����Ϣ
{
		//֡ͷ
		tx_client_delete.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		tx_client_delete.txFrameHeader.data_length = sizeof(ext_client_data_header_t) + sizeof(ext_client_custom_graphic_delete_t);
		tx_client_delete.txFrameHeader.seq = 0;//�����
		memcpy(CliendTxBuffer,&tx_client_delete.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//ͷУ��

		//������
		tx_client_delete.CmdID = ID_robot_interactive_header_data;

		//���ݶ�ͷ�ṹ
		tx_client_delete.dataFrameHeader.data_cmd_id = INTERACT_ID_delete_graphic;
		tx_client_delete.dataFrameHeader.send_ID     = Judge_Hero.robot_status.robot_id;
		tx_client_delete.dataFrameHeader.receiver_ID = Judge_Hero.self_client;
		
		//���ݶ�
		tx_client_delete.clientData.operate_type = ALL_delete;
		tx_client_delete.clientData.layer = delete_layer;
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_client_delete.CmdID, LEN_CMD_ID+tx_client_delete.txFrameHeader.data_length);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(tx_client_delete));
		
		Client_Sent_String(CliendTxBuffer, sizeof(tx_client_delete));
}
void Client_graphic_Info_update()//�߸�ͼ��һ�����
{
		//֡ͷ
		tx_client_graphic_figure.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		tx_client_graphic_figure.txFrameHeader.data_length = sizeof(ext_client_data_header_t) + sizeof(graphic_data_struct_t)*5;
		tx_client_graphic_figure.txFrameHeader.seq = 0;//�����
		memcpy(CliendTxBuffer,&tx_client_graphic_figure.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//ͷУ��

		//������
		tx_client_graphic_figure.CmdID = ID_robot_interactive_header_data;

		//���ݶ�ͷ�ṹ
		tx_client_graphic_figure.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_five_graphic;
		tx_client_graphic_figure.dataFrameHeader.send_ID     = Judge_Hero.robot_status.robot_id;
		tx_client_graphic_figure.dataFrameHeader.receiver_ID = Judge_Hero.self_client;
	
		//���ݶ�
		Draw_Figure_bool();
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_client_graphic_figure.CmdID, LEN_CMD_ID+tx_client_graphic_figure.txFrameHeader.data_length);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(tx_client_graphic_figure));
		
		Client_Sent_String(CliendTxBuffer, sizeof(tx_client_graphic_figure));

}
//*************************���Ʊ仯ͼ��*****************************************/

//void Client_Data_Info_update()
//{
//		
//}
ext_graphic_two_data_t tx_aim_figure;//�������׼��
uint8_t update_aim_flag;//1-add,3ɾ��
static void sight_bead_figrue(uint32_t x,uint32_t y)//���ƶ�׼�ģ���ǿ��ת����uint32_t1920*1080�в��ֵ����޷�����
{
	Figure_Graphic(&tx_aim_figure.clientData[0],"GR1",update_aim_flag,LINE,2,WHITE,0,0,3,  AIM_X,AIM_Y+20  ,0,  AIM_X,AIM_Y-20);//graphic_Remove
	Figure_Graphic(&tx_aim_figure.clientData[1],"GR2",update_aim_flag,LINE,2,WHITE,0,0,3,  AIM_X-20,AIM_Y	 ,0,  AIM_X+20,AIM_Y);
}
void Client_aim_update()//������ͼ��һ�����
{
		//֡ͷ
		tx_aim_figure.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		tx_aim_figure.txFrameHeader.data_length = sizeof(ext_client_data_header_t) + sizeof(graphic_data_struct_t)*2;
		tx_aim_figure.txFrameHeader.seq = 0;//�����
		memcpy(CliendTxBuffer,&tx_aim_figure.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//ͷУ��

		//������
		tx_aim_figure.CmdID = ID_robot_interactive_header_data;

		//���ݶ�ͷ�ṹ
		tx_aim_figure.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_two_graphic;
		tx_aim_figure.dataFrameHeader.send_ID     = Judge_Hero.robot_status.robot_id;
		tx_aim_figure.dataFrameHeader.receiver_ID = Judge_Hero.self_client;
	
		//���ݶ�
		sight_bead_figrue(global_sight_bead_x,global_sight_bead_y);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_aim_figure.CmdID, LEN_CMD_ID+tx_aim_figure.txFrameHeader.data_length);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(tx_aim_figure));
		
		Client_Sent_String(CliendTxBuffer, sizeof(tx_aim_figure));
}
//ʣ�����ֻ��һ��ͼ��
ext_graphic_one_data_t tx_supercapacitor_figure;
uint8_t update_supercapacitor_flag;
static void supercapacitor_figure(float remain_energy,uint32_t turning_point)//ʣ�೬�����ݣ���λ�ٷֱȣ�������ĳ�ٷֱȱ��ɫ
{
	uint32_t remaining = (uint32_t)remain_energy;//ǿ��ת��
	if(remaining >= turning_point)//ֱ�߳���Ϊ3
		Figure_Graphic(&tx_supercapacitor_figure.clientData,"SR1",update_supercapacitor_flag,LINE,3,GREEN,0,0,10,(1920-350),585  ,0,  (1920-350)+remaining*3,585);
	else if(remaining < turning_point)
		Figure_Graphic(&tx_supercapacitor_figure.clientData,"SR1",update_supercapacitor_flag,LINE,3,FUCHSIA,0,0,10,(1920-350),585  ,0,  (1920-350)+remaining*3,585);		
}
void Client_supercapacitor_update()//һ��ͼ�����
{
		//֡ͷ
		tx_supercapacitor_figure.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		tx_supercapacitor_figure.txFrameHeader.data_length = sizeof(ext_client_data_header_t) + sizeof(graphic_data_struct_t);
		tx_supercapacitor_figure.txFrameHeader.seq = 0;//�����
		memcpy(CliendTxBuffer,&tx_supercapacitor_figure.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//ͷУ��

		//������
		tx_supercapacitor_figure.CmdID = ID_robot_interactive_header_data;

		//���ݶ�ͷ�ṹ
		tx_supercapacitor_figure.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_one_graphic;
		tx_supercapacitor_figure.dataFrameHeader.send_ID     = Judge_Hero.robot_status.robot_id;
		tx_supercapacitor_figure.dataFrameHeader.receiver_ID = Judge_Hero.self_client;
	
		//���ݶ�
		supercapacitor_figure(global_supercapacitor_remain,global_supercapacitor_point);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_supercapacitor_figure.CmdID, LEN_CMD_ID+tx_supercapacitor_figure.txFrameHeader.data_length);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(tx_supercapacitor_figure));
		
		Client_Sent_String(CliendTxBuffer, sizeof(tx_supercapacitor_figure));
}
//******************���Ƹ�����*************************/
//�����ͼ��
ext_float_two_data_t tx_gimbal_angle;
uint8_t update_float_flag;
void Float_Graphic(Float_data_struct_t* graphic,//����Ҫ����ȥ����������ݶ�����
									const char* name,
									uint32_t operate_tpye,
									uint32_t graphic_tpye,//����ʲôͼ��
									uint32_t layer,
									uint32_t color,
									uint32_t size,
									uint32_t decimal,
									uint32_t width,
									uint32_t start_x,
									uint32_t start_y,
									int number)							
{
	for(char i=0;i<3;i++)
		graphic->graphic_name[i] = name[i];	//�ַ�����
	graphic->operate_tpye = operate_tpye; //ͼ�����
	graphic->graphic_tpye = graphic_tpye;  
	graphic->layer        = layer;//
	graphic->color        = color;//��ɫ
	graphic->start_angle  = size;
	graphic->end_angle    = decimal;//С����Чλ	
	graphic->width        = width;
	graphic->start_x      = start_x;
	graphic->start_y      = start_y;	
	graphic->number       = number;
}
static void gimbal_angle_float(float gimbal_pitch,float gimbal_yaw)//��ǰ��̨�Ƕ�
{
	//��ɫpitch��һ�У���ɫyaw�ڶ���
		Float_Graphic(&tx_gimbal_angle.clientData[0],"FR1",update_float_flag,FLOAT,4,CYAN_BLUE,30,2,3,(1920*4/6),810  ,(int)(gimbal_pitch*1000));
		Float_Graphic(&tx_gimbal_angle.clientData[1],"FR2",update_float_flag,FLOAT,4,YELLOW,   30,2,3,(1920*4/6),760  ,(int)(gimbal_yaw*1000));		
}
void Client_gimbal_angle_update()//����ͼ�����
{
		//֡ͷ
		tx_gimbal_angle.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		tx_gimbal_angle.txFrameHeader.data_length = sizeof(ext_client_data_header_t) + sizeof(graphic_data_struct_t)*2;
		tx_gimbal_angle.txFrameHeader.seq = 0;//�����
		memcpy(CliendTxBuffer,&tx_gimbal_angle.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//ͷУ��

		//������
		tx_gimbal_angle.CmdID = ID_robot_interactive_header_data;

		//���ݶ�ͷ�ṹ
		tx_gimbal_angle.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_two_graphic;
		tx_gimbal_angle.dataFrameHeader.send_ID     = Judge_Hero.robot_status.robot_id;
		tx_gimbal_angle.dataFrameHeader.receiver_ID = Judge_Hero.self_client;
	
		//���ݶ�
		gimbal_angle_float(global_gimbal_pitch,global_gimbal_yaw);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_gimbal_angle.CmdID, LEN_CMD_ID+tx_gimbal_angle.txFrameHeader.data_length);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(tx_gimbal_angle));
		
		Client_Sent_String(CliendTxBuffer, sizeof(tx_gimbal_angle));
}
//**********************����int����***************************/
ext_int_two_data_t tx_bullet_int;
uint8_t update_int_flag;
void Int_Graphic(Int_data_struct_t* graphic,//����Ҫ����ȥ����������ݶ�����
									const char* name,
									uint32_t operate_tpye,
									uint32_t graphic_tpye,//����ʲôͼ��
									uint32_t layer,
									uint32_t color,
									uint32_t size,
									uint32_t zero,
									uint32_t width,
									uint32_t start_x,
									uint32_t start_y,
									int number)							
{
	for(char i=0;i<3;i++)
		graphic->graphic_name[i] = name[i];	//�ַ�����
	graphic->operate_tpye = operate_tpye; //ͼ�����
	graphic->graphic_tpye = graphic_tpye;        
	graphic->layer        = layer;//���ڵ�һ��
	graphic->color        = color;//��ɫ
	graphic->start_angle  = size;
	graphic->end_angle    = zero;	
	graphic->width        = width;
	graphic->start_x      = start_x;
	graphic->start_y      = start_y;	
	graphic->number       = number;
}
static void bullet_int(int bullet_speed,int bullet_sum)//�ӵ����ٺͷ�����
{
	//��������һ�У����ٵڶ���
		Int_Graphic(&tx_bullet_int.clientData[0],"IR1",update_int_flag,INT,5,WHITE,30,0,3,(1920-280),(820)  ,(int)bullet_sum);
		Int_Graphic(&tx_bullet_int.clientData[1],"IR2",update_int_flag,INT,5,WHITE,30,0,3,(1920-280),(730)  ,(int)bullet_speed);		
}
void Client_bullet_int_update()//����ͼ�����
{
		//֡ͷ
		tx_bullet_int.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		tx_bullet_int.txFrameHeader.data_length = sizeof(ext_client_data_header_t) + sizeof(graphic_data_struct_t)*2;
		tx_bullet_int.txFrameHeader.seq = 0;//�����
		memcpy(CliendTxBuffer,&tx_bullet_int.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//ͷУ��

		//������
		tx_bullet_int.CmdID = ID_robot_interactive_header_data;

		//���ݶ�ͷ�ṹ
		tx_bullet_int.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_two_graphic;
		tx_bullet_int.dataFrameHeader.send_ID     = Judge_Hero.robot_status.robot_id;
		tx_bullet_int.dataFrameHeader.receiver_ID = Judge_Hero.self_client;
	
		//���ݶ�
		bullet_int(global_bullet_speed,global_bullet_sum);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_bullet_int.CmdID, LEN_CMD_ID+tx_bullet_int.txFrameHeader.data_length);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(tx_bullet_int));
		
		Client_Sent_String(CliendTxBuffer, sizeof(tx_bullet_int));
}
//*****************************Ӣ������*************************************/





ext_graphic_seven_data_t high_aim_figure;//������׼��֮��,������
ext_graphic_seven_data_t low_aim_shortfigure_1;//׼���µĵ�һ������
ext_graphic_two_data_t low_aim_shortfigure_2;
ext_graphic_two_data_t  low_aim_shortfigure_3;//��������
ext_graphic_five_data_t  low_aim_longfigure;//׼���µĳ�����
//!!!!!!!!!!!!!!!!!!ȫ�ֱ���
uint32_t division_value = 10;
//division_value�ֶ�ֵ,line_length���߳��ȵ�һ��
static void aim_1(uint32_t division_value,uint32_t line_length)//׼���ϰ벿�ֵĿ��"AH"--aim_high
{
	Figure_Graphic(&high_aim_figure.clientData[0],"AH1",ADD,LINE,3,YELLOW,0,0,2,  AIM_X-line_length, AIM_Y+30                 ,0,  AIM_X+line_length, AIM_Y+30);//graphic_Remove
	Figure_Graphic(&high_aim_figure.clientData[1],"AH2",ADD,LINE,3,YELLOW,0,0,2,  AIM_X-line_length, AIM_Y+30+division_value  ,0,  AIM_X+line_length, AIM_Y+30+division_value  );
	Figure_Graphic(&high_aim_figure.clientData[2],"AH3",ADD,LINE,3,YELLOW,0,0,2,  AIM_X-line_length, AIM_Y+30+division_value*2,0,  AIM_X+line_length, AIM_Y+30+division_value*2);
	Figure_Graphic(&high_aim_figure.clientData[3],"AH4",ADD,LINE,3,YELLOW,0,0,2,  AIM_X-line_length, AIM_Y+30+division_value*3,0,  AIM_X+line_length, AIM_Y+30+division_value*3);
	Figure_Graphic(&high_aim_figure.clientData[4],"AH5",ADD,LINE,3,YELLOW,0,0,2,  AIM_X-line_length, AIM_Y+30+division_value*4,0,  AIM_X+line_length, AIM_Y+30+division_value*4);
	Figure_Graphic(&high_aim_figure.clientData[5],"AH6",ADD,LINE,3,YELLOW,0,0,2,  AIM_X-line_length, AIM_Y+30+division_value*5,0,  AIM_X+line_length, AIM_Y+30+division_value*5);
	Figure_Graphic(&high_aim_figure.clientData[6],"AH7",ADD,LINE,3,YELLOW,0,0,2,  AIM_X-line_length, AIM_Y+30+division_value*6,0,  AIM_X+line_length, AIM_Y+30+division_value*6);
}

void _high_aim_(void)
{
		//֡ͷ
		high_aim_figure.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		high_aim_figure.txFrameHeader.data_length = sizeof(ext_client_data_header_t) + sizeof(graphic_data_struct_t)*7;
		high_aim_figure.txFrameHeader.seq = 0;//�����
		memcpy(CliendTxBuffer,&high_aim_figure.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//ͷУ��

		//������
		high_aim_figure.CmdID = ID_robot_interactive_header_data;

		//���ݶ�ͷ�ṹ
		high_aim_figure.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_seven_graphic;
		high_aim_figure.dataFrameHeader.send_ID     = Judge_Hero.robot_status.robot_id;
		high_aim_figure.dataFrameHeader.receiver_ID = Judge_Hero.self_client;
	
		//���ݶ�
		aim_1(division_value,10);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&high_aim_figure.CmdID, LEN_CMD_ID+high_aim_figure.txFrameHeader.data_length);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(high_aim_figure));
		
		Client_Sent_String(CliendTxBuffer, sizeof(high_aim_figure));
}

static void aim_lowshort_2(uint32_t division_value,uint32_t line_length)//׼���ϰ벿�ֵĿ��"AL"--aim_low
{
	Figure_Graphic(&low_aim_shortfigure_1.clientData[0],"AL1",ADD,LINE,3,ORANGE		,0,0,1,  AIM_X-line_length,	AIM_Y-30                 ,0,  AIM_X+line_length,	AIM_Y-30);//graphic_Remove
	Figure_Graphic(&low_aim_shortfigure_1.clientData[1],"AL2",ADD,LINE,3,ORANGE		,0,0,1,  AIM_X-line_length,	AIM_Y-30-division_value  ,0,  AIM_X+line_length,	AIM_Y-30-division_value  );
	Figure_Graphic(&low_aim_shortfigure_1.clientData[2],"AL3",ADD,LINE,3,ORANGE		,0,0,1,  AIM_X-line_length,	AIM_Y-30-division_value*2,0,  AIM_X+line_length,	AIM_Y-30-division_value*2);
	Figure_Graphic(&low_aim_shortfigure_1.clientData[3],"AL4",ADD,LINE,3,ORANGE		,0,0,1,  AIM_X-line_length,	AIM_Y-30-division_value*4,0,  AIM_X+line_length,	AIM_Y-30-division_value*4);
	Figure_Graphic(&low_aim_shortfigure_1.clientData[4],"AL5",ADD,LINE,3,ORANGE		,0,0,1,  AIM_X-line_length,	AIM_Y-30-division_value*5,0,  AIM_X+line_length,	AIM_Y-30-division_value*5);
	Figure_Graphic(&low_aim_shortfigure_1.clientData[5],"AL6",ADD,LINE,3,CYAN_BLUE,0,0,1,  AIM_X-line_length,	AIM_Y-30-division_value*6,0,  AIM_X+line_length,	AIM_Y-30-division_value*6);
	Figure_Graphic(&low_aim_shortfigure_1.clientData[6],"AL7",ADD,LINE,3,GREEN		,0,0,1,  AIM_X-line_length,	AIM_Y-30-division_value*8,0,  AIM_X+line_length,	AIM_Y-30-division_value*8);
}
void _lowshort_aim_2()
{
		//֡ͷ
		low_aim_shortfigure_1.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		low_aim_shortfigure_1.txFrameHeader.data_length = sizeof(ext_client_data_header_t) + sizeof(graphic_data_struct_t)*7;
		low_aim_shortfigure_1.txFrameHeader.seq = 0;//�����
		memcpy(CliendTxBuffer,&low_aim_shortfigure_1.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//ͷУ��

		//������
		low_aim_shortfigure_1.CmdID = ID_robot_interactive_header_data;

		//���ݶ�ͷ�ṹ
		low_aim_shortfigure_1.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_seven_graphic;
		low_aim_shortfigure_1.dataFrameHeader.send_ID     = Judge_Hero.robot_status.robot_id;
		low_aim_shortfigure_1.dataFrameHeader.receiver_ID = Judge_Hero.self_client;
	
		//���ݶ�
		aim_lowshort_2(division_value,10);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&low_aim_shortfigure_1.CmdID, LEN_CMD_ID+low_aim_shortfigure_1.txFrameHeader.data_length);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(low_aim_shortfigure_1));
		
		Client_Sent_String(CliendTxBuffer, sizeof(low_aim_shortfigure_1));
}
static void aim_lowshort_3(uint32_t division_value,uint32_t line_length)//׼���ϰ벿�ֵĿ��"AM"--aim_low_middle
{
	Figure_Graphic(&low_aim_shortfigure_2.clientData[0],"AM1",ADD,LINE,3,WHITE,0,0,1,  AIM_X-line_length,AIM_Y-30-division_value*9 ,0,  AIM_X+line_length,AIM_Y-30-division_value*9 );//graphic_Remove
	Figure_Graphic(&low_aim_shortfigure_2.clientData[1],"AM2",ADD,LINE,3,WHITE,0,0,1,  AIM_X-line_length,AIM_Y-30-division_value*10,0,  AIM_X+line_length,AIM_Y-30-division_value*10);
//	Figure_Graphic(&low_aim_shortfigure_2.clientData[2],"AM3",ADD,LINE,3,YELLOW,0,0,3,  960-line_length,540-30-division_value*12,0,  960+line_length,540-30-division_value*12);
//	Figure_Graphic(&low_aim_shortfigure_2.clientData[3],"AM4",ADD,LINE,3,YELLOW,0,0,3,  960-line_length,540-30-division_value*13,0,  960+line_length,540-30-division_value*13);
//	Figure_Graphic(&low_aim_shortfigure_2.clientData[4],"AM5",ADD,LINE,3,YELLOW,0,0,3,  960-line_length,540-30-division_value*14,0,  960+line_length,540-30-division_value*14);
//	Figure_Graphic(&low_aim_shortfigure_2.clientData[5],"AM6",ADD,LINE,3,YELLOW,0,0,3,  960-line_length,540-30-division_value*16,0,  960+line_length,540-30-division_value*16);
//	Figure_Graphic(&low_aim_shortfigure_2.clientData[6],"AM7",ADD,LINE,3,YELLOW,0,0,3,  960-line_length,540-30-division_value*17,0,  960+line_length,540-30-division_value*17);
}
void _lowshort_aim_3()
{
		//֡ͷ
		low_aim_shortfigure_2.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		low_aim_shortfigure_2.txFrameHeader.data_length = sizeof(ext_client_data_header_t) + sizeof(graphic_data_struct_t)*2;
		low_aim_shortfigure_2.txFrameHeader.seq = 0;//�����
		memcpy(CliendTxBuffer,&low_aim_shortfigure_2.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//ͷУ��

		//������
		low_aim_shortfigure_2.CmdID = ID_robot_interactive_header_data;

		//���ݶ�ͷ�ṹ
		low_aim_shortfigure_2.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_two_graphic;
		low_aim_shortfigure_2.dataFrameHeader.send_ID     = Judge_Hero.robot_status.robot_id;
		low_aim_shortfigure_2.dataFrameHeader.receiver_ID = Judge_Hero.self_client;
	
		//���ݶ�
		aim_lowshort_3(division_value,10);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&low_aim_shortfigure_2.CmdID, LEN_CMD_ID+low_aim_shortfigure_2.txFrameHeader.data_length);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(low_aim_shortfigure_2));
		
		Client_Sent_String(CliendTxBuffer, sizeof(low_aim_shortfigure_2));
}
//stem--��
static void aim_lowshort_stem(uint32_t division_value,uint32_t line_length)//׼���ϰ벿�ֵĿ��"AM"--aim_low_bottom,"AS"--aim_stem
{ 
//	Figure_Graphic(&low_aim_shortfigure_3.clientData[0],"AB1",ADD,LINE,3,YELLOW,0,0,3,   960-line_length,540-30-division_value*18,0,   960+line_length,540-30-division_value*18);//graphic_Remove
//	Figure_Graphic(&low_aim_shortfigure_3.clientData[1],"AB2",ADD,LINE,3,YELLOW,0,0,3,   960-line_length,540-30-division_value*20,0,   960+line_length,540-30-division_value*20);
//	Figure_Graphic(&low_aim_shortfigure_3.clientData[2],"AB3",ADD,LINE,3,YELLOW,0,0,3,   960-line_length,540-30-division_value*21,0,   960+line_length,540-30-division_value*21);
//	Figure_Graphic(&low_aim_shortfigure_3.clientData[3],"AB4",ADD,LINE,3,YELLOW,0,0,3,   960-line_length,540-30-division_value*22,0,   960+line_length,540-30-division_value*22);
//	Figure_Graphic(&low_aim_shortfigure_3.clientData[4],"AB5",ADD,LINE,3,YELLOW,0,0,3,	 960-line_length-10,540-30-division_value*23,0,960+line_length+10,540-30-division_value*23);
	
	Figure_Graphic(&low_aim_shortfigure_3.clientData[0],"AS1",ADD,LINE,3,YELLOW,0,0,1,   AIM_X,            AIM_Y+30+division_value*6 ,0,   AIM_X,            AIM_Y+30);
	Figure_Graphic(&low_aim_shortfigure_3.clientData[1],"AS2",ADD,LINE,3,YELLOW,0,0,1,   AIM_X,            AIM_Y-30-division_value*22,0,   AIM_X,            AIM_Y-30);

}
void _lowshortstem_aim_4()
{
		//֡ͷ
		low_aim_shortfigure_3.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		low_aim_shortfigure_3.txFrameHeader.data_length = sizeof(ext_client_data_header_t) + sizeof(graphic_data_struct_t)*2;
		low_aim_shortfigure_3.txFrameHeader.seq = 0;//�����
		memcpy(CliendTxBuffer,&low_aim_shortfigure_3.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//ͷУ��

		//������
		low_aim_shortfigure_3.CmdID = ID_robot_interactive_header_data;

		//���ݶ�ͷ�ṹ
		low_aim_shortfigure_3.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_two_graphic;
		low_aim_shortfigure_3.dataFrameHeader.send_ID     = Judge_Hero.robot_status.robot_id;
		low_aim_shortfigure_3.dataFrameHeader.receiver_ID = Judge_Hero.self_client;
	
		//���ݶ�
		aim_lowshort_stem(division_value,10);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&low_aim_shortfigure_3.CmdID, LEN_CMD_ID+low_aim_shortfigure_3.txFrameHeader.data_length);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(low_aim_shortfigure_3));
		
		Client_Sent_String(CliendTxBuffer, sizeof(low_aim_shortfigure_3));
}
//ͼ����
static void aim_lowlong(uint32_t division_value,uint32_t line_length)//׼���ϰ벿�ֵĿ��"AM"--aim_low_Long,"AS"--aim_stem
{ 
	Figure_Graphic(&low_aim_longfigure.clientData[0],"AL1",DELETE,LINE,4,YELLOW,0,0,1,AIM_X-line_length-30,AIM_Y-30-division_value*19,0,AIM_X+line_length+30,AIM_Y-30-division_value*19);//graphic_Remove
	Figure_Graphic(&low_aim_longfigure.clientData[1],"AL2",DELETE,LINE,4,YELLOW,0,0,1,AIM_X-line_length-40,AIM_Y-30-division_value*15,0,AIM_X+line_length+40,AIM_Y-30-division_value*15);
	Figure_Graphic(&low_aim_longfigure.clientData[2],"AL3",ADD,LINE,4,WHITE,0,0,2,		AIM_X-line_length-50,AIM_Y-30-division_value*11,0,AIM_X+line_length+50,AIM_Y-30-division_value*11);
	Figure_Graphic(&low_aim_longfigure.clientData[3],"AL4",ADD,LINE,4,GREEN,0,0,2,		AIM_X-line_length-60,AIM_Y-30-division_value*7 ,0,AIM_X+line_length+60,AIM_Y-30-division_value*7 );
	Figure_Graphic(&low_aim_longfigure.clientData[4],"AL5",ADD,LINE,4,ORANGE,0,0,2,		AIM_X-line_length-70,AIM_Y-30-division_value*3 ,0,AIM_X+line_length+70,AIM_Y-30-division_value*3 );
	
}
void _lowlong_aim_()
{
		//֡ͷ
		low_aim_longfigure.txFrameHeader.sof = JUDGE_FRAME_HEADER;
		low_aim_longfigure.txFrameHeader.data_length = sizeof(ext_client_data_header_t) + sizeof(graphic_data_struct_t)*5;
		low_aim_longfigure.txFrameHeader.seq = 0;//�����
		memcpy(CliendTxBuffer,&low_aim_longfigure.txFrameHeader,sizeof(std_frame_header_t));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(std_frame_header_t));//ͷУ��

		//������
		low_aim_longfigure.CmdID = ID_robot_interactive_header_data;

		//���ݶ�ͷ�ṹ
		low_aim_longfigure.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_five_graphic;
		low_aim_longfigure.dataFrameHeader.send_ID     = Judge_Hero.robot_status.robot_id;
		low_aim_longfigure.dataFrameHeader.receiver_ID = Judge_Hero.self_client;
	
		//���ݶ�
		aim_lowlong(division_value,10);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&low_aim_longfigure.CmdID, LEN_CMD_ID+low_aim_longfigure.txFrameHeader.data_length);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(low_aim_longfigure));
		
		Client_Sent_String(CliendTxBuffer, sizeof(low_aim_longfigure));
}


void Client_draw_aim(void)
{
	
}

///////////////////////////////////////////��������

void Client_flag_update(void)
{
	global_fiction = Slaver_flag.global_fiction;
	global_clip = Slaver_flag.global_clip;
	global_spin = Slaver_flag.global_spin;
	global_auto_aim = Slaver_flag.global_auto_aim;
	global_anti_top = Slaver_flag.global_anti_top;
	global_twist = Slaver_flag.global_twist;
	global_supercapacitor_remain = (SC_R_DATA.volt-14.0f)*10;
	if(global_supercapacitor_remain>100)
	{
		global_supercapacitor_remain = 100;
	}
}

void PITCH_YAW_Analyze(u8 *buf)
{
	float pFloat[2];
	
	uint8_t pChar[8];
	uint8_t i, temp;
	
	memcpy(pFloat, buf, 8);
	
	global_gimbal_pitch = pFloat[0];
	global_gimbal_yaw = pFloat[1];
}

uint8_t Client_circle = 10;
void Client_task(void)
{
	static uint32_t i;
	
	Client_flag_update();
	if(i%Client_circle == 0)
	{
			Client_graphic_Init();//����һֱ���£������޷��ж�ʲôʱ�����ͻ���������Ҫ��ѯ,���Բ�����key����
	}
	else if(i%Client_circle == 1)
	{
		Client_graphic_Info_update();
		update_figure_flag = MODIFY;
	}
	else if(i%Client_circle == 4)
	{
		Client_supercapacitor_update();
		update_supercapacitor_flag = MODIFY;
	}
	else if(i%Client_circle == 2)
	{
		Client_aim_update();
		update_aim_flag = MODIFY;
	}
	else if(i%Client_circle == 3)
	{
		Client_gimbal_angle_update();
		update_float_flag = MODIFY;
	}

	else if(i%Client_circle == 5)
	{
		//Client_bullet_int_update();
		update_int_flag = MODIFY;
	}
	
 //׼�ǲ���
	if(i%200 == 1)
	{
		_lowshortstem_aim_4();
	}
	else if(i%200 == 10)
	{
		_high_aim_();
	}
	else if(i%200 == 20)
	{
		_lowshort_aim_2();
	}
	else if(i%200 == 30)
	{
		_lowshort_aim_3();
	}
	else if(i%200 == 40)
	{
		_lowlong_aim_();
	}
	
	
	if(i%100 == 0)
	{
		update_figure_flag = ADD;
		update_aim_flag = ADD;
		update_float_flag = ADD;
		update_supercapacitor_flag = ADD;
		update_int_flag = ADD;
		
	}
	
	i++;
}
