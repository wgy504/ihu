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
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIO_CTR_Key, ENABLE);  /* ʹ�ܶ˿�ʱ��                        */ 

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_CTR_Key;	                             
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_Init(GPIO_CTR_Key, &GPIO_InitStructure);                 /* ��������Ϊ��������                  */	 
}


/*
*--------------------------------------------------------------------------------------------------------
* Function:        get_key_state
* Description:     ��ȡ����״̬����
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
