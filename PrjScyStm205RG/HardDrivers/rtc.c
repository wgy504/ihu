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
* File name:          rtc.c
* Author:             alvan 
* Version:            V1.0
* Date:               2014-01-10
* Description:		
* Function List:		
* History:    		
*-----------------------------------------------------------------------------------------------------------
*/

#include "stm32f2xx.h"
#include "rtc.h"


struct_rtc_time  g_current_time;                   /* 定义RTC当前时间的全局变量                */
u8   g_rtc_erro = 0;                               /* 定义RTC错误全局标志，0：正确   1：错误   */



#define RTC_CLOCK_SOURCE_LSE	


__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;    



void RTC_Config(void)
{
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
    /* The RTC Clock may varies due to LSI frequency dispersion. */
    /* Enable the LSI OSC */ 
    RCC_LSICmd(ENABLE);

    /* Wait till LSI is ready */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

    SynchPrediv = 0xFF;
    AsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);

    /* Wait till LSE is ready */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    SynchPrediv = 0xFF;
    AsynchPrediv = 0x7F;

#endif /* RTC_CLOCK_SOURCE_LSI */

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro(); 
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         rtc_init
* Description:      初始化RTC
* Input:            none
* Output:           0：RTC初始化失败，一般是32.768晶振问题
*                   1：RTC初始化正常
* Return:           none
* Created by:       alvan 
* Created date:     2014-08-08
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
u8  rtc_init (void)
{ 
    RTC_InitTypeDef RTC_InitStructure;
    
    if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)              /* 检测是否第一次配置RTC       */
    {
        
        RTC_Config();                                               /* RTC 配置                    */
                                                                    /* 配置RTC数据寄存器等         */
        RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
        RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
        RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
        
        if (RTC_Init(&RTC_InitStructure) == ERROR)
        {
            g_rtc_erro = 1;
            return 0;
        }
        g_current_time.second = 12;
        g_current_time.minute = 13;
        g_current_time.hour   = 7;
        g_current_time.week   = 5;
        g_current_time.day    = 8;
        g_current_time.month  = 8;
        g_current_time.year   = 14;
        
        rtc_set_time(g_current_time);                                 /* 设置时间                    */ 
        RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
    }
    else
    {
        /* Check if the Power On Reset flag is set */
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
        }
        /* Check if the Pin Reset flag is set */
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
        }


        /* Enable the PWR clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

        /* Allow access to RTC */
        PWR_BackupAccessCmd(ENABLE);

        /* Wait for RTC APB registers synchronisation */
        RTC_WaitForSynchro();

        /* Clear the RTC Alarm Flag */
        RTC_ClearFlag(RTC_FLAG_ALRAF);

        /* Clear the EXTI Line 17 Pending bit (Connected internally to RTC Alarm) */
        EXTI_ClearITPendingBit(EXTI_Line17);  
    }    
   
    return 1; 
}



void RTC_TimeRegulate(struct_rtc_time temp_time)
{
    RTC_TimeTypeDef  RTC_TimeStructure;
    RTC_DateTypeDef  RTC_DateStructure;
    
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = temp_time.hour;
    RTC_TimeStructure.RTC_Minutes = temp_time.minute;
    RTC_TimeStructure.RTC_Seconds = temp_time.second;
    
    if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) == ERROR)
    {
        g_rtc_erro = 1;
    } 
 
    if(temp_time.week == 0)
    {
        RTC_DateStructure.RTC_WeekDay = 7;
    }
    else
    {
        RTC_DateStructure.RTC_WeekDay = temp_time.week;
    }
    
    RTC_DateStructure.RTC_Date    = temp_time.day;
    RTC_DateStructure.RTC_Month   = temp_time.month;
    RTC_DateStructure.RTC_Year    = temp_time.year;

    if(RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure) == ERROR)
    {
        g_rtc_erro = 1;
    } 
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:      rtc_set_time
* Description:   设置RTC时间
* Input:         
* Output:        none
* Return:        none
* Created by:    alvan 
* Created date:  2014-08-08
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void rtc_set_time(struct_rtc_time temp_data)
{ 
    RTC_TimeRegulate(temp_data); 			
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:          rtc_get_time
* Description:       获取时间
* Input:             none
* Output:            none
* Return:            none
* Created by:        alvan 
* Created date:      2014-08-08
* Others: 	
*---------------------------------------------------------------------------------------------------------
*/
void rtc_get_time(void)
{
    RTC_TimeTypeDef  RTC_TimeStructure;
    RTC_DateTypeDef  RTC_DateStructure;
    
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
    
    g_current_time.second = RTC_TimeStructure.RTC_Seconds;
    g_current_time.minute = RTC_TimeStructure.RTC_Minutes;
    g_current_time.hour   = RTC_TimeStructure.RTC_Hours;
    g_current_time.week   = RTC_DateStructure.RTC_WeekDay;
    g_current_time.day    = RTC_DateStructure.RTC_Date;
    g_current_time.month  = RTC_DateStructure.RTC_Month;
    g_current_time.year   = RTC_DateStructure.RTC_Year + 2000;
    
    if(g_current_time.week == 0)
    {
        g_current_time.week = 7;
    }
    (void)RTC->DR;  
}






 


/************************ (C) COPYRIGHT 39BOARD *****************************END OF FILE********************************/
