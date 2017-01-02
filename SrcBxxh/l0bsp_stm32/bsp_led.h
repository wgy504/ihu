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
#else
#endif

//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名

	
	
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
#define BSP_STM32_LED_POWER_ON              HAL_GPIO_WritePin(BSP_STM32_LED_POWER_GPIO, BSP_STM32_LED_POWER_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
#define BSP_STM32_LED_POWER_OFF             HAL_GPIO_WritePin(BSP_STM32_LED_POWER_GPIO, BSP_STM32_LED_POWER_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
#define BSP_STM32_LED_POWER_TOGGLE          HAL_GPIO_TogglePin(BSP_STM32_LED_POWER_GPIO, BSP_STM32_LED_POWER_GPIO_PIN)                	// 输出反转
#define BSP_STM32_LED_COMMU_ON              HAL_GPIO_WritePin(BSP_STM32_LED_COMMU_GPIO, BSP_STM32_LED_COMMU_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
#define BSP_STM32_LED_COMMU_OFF             HAL_GPIO_WritePin(BSP_STM32_LED_COMMU_GPIO, BSP_STM32_LED_COMMU_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
#define BSP_STM32_LED_COMMU_TOGGLE          HAL_GPIO_TogglePin(BSP_STM32_LED_COMMU_GPIO, BSP_STM32_LED_COMMU_GPIO_PIN)                	// 输出反转
#define BSP_STM32_LED_WORK_STATE_ON         HAL_GPIO_WritePin(BSP_STM32_LED_WORK_STATE_GPIO, BSP_STM32_LED_WORK_STATE_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
#define BSP_STM32_LED_WORK_STATE_OFF        HAL_GPIO_WritePin(BSP_STM32_LED_WORK_STATE_GPIO, BSP_STM32_LED_WORK_STATE_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
#define BSP_STM32_LED_WORK_STATE_TOGGLE     HAL_GPIO_TogglePin(BSP_STM32_LED_WORK_STATE_GPIO, BSP_STM32_LED_WORK_STATE_GPIO_PIN)                	// 输出反转
#define BSP_STM32_BEEP_ON                   HAL_GPIO_WritePin(BSP_STM32_BEEP_GPIO, BSP_STM32_BEEP_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
#define BSP_STM32_BEEP_OFF                  HAL_GPIO_WritePin(BSP_STM32_BEEP_GPIO, BSP_STM32_BEEP_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
#define BSP_STM32_BEEP_TOGGLE               HAL_GPIO_TogglePin(BSP_STM32_BEEP_GPIO, BSP_STM32_BEEP_GPIO_PIN)                	// 输出反转


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

//待去掉，纯用于207VC评估板测试
extern void ihu_bsp_stm32_led_f2board_on(void);
extern void ihu_bsp_stm32_led_f2board_off(void);
extern void ihu_bsp_stm32_led_f2board_negation(void);
extern void ihu_bsp_stm32_led_f2board_timer_ind_on(void);
extern void ihu_bsp_stm32_led_f2board_timer_ind_off(void);
extern void ihu_bsp_stm32_led_f2board_timer_ind_negation(void);



//Local APIs


#ifdef __cplusplus
}
#endif
#endif  // __BSP_STM32_LED_H__

