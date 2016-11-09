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


//工作调用流
OPSTAT RFID_UART_send_AT_command(uint8_t *cmd, uint8_t *ack, UINT16 wait_time);  //秒级！！！
void RFID_UART_clear_receive_buffer(void);
#define RFID_UART_SendLR() RFID_UART_SendString("\r\n");
void RFID_UART_SendString(char* s);



#endif /* L1FREERTOS_MOD_RFID_H_ */

