#ifndef __BSP_STM32_WATCH_DOG_H
#define __BSP_STM32_WATCH_DOG_H

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
#endif

//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名






//本地定义的交换矩阵


//全局函数
extern int ihu_bsp_stm32_watch_dog_start(void);    //启动
extern int ihu_bsp_stm32_watch_dog_refresh(void);  //喂狗


//Local APIs
void MX_IWDG_Init(uint8_t prv ,uint16_t rlv);


#ifdef __cplusplus
}
#endif
#endif	/* __BSP_STM32_WATCH_DOG_H */

