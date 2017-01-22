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

//从MAIN.x中继承过来的函数
//UART_HandleTypeDef husart_debug;
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6; //MAIN中未定义，这里重新定义是为了复用
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6; //MAIN中未定义，这里重新定义是为了复用
#else
	#error Un-correct constant definition
#endif

extern uint8_t zIhuUartRxBuffer[6];

//基础接收缓冲区的全局变量，将用于所有串口的BSP驱动接收
int8_t 	zIhuBspStm32SpsGprsRxBuff[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN];					//串口GPRS数据接收缓冲区 
int8_t 	zIhuBspStm32SpsGprsRxState=0;																						//串口GPRS接收状态
int16_t zIhuBspStm32SpsGprsRxCount=0;																						//当前接收数据的字节数
int16_t zIhuBspStm32SpsGprsRxLen=0;
int8_t 	zIhuBspStm32SpsRfidRxBuff[IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN];					//串口RFID数据接收缓冲区 
int8_t 	zIhuBspStm32SpsRfidRxState=0;																						//串口RFID接收状态
int16_t zIhuBspStm32SpsRfidRxCount=0;																						//当前接收数据的字节数 	  
int16_t zIhuBspStm32SpsRfidRxLen=0;
int8_t 	zIhuBspStm32SpsBleRxBuff[IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN];						//串口BLE数据接收缓冲区 
int8_t 	zIhuBspStm32SpsBleRxState=0;																						//串口BLE接收状态
int16_t zIhuBspStm32SpsBleRxCount=0;																						//当前接收数据的字节数 	  
int16_t zIhuBspStm32SpsBleRxLen=0;
int8_t 	zIhuBspStm32SpsPrintRxBuff[IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN];					//串口PRINT数据接收缓冲区 
int8_t 	zIhuBspStm32SpsPrintRxState=0;																					//串口PRINT接收状态
int16_t zIhuBspStm32SpsPrintRxCount=0;																					//当前接收数据的字节数 	  
int16_t zIhuBspStm32SpsPrintRxLen=0;
int8_t 	zIhuBspStm32SpsSpare1RxBuff[IHU_BSP_STM32_SPS_SPARE1_REC_MAX_LEN];			//串口SPARE1数据接收缓冲区 
int8_t 	zIhuBspStm32SpsSpare1RxState=0;																					//串口SPARE1接收状态
int16_t zIhuBspStm32SpsSpare1RxCount=0;																					//当前接收数据的字节数 	  
int16_t zIhuBspStm32SpsSpare1RxLen=0;
int8_t 	zIhuBspStm32SpsSpare2RxBuff[IHU_BSP_STM32_SPS_SPARE2_REC_MAX_LEN];			//串口SPARE2数据接收缓冲区 
int8_t 	zIhuBspStm32SpsSpare2RxState=0;																					//串口SPARE2接收状态
int16_t zIhuBspStm32SpsSpare2RxCount=0;																					//当前接收数据的字节数 	  
int16_t zIhuBspStm32SpsSpare2RxLen=0;


