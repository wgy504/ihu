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
* File name:             key.h
* Author:                alvan 
* Version:               V1.0
* Date:                  2014-7-29
* Description:		        
* Function List:		
* History:    		
*-----------------------------------------------------------------------------------------------------------
*/

#ifndef  __KEY_H
#define  __KEY_H

#ifdef __cplusplus
extern "C" {
#endif
    
/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
	
#define GPIO_CTR_Key                         GPIOB
#define GPIO_Pin_CTR_Key                     GPIO_Pin_7
#define RCC_AHB1Periph_GPIO_CTR_Key          RCC_AHB1Periph_GPIOB    
		
#define KEY_VALUE_URSER_BUTTON               0x01
	
    
#define get_key_user_button                  GPIO_ReadInputDataBit(GPIO_CTR_Key, GPIO_Pin_CTR_Key)
    
	
void  key_init (void);

u8  get_key_state (void);

//从另外一个例程中得到的程序
//KEY端口定义
#define KEY_RCC     RCC_AHB1Periph_GPIOC                            
#define KEY_PORT	GPIOC   
#define KEY1        GPIO_Pin_10    
#define KEY2        GPIO_Pin_11    

//按键标置
#define KEY_DOWN	0
#define KEY_UP		1

void Key_GPIO_Config(void);
u8 Key_Down_Detection(GPIO_TypeDef* GPIOx, u16 GPIO_Pin);
u8 Key_Down_Scan(void);

	
#ifdef __cplusplus
}
#endif

#endif


