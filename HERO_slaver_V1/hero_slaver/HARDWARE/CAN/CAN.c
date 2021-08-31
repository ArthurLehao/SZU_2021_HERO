#include "CAN.h"



//CAN��ʼ��  ��ʼ��˳��IO���á�CAN�Ĵ������á����������á��ж�����
//tsjw ����ͬ����Ծ��Ԫ�� 1~4tq
//tbs2 ʱ���2��ʱ�䵥Ԫ  1~8tq
//tbs1 ʱ���1��ʱ�䵥Ԫ  1~16tq
//brp  �����ʷ�Ƶ�� 1~1024��config���������Ѿ�������һ���������Դ˴�����Ҫ��һ��
//42/��1+4+9��/3 = 1M
//mode CAN_Mode_Normal ��ͨģʽ CAN_Mode_Silent_LoopBack��Ĭ�ػ�ģʽ
u8 CAN1_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode)// CANģʽ��ʼ��
{
	GPIO_InitTypeDef CAN_Handle;
	CAN_InitTypeDef CAN_Init_Handle;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_Initure;
	
//������CAN����	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
//	
//	
//	CAN_Handle.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
//	CAN_Handle.GPIO_Mode = GPIO_Mode_AF;//����ģʽ
//	CAN_Handle.GPIO_OType = GPIO_OType_PP;//��©���
//	CAN_Handle.GPIO_PuPd = GPIO_PuPd_UP;//��������
//	CAN_Handle.GPIO_Speed = GPIO_Speed_100MHz;//
//	GPIO_Init(GPIOA, &CAN_Handle);
//	
//	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);//����CAN1���
//	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);//����CAN1���
	
	
//������CAN����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
	
	
	CAN_Handle.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
	CAN_Handle.GPIO_Mode = GPIO_Mode_AF;//����ģʽ
	CAN_Handle.GPIO_OType = GPIO_OType_PP;//��©���
	CAN_Handle.GPIO_PuPd = GPIO_PuPd_UP;//��������
	CAN_Handle.GPIO_Speed = GPIO_Speed_100MHz;//
	GPIO_Init(GPIOD, &CAN_Handle);
	
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);//����CAN1���
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);//����CAN1���
	
	//CAN��Ԫ����
	CAN_Init_Handle.CAN_TTCM = DISABLE; //��ʱ�䴥��ͨ��ģʽ
	CAN_Init_Handle.CAN_ABOM = ENABLE;	//�Զ����߻ָ�
	CAN_Init_Handle.CAN_AWUM = DISABLE; //˯��ģʽͨ��������ѣ����CAN->MCR�ı�־λ��
	CAN_Init_Handle.CAN_NART = DISABLE;  //��ֹ�����Զ�����
	CAN_Init_Handle.CAN_RFLM = DISABLE; //���Ĳ�����
	CAN_Init_Handle.CAN_TXFP = ENABLE; //���ȼ��ɱ��ı�ʶ����ʾ
	CAN_Init_Handle.CAN_Mode = mode;		//ģʽ����Ϊ��ʼ��ʱ��ģʽ
	//CAN����������
	CAN_Init_Handle.CAN_SJW = tsjw;   //��������ͬ��ʱ�䳤��4
	CAN_Init_Handle.CAN_BS2 = tbs2;		//����ʱ���2ʱ�䳤��9
	CAN_Init_Handle.CAN_BS1 = tbs1;		//����ʱ���1ʱ�䳤��1
	CAN_Init_Handle.CAN_Prescaler = brp;//���ò����ʷ�Ƶ��3
	CAN_Init(CAN1, &CAN_Init_Handle);			//����CAN�ļĴ���
	
	//���ù�����
	CAN_FilterInitStructure.CAN_FilterNumber = 0;												//������0
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;			//����λģʽ/����ģʽ
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;		//���ù�����Ϊ32λģʽ
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0X0000;									//��������ʮ��λ����Ϊ0
	CAN_FilterInitStructure.CAN_FilterIdLow = 0X0000;										//��������ʮ��λ����Ϊ0
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X0000;							//�����������ʮ��λΪ0
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0X0000;								//�����������ʮ��λΪ0
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//������������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;							//����CAN����
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

	NVIC_Initure.NVIC_IRQChannel = CAN1_RX0_IRQn;				//�����ж�����
	NVIC_Initure.NVIC_IRQChannelPreemptionPriority = 0;	//���������ȼ�Ϊ1
	NVIC_Initure.NVIC_IRQChannelSubPriority = 1;				//���ô����ȼ�Ϊ0
	NVIC_Initure.NVIC_IRQChannelCmd = ENABLE;						//�����ж�״̬
	NVIC_Init(&NVIC_Initure);

	return 1;
}

