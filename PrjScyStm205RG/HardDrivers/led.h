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

#ifndef  __LED_H
#define  __LED_H

#ifdef __cplusplus
extern "C" {
#endif
	
#define GPIO_LED         GPIOC 
	
#define led1_output      GPIO_Pin_2

#define led1_on()        GPIO_SetBits(GPIO_LED, led1_output)		
#define led1_off()       GPIO_ResetBits(GPIO_LED, led1_output)
#define led1_negation()  GPIO_WriteBit(GPIO_LED,                   \
                                       led1_output,                \
                                       (BitAction)(1-(GPIO_ReadOutputDataBit(GPIO_LED, led1_output))))	
			
	
	
	
void  led_init (void);

void  led_all_on (void);

void  led_all_off (void);

void  led_all_negation (void);
	

#ifdef __cplusplus
}
#endif

#endif


