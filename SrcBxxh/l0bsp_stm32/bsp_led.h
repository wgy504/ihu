#ifndef __BSP_STM32_LED_H__
#define __BSP_STM32_LED_H__

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
	#error Un-correct constant definition
#endif

//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名

//目前多个项目约定俗成的基础配置
#if ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID) || \
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID) || \
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID))
//本地定义的交换矩阵	
	#define BSP_STM32_LED_POWER_GPIO_PIN 				CUBEMX_PIN_F2_GPIO_DO_LED_POWER_Pin
	#define BSP_STM32_LED_POWER_GPIO						CUBEMX_PIN_F2_GPIO_DO_LED_POWER_GPIO_Port
	#define BSP_STM32_LED_COMMU_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DO_LED_COMMU_Pin
	#define BSP_STM32_LED_COMMU_GPIO						CUBEMX_PIN_F2_GPIO_DO_LED_COMMU_GPIO_Port
	#define BSP_STM32_LED_WORK_STATE_GPIO_PIN		CUBEMX_PIN_F2_GPIO_DO_LED_WORK_STATE_Pin
	#define BSP_STM32_LED_WORK_STATE_GPIO				CUBEMX_PIN_F2_GPIO_DO_LED_WORK_STATE_GPIO_Port
	#define BSP_STM32_BEEP_GPIO_PIN							CUBEMX_PIN_F2_GPIO_DO_BEEP_Pin
	#define BSP_STM32_BEEP_GPIO									CUBEMX_PIN_F2_GPIO_DO_BEEP_GPIO_Port	
//有效的函数体
	#define BSP_STM32_LED_POWER_ON              HAL_GPIO_WritePin(BSP_STM32_LED_POWER_GPIO, BSP_STM32_LED_POWER_GPIO_PIN, GPIO_PIN_RESET)    	// 输出高电平
	#define BSP_STM32_LED_POWER_OFF             HAL_GPIO_WritePin(BSP_STM32_LED_POWER_GPIO, BSP_STM32_LED_POWER_GPIO_PIN, GPIO_PIN_SET)  	// 输出低电平
	#define BSP_STM32_LED_POWER_TOGGLE          HAL_GPIO_TogglePin(BSP_STM32_LED_POWER_GPIO, BSP_STM32_LED_POWER_GPIO_PIN)                	// 输出反转
	#define BSP_STM32_LED_COMMU_ON              HAL_GPIO_WritePin(BSP_STM32_LED_COMMU_GPIO, BSP_STM32_LED_COMMU_GPIO_PIN, GPIO_PIN_RESET)    	// 输出高电平
	#define BSP_STM32_LED_COMMU_OFF             HAL_GPIO_WritePin(BSP_STM32_LED_COMMU_GPIO, BSP_STM32_LED_COMMU_GPIO_PIN, GPIO_PIN_SET)  	// 输出低电平
	#define BSP_STM32_LED_COMMU_TOGGLE          HAL_GPIO_TogglePin(BSP_STM32_LED_COMMU_GPIO, BSP_STM32_LED_COMMU_GPIO_PIN)                	// 输出反转
	#define BSP_STM32_LED_WORK_STATE_ON         HAL_GPIO_WritePin(BSP_STM32_LED_WORK_STATE_GPIO, BSP_STM32_LED_WORK_STATE_GPIO_PIN, GPIO_PIN_RESET)    	// 输出高电平
	#define BSP_STM32_LED_WORK_STATE_OFF        HAL_GPIO_WritePin(BSP_STM32_LED_WORK_STATE_GPIO, BSP_STM32_LED_WORK_STATE_GPIO_PIN, GPIO_PIN_SET)  	// 输出低电平
	#define BSP_STM32_LED_WORK_STATE_TOGGLE     HAL_GPIO_TogglePin(BSP_STM32_LED_WORK_STATE_GPIO, BSP_STM32_LED_WORK_STATE_GPIO_PIN)                	// 输出反转
	#define BSP_STM32_BEEP_ON                   HAL_GPIO_WritePin(BSP_STM32_BEEP_GPIO, BSP_STM32_BEEP_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_BEEP_OFF                  HAL_GPIO_WritePin(BSP_STM32_BEEP_GPIO, BSP_STM32_BEEP_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_BEEP_TOGGLE               HAL_GPIO_TogglePin(BSP_STM32_BEEP_GPIO, BSP_STM32_BEEP_GPIO_PIN)                	// 输出反转
