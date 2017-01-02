/**
  ******************************************************************************
  * 文件名程: bsp_led.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 
  ******************************************************************************
  ******************************************************************************
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_led.h"

//从MAIN.x中继承过来的函数


//本地全局变量


/* 函数体 --------------------------------------------------------------------*/

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
void ihu_bsp_stm32_led_f2board_on(void)
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

void   ihu_bsp_stm32_led_f2board_off (void)
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

void   ihu_bsp_stm32_led_f2board_negation (void)
{
    LED_F205RG_board_negation();
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
void  ihu_bsp_stm32_led_f2board_timer_ind_on (void)
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

void   ihu_bsp_stm32_led_f2board_timer_ind_off (void)
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

void   ihu_bsp_stm32_led_f2board_timer_ind_negation (void)
{
	LED_F205RG_timer_ind_negation();
}


/**
  * 函数功能: 设置板载蜂鸣器的状态
  * 输入参数：state:设置蜂鸣器的状态。
  * 返 回 值: 无
  * 说    明：该函数使用类似HALA库函数的编程方法，方便理解HAL库函数编程思想。
  */
void ihu_bsp_stm32_beep_f2board_on(void)
{
    BSP_STM32_BEEP_ON;/* 蜂鸣器响 */
}

void ihu_bsp_stm32_beep_f2board_off(void)
{
    BSP_STM32_BEEP_OFF;/* 蜂鸣器停 */
}

void ihu_bsp_stm32_beep_f2board_toggle(void)
{
    BSP_STM32_BEEP_TOGGLE;/* 蜂鸣器反转 */
}

void ihu_bsp_stm32_led_power_f2board_on(void)
{
    BSP_STM32_LED_POWER_ON;
}

void ihu_bsp_stm32_led_power_f2board_off(void)
{
    BSP_STM32_LED_POWER_OFF;
}

void ihu_bsp_stm32_led_power_f2board_toggle(void)
{
    BSP_STM32_LED_POWER_TOGGLE;
}

void ihu_bsp_stm32_led_commu_f2board_on(void)
{
    BSP_STM32_LED_COMMU_ON;
}

void ihu_bsp_stm32_led_commu_f2board_off(void)
{
    BSP_STM32_LED_COMMU_OFF;
}

void ihu_bsp_stm32_led_commu_f2board_toggle(void)
{
    BSP_STM32_LED_COMMU_TOGGLE;
}

void ihu_bsp_stm32_led_work_state_f2board_on(void)
{
    BSP_STM32_LED_WORK_STATE_ON;
}

void ihu_bsp_stm32_led_work_state_f2board_off(void)
{
    BSP_STM32_LED_WORK_STATE_OFF;
}

void ihu_bsp_stm32_led_work_state_f2board_toggle(void)
{
    BSP_STM32_LED_WORK_STATE_TOGGLE;
}





