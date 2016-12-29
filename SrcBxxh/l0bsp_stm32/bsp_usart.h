#ifndef __BSP_DEBUG_USART_H__
#define __BSP_DEBUG_USART_H__

/* 包含头文件 ----------------------------------------------------------------*/
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

//从ucosiii移植过来的函数
//MAX_IHU_MSG_BODY_LENGTH-1是因为发送到上层SPSVIRGO的数据缓冲区受到消息结构msg_struct_spsvirgo_l2frame_rcv_t的影响
#define IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN 				MAX_IHU_MSG_BODY_LENGTH-1	//最大接收数据长度
#define IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN 				MAX_IHU_MSG_BODY_LENGTH-1	//最大接收数据长度
#define IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN 				MAX_IHU_MSG_BODY_LENGTH-1	//最大接收数据长度
#define IHU_BSP_STM32_SPS_PRINT_REC_MAX_LEN 			MAX_IHU_MSG_BODY_LENGTH-1	//最大接收数据长度
#define IHU_BSP_STM32_SPS_SPARE1_REC_MAX_LEN 			MAX_IHU_MSG_BODY_LENGTH-1	//最大接收数据长度
#define IHU_BSP_STM32_SPS_SPARE2_REC_MAX_LEN 			MAX_IHU_MSG_BODY_LENGTH-1	//最大接收数据长度

//HUITP L2FRAME帧结构
typedef struct IHU_HUITP_L2FRAME_STD_UART_frame_header
{
  uint8_t start;
  uint8_t chksum;
  uint16_t len;
}IHU_HUITP_L2FRAME_STD_UART_frame_header_t;
#define IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE 0
#define IHU_HUITP_L2FRAME_STD_RX_STATE_START 1
#define IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM 2
#define IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN 3
#define IHU_HUITP_L2FRAME_STD_RX_STATE_BODY 4
#define IHU_HUITP_L2FRAME_STD_RX_START_FLAG_CHAR 0xFE

//发送和接受数据的延迟时间长度
#define BSP_SUCCESS 0
#define BSP_FAILURE -1
#define IHU_BSP_STM32_SPS_TX_MAX_DELAY 100
#define IHU_BSP_STM32_SPS_RX_MAX_DELAY 100

//形成定义的交换矩阵
#define IHU_BSP_STM32_UART_GPRS_HANDLER					huart1
#define IHU_BSP_STM32_UART_GPRS_HANDLER_ID  		1
#define IHU_BSP_STM32_UART_RFID_HANDLER					huart2
#define IHU_BSP_STM32_UART_RFID_HANDLER_ID  		2
#define IHU_BSP_STM32_UART_PRINT_HANDLER				huart3
#define IHU_BSP_STM32_UART_PRINT_HANDLER_ID  		3
#define IHU_BSP_STM32_UART_BLE_HANDLER					huart4
#define IHU_BSP_STM32_UART_BLE_HANDLER_ID  			4
#define IHU_BSP_STM32_UART_SPARE1_HANDLER				huart5
#define IHU_BSP_STM32_UART_SPARE1_HANDLER_ID  	5
#define IHU_BSP_STM32_UART_SPARE2_HANDLER				huart6
#define IHU_BSP_STM32_UART_SPARE2_HANDLER_ID  	6

//API函数
extern int ihu_bsp_stm32_sps_slave_hw_init(void);

extern int ihu_bsp_stm32_sps_print_fputc(int ch, FILE *f);
extern int ihu_bsp_stm32_sps_print_fgetc(FILE * f);

extern int ihu_bsp_stm32_sps_print_send_data(char *s);
extern int ihu_bsp_stm32_sps_print_rcv_data(char *s, uint16_t len);
extern int ihu_bsp_stm32_sps_print_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout);
extern int ihu_bsp_stm32_sps_gprs_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_gprs_rcv_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_gprs_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout);
extern int ihu_bsp_stm32_sps_rfid_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_rfid_rcv_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_rfid_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout);
extern int ihu_bsp_stm32_sps_ble_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_ble_rcv_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_ble_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout);
extern int ihu_bsp_stm32_sps_spare1_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_spare1_rcv_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_spare1_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout);
extern int ihu_bsp_stm32_sps_spare2_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_spare2_rcv_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_spare2_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle);

#endif  /* __BSP_DEBUG_USART_H__ */


