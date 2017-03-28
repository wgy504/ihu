#ifndef __BSP_STM32_PWR_SAV_H
#define __BSP_STM32_PWR_SAV_H

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////////////
#include "stm32f2xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "sysdim.h"
#include "vmfreeoslayer.h"
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#include "commsgccl.h"
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	#include "commsgbfsc.h"
#else
	#error Un-correct constant definition
#endif

//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名


//本地定义的交换矩阵
#define BSP_STM32_PWR_SAV_KEY1_PIN 						CUBEMX_PIN_F2_SYS_WKUP_KEY1_Pin
#define BSP_STM32_PWR_SAV_KEY1_PORT     			CUBEMX_PIN_F2_SYS_WKUP_KEY1_Port
#define BSP_STM32_PWR_SAV_KEY1_DOWN_LEVEL     1  /* 根据原理图设计，KEY1按下时引脚为高电平，所以这里设置为1 */

#define BSP_STM32_PWR_SAV_KEY2_PIN 						CUBEMX_PIN_F2_WKUP_KEY2_Pin
#define BSP_STM32_PWR_SAV_KEY2_PORT    				CUBEMX_PIN_F2_WKUP_KEY2_GPIO_Port
#define BSP_STM32_PWR_SAV_KEY2_DOWN_LEVEL     0  /* 根据原理图设计，KEY2按下时引脚为低电平，所以这里设置为0 */


//全局函数
extern void ihu_bsp_stm32_enter_into_stop_mode(void);


//Local APIs
static void Delay(__IO uint32_t nCount);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void SYSCLKConfig_STOP(void);


#ifdef __cplusplus
}
#endif
#endif	/* __BSP_STM32_PWR_SAV_H */

