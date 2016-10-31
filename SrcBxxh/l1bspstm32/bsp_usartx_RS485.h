#ifndef __BSP_USARTX_RS485_H__
#define __BSP_USARTX_RS485_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f2xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define USE_RS485                                    0    // 1：使用RS485通信   0：使用RS232通信

#define RS485_USARTx                                 USART3
#define RS485_USARTx_BAUDRATE                        9600
#define RS485_USART_RCC_CLK_ENABLE()                 __HAL_RCC_USART3_CLK_ENABLE()
#define RS485_USART_RCC_CLK_DISABLE()                __HAL_RCC_USART3_CLK_DISABLE()

#define RS485_USARTx_GPIO_ClK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define RS485_USARTx_PORT                            GPIOB
#define RS485_USARTx_Tx_PIN                          GPIO_PIN_10
#define RS485_USARTx_Rx_PIN                          GPIO_PIN_11

#define USARTx_IRQHANDLER                            USART3_IRQHandler
#define USARTx_IRQn                                  USART3_IRQn

#define RS485_REDE_GPIO_ClK_ENABLE()                 __HAL_RCC_GPIOB_CLK_ENABLE()
#define RS485_REDE_PORT                              GPIOB
#define RS485_REDE_PIN                               GPIO_PIN_2
#define RS485_RX_MODE()                              HAL_GPIO_WritePin(RS485_REDE_PORT,RS485_REDE_PIN,GPIO_PIN_RESET)
#define RS485_TX_MODE()                              HAL_GPIO_WritePin(RS485_REDE_PORT,RS485_REDE_PIN,GPIO_PIN_SET)

/* 扩展变量 ------------------------------------------------------------------*/
extern UART_HandleTypeDef husartx_rs485;

/* 函数声明 ------------------------------------------------------------------*/
void RS485_USARTx_Init(void);


#endif  /* __BSP_USARTX_RS485_H__ */

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