//CAN��ʼ��  ��ʼ��˳��IO���á�CAN�Ĵ������á����������á��ж�����
//tsjw ����ͬ����Ծ��Ԫ�� 1~4tq
//tbs2 ʱ���2��ʱ�䵥Ԫ  1~8tq
//tbs1 ʱ���1��ʱ�䵥Ԫ  1~16tq
//brp  �����ʷ�Ƶ�� 1~1024��config���������Ѿ�������һ���������Դ˴�����Ҫ��һ��
//42/��1+4+9��/3 = 1M
//mode CAN_Mode_Normal ��ͨģʽ CAN_Mode_Silent_LoopBack��Ĭ�ػ�ģʽ
u8 CAN2_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode)// CANģʽ��ʼ��
{
	GPIO_InitTypeDef CAN_Handle;
	CAN_InitTypeDef CAN_Init_Handle;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_Initure;

	
//������CAN2�ӿ�
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2,ENABLE);
//	
//	CAN_Handle.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
//	CAN_Handle.GPIO_Mode = GPIO_Mode_AF;//����ģʽ
//	CAN_Handle.GPIO_OType = GPIO_OType_PP;//��©���
//	CAN_Handle.GPIO_PuPd = GPIO_PuPd_UP;//��������
//	CAN_Handle.GPIO_Speed = GPIO_Speed_100MHz;//
//	GPIO_Init(GPIOB, &CAN_Handle);
//	
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_CAN2);//����CAN2���
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_CAN2);//����CAN2���
	
//������CAN2�ӿ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2,ENABLE);
	
	
	CAN_Handle.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_5;
	CAN_Handle.GPIO_Mode = GPIO_Mode_AF;//����ģʽ
	CAN_Handle.GPIO_OType = GPIO_OType_PP;//��©���
	CAN_Handle.GPIO_PuPd = GPIO_PuPd_UP;//��������
	CAN_Handle.GPIO_Speed = GPIO_Speed_100MHz;//
	GPIO_Init(GPIOB, &CAN_Handle);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_CAN2);//����CAN2���
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_CAN2);//����CAN2���
	
	
	//CAN��Ԫ����
	CAN_Init_Handle.CAN_TTCM = DISABLE; //��ʱ�䴥��ͨ��ģʽ
	CAN_Init_Handle.CAN_ABOM = ENABLE;	//�Զ����߻ָ�
	CAN_Init_Handle.CAN_AWUM = DISABLE; //˯��ģʽͨ��������ѣ����CAN->MCR�ı�־λ��
	CAN_Init_Handle.CAN_NART = DISABLE;  //��ֹ�����Զ�����
	CAN_Init_Handle.CAN_RFLM = DISABLE; //���Ĳ�����
	CAN_Init_Handle.CAN_TXFP = ENABLE; //���ȼ��ɱ��ı�ʶ����ʾ
	CAN_Init_Handle.CAN_Mode = mode;		//ģʽ����Ϊ��ʼ��ʱ��ģʽ
	//CAN����������
	CAN_Init_Handle.CAN_SJW = tsjw;   //��������ͬ��ʱ�䳤��4
	CAN_Init_Handle.CAN_BS2 = tbs2;		//����ʱ���2ʱ�䳤��9
	CAN_Init_Handle.CAN_BS1 = tbs1;		//����ʱ���1ʱ�䳤��1
	CAN_Init_Handle.CAN_Prescaler = brp;//���ò����ʷ�Ƶ��3
	CAN_Init(CAN2, &CAN_Init_Handle);			//����CAN�ļĴ���
	
	//���ù�����
	CAN_FilterInitStructure.CAN_FilterNumber = 14;												//������0
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;			//����λģʽ/����ģʽ
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;		//���ù�����Ϊ32λģʽ
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0X0000;									//��������ʮ��λ����Ϊ0
	CAN_FilterInitStructure.CAN_FilterIdLow = 0X0000;										//��������ʮ��λ����Ϊ0
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X0000;							//�����������ʮ��λΪ0
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0X0000;								//�����������ʮ��λΪ0
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//������������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;							//����CAN����
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);

	NVIC_Initure.NVIC_IRQChannel = CAN2_RX0_IRQn;				//�����ж�����
	NVIC_Initure.NVIC_IRQChannelPreemptionPriority = 0;	//���������ȼ�Ϊ1
	NVIC_Initure.NVIC_IRQChannelSubPriority = 0;				//���ô����ȼ�Ϊ0
	NVIC_Initure.NVIC_IRQChannelCmd = ENABLE;						//�����ж�״̬
	NVIC_Init(&NVIC_Initure);

	return 1;
}

