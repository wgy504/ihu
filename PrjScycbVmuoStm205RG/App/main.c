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
* File name:          main.c
* Author:             alvan 
* Version:            V1.0
* Date:               2014-7-29
* Description:		
* Function List:		
* History:    		
*-----------------------------------------------------------------------------------------------------------
*/

#include "stm32f2xx.h"
#include "led.h"
#include "rtc.h"
#include "key.h"
#include "spi_flash.h"

/*
*--------------------------------------------------------------------------------------------------------
* Function:         delay_ms
* Description:      延时函数
* Input:            dly：数值越大延时越长
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void  delay_ms (u32  dly)
{
    volatile  u32  i,j;

    for (i = 0; i < dly; i++) 
    {
        for (j = 0; j < 1000; j++) 
        {				
        }
    }
}


/*
*--------------------------------------------------------------------------------------------------------
* Function:           main
* Description:        该例程实现RTC时钟的设置和读取以及W25Q32 ID的读取，当检测到秒更新时LED1取反闪烁，如果
*                     有按键按下W25Q32 ID 读取正确时LED1快速闪烁，否则慢速闪烁。                 
* Input:              none
* Output:             none
* Return:             none
* Created by:         alvan 
* Created date:       2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
u32 tem;


int  main (void)
{ 
    u8   second_buf;
    u16  temp;  
    
    led_init();                                         /* LED初始化                          */
    key_init();                                         /* 按键初始化                         */
    spi_flash_init();                                   /* spi_flash硬件接口初始化            */
    rtc_init();                                         /* RTC初始化                          */
    
    if(W25Q32 == spi_flash_read_device_ID())
    {
        temp = 0;
    }    
    else
    {
        temp = 3000;
    }   

    while (1) 
    {
        if(KEY_VALUE_URSER_BUTTON == get_key_state())   /* 判断是否有按键按下                 */
        {
            led1_negation();
            delay_ms(temp + 500);
        }
        else
        {
            rtc_get_time();
            if(second_buf != g_current_time.second)
            {
                led1_negation();
                second_buf = g_current_time.second;
            }
        }
    }
}


/************************ (C) COPYRIGHT 39BOARD *****************************END OF FILE********************************/
