/**
  ******************************************************************************
  * 文件名程: bsp_spi.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2016-10-04
  * 功    能: 板载调试串口底层驱动程序：默认使用SPI
  ******************************************************************************
  * 
  ******************************************************************************
*/

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_spi.h"


//ZJL定义
int8_t 	BSP_STM32_SPI_IAU_R_Buff[BSP_STM32_SPI_IAU_REC_MAXLEN];				//串口SPI数据接收缓冲区 
int8_t 	BSP_STM32_SPI_IAU_R_State=0;																	//串口SPI接收状态
int16_t BSP_STM32_SPI_IAU_R_Count=0;																	//当前接收数据的字节数
int16_t BSP_STM32_SPI_IAU_R_Len=0;
int8_t 	BSP_STM32_SPI_SPARE1_R_Buff[BSP_STM32_SPI_SPARE1_REC_MAXLEN];	//串口SPI数据接收缓冲区 
int8_t 	BSP_STM32_SPI_SPARE1_R_State=0;																//串口SPI接收状态
int16_t BSP_STM32_SPI_SPARE1_R_Count=0;																//当前接收数据的字节数 	  
int16_t BSP_STM32_SPI_SPARE1_R_Len=0;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern uint8_t zIhuSpiRxBuffer[6];

/* SPI handler declaration */
// SPI_HandleTypeDef SpiHandle;
extern SPI_HandleTypeDef hspi2;
#define SPILEO_BUF_SIZE 256
uint8_t BSP_SPI_rx_buffer[SPILEO_BUF_SIZE];
uint8_t BSP_SPI_tx_buffer[SPILEO_BUF_SIZE];
uint32_t BSP_SPI_RxState;

static void BSP_SPI_tx_isr(SPI_HandleTypeDef *hspi);
static void BSP_SPI_rx_isr(SPI_HandleTypeDef *hspi);
static void BSP_SPI_tx_complete(SPI_HandleTypeDef *hspi);
static void BSP_SPI_rx_complete(SPI_HandleTypeDef *hspi);

uint8_t BSP_SPI_gen_chksum(BSP_SPI_msgheader_t *pMsgHeader)
{
	uint8_t *pData = (uint8_t *)pMsgHeader;
	
	return pData[0]^pData[2]^pData[3];
}

static void BSP_SPI_rx_isr(SPI_HandleTypeDef *hspi)
{
  uint16_t tmp;
  
  if(hspi->RxXferCount >= hspi->RxXferSize)
	{
	  // buffer is full or Rx isn't started, drop the data
	  tmp = hspi->Instance->DR;
	  UNUSED(tmp);
	}
	else
	{
	  BSP_SPI_msgheader_t *pMsgHeader = (BSP_SPI_msgheader_t *)hspi->pRxBuffPtr;
	  
	  hspi->pRxBuffPtr[hspi->RxXferCount++] = hspi->Instance->DR;

	  switch(BSP_SPI_RxState)
	  {

	  case SPI_RX_STATE_START:
		if(pMsgHeader->start != EXT_BORAD_START_CHAR)
		{
		  // not synchonrized, reset the RxXferCount
		  hspi->RxXferCount = 0;
		}
		else
		  BSP_SPI_RxState = SPI_RX_STATE_HEADER;
		break;
		
	  case SPI_RX_STATE_HEADER:
		if(hspi->RxXferCount == sizeof(BSP_SPI_msgheader_t))
		{
		  if(BSP_SPI_gen_chksum(pMsgHeader) != pMsgHeader->chksum)
		  {
			hspi->RxXferCount = 0;
			BSP_SPI_RxState = SPI_RX_STATE_START;
		  }
		  else
		  {
			BSP_SPI_RxState = SPI_RX_STATE_BODY;
		  }
		}
		else if(hspi->RxXferCount > sizeof(BSP_SPI_msgheader_t))
		{
			/* error case, just reset */
			hspi->RxXferCount = 0;
			BSP_SPI_RxState = SPI_RX_STATE_START;
		}
		break;
		
	  case SPI_RX_STATE_BODY:
		if(hspi->RxXferCount == pMsgHeader->len)
		{
			// call user's callback after the receive is complete
			BSP_SPI_rx_complete(hspi);
			BSP_SPI_RxState = SPI_RX_STATE_START;
			hspi->RxXferCount = 0;
		}

		if(hspi->RxXferCount >= hspi->RxXferSize)
		{
		  hspi->RxXferCount = 0;
		  BSP_SPI_RxState = SPI_RX_STATE_START;
		}
		break;
	  }
	}
}

