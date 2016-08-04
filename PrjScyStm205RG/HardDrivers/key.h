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
    
    
#define GPIO_CTR_Key                         GPIOB
#define GPIO_Pin_CTR_Key                     GPIO_Pin_7
#define RCC_AHB1Periph_GPIO_CTR_Key          RCC_AHB1Periph_GPIOB    
		
#define KEY_VALUE_URSER_BUTTON               0x01
	
    
#define get_key_user_button                  GPIO_ReadInputDataBit(GPIO_CTR_Key, GPIO_Pin_CTR_Key)
    
	
void  key_init (void);

u8  get_key_state (void);

#ifdef __cplusplus
}
#endif

#endif


