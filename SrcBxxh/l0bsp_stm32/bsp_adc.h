#ifndef __BSP_STM32_ADC_H__
#define	__BSP_STM32_ADC_H__

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
#define BSP_STM32_ADC_ARRAY_NUMBER 10

	
//本地定义的交换矩阵
//在该项目下，已经定义了ADC1支持CPU温度测量的基础逻辑
#define IHU_BSP_STM32_ADC1_HANDLER							hadc1
#define IHU_BSP_STM32_ADC1_HANDLER_ID  					1
#define IHU_BSP_STM32_ADC2_HANDLER							hadc2
#define IHU_BSP_STM32_ADC2_HANDLER_ID  					2
#define IHU_BSP_STM32_ADC3_HANDLER							hadc3
#define IHU_BSP_STM32_ADC3_HANDLER_ID  					3

//全局函数API
//ADC的工作模式是：启动，读取，然后停止，不然会造成ADC长时间工作，徒耗功耗，不值得。
//如果某些不需要考虑功耗的地方，比如血压监测、组合秤，可以使用DMA模式，那种模式下，采样将是无限进行的。
//In IT mode
extern void ihu_bsp_stm32_adc_cpu_temp_start(void); 
extern void ihu_bsp_stm32_adc_cpu_temp_stop(void); 
//In normal mode
extern void ihu_bsp_stm32_adc1_start(void);   
extern int 	ihu_bsp_stm32_adc1_get_sample_value(void);
extern void ihu_bsp_stm32_adc1_stop(void); 
extern void ihu_bsp_stm32_adc2_start(void); 
extern int 	ihu_bsp_stm32_adc2_get_sample_value(void);
extern void ihu_bsp_stm32_adc2_stop(void);
extern void ihu_bsp_stm32_adc3_start(void);
extern int 	ihu_bsp_stm32_adc3_get_sample_value(void);
extern void ihu_bsp_stm32_adc3_stop(void);
//In DMA mode
//在该模式下，初始化启动以后，就一直直接访问共享内存中的采样结果即可，不用再关心具体的干活过程
extern void ihu_bsp_stm32_adc1_start_in_DMA_mode(void);
extern void ihu_bsp_stm32_adc2_start_in_DMA_mode(void);
extern void ihu_bsp_stm32_adc3_start_in_DMA_mode(void);

//Local APIs
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);


#ifdef __cplusplus
}
#endif
#endif /* __BSP_STM32_ADC_H__ */