static void BSP_SPI_tx_isr(SPI_HandleTypeDef *hspi)
{
  if(hspi->TxXferCount >= hspi->TxXferSize)
	{
	  // no pending data, send pading
	  hspi->Instance->DR = EXT_BOARD_PADDING_CH;
	}
	else
	{
	  hspi->Instance->DR = hspi->pTxBuffPtr[hspi->TxXferCount++];
	}
	
	/* if there isn't more data to transmit, disable TXE interrupt (must...) */
	if(hspi->TxXferCount == hspi->TxXferSize)
	{
	  __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE));
	  __HAL_SPI_CLEAR_OVRFLAG(hspi);

	  // call user's callback after the transfer is complete
		BSP_SPI_tx_complete(hspi);
	}
}


/**
  * @brief SPI error callbacks
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  /* Prevent unused argument(s) compilation warning */
  IhuErrorPrint("SPI ErrorCode: 0x%08x\n\r", hspi->ErrorCode);
}

static void BSP_SPI_rx_complete(SPI_HandleTypeDef *hspi)
{
/*
  IhuDebugPrint("spi RX complete (%d): packet 0x%08x 0x%08x 0x%08x\n\r", 
                hspi->RxXferCount, *(uint32_t *)&hspi->pRxBuffPtr[0], *(uint32_t *)&hspi->pRxBuffPtr[4], *(uint32_t *)&hspi->pRxBuffPtr[8]);
*/

  if(*(uint16_t *)(&BSP_SPI_rx_buffer[4]) == 0xEEEE)
  {
    /* for loopback test */
		BSP_SPI_msgheader_t *pMsgHeader = (BSP_SPI_msgheader_t *)BSP_SPI_rx_buffer;
		uint16_t msglen = pMsgHeader->len > SPILEO_BUF_SIZE? SPILEO_BUF_SIZE: pMsgHeader->len;
		
		memcpy(BSP_SPI_tx_buffer, BSP_SPI_rx_buffer, msglen);
		BSP_SPI_start_transmit(hspi, BSP_SPI_tx_buffer, msglen);
  }
	else
	{
    /* notify the application */
  }
}

int BSP_SPI_start_receive(SPI_HandleTypeDef *hspi, uint8_t *rx_buffer, uint16_t size)
{
	uint32_t flags;
	
  flags = ulPortSetInterruptMask();

  /* set RX buffer */
  BSP_SPI_RxState = SPI_RX_STATE_START;
  hspi->RxXferCount = 0;
  hspi->RxXferSize = size;
  hspi->pRxBuffPtr = rx_buffer;

  vPortClearInterruptMask(flags);
  
  /* enable RXNE interrupt */
  __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_RXNE | SPI_IT_ERR));

  /* Check if the SPI is already enabled */ 
  if((hspi->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
  {
    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);
  }
  
  return 0;
}

static void BSP_SPI_tx_complete(SPI_HandleTypeDef *hspi)
{
/*
	IhuDebugPrint("spi TX complete: packet 0x%08x 0x%08x size %d\n\r", 
                *(uint32_t *)&hspi->pTxBuffPtr[0], *(uint32_t *)&hspi->pTxBuffPtr[4], hspi->TxXferCount);
*/
}

