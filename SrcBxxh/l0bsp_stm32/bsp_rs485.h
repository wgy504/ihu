#ifndef __BSP_STM32_USARTX_RS485_H__
#define __BSP_STM32_USARTX_RS485_H__

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


//本地定义的交换矩阵



//全局函数定义


//Local APIs
void RS485_USARTx_Init(void);

#ifdef __cplusplus
}
#endif
#endif  /* __BSP_STM32_USARTX_RS485_H__ */