int vola;
CanRxMsg RxMessage1;
u8 len = 0;
u32 cnt = 0;
//CAN1 ����0�����ж�
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg Rxbuf;//CANͨ�ſ��ܳ��ֵ�ë��
	
	if(CAN_GetITStatus(CAN1, CAN_IT_FMP0)!=RESET)
	{
		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage1);		//����can����
		
		Rxbuf = RxMessage1;
		
		//////////////////////////1st begin ������Խ������ݵ�ID�����ж�
		if((Rxbuf.StdId&0x200) != 0)
		{
			cnt++;
			speed_analyze(Rxbuf.StdId, Rxbuf.Data);
		}

		if(Rxbuf.StdId == 0x150)
		{
			PITCH_YAW_Analyze(Rxbuf.Data);
		}
		if(Rxbuf.StdId == 0x151)
		{
			Slaver_flag.global_clip = Rxbuf.Data[0];
			Slaver_flag.global_fiction = Rxbuf.Data[1];
			Slaver_flag.global_auto_aim = Rxbuf.Data[2];
			Slaver_flag.global_spin = Rxbuf.Data[3];
			Slaver_flag.global_anti_top = Rxbuf.Data[4];
			Slaver_flag.global_twist = Rxbuf.Data[5];
			Slaver_flag.user1 = Rxbuf.Data[6];
			Slaver_flag.shift_rush = Rxbuf.Data[7];
		}
		/////////////////////////////////1st end ������Խ������ݵ�ID�����ж�
		
		CAN1->RF0R |= (0x01<<5);//��1�ͷ�����
	}
}


int can2_cnt = 0;
CanRxMsg RxMessage2;
void CAN2_RX0_IRQHandler(void)
{
	CanRxMsg Rxbuf;//CANͨ�ſ��ܳ��ֵ�ë��
	
	if(CAN_GetITStatus(CAN2, CAN_IT_FMP0)!=RESET)
	{
		CAN_Receive(CAN2, CAN_FIFO0, &RxMessage2);		//����can����
		
		Rxbuf = RxMessage2;
		
		//////////////////////////2sc begin ������Խ������ݵ�ID�����ж�
		if((Rxbuf.StdId&0x200) != 0)
		{
			can2_cnt++;
			Motor_analyze2(Rxbuf.StdId, Rxbuf.Data);
		}
		if(Rxbuf.StdId == 0x502)
		{
			memcpy(&SC_R_DATA, Rxbuf.Data, 8);
		}
		//////////////////////////2sc end ������Խ������ݵ�ID�����ж�
		
		CAN2->RF0R |= (0x01<<5);//��1�ͷ�����
	}


}



//CAN ��׼֡�������ݺ���
//msg ���������ݵ��׵�ַ
//len ���������ݵĳ���
//stdid ���ͱ�ʶ�� 
//return 0, ���ͳɹ�
//return 1, ����ʧ��
u8 CAN1_Send_Msg2(u8 *msg, u8 len, u16 stdid)
{
	u8 mbox;
	u16 i=0;
	
	CanTxMsg TxMessage;								//CAN�������ݽṹ�壬�������պ�дЭ��ο���
	TxMessage.StdId = stdid;						//��׼ID/��ʶ�� ��Ȼû�������
	TxMessage.ExtId = 0x00;						//������չ��ʶ��
	TxMessage.IDE = CAN_Id_Standard;	//��׼֡
	TxMessage.RTR = CAN_RTR_Data;			//����֡
	TxMessage.DLC = len;							//Ҫ���͵����ݳ���
	for(i=0; i<len; i++)							//�������������յ���������
	{
		TxMessage.Data[i] = msg[i];
	}
	
	mbox = CAN_Transmit(CAN1, &TxMessage);//CAN ���ͺ���
	
	i = 0;
	while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i<0xfff))
	{
		i++;
	}
	if(i == 0xfff)
		return 1;
	else 
		return 0;
	
}

u8 CAN2_Send_Msg2(u8 *msg, u8 len, u16 stdid)
{
	u8 mbox;
	u16 i=0;
	
	CanTxMsg TxMessage;								//CAN�������ݽṹ�壬�������պ�дЭ��ο���
	TxMessage.StdId = stdid;						//��׼ID/��ʶ�� ��Ȼû�������
	TxMessage.ExtId = 0x00;						//������չ��ʶ��
	TxMessage.IDE = CAN_Id_Standard;	//��׼֡
	TxMessage.RTR = CAN_RTR_Data;			//����֡
	TxMessage.DLC = len;							//Ҫ���͵����ݳ���
	for(i=0; i<len; i++)							//�������������յ���������
	{
		TxMessage.Data[i] = msg[i];
	}
	
	mbox = CAN_Transmit(CAN2, &TxMessage);//CAN ���ͺ���
	
	i = 0;
	while((CAN_TransmitStatus(CAN2, mbox) == CAN_TxStatus_Failed) && (i<0xfff))
	{
		i++;
	}
	if(i == 0xfff)
		return 1;
	else 
		return 0;
}