int BSP_SPI_start_transmit(SPI_HandleTypeDef *hspi, uint8_t *tx_buffer, uint16_t size)
{
	uint32_t flags;
	
  if(hspi->TxXferCount < hspi->TxXferSize)
  {
  	IhuErrorPrint("SPI transmit ongoing...\n");
  	return -1;
  }

  flags = ulPortSetInterruptMask();
  
  hspi->pTxBuffPtr = tx_buffer;
  hspi->TxXferCount = 0;
  hspi->TxXferSize = size;
  
  vPortClearInterruptMask(flags);

  /* enable TXE interrupt */
  __HAL_SPI_ENABLE_IT(hspi, SPI_IT_TXE);

  /* Check if the SPI is already enabled */ 
  if((hspi->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
  {
    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);
  }

  return 0;
}

/**
  * @brief SPI MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - NVIC configuration for SPI interrupt request enable
  * @param hspi: SPI handle pointer
  * @retval None
  */
//void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
//{
//  GPIO_InitTypeDef  GPIO_InitStruct;

//  if (hspi->Instance == SPIx)
//	{
//    /*##-1- Enable peripherals and GPIO Clocks #################################*/
//    /* Enable GPIO TX/RX clock */
//    SPIx_SCK_GPIO_CLK_ENABLE();
//    SPIx_MISO_GPIO_CLK_ENABLE();
//    SPIx_MOSI_GPIO_CLK_ENABLE();
//    /* Enable SPI clock */
//    SPIx_CLK_ENABLE();

//    /*##-2- Configure peripheral GPIO ##########################################*/
//    /* SPI SCK GPIO pin configuration  */
//    GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
//    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull      = GPIO_PULLUP;
//    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
//    GPIO_InitStruct.Alternate = SPIx_SCK_AF;

//    HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

//    /* SPI MISO GPIO pin configuration  */
//    GPIO_InitStruct.Pin = SPIx_MISO_PIN;
//    GPIO_InitStruct.Alternate = SPIx_MISO_AF;

//    HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

//    /* SPI MOSI GPIO pin configuration  */
//    GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
//    GPIO_InitStruct.Alternate = SPIx_MOSI_AF;

//    HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);

//    /*##-3- Configure the NVIC for SPI #########################################*/
//    /* NVIC for SPI */
//    // HAL_NVIC_SetPriority(SPIx_IRQn, 0, 1);
//    HAL_NVIC_EnableIRQ(SPIx_IRQn);
//  }
//}

/**
  * @brief SPI MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param hspi: SPI handle pointer
  * @retval None
  */
//void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
//{
//  if (hspi->Instance == SPIx)
//  {
//    /*##-1- Reset peripherals ##################################################*/
//    SPIx_FORCE_RESET();
//    SPIx_RELEASE_RESET();

//    /*##-2- Disable peripherals and GPIO Clocks ################################*/
//    /* Configure SPI SCK as alternate function  */
//    HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
//    /* Configure SPI MISO as alternate function  */
//    HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
//    /* Configure SPI MOSI as alternate function  */
//    HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);

//    /*##-3- Disable the NVIC for SPI ###########################################*/
//    HAL_NVIC_DisableIRQ(SPIx_IRQn);
//  }
//}


int BSP_SPI_slave_hw_init(int is_clock_phase_1edge, int is_clock_polarity_high)
{
	SPI_HandleTypeDef *hspi = &hspi2;
	
	/* SPI hardware init is already done in main.c MX_SPI2_Init()
	 * So there is no hw init in this function now.
   */
	
	// Please don't use STM32 offical function for receive or transmit
	// Instead, use BSP_SPI_start_receive() and BSP_SPI_start_transmit()
	hspi->RxISR = BSP_SPI_rx_isr;
	hspi->TxISR = BSP_SPI_tx_isr;

	/* attach SPI interrupt: no more need in FreeRTOS */

	/* start SPI receiving: Enable SPI and RX interrupt */
	BSP_SPI_start_receive(hspi, BSP_SPI_rx_buffer, SPILEO_BUF_SIZE);
	
	IhuDebugPrint("BSP_SPI_slave_hw_init() done.\n");
	return 0;
}

//ZJL DEFINITION
//利用串口一致的特性，完成SPI接口的接收
//这里还没有成帧。完整的设计设计需要成帧，然后通过message_send将消息发送到L2SPILEO模块中
/*******************************************************************************
* 函数名  : SPI_SendData
* 描述    : SPI_IAU发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
int BSP_STM32_SPI_IAU_SendData(uint8_t* buff, uint16_t len)
{
	if (HAL_SPI_Transmit(&BSP_STM32_SPI_IAU, (uint8_t *)buff, len, SPI_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int BSP_STM32_SPI_IAU_RcvData(uint8_t* buff, uint16_t len)
{    
	if (HAL_SPI_Receive(&BSP_STM32_SPI_IAU, buff, len, SPI_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

/*******************************************************************************
* 函数名  : SPI_SendData
* 描述    : SPI_SPARE1发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
int BSP_STM32_SPI_SPARE1_SendData(uint8_t* buff, uint16_t len)
{
	if (HAL_SPI_Transmit(&BSP_STM32_SPI_SPARE1, (uint8_t *)buff, len, SPI_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int BSP_STM32_SPI_SPARE1_RcvData(uint8_t* buff, uint16_t len)
{    
	if (HAL_SPI_Receive(&BSP_STM32_SPI_SPARE1, buff, len, SPI_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

/**
  * SPI接口完成回调函数的处理
  * 为什么需要重新执行HAL_SPI_Receive_IT，待确定
  */
//void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *SpiHandle)
//{
//	uint8_t res = 0;
//	msg_struct_spileo_l2frame_rcv_t snd;
//	
//  if(SpiHandle==&BSP_STM32_SPI_IAU)
//  {
//		res = zIhuSpiRxBuffer[BSP_STM32_SPI_IAU_ID-1];
//		BSP_STM32_SPI_IAU_R_Buff[BSP_STM32_SPI_IAU_R_Count++] = res;
//		if (BSP_STM32_SPI_IAU_R_Count >= BSP_STM32_SPI_IAU_REC_MAXLEN)
//			BSP_STM32_SPI_IAU_R_Count = 0;
//		
//		//为了IDLE状态下提高效率，直接分解为IDLE和ELSE
//		if (BSP_STM32_SPI_IAU_R_State == IHU_HUISTD_RX_STATE_IDLE)
//		{
//			//只有满足这么苛刻的条件，才算找到了帧头
//			if ((res == IHU_HUISTD_FRAME_START_CHAR) && (BSP_STM32_SPI_IAU_R_Count == 1))
//			BSP_STM32_SPI_IAU_R_State = IHU_HUISTD_RX_STATE_START;
//		}
//		else
//		{
//			//收到CHECKSUM
//			if((BSP_STM32_SPI_IAU_R_State == IHU_HUISTD_RX_STATE_START) && (BSP_STM32_SPI_IAU_R_Count == 2))
//			{
//				BSP_STM32_SPI_IAU_R_State = IHU_HUISTD_RX_STATE_HEADER_CKSM;
//			}
//			//收到长度高位
//			else if((BSP_STM32_SPI_IAU_R_State == IHU_HUISTD_RX_STATE_HEADER_CKSM) && (BSP_STM32_SPI_IAU_R_Count == 3))
//			{
//				BSP_STM32_SPI_IAU_R_State = IHU_HUISTD_RX_STATE_HEADER_LEN;
//			}
//			//收到长度低位
//			else if((BSP_STM32_SPI_IAU_R_State == IHU_HUISTD_RX_STATE_HEADER_LEN) && (BSP_STM32_SPI_IAU_R_Count == 4))
//			{
//				BSP_STM32_SPI_IAU_R_Len = ((BSP_STM32_SPI_IAU_R_Buff[2] <<8) + BSP_STM32_SPI_IAU_R_Buff[3]);
//				//CHECKSUM及判定
//				if ((BSP_STM32_SPI_IAU_R_Buff[1] == (BSP_STM32_SPI_IAU_R_Buff[0] ^ BSP_STM32_SPI_IAU_R_Buff[2]^BSP_STM32_SPI_IAU_R_Buff[3])) &&\
//					(BSP_STM32_SPI_IAU_R_Len < BSP_STM32_SPI_IAU_REC_MAXLEN-4))
//				BSP_STM32_SPI_IAU_R_State = IHU_HUISTD_RX_STATE_BODY;
//			}
//			//收到BODY位
//			else if((BSP_STM32_SPI_IAU_R_State == IHU_HUISTD_RX_STATE_BODY) && (BSP_STM32_SPI_IAU_R_Len > 1))
//			{
//				BSP_STM32_SPI_IAU_R_Len--;
//			}
//			//收到BODY最后一位
//			else if((BSP_STM32_SPI_IAU_R_State == IHU_HUISTD_RX_STATE_BODY) && (BSP_STM32_SPI_IAU_R_Len == 1))
//			{
//				BSP_STM32_SPI_IAU_R_State = IHU_HUISTD_RX_STATE_IDLE;
//				BSP_STM32_SPI_IAU_R_Len = 0;
//				BSP_STM32_SPI_IAU_R_Count = 0;
//				//发送数据到上层SPILEO模块
//				memset(&snd, 0, sizeof(msg_struct_spileo_l2frame_rcv_t));
//				memcpy(snd.data, &BSP_STM32_SPI_IAU_R_Buff[4], ((BSP_STM32_SPI_IAU_R_Buff[2]<<8)+BSP_STM32_SPI_IAU_R_Buff[3]));
//				snd.length = sizeof(msg_struct_spileo_l2frame_rcv_t);
//				ihu_message_send(MSG_ID_SPI_L2FRAME_RCV, TASK_ID_SPILEO, TASK_ID_VMFO, &snd, snd.length);		
//			}
//			//差错情况
//			else{
//				BSP_STM32_SPI_IAU_R_State = IHU_HUISTD_RX_STATE_IDLE;
//				BSP_STM32_SPI_IAU_R_Len = 0;
//				BSP_STM32_SPI_IAU_R_Count = 0;
//			}
//		}
//		//重新设置中断		
//		HAL_SPI_Receive_IT(&BSP_STM32_SPI_IAU, &zIhuSpiRxBuffer[BSP_STM32_SPI_IAU_ID-1], 1);
//  }
//  else if(SpiHandle==&BSP_STM32_SPI_SPARE1)
//  {
//		BSP_STM32_SPI_SPARE1_R_Buff[BSP_STM32_SPI_SPARE1_R_Count] = zIhuSpiRxBuffer[BSP_STM32_SPI_SPARE1_ID-1];
//		BSP_STM32_SPI_SPARE1_R_Count++;
//		if (BSP_STM32_SPI_SPARE1_R_Count >= BSP_STM32_SPI_SPARE1_REC_MAXLEN)
//			BSP_STM32_SPI_SPARE1_R_Count = 0;
//		HAL_SPI_Receive_IT(&BSP_STM32_SPI_SPARE1, &zIhuSpiRxBuffer[BSP_STM32_SPI_SPARE1_ID-1], 1);
//  }
//}


int BSP_STM32_SPI_slave_hw_init(void)
{
	uint16_t k;
	for(k=0;k<BSP_STM32_SPI_IAU_REC_MAXLEN;k++)      //将缓存内容清零
	{
		BSP_STM32_SPI_IAU_R_Buff[k] = 0x00;
	}
  BSP_STM32_SPI_IAU_R_Count = 0;               //接收字符串的起始存储位置
	BSP_STM32_SPI_IAU_R_State = 0;
	BSP_STM32_SPI_IAU_R_Len = 0;

	for(k=0;k<BSP_STM32_SPI_SPARE1_REC_MAXLEN;k++)      //将缓存内容清零
	{
		BSP_STM32_SPI_SPARE1_R_Buff[k] = 0x00;
	}
  BSP_STM32_SPI_SPARE1_R_Count = 0;               //接收字符串的起始存储位置
	BSP_STM32_SPI_SPARE1_R_State = 0;
	BSP_STM32_SPI_SPARE1_R_Len = 0;
	
	return BSP_SUCCESS;
}
