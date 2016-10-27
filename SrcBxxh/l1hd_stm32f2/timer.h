#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f2xx.h"
#include "bsp.h"

#define TIM_USART_GPRS_INT_VECTOR							BSP_INT_ID_TIM2

//Local
extern void TIM_USART_GPRS_Init_Config(void);

//Global
extern void TIM_USART_GPRS_IRQHandler(void);


#endif