#else
	#error Un-correct constant definition
#endif

//单个项目独立的LED配置
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
//本地定义的交换矩阵	
	#define BSP_STM32_LED_SERV1_GPIO_PIN 				CUBEMX_PIN_F2_GPIO_DO_LED_SERV1_Pin
	#define BSP_STM32_LED_SERV1_GPIO						CUBEMX_PIN_F2_GPIO_DO_LED_SERV1_GPIO_Port
	#define BSP_STM32_LED_SERV2_GPIO_PIN 				CUBEMX_PIN_F2_GPIO_DO_LED_SERV2_Pin
	#define BSP_STM32_LED_SERV2_GPIO						CUBEMX_PIN_F2_GPIO_DO_LED_SERV2_GPIO_Port
	#define BSP_STM32_LED_SERV3_GPIO_PIN 				CUBEMX_PIN_F2_GPIO_DO_LED_SERV3_Pin
	#define BSP_STM32_LED_SERV3_GPIO						CUBEMX_PIN_F2_GPIO_DO_LED_SERV3_GPIO_Port
//有效的函数体
	#define BSP_STM32_LED_SERV1_ON              HAL_GPIO_WritePin(BSP_STM32_LED_SERV1_GPIO, BSP_STM32_LED_SERV1_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_LED_SERV1_OFF             HAL_GPIO_WritePin(BSP_STM32_LED_SERV1_GPIO, BSP_STM32_LED_SERV1_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_LED_SERV1_TOGGLE          HAL_GPIO_TogglePin(BSP_STM32_LED_SERV1_GPIO, BSP_STM32_LED_SERV1_GPIO_PIN)                	// 输出反转
	#define BSP_STM32_LED_SERV2_ON              HAL_GPIO_WritePin(BSP_STM32_LED_SERV2_GPIO, BSP_STM32_LED_SERV2_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_LED_SERV2_OFF             HAL_GPIO_WritePin(BSP_STM32_LED_SERV2_GPIO, BSP_STM32_LED_SERV2_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_LED_SERV2_TOGGLE          HAL_GPIO_TogglePin(BSP_STM32_LED_SERV2_GPIO, BSP_STM32_LED_SERV2_GPIO_PIN)                	// 输出反转
	#define BSP_STM32_LED_SERV3_ON              HAL_GPIO_WritePin(BSP_STM32_LED_SERV3_GPIO, BSP_STM32_LED_SERV3_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_LED_SERV3_OFF             HAL_GPIO_WritePin(BSP_STM32_LED_SERV3_GPIO, BSP_STM32_LED_SERV3_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_LED_SERV3_TOGGLE          HAL_GPIO_TogglePin(BSP_STM32_LED_SERV3_GPIO, BSP_STM32_LED_SERV3_GPIO_PIN)                	// 输出反转
#endif


