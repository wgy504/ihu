#ifndef __BSP_STM32_CALENDAR_H
#define __BSP_STM32_CALENDAR_H

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
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

/******************** (C) COPYRIGHT 2009 www.armjishu.com ************************
* Description      : 超强的日历，支持农历，24节气几乎所有日历的功能
                          日历时间以1970年为元年，用32bit的时间寄存器可以运
                          行到2100年左右
*******************************************************************************/
//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名


extern uint8_t const *WEEK_STR[];
extern uint8_t const *zodiac_sign[];



//本地定义的交换矩阵


//全局函数


//Local APIs
uint8_t GetMoonDay(uint8_t month_p,unsigned short table_addr);
uint8_t GetChinaCalendar(uint16_t  year,uint8_t month,uint8_t day,uint8_t *p);
void GetSkyEarth(uint16_t year,uint8_t *p);
void StrCopy(uint8_t *target,uint8_t const *source,uint8_t no);
void GetChinaCalendarStr(uint16_t year,uint8_t month,uint8_t day,uint8_t *str);
uint8_t GetJieQi(uint16_t year,uint8_t month,uint8_t day,uint8_t *JQdate);
uint8_t GetJieQiStr(uint16_t year,uint8_t month,uint8_t day,uint8_t *str);


#ifdef __cplusplus
}
#endif
#endif	/* __BSP_STM32_CALENDAR_H */

