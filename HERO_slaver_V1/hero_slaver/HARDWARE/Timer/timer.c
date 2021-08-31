#include "timer.h"
#include "led.h"


void TIM3_PWM_Init(u16 pres, u32 reload)
{
	GPIO_InitTypeDef       GPIO_Initure;
	TIM_TimeBaseInitTypeDef  TIM_Initure;
	TIM_OCInitTypeDef         OC_Initure;
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	

	GPIO_Initure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Initure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Initure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_Initure);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7, GPIO_AF_TIM3);    
	
	TIM_Initure.TIM_Prescaler = pres-1;//1MHz
	TIM_Initure.TIM_CounterMode = TIM_CounterMode_Up;		
	TIM_Initure.TIM_Period = reload-1;   //20msһ������	,ÿ+1����ʱ��+1΢��
	TIM_Initure.TIM_ClockDivision = TIM_CKD_DIV1;		
	TIM_TimeBaseInit(TIM3,&TIM_Initure);
	
	OC_Initure.TIM_OCMode = TIM_OCMode_PWM1;		
	OC_Initure.TIM_OutputState = TIM_OutputState_Enable;	
	OC_Initure.TIM_OutputNState = TIM_OutputNState_Disable;		
	OC_Initure.TIM_Pulse = 0;		
	OC_Initure.TIM_OCPolarity = TIM_OCPolarity_High;		
	OC_Initure.TIM_OCNPolarity = TIM_OCPolarity_High;		//������Ը�

	TIM_OC1Init(TIM3,&OC_Initure);		
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);
	
	TIM_OC2Init(TIM3,&OC_Initure);		
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);
	
				 
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	
	TIM_CtrlPWMOutputs(TIM3,ENABLE);
	
	TIM_Cmd(TIM3,ENABLE);

}


//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		LED2=!LED2;//DS1��ת
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}





void TIM4_PWM_Init(u16 pres, u32 reload)
{
	GPIO_InitTypeDef       GPIO_Initure;
	TIM_TimeBaseInitTypeDef  TIM_Initure;
	TIM_OCInitTypeDef         OC_Initure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	

	GPIO_Initure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Initure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Initure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_Initure);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9, GPIO_AF_TIM4);    
	
	TIM_Initure.TIM_Prescaler = pres-1;//1MHz
	TIM_Initure.TIM_CounterMode = TIM_CounterMode_Up;		
	TIM_Initure.TIM_Period = reload-1;   //20msһ������	,ÿ+1����ʱ��+1΢��
	TIM_Initure.TIM_ClockDivision = TIM_CKD_DIV1;		
	TIM_TimeBaseInit(TIM4,&TIM_Initure);
	
	OC_Initure.TIM_OCMode = TIM_OCMode_PWM2;		
	OC_Initure.TIM_OutputState = TIM_OutputState_Enable;		
	OC_Initure.TIM_Pulse = 2000;		
	OC_Initure.TIM_OCPolarity = TIM_OCPolarity_High;		
	OC_Initure.TIM_OCNPolarity = TIM_OCPolarity_High;		//������Ը�

	TIM_OC3Init(TIM4,&OC_Initure);		
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);
	
	TIM_OC3Init(TIM4,&OC_Initure);		
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);
				 
	TIM_ARRPreloadConfig(TIM4,ENABLE);
	
	TIM_CtrlPWMOutputs(TIM4,ENABLE);
	
	TIM_Cmd(TIM4,ENABLE);
}


