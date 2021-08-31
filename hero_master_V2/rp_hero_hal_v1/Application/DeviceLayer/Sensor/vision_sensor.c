/**
 * @file        vision_sensor.c
 * @author      RobotPilots@2020
 * @Version     V1.0
 * @date        24-Feb-2021
 * @brief       Device Vision.
 */
 
/* Includes ------------------------------------------------------------------*/
#include "vision_sensor.h"

#include "rp_math.h"
#include "device.h"
#include "vision_potocol.h"

extern void vision_sensor_init(vision_sensor_t *vision_sen);
extern void vision_sensor_update(vision_sensor_t *vision_sen, uint8_t *rxBuf);

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void vision_sensor_check(vision_sensor_t *vision_sen);
static void vision_sensor_heart_beat(vision_sensor_t *vision_sen);

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
// �Ӿ�����
drv_uart_t	vision_sensor_driver = {
	.type = DRV_UART1,
	.tx_byte = NULL,
};

// �Ӿ���Ϣ
vision_sensor_info_t 	vision_sensor_info = {
	.offline_max_cnt = 60,
};

// �Ӿ�������
vision_sensor_t vision_sensor = {
	.info = &vision_sensor_info,								//���ݽṹ��
	.init = vision_sensor_init,									//��������ʼ��
	.update = vision_sensor_update,							//���ݸ���
	.check = vision_sensor_check,								//���ݺ������ж�
	.heart_beat = vision_sensor_heart_beat,			//״̬����
	.work_state = DEV_OFFLINE,							//״̬��ѯ
	.id = DEV_ID_VISION,
};

uint8_t Vision_SentNum[50];

/* Private functions ---------------------------------------------------------*/
/**
 *	@brief	�Ӿ����ݼ��
 */
static void vision_sensor_check(vision_sensor_t *vision_sen)
{
	
	vision_sen->info->offline_cnt = 0;
}

/**
 *	@brief	�Ӿ�������
 */
static void vision_sensor_heart_beat(vision_sensor_t *vision_sen)//��������
{
	vision_sensor_info_t *vision_info = vision_sen->info;

	vision_info->offline_cnt++;
	if(vision_info->offline_cnt > vision_info->offline_max_cnt)//ÿ�εȴ�һ��ʱ����Զ�����
	{
		vision_info->offline_cnt = vision_info->offline_max_cnt;
		vision_sen->work_state = DEV_OFFLINE;
	} 
	else //ÿ�ν��ճɹ�����ռ���
	{
		/* ����->���� */
		if(vision_sen->work_state == DEV_OFFLINE)
			vision_sen->work_state = DEV_ONLINE;
	}
}

/* Exported functions --------------------------------------------------------*/


void VISION_ResetData(vision_sensor_t *vision)//�Ӿ�������գ�һ��ʧ��ʱʹ��
{

}
