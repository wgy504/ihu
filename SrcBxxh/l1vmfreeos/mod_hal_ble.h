/**
 ****************************************************************************************
 *
 * @file mod_ble.h
 *
 * @brief BLE module control
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */

#ifndef L1FREERTOS_MOD_BLE_H_
#define L1FREERTOS_MOD_BLE_H_

#include "stm32f2xx_hal.h"
#include "vmfreeoslayer.h"
#include "bsp_usart.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"

//全局使用的常亮定义
#define IHU_BLE_UART_REPEAT_CNT 3

//工作调用流
OPSTAT BLE_UART_send_AT_command(uint8_t *cmd, uint8_t *ack, UINT16 wait_time);  //秒级！！！
void BLE_UART_clear_receive_buffer(void);
#define BLE_UART_SendLR() BLE_UART_SendString("\r\n");
void BLE_UART_SendString(char* s);
OPSTAT BLE_HC05_UART_fetch_mac_add_procedure(void);  //HC05模块


#endif /* L1FREERTOS_MOD_BLE_H_ */

