/**
  ******************************************************************************
  * 文件名程: bsp_rtc.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 
  ******************************************************************************
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_rtc.h"

//从MAIN.x中继承过来的函数
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
extern RTC_HandleTypeDef hrtc;
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
extern RTC_HandleTypeDef hrtc;
#endif


//本地全局变量



/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 从串口调试助手获取数字值(把ASCII码转换为数字)
  * 输入参数: value 用户在超级终端中输入的数值
  * 返 回 值: 输入字符的ASCII码对应的数值
  * 说    明：本函数专用于RTC获取时间，若进行其它输入应用，要修改一下
  */
uint8_t USART_Scanf(uint32_t value)
{
  uint32_t index = 0;
  uint32_t tmp[2] = {0, 0};
//  while (index < 2)
//  {
//    /* 等待直到串口接收到数据 */
//    tmp[index++] =getchar();
//    if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))   /*数字0到9的ASCII码为0x30至0x39*/
//    { 		  
//      printf("请输入 0 到 9 之间的数字 -->:\n");
//      index--; 		 
//    }
//  } 
	tmp[0] = rand()&10;
	tmp[1] = rand()%10;
  /* 计算输入字符的ASCII码转换为数字*/
  index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
  
  /* 检查数据有效性 */
  if (index > value)
  {
    printf("Please enter below %d value\n", value);
    return 0xFF;
  }
  return index;
}

/**
  * 函数功能: 配置当前时间和日期
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
	* 由于基于STM32CubeMX已经配置了当前时间，所以并不需要这些人肉设置时间的方法
	* 这个方式其实还是挺好的，提供了让人输入当前时间的方式，肯定要比系统生成强，但考虑到实际使用，这个意义也不大
	* 真正有意义的方式，是从网络自动同步时间
  */
//static void RTC_CalendarConfig(void)
//{
//  RTC_TimeTypeDef sTime;
//  RTC_DateTypeDef DateToUpdate;
//#if 1
//  /* 配置日期 */
//  /* 设置日期：2015年10月4日 星期日 */
//  DateToUpdate.WeekDay = RTC_WEEKDAY_SUNDAY;
//  DateToUpdate.Month = RTC_MONTH_OCTOBER;
//  DateToUpdate.Date = 0x4;
//  DateToUpdate.Year = 0x15;  
//  HAL_RTC_SetDate(&IHU_BSP_STM32_RTC_HANDLER, &DateToUpdate, RTC_FORMAT_BCD);
//  
//  /* 配置时间 */
//  /* 时钟时间：10:15:46 */
//  sTime.Hours = 0x10;
//  sTime.Minutes = 0x15;
//  sTime.Seconds = 0x46;
//  HAL_RTC_SetTime(&IHU_BSP_STM32_RTC_HANDLER, &sTime, RTC_FORMAT_BCD);
//#else
//  __IO uint32_t Tmp_YY = 0xFF, Tmp_MM = 0xFF, Tmp_DD = 0xFF, Tmp_HH = 0xFF, Tmp_MI = 0xFF, Tmp_SS = 0xFF;

//  printf("=========================TIME SET==================\n");
//  printf("Please Enter Year: 20\n");
//  while (Tmp_YY == 0xFF)
//  {
//    Tmp_YY = USART_Scanf(99);
//  }
//  printf("Year set as: 20%0.2d\n", Tmp_YY);

//  DateToUpdate.Year = Tmp_YY;
//  
//  printf("Please enter Month:  \n");
//  while (Tmp_MM == 0xFF)
//  {
//    Tmp_MM = USART_Scanf(12);
//  }
//  printf("Month set as: %d\n", Tmp_MM);

//  DateToUpdate.Month= Tmp_MM;

