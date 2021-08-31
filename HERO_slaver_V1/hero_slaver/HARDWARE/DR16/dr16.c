#include "dr16.h"

DR16_Module DR16_Data_struct;
RC_Module DJI_RC1;

volatile u8 DR16_DataBuf[2][18];

void RC_Init(u32 bound)
{
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE); //ʹ��DMA1ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
 
	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3����ΪUSART2
	
	//USART2�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //������
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA3

   //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx;	//����ģʽ
  USART_Init(USART2, &USART_InitStructure); //��ʼ������2

	
  USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); //ʹ�ܴ��ڵ�DMAģʽ
	
		
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;//DMA1��5 �ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;													//ѡ��ͨ��4
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(USART2->DR);				//DMA�����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&DR16_DataBuf[0][0];			//�洢����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;									//���赽�洢��ģʽ
	DMA_InitStructure.DMA_BufferSize = 18;																	//���ν��յ���֡����Ŀ
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
	
	//���ݵ�ǰ������״̬����һ���л���Ȼ������һ����������ַд��
	//DMA_DoubleBufferModeConfig(DMA1_Stream5,(uint32_t)(&DR16_DataBuf[1][0]), DMA_Memory_0);	//˫������
	//DMA_DoubleBufferModeCmd(DMA1_Stream5, ENABLE); 													//˫������ģʽ����
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);															//DMA1��ʼ��
	

	
	DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Stream5, ENABLE);
	
}

u32 cnt2 = 0;


void DMA1_Stream5_IRQHandler(void)
{
	
	cnt2++;
	if(DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5)!=RESET)//��־�ڱ�����ǰһֱ��Ч
	{
		
		cnt2++;
		DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
		DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);

		if(DMA_GetCurrentMemoryTarget(DMA1_Stream5) == 0)  //�洢��0
		{
			get_dr16_data(&(DJI_RC1.rece_Data), DR16_DataBuf[0]);
			DJI_RC1.Judge.cycle_cnt = 0 ;
		}
	}

}



void get_dr16_data(DR16_Module *dr16, volatile unsigned char *pData)
{
	
	short int temp;
	
	temp = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
	dr16->ch0 = temp-1024;
	
	temp = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
	dr16->ch1 = temp-1024;
	
	temp = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) | ((int16_t)pData[4] << 10)) & 0x07FF;
	dr16->ch2 = temp-1024;
	
	temp = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
  dr16->ch3 = temp-1024;    
	
	
	dr16->sw1 = ((pData[5] >> 4) & 0x000C) >> 2;
	dr16->sw2	= ((pData[5] >> 4) & 0x0003); 
  dr16->mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);
	dr16->mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8); 
	dr16->mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);     
  dr16->mouse.l = pData[12];
	dr16->mouse.r = pData[13];
	dr16->kb.key_code = ((int16_t)pData[14]);
	
	temp = ((int16_t)pData[16] | ((int16_t)pData[17] << 8)) & 0x07FF;
	dr16->wheel = temp-1024; 
	
}


u8 DR_data_check(DR16_Module *dr16)
{
	if(dr16->ch0 < -660 || dr16->ch0 > 660)
		return 1;
	if(dr16->ch1 < -660 || dr16->ch1 > 660)
		return 1;
	if(dr16->ch2 < -660 || dr16->ch2 > 660)
		return 1;
	if(dr16->ch3 < -660 || dr16->ch3 > 660)
		return 1;
	
	if(dr16->sw1 == 0 || dr16->sw1>3)
		return 1;
	if(dr16->sw2 == 0 || dr16->sw2>3)
		return 1;
	
	return 0;
}

void RC_data_Init(DR16_Module *dr16)
{
	
	dr16->ch0 = 0;
	dr16->ch1 = 0;
	dr16->ch2 = 0;
	dr16->ch3 = 0;
	
	dr16->sw1 = 0;
	dr16->sw2 = 0;
	
	dr16->mouse.x = 0;
	dr16->mouse.y = 0;
	dr16->mouse.z = 0;
	dr16->mouse.r = 0;
	dr16->mouse.l = 0;
	
	dr16->kb.key_code = 0;
	
}

u8 RC_Reconnecting_func(void)
{
//	PID_Set(&(Gimbal.Yaw_Motor.speed_PID), 10, 6, 0, 60, 0, 28000);//�ٶȻ�PIDҪ����������ٶ�ʹ��
//	PID_Set(&(Gimbal.Yaw_Motor.angle_PID), 8, 0, 0, 0, 0, 1000);
//	PID_Set(&(Gimbal.Pitch_Motor.speed_PID), 11, 1, 0, 350, 3, 28000);
//	PID_Set(&(Gimbal.Pitch_Motor.angle_PID), 10, 1, 0, 60, 5, 1000);
	
	/////////////////////////////////  ��̨��������
	
	Gimbal.Yaw_Motor.angle_PID.output_limit = 1000;
	Gimbal.Pitch_Motor.angle_PID.output_limit = 1000;
	
	Gimbal.Yaw_Motor.angle_PID.startfalg = PID_ENABLE;
	Gimbal.Pitch_Motor.angle_PID.startfalg = PID_ENABLE;
	
	Motor_set_angle(&(Gimbal.Pitch_Motor), 2000);
	Motor_set_angle(&(Gimbal.Yaw_Motor), 2500);
	
	Gimbal_PID_Cal(0); //PITCH��
	Gimbal_PID_Cal(1);//YAW����
	Gimbal_Set_Output(Gimbal.Yaw_Motor.output, Gimbal.Pitch_Motor.output);
	
	CAN1_Send_Msg2(CAN1_0X2ff_BUF, 8, 0x2ff);
	
	Hero_Launcher.Friction.Target_Speed = 1000; // Ħ��������Ŀ��Ϊ������
	
	if(Gimbal.Yaw_Motor.angle_PID.err <= 20 && Gimbal.Pitch_Motor.angle_PID.err <= 20
		&& Gimbal.Yaw_Motor.angle_PID.err >= -20 && Gimbal.Pitch_Motor.angle_PID.err >= -20)//��������
	{
		
		PID_Set(&(Gimbal.Yaw_Motor.speed_PID), 10, 6, 0, 60, 0, 28000);
		PID_Set(&(Gimbal.Yaw_Motor.angle_PID), 9, 1, 0, 60, 0, 8000);
		PID_Set(&(Gimbal.Pitch_Motor.speed_PID), 11, 1, 0, 350, 3, 28000);
		PID_Set(&(Gimbal.Pitch_Motor.angle_PID), 10, 1, 0, 60, 5, 8000);
		
		Gimbal.Pitch_Motor.output = 0;
		Gimbal.Yaw_Motor.output = 0;
		
		Gimbal_Set_Output(Gimbal.Yaw_Motor.output, Gimbal.Pitch_Motor.output);
		
		return 1;
	}
	
	return 0;
}


