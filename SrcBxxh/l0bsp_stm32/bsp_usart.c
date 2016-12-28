/**
  ******************************************************************************
  * 文件名程: bsp_usart.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2016-10-04
  * 功    能: 板载调试串口底层驱动程序：默认使用USART1
  ******************************************************************************
  * 
  ******************************************************************************
*/

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_usart.h"


/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
//从MAIN.x中继承过来的函数
//UART_HandleTypeDef husart_debug;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
extern uint8_t zIhuUartRxBuffer[6];

//从SCYCB项目中继承过来的全局变量，待定
int8_t BSP_STM32_SPS_GPRS_R_Buff[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN];			//串口GPRS数据接收缓冲区 
int8_t BSP_STM32_SPS_GPRS_R_State=0;												//串口GPRS接收状态
int16_t BSP_STM32_SPS_GPRS_R_Count=0;					//当前接收数据的字节数
int16_t BSP_STM32_SPS_GRPS_R_Len=0;
int8_t BSP_STM32_SPS_RFID_R_Buff[IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN];			//串口RFID数据接收缓冲区 
int8_t BSP_STM32_SPS_RFID_R_State=0;												//串口RFID接收状态
int16_t BSP_STM32_SPS_RFID_R_Count=0;					//当前接收数据的字节数 	  
int16_t BSP_STM32_SPS_RFID_R_Len=0;
int8_t BSP_STM32_SPS_BLE_R_Buff[IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN];			//串口BLE数据接收缓冲区 
int8_t BSP_STM32_SPS_BLE_R_State=0;												//串口BLE接收状态
int16_t BSP_STM32_SPS_BLE_R_Count=0;					//当前接收数据的字节数 	  
int16_t BSP_STM32_SPS_BLE_R_Len=0;
int8_t BSP_STM32_SPS_PRINT_R_Buff[IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN];			//串口PRINT数据接收缓冲区 
int8_t BSP_STM32_SPS_PRINT_R_State=0;												//串口PRINT接收状态
int16_t BSP_STM32_SPS_PRINT_R_Count=0;					//当前接收数据的字节数 	  
int16_t BSP_STM32_SPS_PRINT_R_Len=0;
int8_t BSP_STM32_SPS_SPARE1_R_Buff[IHU_BSP_STM32_SPS_SPARE1_REC_MAX_LEN];			//串口SPARE1数据接收缓冲区 
int8_t BSP_STM32_SPS_SPARE1_R_State=0;												//串口SPARE1接收状态
int16_t BSP_STM32_SPS_SPARE1_R_Count=0;					//当前接收数据的字节数 	  
int16_t BSP_STM32_SPS_SPARE1_R_Len=0;
int8_t BSP_STM32_SPS_SPARE2_R_Buff[IHU_BSP_STM32_SPS_SPARE2_REC_MAX_LEN];			//串口SPARE2数据接收缓冲区 
int8_t BSP_STM32_SPS_SPARE2_R_State=0;												//串口SPARE2接收状态
int16_t BSP_STM32_SPS_SPARE2_R_Count=0;					//当前接收数据的字节数 	  
int16_t BSP_STM32_SPS_SPARE2_R_Len=0;


/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

int ihu_bsp_stm32_sps_slave_hw_init(void)
{
	uint16_t k;
	for(k=0;k<IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		BSP_STM32_SPS_GPRS_R_Buff[k] = 0x00;
	}
  BSP_STM32_SPS_GPRS_R_Count = 0;               //接收字符串的起始存储位置
	BSP_STM32_SPS_GPRS_R_State = 0;
	BSP_STM32_SPS_GRPS_R_Len = 0;

	for(k=0;k<IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		BSP_STM32_SPS_RFID_R_Buff[k] = 0x00;
	}
  BSP_STM32_SPS_RFID_R_Count = 0;               //接收字符串的起始存储位置
	BSP_STM32_SPS_RFID_R_State = 0;
	BSP_STM32_SPS_RFID_R_Len = 0;	

	for(k=0;k<IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		BSP_STM32_SPS_BLE_R_Buff[k] = 0x00;
	}
  BSP_STM32_SPS_BLE_R_Count = 0;               //接收字符串的起始存储位置
	BSP_STM32_SPS_BLE_R_State = 0;
	BSP_STM32_SPS_BLE_R_Len = 0;	
	
	for(k=0;k<IHU_BSP_STM32_SPS_PRINT_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		BSP_STM32_SPS_PRINT_R_Buff[k] = 0x00;
	}
  BSP_STM32_SPS_PRINT_R_Count = 0;               //接收字符串的起始存储位置
	BSP_STM32_SPS_PRINT_R_State = 0;
	BSP_STM32_SPS_PRINT_R_Len = 0;	

	for(k=0;k<IHU_BSP_STM32_SPS_SPARE1_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		BSP_STM32_SPS_SPARE1_R_Buff[k] = 0x00;
	}
  BSP_STM32_SPS_SPARE1_R_Count = 0;               //接收字符串的起始存储位置
	BSP_STM32_SPS_SPARE1_R_State = 0;
	BSP_STM32_SPS_SPARE1_R_Len = 0;

	for(k=0;k<IHU_BSP_STM32_SPS_SPARE2_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		BSP_STM32_SPS_SPARE2_R_Buff[k] = 0x00;
	}
  BSP_STM32_SPS_SPARE2_R_Count = 0;               //接收字符串的起始存储位置
	BSP_STM32_SPS_SPARE2_R_State = 0;
	BSP_STM32_SPS_SPARE2_R_Len = 0;
	
	return BSP_SUCCESS;
}

