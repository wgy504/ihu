/*----------------------------------------Copyright (c)---------------------------------------------------                              
*                                        http://www.39board.com
*
*      39board�������ṩ������ST��TI��Freescale��Altera��ƽ̨�����̡��ĵ���������������Ϣּ��Ϊѧϰ���ṩ
*  ѧϰ�ο�����ʹ������ʹ�ù��������κ�ԭ�����ֱ�ӻ��ӵ���ʧ���������Ҳ��е��κ����Ρ�
*  
*
*
*                                                                             ����������39board������
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
* Description:      ��ʱ����
* Input:            dly����ֵԽ����ʱԽ��
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
* Description:        ������ʵ��RTCʱ�ӵ����úͶ�ȡ�Լ�W25Q32 ID�Ķ�ȡ������⵽�����ʱLED1ȡ����˸�����
*                     �а�������W25Q32 ID ��ȡ��ȷʱLED1������˸������������˸��                 
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
    
    led_init();                                         /* LED��ʼ��                          */
    key_init();                                         /* ������ʼ��                         */
    spi_flash_init();                                   /* spi_flashӲ���ӿڳ�ʼ��            */
    rtc_init();                                         /* RTC��ʼ��                          */
    
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
        if(KEY_VALUE_URSER_BUTTON == get_key_state())   /* �ж��Ƿ��а�������                 */
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
