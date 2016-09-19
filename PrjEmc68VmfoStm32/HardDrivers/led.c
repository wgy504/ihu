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
#include "led.h"


/*
*--------------------------------------------------------------------------------------------------------
* Function:          led_init
* Description:       ST-M3-LITE-205RG���򿪷�����1������������LED��������ܣ�
                     GPIOC.2--LED1���ú����������Ƕ�������Ž��г�ʼ����
* Input:             none
* Output:            none
* Return:            none
* Created by:        alvan
* Created date:      2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/

void  led_init (void)
{ 
	
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);    /* ʹ�ܶ˿�PORTCʱ��                                  */  	 
    
    GPIO_InitStructure.GPIO_Pin   = led1_output;               
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
    GPIO_Init(GPIO_LED, &GPIO_InitStructure);                /* ��������GPIOC.2Ϊ�������,IO����ٶ�50M            */
	
    GPIO_SetBits(GPIO_LED, led1_output);   

# if 1    /* IO ����   */
	
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
	                       RCC_AHB1Periph_GPIOB |
	                       RCC_AHB1Periph_GPIOC | 
	                       RCC_AHB1Periph_GPIOD , ENABLE);    /* ʹ�ܶ˿�ʱ��                                     */  	 
    
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
    GPIO_Init(GPIOA, &GPIO_InitStructure);                   /* ��������GPIOA.0~GPIOA.12Ϊ�������,IO����ٶ�50M  */
	
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
    GPIOD->ODR &= 0xFFFB;                       /* ��Ӧ��IO�����                                     */
		
    GPIOA->ODR |= ~0xE000;                       
    GPIOB->ODR |= ~0x0018;
    GPIOC->ODR |= ~0xC000;                      
    GPIOD->ODR |= ~0xFFFB;                      /* ��Ӧ��IO�����                                     */		
				
#endif    


}


/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_on
* Description:      LEDȫ��
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void  led_all_on (void)
{
    led1_on();
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_off
* Description:      LEDȫ��
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/

void   led_all_off (void)
{
    led1_off();
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_negation
* Description:      LEDȡ����˸
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/

void   led_all_negation (void)
{
    led1_negation();
}












/************************ (C) COPYRIGHT 39BOARD *****************************END OF FILE********************************/
