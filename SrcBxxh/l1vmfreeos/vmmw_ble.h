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
#include "bsp_dido.h"
//#include "bsp_usart.h"

//全局使用的常亮定义
#define IHU_VMMW_BLEMOD_UART_REPEAT_CNT 3


//信息输出：留下来，以后可以将这种技巧用到别的地方！
//#define IHU_VMWM_BLEMOD_HC05_DEBUG_ON                   0
//#define IHU_VMWM_BLEMOD_HC05_DEBUG_FUNC_ON              0

//#define HC05_INFO(fmt,arg...)           printf("<<-HC05-INFO->> "fmt"\n",##arg)
//#define HC05_ERROR(fmt,arg...)          printf("<<-HC05-ERROR->> "fmt"\n",##arg)
//#define HC05_DEBUG(fmt,arg...)          do{\
//                                          if(IHU_VMWM_BLEMOD_HC05_DEBUG_ON)\
//                                          printf("<<-HC05-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
//                                          }while(0)

//#define HC05_DEBUG_FUNC()               do{\
//                                         if(IHU_VMWM_BLEMOD_HC05_DEBUG_FUNC_ON)\
//                                         printf("<<-HC05-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
//                                       }while(0)

//向上提供全局统一服务的入口
extern OPSTAT ihu_vmmw_blemod_hc05_uart_fetch_mac_addr_procedure(void);  //HC05模块
extern void 	ihu_vmmw_blemd_hc05_working_process(void);  //HC05模块
																			 

//Local API
//工作调用流
OPSTAT func_blemod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, UINT16 wait_time);  //秒级！！！
void func_blemod_uart_clear_receive_buffer(void);
OPSTAT func_blemod_find_char(char *a);
char *ihu_bsp_stm32_ble_get_rebuff(uint16_t *len);
void func_blemod_uart_send_string(char* s);
#define func_blemod_uart_send_LR() func_blemod_uart_send_string("\r\n");

void func_blemod_uart_hc05_init(void);
uint8_t func_blemod_uart_hc05_send_cmd(char* cmd,uint8_t clean);																 
void func_blemod_uart_hc05_str_blt_addr(BLTDev *bltDev,char delimiter);
uint8_t func_blemod_uart_hc05_get_remote_device_name(BLTDev *bltDev);
void func_blemod_uart_hc05_print_blt_info(BLTDev *bltDev);
uint8_t func_blemod_uart_hc05_link(void);
int func_blemod_uart_hc05_get_line(char* line, char* stream ,int max_size);																			 
																			 
#endif /* L1FREERTOS_MOD_BLE_H_ */

