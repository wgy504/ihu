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

//向上提供全局统一服务的入口
//以下范式是正常的提供MAC地址的函数
extern OPSTAT ihu_vmmw_blemod_hc05_uart_fetch_mac_addr_procedure(char *macAddr, uint8_t len);  //HC05模块
//以下函数是为了支持5204型号，对BLE进行详细控制的函数api，还有待完整性测试，并调整控制方式
extern void 	ihu_vmmw_blemod_hc05_working_process(void);  //HC05模块


//Local API
//工作调用流
OPSTAT func_blemod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, UINT16 wait_time);  //秒级！！！
void func_blemod_uart_clear_receive_buffer(void);
OPSTAT func_blemod_find_char(char *a);
char *ihu_bsp_stm32_ble_get_rebuff(uint16_t *len);
void func_blemod_uart_send_string(char* s);
#define func_blemod_uart_send_LR() func_blemod_uart_send_string("\r\n");

//纯HC05 BLE设备相关的操作
void func_blemod_uart_hc05_init(void);
void func_blemod_uart_hc05_blt_addr_convert_str(BLTDev *bltDev,char delimiter);
void func_blemod_uart_hc05_print_blt_info(BLTDev *bltDev);
int     func_blemod_uart_hc05_get_line(char* line, char* stream ,int max_size);
uint8_t func_blemod_uart_hc05_get_remote_device_name(BLTDev *bltDev);
uint8_t func_belmod_uart_hc05_scan_bluetooth_address(BLTDev *bltDev);
uint8_t func_blemod_uart_hc05_link_remote_device(void);


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

#endif /* L1FREERTOS_MOD_BLE_H_ */

