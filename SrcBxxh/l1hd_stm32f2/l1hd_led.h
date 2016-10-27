/*----------------------------------------Copyright (c)---------------------------------------------------                              
*                                        http://www.39board.com
*
*      39board工作室提供的所有ST、TI、Freescale、Altera等平台的例程、文档、方案等资料信息旨在为学习者提供
*  学习参考，若使用者在使用过程中因任何原因造成直接或间接的损失，本工作室不承担任何责任。
*  
*
*
*                                                                             —————39board工作室
*
*----------------------------------------------------------------------------------------------------------
* File name:                led.h
* Author:                   alvan 
* Version:                  V1.0
* Date:                     2013-12-26
* Description:              LED驱动头文件
* Function List:		
* History:    		
*-----------------------------------------------------------------------------------------------------------
*/

#ifndef  __L1HD_LED_H
#define  __L1HD_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"

//LED_F205_BOARD
#define LED_F205_BOARD_PORT 						GPIOC
#define LED_F205_BOARD_PIN							GPIO_Pin_2
#define LED_F205RG_board_on()						GPIO_SetBits(LED_F205_BOARD_PORT, LED_F205_BOARD_PIN)
#define LED_F205RG_board_off()					GPIO_ResetBits(LED_F205_BOARD_PORT, LED_F205_BOARD_PIN)
#define LED_F205RG_board_negation() 	 	GPIO_WriteBit(LED_F205_BOARD_PORT, LED_F205_BOARD_PIN, \
	(BitAction)(1-(GPIO_ReadOutputDataBit(LED_F205_BOARD_PORT, LED_F205_BOARD_PIN))))	
#define LED_F205RG_BOARD_CLK_ENABLE()		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)
	
//LED_F205_TIMER_IND
#define LED_F205_TIMER_IND_PORT 						GPIOC
#define LED_F205_TIMER_IND_PIN							GPIO_Pin_3
#define LED_F205RG_timer_ind_on()						GPIO_SetBits(LED_F205_TIMER_IND_PORT, LED_F205_TIMER_IND_PIN)
#define LED_F205RG_timer_ind_off()					GPIO_ResetBits(LED_F205_TIMER_IND_PORT, LED_F205_TIMER_IND_PIN)
#define LED_F205RG_timer_ind_negation() 	 	GPIO_WriteBit(LED_F205_TIMER_IND_PORT, LED_F205_TIMER_IND_PIN, \
	(BitAction)(1-(GPIO_ReadOutputDataBit(LED_F205_TIMER_IND_PORT, LED_F205_TIMER_IND_PIN))))	
#define LED_F205RG_TIMER_IND_CLK_ENABLE()		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)

//ZJL: 另外一种方式，就是直接使用LED宏定义搞定
#define LED_F205RG_onoff(port, pin, onoff) if (onoff) GPIO_SetBits(port, pin); else GPIO_ResetBits (port, pin);

//全局函数定义
extern void LED_F205_board_init(void);
extern void LED_F205_board_on(void);
extern void LED_F205_board_off(void);
extern void LED_F205_board_negation(void);
extern void LED_F205_timer_ind_init(void);
extern void LED_F205_timer_ind_on(void);
extern void LED_F205_timer_ind_off(void);
extern void LED_F205_timer_ind_negation(void);
extern void LED_F205_onoff(GPIO_TypeDef* port, uint16_t pin, u8 onoff);

#ifdef __cplusplus
}
#endif

#endif


