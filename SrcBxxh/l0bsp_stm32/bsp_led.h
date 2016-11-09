#ifndef __BSP_LED_H__
#define __BSP_LED_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f2xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
typedef enum
{
  LED_OFF = 0,
  LED_ON  = 1,
  LED_TOGGLE = 2,
}LEDState_TypeDef;
#define IS_LED_STATE(STATE)           (((STATE) == LED_OFF) || ((STATE) == LED_ON) || ((STATE) == LED_TOGGLE))

/* 宏定义 --------------------------------------------------------------------*/
#define LED1                          (uint8_t)0x01
#define LED2                          (uint8_t)0x02
#define LED3                          (uint8_t)0x06
#define IS_LED_TYPEDEF(LED)           (((LED) == LED1) || ((LED) == LED2) || ((LED) == LED3))

/*
 * 以下宏定义内容跟开发板硬件息息相关，需要查看开发板电路原理图才能正确编写。
 * 例如，查原理图可在LED1灯接在stm32f103芯片的PB0引脚上，所以有关LED1的宏定义
 * 都是与GPIOB，GPIO_Pin_0相关的，我们专门把这些与开发板硬件相关的内容定义为宏，
 * 这对于修改或者移植程序非常方便。
 */
#define LED1_RCC_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED1_GPIO_PIN                 GPIO_PIN_0
#define LED1_GPIO                     GPIOB


#define LED2_RCC_CLK_ENABLE()         __HAL_RCC_GPIOG_CLK_ENABLE()
#define LED2_GPIO_PIN                 GPIO_PIN_6
#define LED2_GPIO                     GPIOG


#define LED3_RCC_CLK_ENABLE()         __HAL_RCC_GPIOG_CLK_ENABLE()
#define LED3_GPIO_PIN                 GPIO_PIN_7
#define LED3_GPIO                     GPIOG

#define LED1_ON                       HAL_GPIO_WritePin(LED1_GPIO,LED1_GPIO_PIN,GPIO_PIN_SET)    // 输出高电平
#define LED1_OFF                      HAL_GPIO_WritePin(LED1_GPIO,LED1_GPIO_PIN,GPIO_PIN_RESET)  // 输出低电平
#define LED1_TOGGLE                   HAL_GPIO_TogglePin(LED1_GPIO,LED1_GPIO_PIN)                // 输出反转

#define LED2_ON                       HAL_GPIO_WritePin(LED2_GPIO,LED2_GPIO_PIN,GPIO_PIN_SET)    // 输出高电平
#define LED2_OFF                      HAL_GPIO_WritePin(LED2_GPIO,LED2_GPIO_PIN,GPIO_PIN_RESET)  // 输出低电平
#define LED2_TOGGLE                   HAL_GPIO_TogglePin(LED2_GPIO,LED2_GPIO_PIN)                // 输出反转

#define LED3_ON                       HAL_GPIO_WritePin(LED3_GPIO,LED3_GPIO_PIN,GPIO_PIN_SET)    // 输出高电平
#define LED3_OFF                      HAL_GPIO_WritePin(LED3_GPIO,LED3_GPIO_PIN,GPIO_PIN_RESET)  // 输出低电平
#define LED3_TOGGLE                   HAL_GPIO_TogglePin(LED3_GPIO,LED3_GPIO_PIN)                // 输出反转

//BLE串口模块的GPIO端口定义
#define BLE_UART_PORT_PIN							GPIO_PIN_3  //纯属于测试，待修改
#define BLE_UART_PORT									GPIOA
#define BLE_UART_PORT_ENABLE()				HAL_GPIO_WritePin(BLE_UART_PORT, BLE_UART_PORT_PIN, GPIO_PIN_SET)    // 输出高电平
#define BLE_UART_PORT_DISABLE()				HAL_GPIO_WritePin(BLE_UART_PORT, BLE_UART_PORT_PIN, GPIO_PIN_RESET)  // 输出高电平


/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void LED_GPIO_Init(void);
void LEDx_StateSet(uint8_t LEDx,LEDState_TypeDef state);


//旧式方式编写的函数
//LED_F205_BOARD
#define LED_F205_BOARD_PORT 						GPIOC
#define LED_F205_BOARD_PIN							GPIO_PIN_2
#define LED_F205RG_board_on()						HAL_GPIO_WritePin(LED_F205_BOARD_PORT, LED_F205_BOARD_PIN, GPIO_PIN_SET)
#define LED_F205RG_board_off()					HAL_GPIO_WritePin(LED_F205_BOARD_PORT, LED_F205_BOARD_PIN, GPIO_PIN_RESET)
#define LED_F205RG_BOARD_CLK_ENABLE()		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)
#define LED_F205RG_board_negation() 	 	HAL_GPIO_TogglePin(LED_F205_BOARD_PORT, LED_F205_BOARD_PIN)
	
//LED_F205_TIMER_IND
#define LED_F205_TIMER_IND_PORT 						GPIOC
#define LED_F205_TIMER_IND_PIN							GPIO_PIN_3
#define LED_F205RG_timer_ind_on()						HAL_GPIO_WritePin(LED_F205_TIMER_IND_PORT, LED_F205_TIMER_IND_PIN, GPIO_PIN_SET)
#define LED_F205RG_timer_ind_off()					HAL_GPIO_WritePin(LED_F205_TIMER_IND_PORT, LED_F205_TIMER_IND_PIN, GPIO_PIN_RESET)
#define LED_F205RG_TIMER_IND_CLK_ENABLE()		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)
#define LED_F205RG_timer_ind_negation() 	 	HAL_GPIO_TogglePin(LED_F205_TIMER_IND_PORT, LED_F205_TIMER_IND_PIN)

//ZJL: 另外一种方式，就是直接使用LED宏定义搞定
#define LED_F205RG_onoff(port, pin, onoff) if (onoff) HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET); else HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);

//全局函数定义
extern void BSP_STM32_LED_F205_board_init(void);
extern void BSP_STM32_LED_F205_board_on(void);
extern void BSP_STM32_LED_F205_board_off(void);
extern void BSP_STM32_LED_F205_board_negation(void);
extern void BSP_STM32_LED_F205_timer_ind_init(void);
extern void BSP_STM32_LED_F205_timer_ind_on(void);
extern void BSP_STM32_LED_F205_timer_ind_off(void);
extern void BSP_STM32_LED_F205_timer_ind_negation(void);
extern void BSP_STM32_LED_F205_onoff(GPIO_TypeDef* port, uint16_t pin, uint8_t onoff);

extern void BSP_STM32_LED_F205_board_on(void);




#endif  // __BSP_LED_H__

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