/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int ihu_bsp_stm32_sps_print_fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&IHU_BSP_STM32_UART_PRINT_HANDLER, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int ihu_bsp_stm32_sps_print_fgetc(FILE * f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&IHU_BSP_STM32_UART_PRINT_HANDLER, &ch, 1, 0xffff);
  return ch;
}

//数据发送的时间延迟，这里设置为无穷大0xffff，未来需要进一步优化
int ihu_bsp_stm32_sps_print_data_send(char *s)
{
//	int i=0;
//	for (i=0; i<IHU_PRINT_CHAR_SIZE; i++){
//		if ((s[i]!='\0') &&(i<strlen(s))) HAL_UART_Transmit(&IHU_BSP_STM32_UART_PRINT_HANDLER, (uint8_t *)(s+i), 1, 100);					
//	}
//	return 1;
	if (HAL_UART_Transmit(&IHU_BSP_STM32_UART_PRINT_HANDLER, (uint8_t *)s, strlen(s), SPS_UART_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int ihu_bsp_stm32_sps_print_data_receive(char *s, uint16_t len)
{
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_PRINT_HANDLER, (uint8_t *)s, len, SPS_UART_RX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}


/*******************************************************************************
* 函数名  : USART_SendData
* 描述    : USART_GPRS发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
int ihu_bsp_stm32_sps_gprs_send_data(uint8_t* buff, uint16_t len)
{    
//	uint16_t i;

//	for(i=0; i<len; i++)  
//	{
//		HAL_UART_Transmit(&IHU_BSP_STM32_UART_GPRS_HANDLER, (uint8_t *)(buff+i), 1, 100);
//	}
	if (HAL_UART_Transmit(&IHU_BSP_STM32_UART_GPRS_HANDLER, (uint8_t *)buff, len, SPS_UART_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int ihu_bsp_stm32_sps_gprs_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_GPRS_HANDLER, buff, len, SPS_UART_RX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

int ihu_bsp_stm32_sps_gprs_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_GPRS_HANDLER, buff, len, timeout) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}


/*******************************************************************************
* 函数名  : USART_SendData
* 描述    : USART_RFID发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
int ihu_bsp_stm32_sps_rfid_send_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_UART_Transmit(&IHU_BSP_STM32_UART_RFID_HANDLER, (uint8_t *)buff, len, SPS_UART_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int ihu_bsp_stm32_sps_rfid_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_RFID_HANDLER, buff, len, SPS_UART_RX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

/*******************************************************************************
* 函数名  : USART_SendData
* 描述    : UART_BLE发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
int ihu_bsp_stm32_sps_ble_send_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_UART_Transmit(&IHU_BSP_STM32_UART_BLE_HANDLER, (uint8_t *)buff, len, SPS_UART_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

int ihu_bsp_stm32_sps_ble_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_BLE_HANDLER, buff, len, SPS_UART_RX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

/*******************************************************************************
* 函数名  : USART_SendData
* 描述    : UART_SPARE1发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
int ihu_bsp_stm32_sps_spare1_send_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_UART_Transmit(&IHU_BSP_STM32_UART_SPARE1_HANDLER, (uint8_t *)buff, len, SPS_UART_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

int ihu_bsp_stm32_sps_spare1_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_SPARE1_HANDLER, buff, len, SPS_UART_RX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}


/**
  * 串口接口完成回调函数的处理
  * 为什么需要重新执行HAL_UART_Receive_IT，待确定
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	uint8_t res = 0;
	msg_struct_spsvirgo_l2frame_rcv_t snd;
  if(UartHandle==&IHU_BSP_STM32_UART_GPRS_HANDLER)
  {
		res = zIhuUartRxBuffer[IHU_BSP_STM32_UART_GPRS_HANDLER_ID-1];
		BSP_STM32_SPS_GPRS_R_Buff[BSP_STM32_SPS_GPRS_R_Count++] = res;
		if (BSP_STM32_SPS_GPRS_R_Count >= IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN)
			BSP_STM32_SPS_GPRS_R_Count = 0;

		//为了IDLE状态下提高效率，直接分解为IDLE和ELSE
		if (BSP_STM32_SPS_GPRS_R_State == IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE)
		{
			//只有满足这么苛刻的条件，才算找到了帧头
			if ((res == IHU_HUITP_L2FRAME_STD_RX_START_FLAG_CHAR) && (BSP_STM32_SPS_GPRS_R_Count == 1))
			BSP_STM32_SPS_GPRS_R_State = IHU_HUITP_L2FRAME_STD_RX_STATE_START;
		}
		else
		{
			//收到CHECKSUM
			if((BSP_STM32_SPS_GPRS_R_State == IHU_HUITP_L2FRAME_STD_RX_STATE_START) && (BSP_STM32_SPS_GPRS_R_Count == 2))
			{
				BSP_STM32_SPS_GPRS_R_State = IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM;
			}
			//收到长度高位
			else if((BSP_STM32_SPS_GPRS_R_State == IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM) && (BSP_STM32_SPS_GPRS_R_Count == 3))
			{
				BSP_STM32_SPS_GPRS_R_State = IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN;
			}
			//收到长度低位
			else if((BSP_STM32_SPS_GPRS_R_State == IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN) && (BSP_STM32_SPS_GPRS_R_Count == 4))
			{
				BSP_STM32_SPS_GRPS_R_Len = ((BSP_STM32_SPS_GPRS_R_Buff[2] <<8) + BSP_STM32_SPS_GPRS_R_Buff[3]);
				//CHECKSUM及判定
				if ((BSP_STM32_SPS_GPRS_R_Buff[1] == (BSP_STM32_SPS_GPRS_R_Buff[0] ^ BSP_STM32_SPS_GPRS_R_Buff[2]^BSP_STM32_SPS_GPRS_R_Buff[3])) &&\
					(BSP_STM32_SPS_GRPS_R_Len < IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN-4))
				BSP_STM32_SPS_GPRS_R_State = IHU_HUITP_L2FRAME_STD_RX_STATE_BODY;
			}
			//收到BODY位
			else if((BSP_STM32_SPS_GPRS_R_State == IHU_HUITP_L2FRAME_STD_RX_STATE_BODY) && (BSP_STM32_SPS_GRPS_R_Len > 1))
			{
				BSP_STM32_SPS_GRPS_R_Len--;
			}
			//收到BODY最后一位
			else if((BSP_STM32_SPS_GPRS_R_State == IHU_HUITP_L2FRAME_STD_RX_STATE_BODY) && (BSP_STM32_SPS_GRPS_R_Len == 1))
			{
				BSP_STM32_SPS_GPRS_R_State = IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE;
				BSP_STM32_SPS_GRPS_R_Len = 0;
				BSP_STM32_SPS_GPRS_R_Count = 0;
				//发送数据到上层SPSVIRGO模块
				memset(&snd, 0, sizeof(msg_struct_spsvirgo_l2frame_rcv_t));
				memcpy(snd.data, &BSP_STM32_SPS_GPRS_R_Buff[4], ((BSP_STM32_SPS_GPRS_R_Buff[2]<<8)+BSP_STM32_SPS_GPRS_R_Buff[3]));
				snd.length = sizeof(msg_struct_spsvirgo_l2frame_rcv_t);				
				ihu_message_send(MSG_ID_SPS_L2FRAME_RCV, TASK_ID_SPSVIRGO, TASK_ID_VMFO, &snd, snd.length);				
			}
			//差错情况
			else{
				BSP_STM32_SPS_GPRS_R_State = IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE;
				BSP_STM32_SPS_GRPS_R_Len = 0;
				BSP_STM32_SPS_GPRS_R_Count = 0;
			}
		}
		//重新设置中断
		HAL_UART_Receive_IT(&IHU_BSP_STM32_UART_GPRS_HANDLER, &zIhuUartRxBuffer[IHU_BSP_STM32_UART_GPRS_HANDLER_ID-1], 1);
  }
  else if(UartHandle==&IHU_BSP_STM32_UART_RFID_HANDLER)
  {
		BSP_STM32_SPS_RFID_R_Buff[BSP_STM32_SPS_RFID_R_Count] = zIhuUartRxBuffer[IHU_BSP_STM32_UART_RFID_HANDLER_ID-1];
		BSP_STM32_SPS_RFID_R_Count++;
		if (BSP_STM32_SPS_RFID_R_Count >= IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN)
			BSP_STM32_SPS_RFID_R_Count = 0;
		HAL_UART_Receive_IT(&IHU_BSP_STM32_UART_RFID_HANDLER, &zIhuUartRxBuffer[IHU_BSP_STM32_UART_RFID_HANDLER_ID-1], 1);
  }
  else if(UartHandle==&IHU_BSP_STM32_UART_PRINT_HANDLER)
  {
		BSP_STM32_SPS_PRINT_R_Buff[BSP_STM32_SPS_PRINT_R_Count] = zIhuUartRxBuffer[IHU_BSP_STM32_UART_PRINT_HANDLER_ID-1];
		BSP_STM32_SPS_PRINT_R_Count++;
		if (BSP_STM32_SPS_PRINT_R_Count >= IHU_BSP_STM32_SPS_PRINT_REC_MAX_LEN)
			BSP_STM32_SPS_PRINT_R_Count = 0;
		HAL_UART_Receive_IT(&IHU_BSP_STM32_UART_PRINT_HANDLER, &zIhuUartRxBuffer[IHU_BSP_STM32_UART_PRINT_HANDLER_ID-1], 1);
  }	
  else if(UartHandle==&IHU_BSP_STM32_UART_BLE_HANDLER)
  {
		BSP_STM32_SPS_BLE_R_Buff[BSP_STM32_SPS_BLE_R_Count] = zIhuUartRxBuffer[IHU_BSP_STM32_UART_BLE_HANDLER_ID-1];
		BSP_STM32_SPS_BLE_R_Count++;
		if (BSP_STM32_SPS_BLE_R_Count >= IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN)
			BSP_STM32_SPS_BLE_R_Count = 0;
		HAL_UART_Receive_IT(&IHU_BSP_STM32_UART_BLE_HANDLER, &zIhuUartRxBuffer[IHU_BSP_STM32_UART_BLE_HANDLER_ID-1], 1);
  }	
  else if(UartHandle==&IHU_BSP_STM32_UART_SPARE1_HANDLER)
  {
		BSP_STM32_SPS_SPARE1_R_Buff[BSP_STM32_SPS_SPARE1_R_Count] = zIhuUartRxBuffer[IHU_BSP_STM32_UART_SPARE1_HANDLER_ID-1];
		BSP_STM32_SPS_SPARE1_R_Count++;
		if (BSP_STM32_SPS_SPARE1_R_Count >= IHU_BSP_STM32_SPS_SPARE1_REC_MAX_LEN)
			BSP_STM32_SPS_SPARE1_R_Count = 0;
		HAL_UART_Receive_IT(&IHU_BSP_STM32_UART_SPARE1_HANDLER, &zIhuUartRxBuffer[IHU_BSP_STM32_UART_SPARE1_HANDLER_ID-1], 1);
  }	
  else if(UartHandle==&IHU_BSP_STM32_UART_SPARE2_HANDLER)
  {
		BSP_STM32_SPS_SPARE2_R_Buff[BSP_STM32_SPS_SPARE2_R_Count] = zIhuUartRxBuffer[IHU_BSP_STM32_UART_SPARE2_HANDLER_ID-1];
		BSP_STM32_SPS_SPARE2_R_Count++;
		if (BSP_STM32_SPS_SPARE2_R_Count >= IHU_BSP_STM32_SPS_SPARE2_REC_MAX_LEN)
			BSP_STM32_SPS_SPARE2_R_Count = 0;
		HAL_UART_Receive_IT(&IHU_BSP_STM32_UART_SPARE2_HANDLER, &zIhuUartRxBuffer[IHU_BSP_STM32_UART_SPARE2_HANDLER_ID-1], 1);
  }
}


/**
  * 函数功能: 串口硬件初始化配置
  * 输入参数: huart：串口句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
//void HAL_UART_MspInit(UART_HandleTypeDef* huart)
//{

//  GPIO_InitTypeDef GPIO_InitStruct;
//  if(huart->Instance==DEBUG_USARTx)
//  {
//    /* 串口外设时钟使能 */
//    DEBUG_USART_RCC_CLK_ENABLE();
//  
//    /* 串口外设功能GPIO配置 */
//    GPIO_InitStruct.Pin = DEBUG_USARTx_Tx_GPIO_PIN;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(DEBUG_USARTx_Tx_GPIO, &GPIO_InitStruct);

//    GPIO_InitStruct.Pin = DEBUG_USARTx_Rx_GPIO_PIN;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    HAL_GPIO_Init(DEBUG_USARTx_Rx_GPIO, &GPIO_InitStruct);
//  }
//}

/**
  * 函数功能: 串口硬件反初始化配置
  * 输入参数: huart：串口句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
//void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
//{

//  if(huart->Instance==DEBUG_USARTx)
//  {
//    /* 串口外设时钟禁用 */
//    DEBUG_USART_RCC_CLK_DISABLE();
//  
//    /* 串口外设功能GPIO配置 */
//    HAL_GPIO_DeInit(DEBUG_USARTx_Tx_GPIO, DEBUG_USARTx_Tx_GPIO_PIN);
//    HAL_GPIO_DeInit(DEBUG_USARTx_Rx_GPIO, DEBUG_USARTx_Rx_GPIO_PIN);
//    
//    /* 串口中断禁用 */
//    HAL_NVIC_DisableIRQ(DEBUG_USART_IRQn);
//  }
//}

/**
  * 函数功能: 串口参数配置.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
//void MX_DEBUG_USART_Init(void)
//{
//  /* 使能串口功能引脚GPIO时钟 */
//  DEBUG_USARTx_GPIO_ClK_ENABLE();
//  
//  husart_debug.Instance = DEBUG_USARTx;
//  husart_debug.Init.BaudRate = DEBUG_USARTx_BAUDRATE;
//  husart_debug.Init.WordLength = UART_WORDLENGTH_9B;
//  husart_debug.Init.StopBits = UART_STOPBITS_1;
//  husart_debug.Init.Parity = UART_PARITY_EVEN;
//  husart_debug.Init.Mode = UART_MODE_TX_RX;
//  husart_debug.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//  husart_debug.Init.OverSampling = UART_OVERSAMPLING_16;
//  HAL_UART_Init(&husart_debug);
//  
//}

/*******************************************************************************
* 函数名  : USART_GPRS_Init_Config
* 描述    : USART_GPRS初始化配置
* 输入    : bound：波特率(常用：2400、4800、9600、19200、38400、115200等)
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
//void SPS_GPRS_Init_Config(uint32_t bound)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef  NVIC_InitStructure;
//	
//	/*使能USART_GPRS和GPIO外设时钟*/  
//	USART_GPRS_TX_ENABLE();
//	USART_GPRS_RX_ENABLE();
//	USART_GPRS_CLK_ENABLE();
//	
//	/*复位串口GPRS*/
// 	USART_DeInit(USART_GPRS);  
//	
//	/*USART_GPRS_GPIO初始化设置*/ 
//	GPIO_PinAFConfig(USART_GPRS_TX_GPIO_PORT, USART_GPRS_TX_SOURCE, USART_GPRS_TX_AF);
//	GPIO_PinAFConfig(USART_GPRS_RX_GPIO_PORT, USART_GPRS_RX_SOURCE, USART_GPRS_RX_AF);
//	GPIO_InitStructure.GPIO_Pin = USART_GPRS_TX_GPIO_PIN;			//USART_GPRS_TXD(PA.9)     
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置引脚输出最大速率为50MHz
//	GPIO_Init(USART_GPRS_TX_GPIO_PORT, &GPIO_InitStructure);				 //调用库函数中的GPIO初始化函数，初始化USART_GPRS_TXD(PA.9)  
// 
//	GPIO_InitStructure.GPIO_Pin = USART_GPRS_RX_GPIO_PIN;	 			//USART_GPRS_RXD(PA.10)
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  //浮空输入
//	GPIO_Init(USART_GPRS_RX_GPIO_PORT, &GPIO_InitStructure);					//调用库函数中的GPIO初始化函数，初始化USART_GPRS_RXD(PA.10)

//  /*USART_GPRS 初始化设置*/
//	USART_InitStructure.USART_BaudRate = bound;										//设置波特率
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据格式
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
//	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//工作模式设置为收发模式
//	USART_Init(USART_GPRS, &USART_InitStructure);										//初始化串口1

//  /*USART_GPRS NVIC配置*/
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
//  NVIC_InitStructure.NVIC_IRQChannel = USART_GPRS_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;	//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//从优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);													//根据指定的参数初始化VIC寄存器 
//	  
//  USART_ITConfig(USART_GPRS, USART_IT_RXNE, ENABLE);			//使能串口1接收中断 Receive Data register not empty interrupt 
//  USART_Cmd(USART_GPRS, ENABLE);                    			//使能串口 
//	USART_ClearFlag(USART_GPRS, USART_FLAG_TC);							//清除发送完成标志 Transmission complete interrupt
//	
//	//挂载中断
//	BSP_IntVectSet(USART_GPRS_INT_VECTOR, SPS_GPRS_IRQHandler);
//  BSP_IntEn(USART_GPRS_INT_VECTOR);
//}

///*******************************************************************************
//* 函数名  : USART_RFID_Init_Config
//* 描述    : USART_RFID初始化配置
//* 输入    : bound：波特率(常用：2400、4800、9600、19200、38400、115200等)
//* 输出    : 无
//* 返回    : 无 
//* 说明    : 无
//*******************************************************************************/
//void SPS_RFID_Init_Config(uint32_t bound)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef  NVIC_InitStructure;
//	
//	/*使能USART_RFID和GPIO外设时钟*/  
//	USART_RFID_TX_ENABLE();
//	USART_RFID_RX_ENABLE();
//	USART_RFID_CLK_ENABLE();
//	
//	/*复位串口RFID*/
// 	USART_DeInit(USART_RFID);  
//	
//	/*USART_RFID_GPIO初始化设置*/ 
//	GPIO_PinAFConfig(USART_RFID_TX_GPIO_PORT, USART_RFID_TX_SOURCE, USART_RFID_TX_AF);
//	GPIO_PinAFConfig(USART_RFID_RX_GPIO_PORT, USART_RFID_RX_SOURCE, USART_RFID_RX_AF);
//	GPIO_InitStructure.GPIO_Pin = USART_RFID_TX_GPIO_PIN;			//USART_RFID_TXD(PA.9)     
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置引脚输出最大速率为50MHz
//	GPIO_Init(USART_RFID_TX_GPIO_PORT, &GPIO_InitStructure);				 //调用库函数中的GPIO初始化函数，初始化USART_RFID_TXD(PA.9)  
// 
//	GPIO_InitStructure.GPIO_Pin = USART_RFID_RX_GPIO_PIN;	 			//USART_RFID_RXD(PA.10)
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  //浮空输入
//	GPIO_Init(USART_RFID_RX_GPIO_PORT, &GPIO_InitStructure);					//调用库函数中的GPIO初始化函数，初始化USART_RFID_RXD(PA.10)

//  /*USART_RFID 初始化设置*/
//	USART_InitStructure.USART_BaudRate = bound;										//设置波特率
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据格式
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
//	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//工作模式设置为收发模式
//	USART_Init(USART_RFID, &USART_InitStructure);										//初始化串口1

//  /*USART_RFID NVIC配置*/
//  NVIC_InitStructure.NVIC_IRQChannel = USART_RFID_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;	//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//从优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);							//根据指定的参数初始化VIC寄存器 
//	  
//  USART_ITConfig(USART_RFID, USART_IT_RXNE, ENABLE);			//使能串口1接收中断
//  USART_Cmd(USART_RFID, ENABLE);                    			//使能串口 
//	USART_ClearFlag(USART_RFID, USART_FLAG_TC);					//清除发送完成标志
//	
//	//挂载中断
//	BSP_IntVectSet(USART_RFID_INT_VECTOR, SPS_RFID_IRQHandler);
//  BSP_IntEn(USART_RFID_INT_VECTOR);
//}

///*******************************************************************************
//* 函数名  : UART_BLE_Init_Config
//* 描述    : UART_BLE初始化配置
//* 输入    : bound：波特率(常用：2400、4800、9600、19200、38400、115200等)
//* 输出    : 无
//* 返回    : 无 
//* 说明    : 无
//*******************************************************************************/
//void SPS_BLE_Init_Config(uint32_t bound)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef  NVIC_InitStructure;
//	
//	/*使能UART_BLE和GPIO外设时钟*/  
//	UART_BLE_TX_ENABLE();
//	UART_BLE_RX_ENABLE();
//	UART_BLE_CLK_ENABLE();
//	
//	/*复位串口BLE*/
// 	USART_DeInit(UART_BLE);  
//	
//	/*UART_BLE_GPIO初始化设置*/ 
//	GPIO_PinAFConfig(UART_BLE_TX_GPIO_PORT, UART_BLE_TX_SOURCE, UART_BLE_TX_AF);
//	GPIO_PinAFConfig(UART_BLE_RX_GPIO_PORT, UART_BLE_RX_SOURCE, UART_BLE_RX_AF);
//	GPIO_InitStructure.GPIO_Pin = UART_BLE_TX_GPIO_PIN;			//UART_BLE_TXD(PA.9)     
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置引脚输出最大速率为50MHz
//	GPIO_Init(UART_BLE_TX_GPIO_PORT, &GPIO_InitStructure);				 //调用库函数中的GPIO初始化函数，初始化UART_BLE_TXD(PA.9)  
// 
//	GPIO_InitStructure.GPIO_Pin = UART_BLE_RX_GPIO_PIN;	 			//UART_BLE_RXD(PA.10)
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  //浮空输入
//	GPIO_Init(UART_BLE_RX_GPIO_PORT, &GPIO_InitStructure);					//调用库函数中的GPIO初始化函数，初始化UART_BLE_RXD(PA.10)

//  /*UART_BLE 初始化设置*/
//	USART_InitStructure.USART_BaudRate = bound;										//设置波特率
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据格式
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
//	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//工作模式设置为收发模式
//	USART_Init(UART_BLE, &USART_InitStructure);										//初始化串口1

//  /*UART_BLE NVIC配置*/
//  NVIC_InitStructure.NVIC_IRQChannel = UART_BLE_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;	//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//从优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);							//根据指定的参数初始化VIC寄存器 
//	  
//  USART_ITConfig(UART_BLE, USART_IT_RXNE, ENABLE);			//使能串口1接收中断
//  USART_Cmd(UART_BLE, ENABLE);                    			//使能串口 
//	USART_ClearFlag(UART_BLE, USART_FLAG_TC);					//清除发送完成标志
//	
//	//挂载中断
//	BSP_IntVectSet(UART_BLE_INT_VECTOR, SPS_BLE_IRQHandler);
//  BSP_IntEn(UART_BLE_INT_VECTOR);
//}

///*******************************************************************************
//* 函数名  : UART_SPARE1_Init_Config
//* 描述    : UART_SPARE1初始化配置
//* 输入    : bound：波特率(常用：2400、4800、9600、19200、38400、115200等)
//* 输出    : 无
//* 返回    : 无 
//* 说明    : 无
//*******************************************************************************/
//void SPS_SPARE1_Init_Config(uint32_t bound)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef  NVIC_InitStructure;
//	
//	/*使能UART_SPARE1和GPIO外设时钟*/  
//	UART_SPARE1_TX_ENABLE();
//	UART_SPARE1_RX_ENABLE();
//	UART_SPARE1_CLK_ENABLE();
//	
//	/*复位串口SPARE1*/
// 	USART_DeInit(UART_SPARE1);  
//	
//	/*UART_SPARE1_GPIO初始化设置*/ 
//	GPIO_PinAFConfig(UART_SPARE1_TX_GPIO_PORT, UART_SPARE1_TX_SOURCE, UART_SPARE1_TX_AF);
//	GPIO_PinAFConfig(UART_SPARE1_RX_GPIO_PORT, UART_SPARE1_RX_SOURCE, UART_SPARE1_RX_AF);
//	GPIO_InitStructure.GPIO_Pin = UART_SPARE1_TX_GPIO_PIN;			//UART_SPARE1_TXD(PA.9)     
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置引脚输出最大速率为50MHz
//	GPIO_Init(UART_SPARE1_TX_GPIO_PORT, &GPIO_InitStructure);				 //调用库函数中的GPIO初始化函数，初始化UART_SPARE1_TXD(PA.9)  
// 
//	GPIO_InitStructure.GPIO_Pin = UART_SPARE1_RX_GPIO_PIN;	 			//UART_SPARE1_RXD(PA.10)
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  //浮空输入
//	GPIO_Init(UART_SPARE1_RX_GPIO_PORT, &GPIO_InitStructure);					//调用库函数中的GPIO初始化函数，初始化UART_SPARE1_RXD(PA.10)

//  /*UART_SPARE1 初始化设置*/
//	USART_InitStructure.USART_BaudRate = bound;										//设置波特率
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据格式
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
//	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//工作模式设置为收发模式
//	USART_Init(UART_SPARE1, &USART_InitStructure);										//初始化串口1

//  /*UART_SPARE1 NVIC配置*/
//  NVIC_InitStructure.NVIC_IRQChannel = UART_SPARE1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;	//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//从优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);							//根据指定的参数初始化VIC寄存器 
//	  
//  USART_ITConfig(UART_SPARE1, USART_IT_RXNE, ENABLE);			//使能串口1接收中断
//  USART_Cmd(UART_SPARE1, ENABLE);                    			//使能串口 
//	USART_ClearFlag(UART_SPARE1, USART_FLAG_TC);					//清除发送完成标志

//	//挂载中断
//	BSP_IntVectSet(UART_SPARE1_INT_VECTOR, SPS_SPARE1_IRQHandler);
//  BSP_IntEn(UART_SPARE1_INT_VECTOR);
//}

/*******************************************************************************
* 函数名  : SPS_GPRS_IRQHandler
* 描述    : 串口1中断服务程序
* 输入    : 无
* 返回    : 无 
* 说明    : 
*******************************************************************************/
//void SPS_GPRS_IRQHandler(void)                	
//{
//	SPS_GPRS_TIMER_TRIGGER_Count=0;
//	//要不要设计成这样的：中断进来后立即禁止再次重入，等待接收R_BUFFER被处理好了后再行ENABLE该中断？
//	//还是我们有更好的方式，比如直接采用USART_IT_RXNE比特位，这个比特位一旦拉高后，不会再次进来了？
//	//当然还有一种方式来规避这个问题，就是接收到了数据后立即发送给任务模块，这个接收就继续了。
//	//由于状态机控制的复杂性，如果不是要做正常的双向通信机制，我们这里将采用与TIMER配合的轮询方式来工作
//	//USART_ITConfig(USART_GPRS, USART_IT_RXNE, DISABLE);
//	
//	// 原始代码中，使用了USART_IT_RXNE进行判定，而非USART_FLAG_RXNE，后续需要仔细确定，到底该如何？
//	if(USART_GetITStatus(USART_GPRS, USART_IT_RXNE) != RESET) //接收中断
//	{
//		USART_ClearFlag(USART_GPRS, USART_FLAG_RXNE); 
//		USART_ClearITPendingBit(USART_GPRS, USART_IT_RXNE);
//		SPS_GPRS_R_Buff[SPS_GPRS_R_Count++] = USART_ReceiveData(USART_GPRS); //读取接收到的数据(USART_GPRS->DR)
//		if(SPS_GPRS_R_Count > SPS_GPRS_REC_MAXLEN)       		//如果缓存满,将缓存指针指向缓存的首地址
//		{
//			SPS_GPRS_R_Count = 0;
//		}
//	}
//}

///*******************************************************************************
//* 函数名  : USART_RFID_IRQHandler
//* 描述    : 串口1中断服务程序
//* 输入    : 无
//* 返回    : 无 
//* 说明    : 1)、只启动了USART_RFID中断接收，未启动USART_RFID中断发送。
//*           2)、接收到0x0d 0x0a(回车、"\r\n")代表帧数据接收完成
//*******************************************************************************/
//void SPS_RFID_IRQHandler(void)                	
//{
//	uint8_t Res=0;

//	if(USART_GetITStatus(USART_RFID, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//		Res =USART_ReceiveData(USART_RFID); //读取接收到的数据(USART_RFID->DR)
//		
//		SPS_RFID_R_Buff[SPS_RFID_R_Count++] = Res;
//		if(SPS_RFID_R_State == 0)//数据接收未完成
//		{
//			if(Res == 0x0d)//接收到0x0d,下一个字节接收到0x0a则接收完成
//			{
//				SPS_RFID_R_State =2;
//			}
//		}
//		else if(SPS_RFID_R_State == 2)
//		{
//			if(Res == 0x0a)//上一个字节接收到0x0d,这个字节接收到oxoa则接收完成
//			{
//				SPS_RFID_R_State =1;//数据接收完成
//			}
//			else//接收错误
//			{
//				SPS_RFID_R_State =0;
//				SPS_RFID_R_Count =0;
//			}
//		}
//		if(SPS_RFID_R_Count >= SPS_RFID_REC_MAXLEN)//接收数据长度走出接收数据缓冲区
//		{
//			if((SPS_RFID_R_Buff[SPS_RFID_REC_MAXLEN-2] != 0x0d) || (SPS_RFID_R_Buff[SPS_RFID_REC_MAXLEN-1] != 0x0a))
//			{
//				SPS_RFID_R_Count =0;
//				SPS_RFID_R_State =0;
//			}
//		} 		 
//	} 
//} 	

///*******************************************************************************
//* 函数名  : UART_BLE_IRQHandler
//* 描述    : 串口1中断服务程序
//* 输入    : 无
//* 返回    : 无 
//* 说明    : 1)、只启动了UART_BLE中断接收，未启动UART_BLE中断发送。
//*           2)、接收到0x0d 0x0a(回车、"\r\n")代表帧数据接收完成
//*******************************************************************************/
//void SPS_BLE_IRQHandler(void)                	
//{
//	uint8_t Res=0;

//	if(USART_GetITStatus(UART_BLE, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//		Res =USART_ReceiveData(UART_BLE); //读取接收到的数据(UART_BLE->DR)
//		
//		SPS_BLE_R_Buff[SPS_BLE_R_Count++] = Res;
//		if(SPS_BLE_R_State == 0)//数据接收未完成
//		{
//			if(Res == 0x0d)//接收到0x0d,下一个字节接收到0x0a则接收完成
//			{
//				SPS_BLE_R_State =2;
//			}
//		}
//		else if(SPS_BLE_R_State == 2)
//		{
//			if(Res == 0x0a)//上一个字节接收到0x0d,这个字节接收到oxoa则接收完成
//			{
//				SPS_BLE_R_State =1;//数据接收完成
//			}
//			else//接收错误
//			{
//				SPS_BLE_R_State =0;
//				SPS_BLE_R_Count =0;
//			}
//		}
//		if(SPS_BLE_R_Count >= SPS_BLE_REC_MAXLEN)//接收数据长度走出接收数据缓冲区
//		{
//			if((SPS_BLE_R_Buff[SPS_BLE_REC_MAXLEN-2] != 0x0d) || (SPS_BLE_R_Buff[SPS_BLE_REC_MAXLEN-1] != 0x0a))
//			{
//				SPS_BLE_R_Count =0;
//				SPS_BLE_R_State =0;
//			}
//		} 		 
//	} 
//}

///*******************************************************************************
//* 函数名  : UART_SPARE1_IRQHandler
//* 描述    : 串口1中断服务程序
//* 输入    : 无
//* 返回    : 无 
//* 说明    : 1)、只启动了UART_SPARE1中断接收，未启动UART_SPARE1中断发送。
//*           2)、接收到0x0d 0x0a(回车、"\r\n")代表帧数据接收完成
//*******************************************************************************/
//void SPS_SPARE1_IRQHandler(void)                	
//{
//	uint8_t Res=0;

//	if(USART_GetITStatus(UART_SPARE1, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//		Res =USART_ReceiveData(UART_SPARE1); //读取接收到的数据(UART_SPARE1->DR)
//		
//		SPS_SPARE1_R_Buff[SPS_SPARE1_R_Count++] = Res;
//		if(SPS_SPARE1_R_State == 0)//数据接收未完成
//		{
//			if(Res == 0x0d)//接收到0x0d,下一个字节接收到0x0a则接收完成
//			{
//				SPS_SPARE1_R_State =2;
//			}
//		}
//		else if(SPS_SPARE1_R_State == 2)
//		{
//			if(Res == 0x0a)//上一个字节接收到0x0d,这个字节接收到oxoa则接收完成
//			{
//				SPS_SPARE1_R_State =1;//数据接收完成
//			}
//			else//接收错误
//			{
//				SPS_SPARE1_R_State =0;
//				SPS_SPARE1_R_Count =0;
//			}
//		}
//		if(SPS_SPARE1_R_Count >= SPS_SPARE1_REC_MAXLEN)//接收数据长度走出接收数据缓冲区
//		{
//			if((SPS_SPARE1_R_Buff[SPS_SPARE1_REC_MAXLEN-2] != 0x0d) || (SPS_SPARE1_R_Buff[SPS_SPARE1_REC_MAXLEN-1] != 0x0a))
//			{
//				SPS_SPARE1_R_Count =0;
//				SPS_SPARE1_R_State =0;
//			}
//		} 		 
//	} 
//}



