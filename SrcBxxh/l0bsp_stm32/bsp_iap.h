#ifndef __BSP_STM32_IAP_H
#define __BSP_STM32_IAP_H

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
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)
	#include "commsgiap.h"	
#else
#endif

//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名

/* 类型定义 ------------------------------------------------------------------*/
/************************** IAP 数据类型定义********************************/
typedef  void ( * pIapFun_TypeDef ) ( void ); //定义一个函数类型的参数.

/* 宏定义 --------------------------------------------------------------------*/
/************************** IAP 宏参数定义********************************/
 //是否更新 APP 到 FLASH，否则更新到 RAM
#define User_Flash                                        

#ifdef User_Flash
  #define APP_START_ADDR       0x8010000  	//应用程序起始地址(存放在FLASH)
#else
  #define APP_START_ADDR       0x20001000  	//应用程序起始地址(存放在RAM)
#endif

/************************** IAP 外部变量********************************/
#define APP_FLASH_LEN  			   2 //56320u       //定义 APP 固件最大容量，55kB=55*1024=56320

/* 扩展变量 ------------------------------------------------------------------*/
extern struct  STRUCT_IAP_RECIEVE      //串口数据帧的处理结构体
{
	uint8_t   ucDataBuf[APP_FLASH_LEN];
	uint16_t  usLength;
} strAppBin;


//本地定义的交换矩阵


//全局函数


//Local APIs
void IAP_Write_App_Bin( uint32_t appxaddr, uint8_t * appbuf, uint32_t applen);	//在指定地址开始,写入bin
void IAP_ExecuteApp( uint32_t appxaddr );			                              //执行flash里面的app程序
void IAP_write_app_bin_all_in_one(void);
void IAP_execute_app_bin_all_in_one(void);


#ifdef __cplusplus
}
#endif
#endif /* __BSP_STM32_IAP_H */

