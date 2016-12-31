#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

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

#define KEY_ON                        1
#define KEY_OFF                       0

#define KEY1_DOWNSTATE                1          /* 根据原理图设计，KEY1按下时引脚为高电平，所以这里设置为1 */
#define KEY2_DOWNSTATE                0          /* 根据原理图设计，KEY1按下时引脚为低电平，所以这里设置为0 */

/* 宏定义 --------------------------------------------------------------------*/
#define KEY1_RCC_CLK_ENABLE           __HAL_RCC_GPIOA_CLK_ENABLE
#define KEY1_GPIO_PIN                 GPIO_PIN_0
#define KEY1_GPIO                     GPIOA
//#define KEY1_DOWN_LEVEL               1  /* 根据原理图设计，KEY1按下时引脚为高电平，所以这里设置为1 */

#define KEY2_RCC_CLK_ENABLE           __HAL_RCC_GPIOC_CLK_ENABLE
#define KEY2_GPIO_PIN                 GPIO_PIN_13
#define KEY2_GPIO                     GPIOC
//#define KEY2_DOWN_LEVEL               0  /* 根据原理图设计，KEY1按下时引脚为低电平，所以这里设置为0 */

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
//用户函数声明
void KEY_GPIO_Init(void);
uint8_t GetPinStateOfKey1(void);
uint8_t GetPinStateOfKey2(void);
//用户函数声明结束


/*使能用户数据，每一个按键有一个变量可以供用户任意使用，如果不用
 *这个变量把括号内改成0即可；如果要用就改成1，但是这会浪费一个字
 *节的内存
 */
#define USER_DATA_EN			0

#define KEY_WOBBLE_TIME		10      //按键抖动时间。也就是消抖时间,单位ms
 
#define KEY_FIXED_PERIOD	10      //固定频率调用按键状态更新函数,括号内为调用周期，周期单位为ms

//定义回调函数类型
typedef  uint8_t    (*KEY_CALLBACK_PTR)(void);

#define KEY_TIMES_MAX (0XFF)
typedef enum{
	KEY_ACCESS_READ = 0,   //默认是读取
	KEY_ACCESS_WRITE_CLEAR = 0x01
}ACCESS_TYPE;

/************以下内容均不需要更改，直接使用即可*******************************/
typedef enum
{
	KEY_DOWN 				= 1,
	KEY_UP	 				= 2,
	KEY_UP_WOBBLE 	= 3,//确认弹起消抖状态
	KEY_DOWN_WOBBLE = 4 //确认按下消抖状态
}KEY_STATE;

typedef enum
{
	CHANGED 				    = 1,
	NOT_CHANGED	 				= 2,
}KEY_STATE_CHAGE;

typedef struct
{
	KEY_CALLBACK_PTR    GetStatePtr;	//用于获取按键状态的函数 
	uint8_t     				Times;				//按下并弹出后加1，使用后由应用程序减1
  KEY_STATE		        State;
  KEY_STATE_CHAGE     StateChange;                      
#ifdef KEY_FIXED_PERIOD
	unsigned char				Time_ms;			//用于固定周期调用状态更新函数的计时
#endif
  
#if USER_DATA_EN>0             
	uint8_t				      User_Data;		//用户变量，可由用户任意使用
#endif
  
}KEY;

/*			函数声明：	*/
void KeyCreate(KEY *p_Key,KEY_CALLBACK_PTR p_CallBack);
void Key_RefreshState(KEY* theKey);
uint8_t Key_AccessTimes(KEY* p_Key,ACCESS_TYPE opt);
uint8_t Key_AccessState(KEY* p_Key,KEY_STATE *p_State);

#endif  // __BSP_KEY_H__

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
