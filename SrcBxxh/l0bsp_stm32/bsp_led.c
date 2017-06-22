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

static void func_bsp_stm32_led_delay(__IO uint32_t nCount)	
{
	for(; nCount != 0; nCount--);
}

void ihu_bsp_stm32_led_f2board_test_flag(int times, int dur)
{
	int i = 0;
	for (i = 0; i<times; i++){
    ihu_bsp_stm32_led_f2board_on();
		func_bsp_stm32_led_delay(dur);
		ihu_bsp_stm32_led_f2board_off();
		func_bsp_stm32_led_delay(dur);
	}
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

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)

//void ihu_bsp_stm32_beep_f2board_on(void)
//{
//    BSP_STM32_BEEP_ON;/* 蜂鸣器响 */
//}

//void ihu_bsp_stm32_beep_f2board_off(void)
//{
//    BSP_STM32_BEEP_OFF;/* 蜂鸣器停 */
//}

//void ihu_bsp_stm32_beep_f2board_toggle(void)
//{
//    BSP_STM32_BEEP_TOGGLE;/* 蜂鸣器反转 */
//}

//void ihu_bsp_stm32_led_power_f2board_on(void)
//{
//    BSP_STM32_LED_POWER_ON;
//}

//void ihu_bsp_stm32_led_power_f2board_off(void)
//{
//    BSP_STM32_LED_POWER_OFF;
//}

//void ihu_bsp_stm32_led_power_f2board_toggle(void)
//{
//    BSP_STM32_LED_POWER_TOGGLE;
//}

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
#endif //#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)

#if ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID) || \
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID))

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

void ihu_bsp_stm32_beep_f2board_pattern_05s(void)
{
  ihu_bsp_stm32_beep_f2board_on();
	ihu_usleep(500);
  ihu_bsp_stm32_beep_f2board_off();
	ihu_usleep(500);	
}

void ihu_bsp_stm32_beep_f2board_pattern_1s(void)
{
  ihu_bsp_stm32_beep_f2board_on();
	ihu_usleep(1000);
  ihu_bsp_stm32_beep_f2board_off();
	ihu_usleep(1000);	
}

void ihu_bsp_stm32_beep_f2board_pattern_2s(void)
{
  ihu_bsp_stm32_beep_f2board_on();
	ihu_usleep(2000);
  ihu_bsp_stm32_beep_f2board_off();
	ihu_usleep(2000);	
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
#endif //#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)


#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
void ihu_bsp_stm32_led_serv1_f2board_on(void)
{
    BSP_STM32_LED_SERV1_ON;
}

void ihu_bsp_stm32_led_serv1_f2board_off(void)
{
    BSP_STM32_LED_SERV1_OFF;
}

void ihu_bsp_stm32_led_serv1_f2board_toggle(void)
{
    BSP_STM32_LED_SERV1_TOGGLE;
}

//void ihu_bsp_stm32_led_serv2_f2board_on(void)
//{
//    BSP_STM32_LED_SERV2_ON;
//}

//void ihu_bsp_stm32_led_serv2_f2board_off(void)
//{
//    BSP_STM32_LED_SERV2_OFF;
//}

//void ihu_bsp_stm32_led_serv2_f2board_toggle(void)
//{
//    BSP_STM32_LED_SERV2_TOGGLE;
//}

//void ihu_bsp_stm32_led_serv3_f2board_on(void)
//{
//    BSP_STM32_LED_SERV3_ON;
//}

//void ihu_bsp_stm32_led_serv3_f2board_off(void)
//{
//    BSP_STM32_LED_SERV3_OFF;
//}

//void ihu_bsp_stm32_led_serv3_f2board_toggle(void)
//{
//    BSP_STM32_LED_SERV3_TOGGLE;
//}

#endif
