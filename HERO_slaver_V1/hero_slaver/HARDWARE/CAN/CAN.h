#ifndef RP__CAN_H
#define RP__CAN_H

#include "link.h"

//CAN����������CAN.c ���ж������


u8 CAN1_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode);// CANģʽ��ʼ��
u8 CAN1_Send_Msg2(u8 *msg, u8 len, u16 stdid);								//CAN1��������

u8 CAN2_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode);// CANģʽ��ʼ��
u8 CAN2_Send_Msg2(u8 *msg, u8 len, u16 stdid);								//CAN1��������

#endif

