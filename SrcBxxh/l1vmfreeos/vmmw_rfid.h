/**
 ****************************************************************************************
 *
 * @file mod_rfid.h
 *
 * @brief RFID module control
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */

#ifndef L1FREERTOS_MOD_RFID_H_
#define L1FREERTOS_MOD_RFID_H_

#include "stm32f2xx_hal.h"
#include "vmfreeoslayer.h"
#include "bsp_usart.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"

//全局使用的常亮定义

//向上提供全局统一服务的入口


//工作调用流
OPSTAT func_rfidmod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, UINT16 wait_time);  //秒级！！！
void func_rfidmod_uart_clear_receive_buffer(void);
void func_rfidmod_uart_send_string(char* s);
#define func_rfidmod_uart_send_LR() func_rfidmod_uart_send_string("\r\n");

#endif /* L1FREERTOS_MOD_RFID_H_ */

