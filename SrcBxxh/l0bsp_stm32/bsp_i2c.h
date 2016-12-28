#ifndef __BSP_DEBUG_I2C_H__
#define __BSP_DEBUG_I2C_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f2xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "sysdim.h"
#include "vmfreeoslayer.h"
#include "bsp_usart.h"
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#include "commsgccl.h"
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	#include "commsgbfsc.h"
#else
#endif

//从ucosiii移植过来的函数
//MAX_IHU_MSG_BODY_LENGTH-1是因为发送到上层SPSVIRGO的数据缓冲区受到消息结构msg_struct_spsvirgo_l2frame_rcv_t的影响
#define BSP_STM32_I2C_IAU_REC_MAXLEN MAX_IHU_MSG_BODY_LENGTH-1	//最大接收数据长度
#define BSP_STM32_I2C_SPARE1_REC_MAXLEN MAX_IHU_MSG_BODY_LENGTH-1	//最大接收数据长度

//发送和接受数据的延迟时间长度
#define I2C_TX_MAX_DELAY_DURATION 100
#define I2C_RX_MAX_DELAY_DURATION 100

//形成定义的交换矩阵
#define BSP_STM32_I2C_IAU					hi2c1
#define BSP_STM32_I2C_IAU_ID  		1
#define BSP_STM32_I2C_SPARE1			hi2c1
#define BSP_STM32_I2C_SPARE1_ID  	2

//API函数
extern int ihu_bsp_stm32_i2c_slave_hw_init(void);
extern int ihu_bsp_stm32_i2c_iau_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_i2c_iau_rcv_data(uint8_t* buff, uint16_t len);	
extern int ihu_bsp_stm32_i2c_spare1_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_i2c_spare1_rcv_data(uint8_t* buff, uint16_t len);

void HAL_I2C_RxCpltCallback(I2C_HandleTypeDef *I2cHandle);
	


#endif  /* __BSP_DEBUG_I2C_H__ */


