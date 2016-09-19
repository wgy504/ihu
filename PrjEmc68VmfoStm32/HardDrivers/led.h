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
* File name:                led.h
* Author:                   alvan 
* Version:                  V1.0
* Date:                     2013-12-26
* Description:              LED����ͷ�ļ�
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


