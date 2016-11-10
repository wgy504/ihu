#ifndef __BSP_DIDO_H__
#define __BSP_DIDO_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f2xx_hal.h"

//BLE串口模块的使能GPIO端口定义
#define BLE_UART_PORT_PIN							GPIO_PIN_3  //纯属于测试，待修改
#define BLE_UART_PORT									GPIOA
#define BLE_UART_PORT_ENABLE()				HAL_GPIO_WritePin(BLE_UART_PORT, BLE_UART_PORT_PIN, GPIO_PIN_SET)    // 输出高电平
#define BLE_UART_PORT_DISABLE()				HAL_GPIO_WritePin(BLE_UART_PORT, BLE_UART_PORT_PIN, GPIO_PIN_RESET)  // 输出高电平

#endif  // __BSP_DIDO_H__

