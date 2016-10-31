/**
  ******************************************************************************
  * 文件名程: bsp_usartx_RS485.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 板载串口底层驱动程序
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_usartx_RS485.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
UART_HandleTypeDef husartx_rs485;

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: RS485通信功能引脚GPIO初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void RS485_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 串口外设时钟使能 */
  RS485_USART_RCC_CLK_ENABLE();
  RS485_USARTx_GPIO_ClK_ENABLE();
  RS485_REDE_GPIO_ClK_ENABLE();
  
  /* 串口外设功能GPIO配置 */
  GPIO_InitStruct.Pin = RS485_USARTx_Tx_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(RS485_USARTx_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = RS485_USARTx_Rx_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RS485_USARTx_PORT, &GPIO_InitStruct);
  
#if USE_RS485==1
  /* RS485控制引脚：(1)低电平：接收模式 (2)高电平：发送模式 */
  HAL_GPIO_WritePin(RS485_REDE_PORT,RS485_REDE_PIN,GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = RS485_REDE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(RS485_REDE_PORT, &GPIO_InitStruct);
  RS485_RX_MODE();
#endif
}

/**
  * 函数功能: 串口参数配置.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void RS485_USARTx_Init(void)
{ 
  /* RS485通信功能引脚GPIO初始化 */
  RS485_GPIO_Init();
  
  husartx_rs485.Instance = RS485_USARTx;
  husartx_rs485.Init.BaudRate = RS485_USARTx_BAUDRATE;
  husartx_rs485.Init.WordLength = UART_WORDLENGTH_8B;
  husartx_rs485.Init.StopBits = UART_STOPBITS_1;
  husartx_rs485.Init.Parity = UART_PARITY_NONE;
  husartx_rs485.Init.Mode = UART_MODE_TX_RX;
  husartx_rs485.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husartx_rs485.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&husartx_rs485);
  
  __HAL_UART_ENABLE(&husartx_rs485);
  
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USARTx_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USARTx_IRQn);  
}
/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
