/**
  ******************************************************************************
  * 文件名程: bsp_adc.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 
  ******************************************************************************
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_adc.h"


//从MAIN.x中继承过来的函数
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	extern ADC_HandleTypeDef hadc1;
	ADC_HandleTypeDef hadc2; //MAIN中为定义，这里重新定义是为了复用
	ADC_HandleTypeDef hadc3; //MAIN中为定义，这里重新定义是为了复用
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	extern ADC_HandleTypeDef hadc1;
	ADC_HandleTypeDef hadc2;  //MAIN中为定义，这里重新定义是为了复用
	ADC_HandleTypeDef hadc3;	//MAIN中为定义，这里重新定义是为了复用
#else
	#error Un-correct constant definition
#endif

//本地全局变量
//CPU TEMP对应的AD转换结果值
uint16_t zIhuAdc1TemperatureSensorValue;
uint16_t zIhuAdc1ConvertedValue[BSP_STM32_ADC_ARRAY_NUMBER];
uint16_t zIhuAdc2ConvertedValue[BSP_STM32_ADC_ARRAY_NUMBER];
uint16_t zIhuAdc3ConvertedValue[BSP_STM32_ADC_ARRAY_NUMBER];

extern WeightSensorParamaters_t					zWeightSensorParam;

//In IT mode
void ihu_bsp_stm32_adc_cpu_temp_start(void)
{
	HAL_ADC_Start_IT(&IHU_BSP_STM32_ADC1_HANDLER);
}

//In IT mode
void ihu_bsp_stm32_adc_cpu_temp_stop(void)
{
	HAL_ADC_Stop_IT(&IHU_BSP_STM32_ADC1_HANDLER);
}

//In normal mode
void ihu_bsp_stm32_adc1_start(void)
{
	HAL_ADC_Start(&IHU_BSP_STM32_ADC1_HANDLER);
}

//In normal mode
void ihu_bsp_stm32_adc1_stop(void)
{
	HAL_ADC_Stop(&IHU_BSP_STM32_ADC1_HANDLER);
}

//In normal mode
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
int ihu_bsp_stm32_adc1_get_sample_value(void)
{
	//return HAL_ADC_GetValue(&IHU_BSP_STM32_ADC1_HANDLER);
	return WeightSensorReadCurrent(&zWeightSensorParam);  //MYC
}
#endif


//In normal mode
//定标，未来再完善，这里给出的数值只是NF2两位小数数据
//目前STM32CubeMX选择的12bit，所以应该将该小数转化为有意义的数据
//满量程是4095，而非4096
//这里选择的是百分比输出，如果用户真正希望看到的是电压，那又不一样的
//由于电压各式各样，原则上选择输出百分比，更好
//输出的百分比，带两位小数的，但采用了NF2+百分比的形式
int16_t ihu_bsp_stm32_adc1_ccl_get_battery_value(void)
{
	float temp = 0;
	
	ihu_bsp_stm32_adc1_start();
	temp = (float)HAL_ADC_GetValue(&IHU_BSP_STM32_ADC1_HANDLER);
	temp = temp / 4095 * 10000;
	ihu_bsp_stm32_adc1_stop();
	return (((int)temp)&0xFFFF);
}

/**
  * 函数功能: AD转换结束回调函数
  * 输入参数: hadc：AD设备类型句柄
  * 返 回 值: 无
  * 说    明: 无
  */
//对于12位的ADC，3.3V的ADC值为0xfff,温度为25度时对应的电压值为1.43V即0x6EE
#define BSP_STM32_ADC_V25  0x6EE
//斜率 每摄氏度4.3mV 对应每摄氏度0x05
#define BSP_STM32_ADC_AVG_SLOPE 0x05 
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc == &IHU_BSP_STM32_ADC1_HANDLER){
		zIhuAdc1TemperatureSensorValue = HAL_ADC_GetValue(hadc);
		zIhuAdc1TemperatureSensorValue = (BSP_STM32_ADC_V25-zIhuAdc1TemperatureSensorValue)/BSP_STM32_ADC_AVG_SLOPE + 25;
	}
}

//In DMA mode
void ihu_bsp_stm32_adc1_start_in_DMA_mode(void)
{
  /* 启动AD转换并使能DMA传输和中断 */
  HAL_ADC_Start_DMA(&IHU_BSP_STM32_ADC1_HANDLER,(uint32_t *)&zIhuAdc1ConvertedValue, sizeof(zIhuAdc1ConvertedValue));  
}

//In normal mode
void ihu_bsp_stm32_adc2_start(void)
{
	HAL_ADC_Start(&IHU_BSP_STM32_ADC2_HANDLER);
}

//In normal mode
void ihu_bsp_stm32_adc2_stop(void)
{
	HAL_ADC_Stop(&IHU_BSP_STM32_ADC2_HANDLER);
}

//In normal mode
void ihu_bsp_stm32_adc3_start(void)
{
	HAL_ADC_Start(&IHU_BSP_STM32_ADC3_HANDLER);
}

//In normal mode
void ihu_bsp_stm32_adc3_stop(void)
{
	HAL_ADC_Stop(&IHU_BSP_STM32_ADC3_HANDLER);
}

//In normal mode
int ihu_bsp_stm32_adc2_get_sample_value(void)
{
	return HAL_ADC_GetValue(&IHU_BSP_STM32_ADC2_HANDLER);
}

//In normal mode
int ihu_bsp_stm32_adc3_get_sample_value(void)
{
	return HAL_ADC_GetValue(&IHU_BSP_STM32_ADC3_HANDLER);
}

//In DMA mode
void ihu_bsp_stm32_adc2_start_in_DMA_mode(void)
{
  /* 启动AD转换并使能DMA传输和中断 */
  HAL_ADC_Start_DMA(&IHU_BSP_STM32_ADC2_HANDLER,(uint32_t *)&zIhuAdc2ConvertedValue, sizeof(zIhuAdc2ConvertedValue));  
}

//In DMA mode
void ihu_bsp_stm32_adc3_start_in_DMA_mode(void)
{
  /* 启动AD转换并使能DMA传输和中断 */
  HAL_ADC_Start_DMA(&IHU_BSP_STM32_ADC3_HANDLER,(uint32_t *)&zIhuAdc3ConvertedValue, sizeof(zIhuAdc3ConvertedValue));  
}


