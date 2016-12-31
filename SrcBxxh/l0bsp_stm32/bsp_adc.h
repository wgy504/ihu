#ifndef __ADC_H__
#define	__ADC_H__

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


/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
// 注意：用作ADC采集的IO必须没有复用，否则采集电压会有影响
/********************ADC输入通道（引脚）配置**************************/
#define ADCx_RCC_CLK_ENABLE()            __HAL_RCC_ADC1_CLK_ENABLE()
#define ADCx_RCC_CLK_DISABLE()           __HAL_RCC_ADC1_CLK_DISABLE()
#define DMAx_RCC_CLK_ENABLE()            __HAL_RCC_DMA1_CLK_ENABLE()
#define ADCx                             ADC1
#define ADC_DMAx_CHANNELn                DMA1_Channel1
#define ADC_DMAx_CHANNELn_IRQn           DMA1_Channel1_IRQn
#define ADC_DMAx_CHANNELn_IRQHANDLER     DMA1_Channel1_IRQHandler

#define ADC_GPIO_ClK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADC_GPIO                         GPIOC

#define ADC_GPIO_PIN                     GPIO_PIN_2        
#define ADC_CHANNEL                      ADC_CHANNEL_12   

/********************光敏模块D0端（引脚）配置**************************/    
#define XIN_GPIO_ClK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define XIN_GPIO                         GPIOC
#define XIN_GPIO_PIN                     GPIO_PIN_3           
           
#define XIN_EXTI_IRQn                    EXTI3_IRQn           
#define XIN_EXTI_IRQHandler              EXTI3_IRQHandler

/* 扩展变量 ------------------------------------------------------------------*/
extern ADC_HandleTypeDef hadcx;
extern DMA_HandleTypeDef hdma_adcx;
/* 函数声明 ------------------------------------------------------------------*/
void ihu_bsp_stm32_adc_hw_init(void);
void XIN_GPIO_Init(void);
int ihu_bsp_stm32_adc_get_sample_value(void);

#endif /* __ADC_H__ */