//  printf("Please enter Day: \n");
//  while (Tmp_DD == 0xFF)
//  {
//    Tmp_DD = USART_Scanf(31);
//  }
//  printf("Day set as: %d\n", Tmp_DD);
//  DateToUpdate.Date= Tmp_DD;
//  HAL_RTC_SetDate(&IHU_BSP_STM32_RTC_HANDLER, &DateToUpdate, RTC_FORMAT_BIN);
//  
//  printf("Please enter hour: \n");
//  while (Tmp_HH == 0xFF)
//  {
//    Tmp_HH = USART_Scanf(23);
//  }
//  printf("Hour set as: %d\n", Tmp_HH );
//  sTime.Hours= Tmp_HH;


//  printf("Please enter minute: \n");
//  while (Tmp_MI == 0xFF)
//  {
//    Tmp_MI = USART_Scanf(59);
//  }
//  printf("Minute set as: %d\n", Tmp_MI);
//  sTime.Minutes= Tmp_MI;

//  printf("Please enter second: \n");
//  while (Tmp_SS == 0xFF)
//  {
//    Tmp_SS = USART_Scanf(59);
//  }
//  printf("Second set as: %d\n", Tmp_SS);
//  sTime.Seconds= Tmp_SS;
//  HAL_RTC_SetTime(&IHU_BSP_STM32_RTC_HANDLER, &sTime, RTC_FORMAT_BIN);
//#endif

//  /* 写入一个数值：0x32F1到RTC备份数据寄存器1 */
//  HAL_RTCEx_BKUPWrite(&IHU_BSP_STM32_RTC_HANDLER, RTC_BKP_DR1, 0x32F1);
//}

//HAL_RTC_GetTime(),HAL_RTC_GetDate()

uint32_t ihu_bsp_stm32_rtc_get_current_unix_time(void)
{
	RTC_DateTypeDef sdatestructure;
	RTC_TimeTypeDef stimestructure;
	
	uint32_t secondvalue = 0;
	HAL_RTC_GetDate(&IHU_BSP_STM32_RTC_HANDLER, &sdatestructure, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&IHU_BSP_STM32_RTC_HANDLER, &stimestructure, RTC_FORMAT_BIN);	

	//根据年月日的精确计算（闰年/大小月），未来再优化
	secondvalue = (sdatestructure.Year+2000-1970)*365*24*3600 + sdatestructure.Month*30*24*3600 + sdatestructure.Date*24*3600 + \
	  stimestructure.Hours*3600 + stimestructure.Minutes*60 + stimestructure.Seconds;
	
	return secondvalue;
}

//取得当前以字符串为标识的时间
void ihu_bsp_stm32_rtc_get_current_ymdhms_time(char *output)
{
	RTC_DateTypeDef sdatestructure;
	RTC_TimeTypeDef stimestructure;

	HAL_RTC_GetDate(&IHU_BSP_STM32_RTC_HANDLER, &sdatestructure, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&IHU_BSP_STM32_RTC_HANDLER, &stimestructure, RTC_FORMAT_BIN);	
	//输出
	sprintf(output, "%04d/%02d/%02d, %02d:%02d:%02d", sdatestructure.Year+2000, sdatestructure.Month, sdatestructure.Date, stimestructure.Hours, stimestructure.Minutes, stimestructure.Seconds);	
}

//取得当前以字符串为标识的日期
void ihu_bsp_stm32_rtc_get_current_ymd_time(char *output)
{
	RTC_DateTypeDef sdatestructure;

	HAL_RTC_GetDate(&IHU_BSP_STM32_RTC_HANDLER, &sdatestructure, RTC_FORMAT_BIN);
	//输出
	sprintf(output, "%04d/%02d/%02d", sdatestructure.Year+2000, sdatestructure.Month, sdatestructure.Date);	
}

//取得当前以字符串为标识的时间
void ihu_bsp_stm32_rtc_get_current_hms_time(char *output)
{
	RTC_TimeTypeDef stimestructure;

	HAL_RTC_GetTime(&IHU_BSP_STM32_RTC_HANDLER, &stimestructure, RTC_FORMAT_BIN);	
	//输出
	sprintf(output, "%02d:%02d:%02d", stimestructure.Hours, stimestructure.Minutes, stimestructure.Seconds);	
}


