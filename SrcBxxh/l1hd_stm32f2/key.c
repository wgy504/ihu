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
* File name:          key.c
* Author:             alvan 
* Version:            V1.0
* Date:               2013-12-29
* Description:		
* Function List:		
* History:    		
*-----------------------------------------------------------------------------------------------------------
*/

#include "stm32f2xx.h"
#include "key.h"


/*
*--------------------------------------------------------------------------------------------------------
* Function:       key_init
* Description:    
* Input:          none
* Output:         none
* Return:         none
* Created by:     alvan 
* Created date:   2014-07-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void  key_init (void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIO_CTR_Key, ENABLE);  /* 使能端口时钟                        */ 

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_CTR_Key;	                             
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_Init(GPIO_CTR_Key, &GPIO_InitStructure);                 /* 配置引脚为上拉输入                  */	 
}


/*
*--------------------------------------------------------------------------------------------------------
* Function:        get_key_state
* Description:     读取按键状态函数
* Input:           none
* Output:          none
* Return:          
* Created by:      alvan 
* Created date:    2014-07-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
u8  get_key_state (void)
{
    if (get_key_user_button == 0)
        return KEY_VALUE_URSER_BUTTON;
  
    return 0x00;
}



/************************ (C) COPYRIGHT 39BOARD *****************************END OF FILE********************************/
