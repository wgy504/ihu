/**
 ****************************************************************************************
 *
 * @file mod_cam.h
 *
 * @brief CAM module control
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */

#ifndef L1FREERTOS_MOD_CAM_H_
#define L1FREERTOS_MOD_CAM_H_

#include "stm32f2xx_hal.h"
#include "vmfreeoslayer.h"
#include "bsp_usart.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "bsp_dido.h"
#include "bsp_usart.h"

//全局使用的常亮定义
#define IHU_VMMW_CAM_UART_FRAME_SIZE 6
#define IHU_VMMW_CAM_UART_PKG_SIZE 512

#define IHU_VMMW_CAM_UART_SYN_REPEAT_CNT 10
#define IHU_VMMW_CAM_UART_PKG_REPEAT_CNT 3
#define IHU_VMMW_CAM_UART_RX_MAX_DELAY 1000
#define IHU_VMMW_CAM_UART_PICTURE_MAX_DELAY 5000


//向上提供全局统一服务的入口
//ULC_DSC03 200万像素串口摄像头
extern OPSTAT ihu_vmmw_cam_ulcdsc03_uart_get_picture(uint8_t *buffer, uint32_t bufLen, UINT32 *actualPkg);  

//Local API
OPSTAT ihu_vmmw_cam_ulcdsc03_uart_frame_procedure_sync(void);
uint32_t ihu_vmmw_cam_ulcdsc03_uart_frame_procedure_take_picture(void);
UINT16 ihu_vmmw_cam_ulcdsc03_uart_frame_procedure_fetch_picture(UINT8 *buffer, UINT16 index);


//External APIs
extern int ihu_bsp_stm32_sps_cam_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_cam_rcv_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_cam_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout);

//高级定义，简化程序的可读性
#define IHU_ERROR_PRINT_CAM(arg...)	do{zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++; IhuErrorPrint(##arg);  return IHU_FAILURE; }while(0)


#endif /* L1FREERTOS_MOD_CAM_H_ */

