/**
  ******************************************************************************
  * 文件名程: bsp_pwr_sav.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 
  ******************************************************************************
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_pwr_sav.h" 

//从MAIN.x中继承过来的函数


//本地全局变量


/*
 *
 * 低功耗STOP MODE
 *  
 * 硬件必须配置为：KEY1=PA0，系统唤醒按钮，KEY2=PC13(EXTI15_10_IRQn)
 * 
 *
 *
*/

/* 函数体 --------------------------------------------------------------------*/

//进入STOP_MODE的函数
//使用的方式是：先关闭所有的传感器电源（MOSFET开关），然后进入执行该函数，进入STOP MODE
void ihu_bsp_stm32_enter_into_stop_mode(void)
{
	ihu_l1hd_led_f2board_test_flag(5, 0x1FFFFF);
  /* 进入停止模式，设置电压调节器为低功耗模式，等待中断唤醒 KEY2 按键下降沿唤醒*/
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI);
}

/**
  * 函数功能: 停机唤醒后配置系统时钟: 使能 HSE, PLL
  *           并且选择PLL作为系统时钟.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void SYSCLKConfig_STOP(void)
{
  /* 使能 HSE */
  __HAL_RCC_HSE_CONFIG(RCC_HSE_ON);

  /* 等待 HSE 准备就绪 */
  while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET);
  
  /* 使能 PLL */ 
  __HAL_RCC_PLL_ENABLE();

  /* 等待 PLL 准备就绪 */
  while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
  {
  }

  /* 选择PLL作为系统时钟源 */
  __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);

  /* 等待PLL被选择为系统时钟源 */
  while(__HAL_RCC_GET_SYSCLK_SOURCE() != 0x08)
  {
  }
}

//简单的延时函数
//static void Delay(__IO uint32_t nCount)	
//{
//	for(; nCount != 0; nCount--);
//}

/**
  * 函数功能: 按键外部中断服务函数
  * 输入参数: GPIO_Pin：中断引脚
  * 返 回 值: 无
  * 说    明: 无
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin==BSP_STM32_PWR_SAV_KEY2_PIN)
  {
    /*  刚从停机唤醒，由于时钟未配置正确，
		此printf语句的内容不能正常发送出去 */
		//printf("\n 进入中断 \n");		
		
		SYSCLKConfig_STOP();											//停机唤醒后需要启动HSE	
		
		//亮板子上的状态灯，
		//IHU_DEBUG_PRINT_FAT("BSP_PWR_SAV: Test on  EXTI Call back function.\n");
		ihu_l1hd_led_f2board_test_flag(3, 0x7FFFFF);
		
//		//LED1_ON;	LED2_ON;	LED3_ON;				//点亮所有LED一段时间指示停机唤醒
//		Delay(0x1FFFFF);
//		ihu_l1hd_led_f2board_off();
		//LED1_OFF;	LED2_OFF;	LED3_OFF;	
		
		/*由于前面已经重新启动了HSE，
		所以本printf语句能正常发出 */
		//printf("\n 退出中断 \n");	
    __HAL_GPIO_EXTI_CLEAR_IT(BSP_STM32_PWR_SAV_KEY2_PIN);
  }
}