//以下两个是DEMO板子上的测试代码，未来需要移除掉
//LED_F205_BOARD
#define LED_F205_BOARD_PORT 								GPIOC
#define LED_F205_BOARD_PIN									GPIO_PIN_2
#define LED_F205RG_board_on()								HAL_GPIO_WritePin(LED_F205_BOARD_PORT, LED_F205_BOARD_PIN, GPIO_PIN_SET)
#define LED_F205RG_board_off()							HAL_GPIO_WritePin(LED_F205_BOARD_PORT, LED_F205_BOARD_PIN, GPIO_PIN_RESET)
#define LED_F205RG_board_negation() 	 			HAL_GPIO_TogglePin(LED_F205_BOARD_PORT, LED_F205_BOARD_PIN)
//LED_F205_TIMER_IND
#define LED_F205_TIMER_IND_PORT 						GPIOC
#define LED_F205_TIMER_IND_PIN							GPIO_PIN_3
#define LED_F205RG_timer_ind_on()						HAL_GPIO_WritePin(LED_F205_TIMER_IND_PORT, LED_F205_TIMER_IND_PIN, GPIO_PIN_SET)
#define LED_F205RG_timer_ind_off()					HAL_GPIO_WritePin(LED_F205_TIMER_IND_PORT, LED_F205_TIMER_IND_PIN, GPIO_PIN_RESET)
#define LED_F205RG_timer_ind_negation() 	 	HAL_GPIO_TogglePin(LED_F205_TIMER_IND_PORT, LED_F205_TIMER_IND_PIN)
//ZJL: 另外一种方式，就是直接使用LED宏定义搞定
#define LED_F205RG_onoff(port, pin, onoff) if (onoff) HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET); else HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);


//全局函数定义
#if ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID) || \
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID))
extern void ihu_bsp_stm32_led_power_f2board_on(void);
extern void ihu_bsp_stm32_led_power_f2board_off(void);
extern void ihu_bsp_stm32_led_power_f2board_toggle(void);
extern void ihu_bsp_stm32_led_commu_f2board_on(void);
extern void ihu_bsp_stm32_led_commu_f2board_off(void);
extern void ihu_bsp_stm32_led_commu_f2board_toggle(void);
extern void ihu_bsp_stm32_led_work_state_f2board_on(void);
extern void ihu_bsp_stm32_led_work_state_f2board_off(void);
extern void ihu_bsp_stm32_led_work_state_f2board_toggle(void);
extern void ihu_bsp_stm32_beep_f2board_on(void);
extern void ihu_bsp_stm32_beep_f2board_off(void);
extern void ihu_bsp_stm32_beep_f2board_toggle(void);
extern void ihu_bsp_stm32_beep_f2board_pattern_05s(void);
extern void ihu_bsp_stm32_beep_f2board_pattern_1s(void);
extern void ihu_bsp_stm32_beep_f2board_pattern_2s(void);
#else
	#error Un-correct constant definition
#endif
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
extern void ihu_bsp_stm32_led_serv1_f2board_on(void);
extern void ihu_bsp_stm32_led_serv1_f2board_off(void);
extern void ihu_bsp_stm32_led_serv1_f2board_toggle(void);
extern void ihu_bsp_stm32_led_serv2_f2board_on(void);
extern void ihu_bsp_stm32_led_serv2_f2board_off(void);
extern void ihu_bsp_stm32_led_serv2_f2board_toggle(void);
extern void ihu_bsp_stm32_led_serv3_f2board_on(void);
extern void ihu_bsp_stm32_led_serv3_f2board_off(void);
extern void ihu_bsp_stm32_led_serv3_f2board_toggle(void);
#endif


//待改进，目前纯粹用于207VC评估板测试，未来需要改进管腿配置，从而明确工作状态指示灯
extern void ihu_bsp_stm32_led_f2board_on(void);
extern void ihu_bsp_stm32_led_f2board_off(void);
extern void ihu_bsp_stm32_led_f2board_negation(void);
extern void ihu_bsp_stm32_led_f2board_timer_ind_on(void);
extern void ihu_bsp_stm32_led_f2board_timer_ind_off(void);
extern void ihu_bsp_stm32_led_f2board_timer_ind_negation(void);
extern void ihu_bsp_stm32_led_f2board_test_flag(int times, int dur);


//Local APIs
static void func_bsp_stm32_led_delay(__IO uint32_t nCount);



#ifdef __cplusplus
}
#endif
#endif  // __BSP_STM32_LED_H__

