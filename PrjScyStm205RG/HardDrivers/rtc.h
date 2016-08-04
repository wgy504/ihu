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


	
	

	
typedef struct rtc_time_struct        //��ʱ���Ľṹ��
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