/* 函数体 --------------------------------------------------------------------*/
int ihu_bsp_stm32_sps_slave_hw_init(void)
{
	uint16_t k;
	for(k=0;k<IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpsGprsRxBuff[k] = 0x00;
	}
  zIhuBspStm32SpsGprsRxCount = 0;               //接收字符串的起始存储位置
	zIhuBspStm32SpsGprsRxState = 0;
	zIhuBspStm32SpsGprsRxLen = 0;

	for(k=0;k<IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpsRfidRxBuff[k] = 0x00;
	}
  zIhuBspStm32SpsRfidRxCount = 0;               //接收字符串的起始存储位置
	zIhuBspStm32SpsRfidRxState = 0;
	zIhuBspStm32SpsRfidRxLen = 0;	

	for(k=0;k<IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpsBleRxBuff[k] = 0x00;
	}
  zIhuBspStm32SpsBleRxCount = 0;               //接收字符串的起始存储位置
	zIhuBspStm32SpsBleRxState = 0;
	zIhuBspStm32SpsBleRxLen = 0;	
	
	for(k=0;k<IHU_BSP_STM32_SPS_PRINT_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpsPrintRxBuff[k] = 0x00;
	}
  zIhuBspStm32SpsPrintRxCount = 0;               //接收字符串的起始存储位置
	zIhuBspStm32SpsPrintRxState = 0;
	zIhuBspStm32SpsPrintRxLen = 0;	

	for(k=0;k<IHU_BSP_STM32_SPS_SPARE1_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpsSpare1RxBuff[k] = 0x00;
	}
  zIhuBspStm32SpsSpare1RxCount = 0;               //接收字符串的起始存储位置
	zIhuBspStm32SpsSpare1RxState = 0;
	zIhuBspStm32SpsSpare1RxLen = 0;

	for(k=0;k<IHU_BSP_STM32_SPS_SPARE2_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpsSpare2RxBuff[k] = 0x00;
	}
  zIhuBspStm32SpsSpare2RxCount = 0;               //接收字符串的起始存储位置
	zIhuBspStm32SpsSpare2RxState = 0;
	zIhuBspStm32SpsSpare2RxLen = 0;
	
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

//数据发送的时间延迟
int ihu_bsp_stm32_sps_print_send_data(char *s)
{
	if (HAL_UART_Transmit(&IHU_BSP_STM32_UART_PRINT_HANDLER, (uint8_t *)s, strlen(s), IHU_BSP_STM32_SPS_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

//数据接收的时间延迟
int ihu_bsp_stm32_sps_print_rcv_data(char *s, uint16_t len)
{
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_PRINT_HANDLER, (uint8_t *)s, len, IHU_BSP_STM32_SPS_RX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

int ihu_bsp_stm32_sps_print_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_PRINT_HANDLER, buff, len, timeout) == HAL_OK)
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
	if (HAL_UART_Transmit(&IHU_BSP_STM32_UART_GPRS_HANDLER, (uint8_t *)buff, len, IHU_BSP_STM32_SPS_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int ihu_bsp_stm32_sps_gprs_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_GPRS_HANDLER, buff, len, IHU_BSP_STM32_SPS_RX_MAX_DELAY) == HAL_OK)
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
	if (HAL_UART_Transmit(&IHU_BSP_STM32_UART_RFID_HANDLER, (uint8_t *)buff, len, IHU_BSP_STM32_SPS_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int ihu_bsp_stm32_sps_rfid_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_RFID_HANDLER, buff, len, IHU_BSP_STM32_SPS_RX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

int ihu_bsp_stm32_sps_rfid_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_RFID_HANDLER, buff, len, timeout) == HAL_OK)
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
	if (HAL_UART_Transmit(&IHU_BSP_STM32_UART_BLE_HANDLER, (uint8_t *)buff, len, IHU_BSP_STM32_SPS_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

int ihu_bsp_stm32_sps_ble_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_BLE_HANDLER, buff, len, IHU_BSP_STM32_SPS_RX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

int ihu_bsp_stm32_sps_ble_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_BLE_HANDLER, buff, len, timeout) == HAL_OK)
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
	if (HAL_UART_Transmit(&IHU_BSP_STM32_UART_SPARE1_HANDLER, (uint8_t *)buff, len, IHU_BSP_STM32_SPS_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

int ihu_bsp_stm32_sps_spare1_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_SPARE1_HANDLER, buff, len, IHU_BSP_STM32_SPS_RX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

int ihu_bsp_stm32_sps_spare1_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART_SPARE1_HANDLER, buff, len, timeout) == HAL_OK)
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
int ihu_bsp_stm32_sps_spare2_send_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_UART_Transmit(&IHU_BSP_STM32_UART6_PRESENT_HANDLER, (uint8_t *)buff, len, IHU_BSP_STM32_SPS_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

int ihu_bsp_stm32_sps_spare2_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART6_PRESENT_HANDLER, buff, len, IHU_BSP_STM32_SPS_RX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

int ihu_bsp_stm32_sps_spare2_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout)
{    
	if (HAL_UART_Receive(&IHU_BSP_STM32_UART6_PRESENT_HANDLER, buff, len, timeout) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

/**
  *
  * 串口接口完成回调函数的处理
  * 为什么需要重新执行HAL_UART_Receive_IT，待确定
	* 这里既要考虑支持L2FRAME格式，也要考虑支持HUIXML格式的AT CMD模式
  * 因为GPRS和UART模式不可能重复，这里将L2FRAME放在SPARE1中断中，留待未来进一步使用。GPRS是非常明确的，不需要这个机制。
  *
  * L2FRAME需要未来进一步进行稳定性测试，暂时需要等待
  *
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	uint8_t res = 0;
  if(UartHandle==&IHU_BSP_STM32_UART_GPRS_HANDLER)
  {
		res = zIhuUartRxBuffer[IHU_BSP_STM32_UART_GPRS_HANDLER_ID-1];
		zIhuBspStm32SpsGprsRxBuff[zIhuBspStm32SpsGprsRxCount++] = res;
		if (zIhuBspStm32SpsGprsRxCount >= IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN)
			zIhuBspStm32SpsGprsRxCount = 0;
		//重新设置中断
		HAL_UART_Receive_IT(&IHU_BSP_STM32_UART_GPRS_HANDLER, &zIhuUartRxBuffer[IHU_BSP_STM32_UART_GPRS_HANDLER_ID-1], 1);
  }
  else if(UartHandle==&IHU_BSP_STM32_UART_RFID_HANDLER)
  {
		zIhuBspStm32SpsRfidRxBuff[zIhuBspStm32SpsRfidRxCount] = zIhuUartRxBuffer[IHU_BSP_STM32_UART_RFID_HANDLER_ID-1];
		zIhuBspStm32SpsRfidRxCount++;
		if (zIhuBspStm32SpsRfidRxCount >= IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN)
			zIhuBspStm32SpsRfidRxCount = 0;
		//重新设置中断
		HAL_UART_Receive_IT(&IHU_BSP_STM32_UART_RFID_HANDLER, &zIhuUartRxBuffer[IHU_BSP_STM32_UART_RFID_HANDLER_ID-1], 1);
  }
  else if(UartHandle==&IHU_BSP_STM32_UART_PRINT_HANDLER)
  {
		zIhuBspStm32SpsPrintRxBuff[zIhuBspStm32SpsPrintRxCount] = zIhuUartRxBuffer[IHU_BSP_STM32_UART_PRINT_HANDLER_ID-1];
		zIhuBspStm32SpsPrintRxCount++;
		if (zIhuBspStm32SpsPrintRxCount >= IHU_BSP_STM32_SPS_PRINT_REC_MAX_LEN)
			zIhuBspStm32SpsPrintRxCount = 0;
		//重新设置中断
		HAL_UART_Receive_IT(&IHU_BSP_STM32_UART_PRINT_HANDLER, &zIhuUartRxBuffer[IHU_BSP_STM32_UART_PRINT_HANDLER_ID-1], 1);
  }	
  else if(UartHandle==&IHU_BSP_STM32_UART_BLE_HANDLER)
  {
		zIhuBspStm32SpsBleRxBuff[zIhuBspStm32SpsBleRxCount] = zIhuUartRxBuffer[IHU_BSP_STM32_UART_BLE_HANDLER_ID-1];
		zIhuBspStm32SpsBleRxCount++;
		if (zIhuBspStm32SpsBleRxCount >= IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN)
			zIhuBspStm32SpsBleRxCount = 0;
		//重新设置中断
		HAL_UART_Receive_IT(&IHU_BSP_STM32_UART_BLE_HANDLER, &zIhuUartRxBuffer[IHU_BSP_STM32_UART_BLE_HANDLER_ID-1], 1);
  }	
  else if(UartHandle==&IHU_BSP_STM32_UART_SPARE1_HANDLER)
  {
		zIhuBspStm32SpsSpare1RxBuff[zIhuBspStm32SpsSpare1RxCount] = zIhuUartRxBuffer[IHU_BSP_STM32_UART_SPARE1_HANDLER_ID-1];
		zIhuBspStm32SpsSpare1RxCount++;
		if (zIhuBspStm32SpsSpare1RxCount >= IHU_BSP_STM32_SPS_SPARE1_REC_MAX_LEN)
			zIhuBspStm32SpsSpare1RxCount = 0;
		
		//这是为了L2FRAME处理方式
		//为了IDLE状态下提高效率，直接分解为IDLE和ELSE
		if (zIhuBspStm32SpsSpare1RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE)
		{
			//只有满足这么苛刻的条件，才算找到了帧头
			if ((res == IHU_HUITP_L2FRAME_STD_RX_START_FLAG_CHAR) && (zIhuBspStm32SpsSpare1RxCount == 1))
			zIhuBspStm32SpsSpare1RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_START;
		}
		else
		{
			//收到CHECKSUM
			if((zIhuBspStm32SpsSpare1RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_START) && (zIhuBspStm32SpsSpare1RxCount == 2))
			{
				zIhuBspStm32SpsSpare1RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM;
			}
			//收到长度高位
			else if((zIhuBspStm32SpsSpare1RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM) && (zIhuBspStm32SpsSpare1RxCount == 3))
			{
				zIhuBspStm32SpsSpare1RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN;
			}
			//收到长度低位
			else if((zIhuBspStm32SpsSpare1RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN) && (zIhuBspStm32SpsSpare1RxCount == 4))
			{
				zIhuBspStm32SpsSpare1RxLen = ((zIhuBspStm32SpsSpare1RxBuff[2] <<8) + zIhuBspStm32SpsSpare1RxBuff[3]);
				//CHECKSUM及判定
				if ((zIhuBspStm32SpsSpare1RxBuff[1] == (zIhuBspStm32SpsSpare1RxBuff[0] ^ zIhuBspStm32SpsSpare1RxBuff[2]^zIhuBspStm32SpsSpare1RxBuff[3])) &&\
					(zIhuBspStm32SpsSpare1RxLen < IHU_BSP_STM32_SPS_SPARE1_REC_MAX_LEN-4))
				zIhuBspStm32SpsSpare1RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_BODY;
			}
			//收到BODY位
			else if((zIhuBspStm32SpsSpare1RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_BODY) && (zIhuBspStm32SpsSpare1RxLen > 1))
			{
				zIhuBspStm32SpsSpare1RxLen--;
			}
			//收到BODY最后一位
			//BFSC项目暂时没有SPS串口双向通信，只能先删掉	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)			
			else if((zIhuBspStm32SpsSpare1RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_BODY) && (zIhuBspStm32SpsSpare1RxLen == 1))
			{
				msg_struct_spsvirgo_l2frame_rcv_t snd;
				zIhuBspStm32SpsSpare1RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE;
				zIhuBspStm32SpsSpare1RxLen = 0;
				zIhuBspStm32SpsSpare1RxCount = 0;
				//发送数据到上层SPSVIRGO模块
				memset(&snd, 0, sizeof(msg_struct_spsvirgo_l2frame_rcv_t));
				memcpy(snd.data, &zIhuBspStm32SpsSpare1RxBuff[4], ((zIhuBspStm32SpsSpare1RxBuff[2]<<8)+zIhuBspStm32SpsSpare1RxBuff[3]));
				snd.length = sizeof(msg_struct_spsvirgo_l2frame_rcv_t);
				ihu_message_send(MSG_ID_SPS_L2FRAME_RCV, TASK_ID_SPSVIRGO, TASK_ID_VMFO, &snd, snd.length);
				//IhuDebugPrint("BSPUART: I just send a test message to SPSVIRGO by SPARE1 number!");
			}
#endif	
			//差错情况
			else{
				zIhuBspStm32SpsSpare1RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE;
				zIhuBspStm32SpsSpare1RxLen = 0;
				zIhuBspStm32SpsSpare1RxCount = 0;
			}
		}
		//重新设置中断
		HAL_UART_Receive_IT(&IHU_BSP_STM32_UART_SPARE1_HANDLER, &zIhuUartRxBuffer[IHU_BSP_STM32_UART_SPARE1_HANDLER_ID-1], 1);
  }
  else if(UartHandle==&IHU_BSP_STM32_UART6_PRESENT_HANDLER)
  {
		zIhuBspStm32SpsSpare2RxBuff[zIhuBspStm32SpsSpare2RxCount] = zIhuUartRxBuffer[IHU_BSP_STM32_UART6_PRESENT_HANDLER_ID-1];
		zIhuBspStm32SpsSpare2RxCount++;
		if (zIhuBspStm32SpsSpare2RxCount >= IHU_BSP_STM32_SPS_SPARE2_REC_MAX_LEN)
			zIhuBspStm32SpsSpare2RxCount = 0;
		//重新设置中断
		HAL_UART_Receive_IT(&IHU_BSP_STM32_UART6_PRESENT_HANDLER, &zIhuUartRxBuffer[IHU_BSP_STM32_UART6_PRESENT_HANDLER_ID-1], 1);
  }
}



