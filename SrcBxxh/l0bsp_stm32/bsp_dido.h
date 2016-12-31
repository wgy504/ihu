#ifndef __BSP_DIDO_H__
#define __BSP_DIDO_H__

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

//BLE串口模块的使能GPIO端口定义
#define BLE_UART_PORT_PIN							GPIO_PIN_3  //纯属于测试，待修改
#define BLE_UART_PORT									GPIOA
#define BLE_UART_PORT_ENABLE()				HAL_GPIO_WritePin(BLE_UART_PORT, BLE_UART_PORT_PIN, GPIO_PIN_SET)    // 输出高电平
#define BLE_UART_PORT_DISABLE()				HAL_GPIO_WritePin(BLE_UART_PORT, BLE_UART_PORT_PIN, GPIO_PIN_RESET)  // 输出高电平

#endif  // __BSP_DIDO_H__

