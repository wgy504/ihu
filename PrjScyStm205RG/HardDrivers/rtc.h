/*----------------------------------------Copyright (c)---------------------------------------------------                              
*                                        http://www.39board.com
*
*      39board工作室提供的所有ST、TI、Freescale、Altera等平台的例程、文档、方案等资料信息旨在为学习者提供
*  学习参考，若使用者在使用过程中因任何原因造成直接或间接的损失，本工作室不承担任何责任。
*  
*
*
*                                                                             —————39board工作室
*
*----------------------------------------------------------------------------------------------------------
* File name:          rtc.h
* Author:             alvan 
* Version:            V1.0
* Date:               2014-01-10
* Description:		
* Function List:		
* History:    		
*-----------------------------------------------------------------------------------------------------------
*/

#ifndef  __RTC_H
#define  __RTC_H

#ifdef __cplusplus
extern "C" {
#endif
	
#define  CALENDAR_START_YEAR	              2000
#define  CALENDAR_END_YEAR	                  2079		
#define  CALENDAR_START_YEAR_FIRST_DATE	      6	


	
	

	
typedef struct rtc_time_struct        //定时器的结构体
{ 
    u32 year;
    u8  month;
    u8  day;
    u8  week;
    u8  hour;
    u8  minute;
    u8  second;
}struct_rtc_time;  
	

extern struct_rtc_time g_current_time;     




	
u8  rtc_init (void);

void rtc_set_time(struct_rtc_time temp_data);

void rtc_get_time(void);
	

#ifdef __cplusplus
}
#endif

#endif


