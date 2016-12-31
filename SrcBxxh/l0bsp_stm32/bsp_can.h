#ifndef __BSP_DEBUG_CAN_H__
#define __BSP_DEBUG_CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////////////
#include "stm32f2xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "sysdim.h"
#include "vmfreeoslayer.h"
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#include "commsgccl.h"
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	#include "commsgbfsc.h"
#else
#endif

//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名

	
//MAX_IHU_MSG_BODY_LENGTH-1是因为发送到上层SPSVIRGO的数据缓冲区受到消息结构msg_struct_spsvirgo_l2frame_rcv_t的影响
#define IHU_BSP_STM32_CAN_IAU_REC_MAX_LEN 				MAX_IHU_MSG_BODY_LENGTH-1	//最大接收数据长度
#define IHU_BSP_STM32_CAN_SPARE1_REC_MAX_LEN 		MAX_IHU_MSG_BODY_LENGTH-1	//最大接收数据长度

//发送和接受数据的延迟时间长度
#define IHU_BSP_STM32_CAN_TX_MAX_DELAY 				100
#define IHU_BSP_STM32_CAN_RX_MAX_DELAY 				100

//形成定义的交换矩阵
#define IHU_BSP_STM32_CAN_IAU_HANDLER					hcan1
#define IHU_BSP_STM32_CAN_IAU_HANDLER_ID  		1
#define IHU_BSP_STM32_CAN_SPARE1_HANDLER			hcan2
#define IHU_BSP_STM32_CAN_SPARE1_HANDLER_ID  	2

//API函数
extern int ihu_bsp_stm32_can_slave_hw_init(void);
extern int ihu_bsp_stm32_can_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_can_rcv_data(uint8_t* buff, uint16_t len);	
extern int ihu_bsp_stm32_can_spare1_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_can_spare1_rcv_data(uint8_t* buff, uint16_t len);

//重载接收函数，以便通过IT中断方式搞定接收通信，否则需要通过轮询或者单独线程搞定，更加麻烦
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *CanHandle);

#endif  /* __BSP_DEBUG_CAN_H__ */


