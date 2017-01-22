#ifndef __BSP_STM32_RTC_H
#define __BSP_STM32_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////////////
#include "stm32f2xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "sysdim.h"
#include "vmfreeoslayer.h"
#include "bsp_calendar.h"
	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#include "commsgccl.h"
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	#include "commsgbfsc.h"
#else
	#error Un-correct constant definition
#endif

//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名






//本地定义的交换矩阵
#define IHU_BSP_STM32_RTC_HANDLER					hrtc
#define IHU_BSP_STM32_RTC_HANDLER_ID  		1

//全局函数
extern uint32_t ihu_bsp_stm32_rtc_get_current_unix_time(void);
extern void ihu_bsp_stm32_rtc_get_current_ymdhms_time(char *output);
extern void ihu_bsp_stm32_rtc_get_current_ymd_time(char *output);
extern void ihu_bsp_stm32_rtc_get_current_hms_time(char *output);
	

//Local APIs



#ifdef __cplusplus
}
#endif
#endif	/* __BSP_STM32_RTC_H */


