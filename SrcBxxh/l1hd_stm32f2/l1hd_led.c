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
* File name:            led.c
* Author:               alvan 
* Version:              V1.0
* Date:                 2014-7-29
* Description:		
* Function List:		
* History:    		
*-----------------------------------------------------------------------------------------------------------
*/
#include "stm32f2xx.h"
#include "l1hd_led.h"

/*
*--------------------------------------------------------------------------------------------------------
* Function:          led_init
* Description:       ST-M3-LITE-205RG精简开发板有1个引脚连接了LED发光二极管，
                     GPIOC.2--LED1，该函数的作用是对其各引脚进行初始化。
* Input:             none
* Output:            none
* Return:            none
* Created by:        alvan
* Created date:      2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/

/*
*--------------------------------------------------------------------------------------------------------
* Function:          led_init
* Description:       ST-M3-LITE-205RG精简开发板有1个引脚连接了LED发光二极管，
                     GPIOC.2--LED1，该函数的作用是对其各引脚进行初始化。
* Input:             none
* Output:            none
* Return:            none
* Created by:        alvan
* Created date:      2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void LED_F205_board_init(void)
{ 	
	GPIO_InitTypeDef GPIO_InitStructure;
	LED_F205RG_BOARD_CLK_ENABLE();    /* 使能端口PORTC时钟                                  */  	 
	GPIO_InitStructure.GPIO_Pin    = LED_F205_BOARD_PIN;               
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
	GPIO_Init(LED_F205_BOARD_PORT, &GPIO_InitStructure);                /* 配置引脚GPIOC.2为推挽输出,IO最大速度50M            */
	GPIO_SetBits(LED_F205_BOARD_PORT, LED_F205_BOARD_PIN);   
	
	# if 0    /* IO 测试   */


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
								 RCC_AHB1Periph_GPIOB |
								 RCC_AHB1Periph_GPIOC | 
								 RCC_AHB1Periph_GPIOD , ENABLE);    /* 使能端口时钟                                     */  	 

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0  |
														GPIO_Pin_1  |
														GPIO_Pin_2  |
														GPIO_Pin_3  |
														GPIO_Pin_4  |
														GPIO_Pin_5  |
														GPIO_Pin_6  |
														GPIO_Pin_7  |
														GPIO_Pin_8  |
														GPIO_Pin_9  |
														GPIO_Pin_10 |
														GPIO_Pin_11 |
														GPIO_Pin_12;																		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);                   /* 配置引脚GPIOA.0~GPIOA.12为推挽输出,IO最大速度50M  */

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0  |
														GPIO_Pin_1  |
														GPIO_Pin_2  |
														GPIO_Pin_5  |
														GPIO_Pin_6  |
														GPIO_Pin_7  |
														GPIO_Pin_8  |
														GPIO_Pin_9  |
														GPIO_Pin_10 |
														GPIO_Pin_11 | 
														GPIO_Pin_12 |
														GPIO_Pin_13 |
														GPIO_Pin_14 |
														GPIO_Pin_15;																		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);                   	

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0  |
														GPIO_Pin_1  |
														GPIO_Pin_2  |
														GPIO_Pin_3  |																		
														GPIO_Pin_4  |
														GPIO_Pin_5  |
														GPIO_Pin_6  |
														GPIO_Pin_7  |
														GPIO_Pin_8  |
														GPIO_Pin_9  |
														GPIO_Pin_10 |
														GPIO_Pin_11 | 
														GPIO_Pin_12 |
														GPIO_Pin_13;																		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);   

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2; 
																														
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	

	GPIOA->ODR &= 0xE000;                       
	GPIOB->ODR &= 0x0018;
	GPIOC->ODR &= 0xC000;                      
	GPIOD->ODR &= 0xFFFB;                       /* 对应的IO输出低                                     */

	GPIOA->ODR |= ~0xE000;                       
	GPIOB->ODR |= ~0x0018;
	GPIOC->ODR |= ~0xC000;                      
	GPIOD->ODR |= ~0xFFFB;                      /* 对应的IO输出高                                     */		

	#endif    
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_on
* Description:      LED全亮
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void LED_F205_board_on(void)
{
    LED_F205RG_board_on();
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_off
* Description:      LED全灭
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/

void   LED_F205_board_off (void)
{
    LED_F205RG_board_off();
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_negation
* Description:      LED取反闪烁
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/

void   LED_F205_board_negation (void)
{
    LED_F205RG_board_negation();
}

void LED_F205_timer_ind_init(void)
{ 	
	GPIO_InitTypeDef GPIO_InitStructure;
	LED_F205RG_TIMER_IND_CLK_ENABLE();    															/* 使能端口PORTC时钟                                  */  	 
	GPIO_InitStructure.GPIO_Pin    = LED_F205_TIMER_IND_PIN;               
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
	GPIO_Init(LED_F205_TIMER_IND_PORT, &GPIO_InitStructure);                /* 配置引脚GPIOC.2为推挽输出,IO最大速度50M            */
	GPIO_SetBits(LED_F205_TIMER_IND_PORT, LED_F205_TIMER_IND_PIN);   
	
	# if 0    /* IO 测试   */


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
								 RCC_AHB1Periph_GPIOB |
								 RCC_AHB1Periph_GPIOC | 
								 RCC_AHB1Periph_GPIOD , ENABLE);    /* 使能端口时钟                                     */  	 

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0  |
														GPIO_Pin_1  |
														GPIO_Pin_2  |
														GPIO_Pin_3  |
														GPIO_Pin_4  |
														GPIO_Pin_5  |
														GPIO_Pin_6  |
														GPIO_Pin_7  |
														GPIO_Pin_8  |
														GPIO_Pin_9  |
														GPIO_Pin_10 |
														GPIO_Pin_11 |
														GPIO_Pin_12;																		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);                   /* 配置引脚GPIOA.0~GPIOA.12为推挽输出,IO最大速度50M  */

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0  |
														GPIO_Pin_1  |
														GPIO_Pin_2  |
														GPIO_Pin_5  |
														GPIO_Pin_6  |
														GPIO_Pin_7  |
														GPIO_Pin_8  |
														GPIO_Pin_9  |
														GPIO_Pin_10 |
														GPIO_Pin_11 | 
														GPIO_Pin_12 |
														GPIO_Pin_13 |
														GPIO_Pin_14 |
														GPIO_Pin_15;																		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);                   	

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0  |
														GPIO_Pin_1  |
														GPIO_Pin_2  |
														GPIO_Pin_3  |																		
														GPIO_Pin_4  |
														GPIO_Pin_5  |
														GPIO_Pin_6  |
														GPIO_Pin_7  |
														GPIO_Pin_8  |
														GPIO_Pin_9  |
														GPIO_Pin_10 |
														GPIO_Pin_11 | 
														GPIO_Pin_12 |
														GPIO_Pin_13;																		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);   

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2; 
																														
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	

	GPIOA->ODR &= 0xE000;                       
	GPIOB->ODR &= 0x0018;
	GPIOC->ODR &= 0xC000;                      
	GPIOD->ODR &= 0xFFFB;                       /* 对应的IO输出低                                     */

	GPIOA->ODR |= ~0xE000;                       
	GPIOB->ODR |= ~0x0018;
	GPIOC->ODR |= ~0xC000;                      
	GPIOD->ODR |= ~0xFFFB;                      /* 对应的IO输出高                                     */		

	#endif    
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_on
* Description:      LED全亮
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void  LED_F205_timer_ind_on (void)
{
    LED_F205RG_timer_ind_on();
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_off
* Description:      LED全灭
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/

void   LED_F205_timer_ind_off (void)
{
    LED_F205RG_timer_ind_off();
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_negation
* Description:      LED取反闪烁
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/

void   LED_F205_timer_ind_negation (void)
{
	LED_F205RG_timer_ind_negation();
}


void LED_F205_onoff(GPIO_TypeDef* port, uint16_t pin, u8 onoff)
{
	LED_F205RG_onoff(port, pin, onoff);
}







/************************ (C) COPYRIGHT 39BOARD *****************************END OF FILE********************************/
