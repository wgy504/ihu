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
#include "bsp_spi.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"

//全局使用的常量定义
#define IHU_VMWM_RFIDMOD_USING_ITF_SPS_RFID 1
#define IHU_VMWM_RFIDMOD_USING_ITF_SPI1 2
#define IHU_VMWM_RFIDMOD_USING_ITF_SPI2 3
#define IHU_VMWM_RFIDMOD_USING_ITF_SET  IHU_VMWM_RFIDMOD_USING_ITF_SPI2  //CCL项目使用的接口为SPI2，BSP中有映射

//向上提供全局统一服务的入口
extern OPSTAT ihu_vmmw_rfidmod_spi_send_command(uint8_t *command);
extern OPSTAT ihu_vmmw_rfidmod_spi_read_id(uint8_t *output);

//工作调用流
OPSTAT func_rfidmod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, UINT16 wait_time);  //秒级！！！
OPSTAT func_rfidmod_wait_command_fb(uint8_t *ack, uint16_t wait_time);
void func_rfidmod_clear_receive_buffer(void);
void func_rfidmod_send_string(char* s);
#define func_rfidmod_send_LR() func_rfidmod_send_string("\r\n");

#endif /* L1FREERTOS_MOD_RFID_H_ */

