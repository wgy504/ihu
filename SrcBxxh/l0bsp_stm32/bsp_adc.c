/**
  ******************************************************************************
  * 文件名程: bsp_adc.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 光敏电阻模块驱动程序
  ******************************************************************************
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_adc.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
ADC_HandleTypeDef hadcx;
DMA_HandleTypeDef hdma_adcx;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: AD转换初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void MX_ADCx_Init(void)
{
  ADC_ChannelConfTypeDef sConfig;

  // ADC功能配置
  hadcx.Instance = ADCx;
  //hadcx.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadcx.Init.ContinuousConvMode = ENABLE;
  hadcx.Init.DiscontinuousConvMode = DISABLE;
  hadcx.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadcx.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadcx.Init.NbrOfConversion = 1;
  HAL_ADC_Init(&hadcx);

  // 配置采样通道
  sConfig.Channel = ADC_CHANNEL;
  sConfig.Rank = 1;
  //sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  HAL_ADC_ConfigChannel(&hadcx, &sConfig);
}

/**
  * 函数功能: ADC外设初始化配置
  * 输入参数: hadc：AD外设句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
//void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
//{
//  GPIO_InitTypeDef GPIO_InitStruct;
//  if(hadc->Instance==ADCx)
//  {
//    /* 外设时钟使能 */
//    ADCx_RCC_CLK_ENABLE();
//    
//    /* AD转换通道引脚时钟使能 */
//    ADC_GPIO_ClK_ENABLE();
//    
//    /* DMA时钟使能 */
//    DMAx_RCC_CLK_ENABLE();
//      
//    /* AD转换通道引脚初始化 */
//    GPIO_InitStruct.Pin = ADC_GPIO_PIN;
//    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//    HAL_GPIO_Init(ADC_GPIO, &GPIO_InitStruct);

//    /* DMA外设初始化配置 */  
//    //hdma_adcx.Instance = ADC_DMAx_CHANNELn;
//    hdma_adcx.Init.Direction = DMA_PERIPH_TO_MEMORY;
//    hdma_adcx.Init.PeriphInc = DMA_PINC_DISABLE;
//    hdma_adcx.Init.MemInc = DMA_MINC_ENABLE;
//    hdma_adcx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
//    hdma_adcx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
//    hdma_adcx.Init.Mode = DMA_CIRCULAR;
//    hdma_adcx.Init.Priority = DMA_PRIORITY_HIGH;
//    HAL_DMA_Init(&hdma_adcx);
//    /* 连接DMA */
//    __HAL_LINKDMA(hadc,DMA_Handle,hdma_adcx);
//    
//    /* 外设中断优先级配置和使能中断 */
//    HAL_NVIC_SetPriority(ADC_DMAx_CHANNELn_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(ADC_DMAx_CHANNELn_IRQn);
//  }
//}

/**
  * 函数功能: ADC外设反初始化配置
  * 输入参数: hadc：AD外设句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
//void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
//{
//  if(hadc->Instance==ADCx)
//  {
//    /* 禁用ADC外设时钟 */
//    ADCx_RCC_CLK_DISABLE();
//  
//    /* AD转换通道引脚反初始化 */
//    HAL_GPIO_DeInit(ADC_GPIO, ADC_GPIO_PIN);

//    /* DMA外设反初始化*/
//    HAL_DMA_DeInit(hadc->DMA_Handle);
//  }
//} 

void XIN_GPIO_Init(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStruct;
	
  XIN_GPIO_ClK_ENABLE();
  
  GPIO_InitStruct.Pin = XIN_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(XIN_GPIO, &GPIO_InitStruct);
  /* 外部中断初始化*/
  HAL_NVIC_SetPriority(XIN_EXTI_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(XIN_EXTI_IRQn);

}


