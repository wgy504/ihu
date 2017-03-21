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

//从MAIN.x中继承过来的函数
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
SPI_HandleTypeDef hspi1; //MAIN中为定义，这里重新定义是为了复用
extern SPI_HandleTypeDef hspi2;
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
SPI_HandleTypeDef hspi1;  //MAIN中为定义，这里重新定义是为了复用
extern SPI_HandleTypeDef hspi2;
#endif

extern uint8_t zIhuSpiRxBuffer[2];

//本地全局变量
int8_t 	zIhuBspStm32SpiGeneral1RxBuff[IHU_BSP_STM32_SPI1_GENERAL_REC_MAX_LEN];				//SPI数据接收缓冲区 
int8_t 	zIhuBspStm32SpiGeneral1RxState=0;																							//SPI接收状态
int16_t zIhuBspStm32SpiGeneral1RxCount=0;																							//当前接收数据的字节数 	  
int16_t zIhuBspStm32SpiGeneral1RxLen=0;
int8_t 	zIhuBspStm32SpiGeneral2RxBuff[IHU_BSP_STM32_SPI2_GENERAL_REC_MAX_LEN];				//SPI数据接收缓冲区 
int8_t 	zIhuBspStm32SpiGeneral2RxState=0;																							//SPI接收状态
int16_t zIhuBspStm32SpiGeneral2RxCount=0;																							//当前接收数据的字节数
int16_t zIhuBspStm32SpiGeneral2RxLen=0;

//Puhui定义的函数和全局变量区域
#define SPILEO_BUF_SIZE 256
uint8_t BSP_SPI_rx_buffer[SPILEO_BUF_SIZE];
uint8_t BSP_SPI_tx_buffer[SPILEO_BUF_SIZE];
uint32_t BSP_SPI_RxState;



/* 函数体 --------------------------------------------------------------------*/
uint8_t BSP_SPI_gen_chksum(IHU_HUITP_L2FRAME_STD_SPI_frame_header_t *pMsgHeader)
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
	  IHU_HUITP_L2FRAME_STD_SPI_frame_header_t *pMsgHeader = (IHU_HUITP_L2FRAME_STD_SPI_frame_header_t *)hspi->pRxBuffPtr;
	  
	  hspi->pRxBuffPtr[hspi->RxXferCount++] = hspi->Instance->DR;

	  switch(BSP_SPI_RxState)
	  {

	  case IHU_BSP_STM32_SPI_RX_STATE_START:
		if(pMsgHeader->start != IHU_BSP_STM32_SPI_EXT_BORAD_START_CHAR)
		{
		  // not synchonrized, reset the RxXferCount
		  hspi->RxXferCount = 0;
		}
		else
		  BSP_SPI_RxState = IHU_BSP_STM32_SPI_RX_STATE_HEADER;
		break;
		
	  case IHU_BSP_STM32_SPI_RX_STATE_HEADER:
		if(hspi->RxXferCount == sizeof(IHU_HUITP_L2FRAME_STD_SPI_frame_header_t))
		{
		  if(BSP_SPI_gen_chksum(pMsgHeader) != pMsgHeader->chksum)
		  {
			hspi->RxXferCount = 0;
			BSP_SPI_RxState = IHU_BSP_STM32_SPI_RX_STATE_START;
		  }
		  else
		  {
			BSP_SPI_RxState = IHU_BSP_STM32_SPI_RX_STATE_BODY;
		  }
		}
		else if(hspi->RxXferCount > sizeof(IHU_HUITP_L2FRAME_STD_SPI_frame_header_t))
		{
			/* error case, just reset */
			hspi->RxXferCount = 0;
			BSP_SPI_RxState = IHU_BSP_STM32_SPI_RX_STATE_START;
		}
		break;
		
	  case IHU_BSP_STM32_SPI_RX_STATE_BODY:
		if(hspi->RxXferCount == pMsgHeader->len)
		{
			// call user's callback after the receive is complete
			BSP_SPI_rx_complete(hspi);
			BSP_SPI_RxState = IHU_BSP_STM32_SPI_RX_STATE_START;
			hspi->RxXferCount = 0;
		}

		if(hspi->RxXferCount >= hspi->RxXferSize)
		{
		  hspi->RxXferCount = 0;
		  BSP_SPI_RxState = IHU_BSP_STM32_SPI_RX_STATE_START;
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
	  hspi->Instance->DR = IHU_BSP_STM32_SPI_EXT_BOARD_PADDING_CH;
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
		IHU_HUITP_L2FRAME_STD_SPI_frame_header_t *pMsgHeader = (IHU_HUITP_L2FRAME_STD_SPI_frame_header_t *)BSP_SPI_rx_buffer;
		uint16_t msglen = pMsgHeader->len > SPILEO_BUF_SIZE? SPILEO_BUF_SIZE: pMsgHeader->len;
		
		memcpy(BSP_SPI_tx_buffer, BSP_SPI_rx_buffer, msglen);
		func_bsp_spi_start_transmit(hspi, BSP_SPI_tx_buffer, msglen);
  }
	else
	{
    /* notify the application */
  }
}

int func_bsp_spi_start_receive(SPI_HandleTypeDef *hspi, uint8_t *rx_buffer, uint16_t size)
{
	uint32_t flags;
	
  flags = ulPortSetInterruptMask();

  /* set RX buffer */
  BSP_SPI_RxState = IHU_BSP_STM32_SPI_RX_STATE_START;
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

int func_bsp_spi_start_transmit(SPI_HandleTypeDef *hspi, uint8_t *tx_buffer, uint16_t size)
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


int func_bsp_spi_slave_hw_init(int is_clock_phase_1edge, int is_clock_polarity_high)
{
	SPI_HandleTypeDef *hspi = &hspi2;
	
	/* SPI hardware init is already done in main.c MX_SPI2_Init()
	 * So there is no hw init in this function now.
   */
	
	// Please don't use STM32 offical function for receive or transmit
	// Instead, use func_bsp_spi_start_receive() and func_bsp_spi_start_transmit()
	hspi->RxISR = BSP_SPI_rx_isr;
	hspi->TxISR = BSP_SPI_tx_isr;

	/* attach SPI interrupt: no more need in FreeRTOS */

	/* start SPI receiving: Enable SPI and RX interrupt */
	func_bsp_spi_start_receive(hspi, BSP_SPI_rx_buffer, SPILEO_BUF_SIZE);
	
	IhuDebugPrint("func_bsp_spi_slave_hw_init() done.\n");
	return 0;
}


/*
 *
 *
 *  自定义IAU对应的驱动程序
 *
 *
 *
*/

//ZJL DEFINITION
//利用串口一致的特性，完成SPI接口的接收
int ihu_bsp_stm32_spi_slave_hw_init(void)
{
	uint16_t k;
	for(k=0;k<IHU_BSP_STM32_SPI2_GENERAL_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpiGeneral2RxBuff[k] = 0x00;
	}
  zIhuBspStm32SpiGeneral2RxCount = 0;               //接收字符串的起始存储位置
	zIhuBspStm32SpiGeneral2RxState = 0;
	zIhuBspStm32SpiGeneral2RxLen = 0;

	for(k=0;k<IHU_BSP_STM32_SPI1_GENERAL_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpiGeneral1RxBuff[k] = 0x00;
	}
  zIhuBspStm32SpiGeneral1RxCount = 0;               //接收字符串的起始存储位置
	zIhuBspStm32SpiGeneral1RxState = 0;
	zIhuBspStm32SpiGeneral1RxLen = 0;

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)	
  ihu_bsp_stm32_spi_mf522_reset_disable();
	ihu_bsp_stm32_spi_mf522_cs_disable();	
#endif
	
	return BSP_SUCCESS;
}


//这里还没有成帧。完整的设计设计需要成帧，然后通过message_send将消息发送到L2SPILEO模块中
/*******************************************************************************
* 函数名  : SPI_SendData
* 描述    : SPI_IAU发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
int ihu_bsp_stm32_spi_iau_send_data(uint8_t* buff, uint16_t len)
{
	if (HAL_SPI_Transmit(&IHU_BSP_STM32_SPI_IAU_HANDLER, (uint8_t *)buff, len, IHU_BSP_STM32_SPI_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int ihu_bsp_stm32_spi_iau_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_SPI_Receive(&IHU_BSP_STM32_SPI_IAU_HANDLER, buff, len, IHU_BSP_STM32_SPI_TX_MAX_DELAY) == HAL_OK)
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
int ihu_bsp_stm32_spi_spare1_send_data(uint8_t* buff, uint16_t len)
{
	if (HAL_SPI_Transmit(&IHU_BSP_STM32_SPI_SPARE1_HANDLER, (uint8_t *)buff, len, IHU_BSP_STM32_SPI_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int ihu_bsp_stm32_spi_spare1_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_SPI_Receive(&IHU_BSP_STM32_SPI_SPARE1_HANDLER, buff, len, IHU_BSP_STM32_SPI_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
/*
 *
 *
 *  MF522对应的驱动程序
 *
 *
 *
*/

void ihu_bsp_stm32_spi_mf522_cs_enable(void)
{
	BSP_STM32_SPI_MF522_SPI_CS_ENABLE();
}

void ihu_bsp_stm32_spi_mf522_cs_disable(void)
{
	BSP_STM32_SPI_MF522_SPI_CS_DISABLE();
}

void ihu_bsp_stm32_spi_mf522_reset_enable(void)
{
	BSP_STM32_SPI_MF522_SPI_RESET_ENABLE();
}

void ihu_bsp_stm32_spi_mf522_reset_disable(void)
{
	BSP_STM32_SPI_MF522_SPI_RESET_DISABLE();
}

/*******************************************************************************
* 函数名  : SPI_SendData
* 描述    : SPI_IAU发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
int ihu_bsp_stm32_spi_rfid522_send_data(uint8_t* buff, uint16_t len)
{
	if (HAL_SPI_Transmit(&IHU_BSP_STM32_SPI_RFID522_HANDLER, (uint8_t *)buff, len, IHU_BSP_STM32_SPI_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int ihu_bsp_stm32_spi_rfid522_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_SPI_Receive(&IHU_BSP_STM32_SPI_RFID522_HANDLER, buff, len, IHU_BSP_STM32_SPI_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

/**
  * 函数功能: 从串行Flash读取一个字节数据
  * 输入参数: 无
  * 返 回 值: uint8_t：读取到的数据
  * 说    明：This function must be used only if the Start_Read_Sequence
  *           function has been previously called.
  */
uint8_t ihu_bsp_stm32_spi_flash_read_byte(void)
{
  uint8_t d_read,d_send=IHU_BSP_STM32_SPI_TX_RX_MAX_DELAY;
  if(HAL_SPI_TransmitReceive(&IHU_BSP_STM32_SPI_RFID522_HANDLER,&d_send,&d_read,1,0xFFFFFF)!=HAL_OK)
    d_read=IHU_BSP_STM32_SPI_TX_RX_MAX_DELAY;
  
  return d_read;    
}

/**
  * 函数功能: 往串行Flash读取写入一个字节数据并接收一个字节数据
  * 输入参数: byte：待发送数据
  * 返 回 值: uint8_t：接收到的数据
  * 说    明：无
  */
uint8_t ihu_bsp_stm32_spi_flash_send_byte(uint8_t byte)
{
  uint8_t d_read,d_send=byte;
  if(HAL_SPI_TransmitReceive(&IHU_BSP_STM32_SPI_RFID522_HANDLER,&d_send,&d_read,1,0xFFFFFF)!=HAL_OK)
    d_read=IHU_BSP_STM32_SPI_TX_RX_MAX_DELAY;
  
  return d_read; 
}

#endif

/*
 *
 *
 *  NFSC AD Scale对应的驱动程序
 *
 *
 *
*/



/*******************************************************************************
* 函数名  : SPI_SendData
* 描述    : SPI_IAU发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
int ihu_bsp_stm32_spi_ad_scale_send_data(uint8_t* buff, uint16_t len)
{
	if (HAL_SPI_Transmit(&IHU_BSP_STM32_SPI_AD_SCALE_HANDLER, (uint8_t *)buff, len, IHU_BSP_STM32_SPI_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int ihu_bsp_stm32_spi_ad_scale_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_SPI_Receive(&IHU_BSP_STM32_SPI_AD_SCALE_HANDLER, buff, len, IHU_BSP_STM32_SPI_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}


/*
 *
 *
 *  NRF24L01对应的驱动程序
 *
 *
 *
*/

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
void ihu_bsp_stm32_spi_nrf24l01_cs_enable(void)
{
	BSP_STM32_SPI_NRF24L01_SPI_CS_ENABLE();
}

void ihu_bsp_stm32_spi_nrf24l01_cs_disable(void)
{
	BSP_STM32_SPI_NRF24L01_SPI_CS_DISABLE();
}

void ihu_bsp_stm32_spi_nrf24l01_ce_low(void)
{
	BSP_STM32_SPI_NRF24L01_CE_LOW();
}

void ihu_bsp_stm32_spi_nrf24l01_ce_high(void)
{
	BSP_STM32_SPI_NRF24L01_CE_HIGH();
}

uint8_t ihu_bsp_stm32_spi_nrf24l01_irq_read(void)
{
	return BSP_STM32_SPI_NRF24L01_IRQ_PIN_READ();
}



/**
  * 函数功能: 往目标读取写入一个字节数据并接收一个字节数据
  * 输入参数: byte：待发送数据
  * 返 回 值: uint8_t：接收到的数据
  * 说    明：无
  */
uint8_t ihu_bsp_stm32_spi_nrf24l01_read_write_byte(uint8_t byte)
{
  uint8_t d_read = 0, d_send = byte;
  if(HAL_SPI_TransmitReceive(&IHU_BSP_STM32_SPI_RFID_NRF24L01_HANDLER, &d_send,&d_read,1,IHU_BSP_STM32_SPI_TX_RX_MAX_DELAY)!=HAL_OK)
  {
    d_read = IHU_BSP_STM32_SPI_TX_RX_MAX_DELAY;
  }
  return d_read; 
}
#endif

/*
 *
 *
 *  回调函数重载
 *
 *
 *
*/



/**
  * SPI接口完成回调函数的处理
  * 为什么需要重新执行HAL_SPI_Receive_IT，待确定
  */
//重载系统函数
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *SpiHandle)
{
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID)	
	uint8_t res = 0;
	msg_struct_spileo_l2frame_rcv_t snd;
  if(SpiHandle==&IHU_BSP_STM32_SPI_SPARE1_HANDLER)
  {
		zIhuBspStm32SpiGeneral1RxBuff[zIhuBspStm32SpiGeneral1RxCount] = zIhuSpiRxBuffer[IHU_BSP_STM32_SPI_SPARE1_HANDLER_ID-1];
		zIhuBspStm32SpiGeneral1RxCount++;
		if (zIhuBspStm32SpiGeneral1RxCount >= IHU_BSP_STM32_SPI1_GENERAL_REC_MAX_LEN)
			zIhuBspStm32SpiGeneral1RxCount = 0;
		HAL_SPI_Receive_IT(&IHU_BSP_STM32_SPI_SPARE1_HANDLER, &zIhuSpiRxBuffer[IHU_BSP_STM32_SPI_SPARE1_HANDLER_ID-1], 1);
  }	
  else if(SpiHandle==&IHU_BSP_STM32_SPI_IAU_HANDLER)
  {
		res = zIhuSpiRxBuffer[IHU_BSP_STM32_SPI_IAU_HANDLER_ID-1];
		zIhuBspStm32SpiGeneral2RxBuff[zIhuBspStm32SpiGeneral2RxCount++] = res;
		if (zIhuBspStm32SpiGeneral2RxCount >= IHU_BSP_STM32_SPI2_GENERAL_REC_MAX_LEN)
			zIhuBspStm32SpiGeneral2RxCount = 0;
		
		//为了IDLE状态下提高效率，直接分解为IDLE和ELSE
		if (zIhuBspStm32SpiGeneral2RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE)
		{
			//只有满足这么苛刻的条件，才算找到了帧头
			if ((res == IHU_HUITP_L2FRAME_STD_RX_START_FLAG_CHAR) && (zIhuBspStm32SpiGeneral2RxCount == 1))
			zIhuBspStm32SpiGeneral2RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_START;
		}
		else
		{
			//收到CHECKSUM
			if((zIhuBspStm32SpiGeneral2RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_START) && (zIhuBspStm32SpiGeneral2RxCount == 2))
			{
				zIhuBspStm32SpiGeneral2RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM;
			}
			//收到长度高位
			else if((zIhuBspStm32SpiGeneral2RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM) && (zIhuBspStm32SpiGeneral2RxCount == 3))
			{
				zIhuBspStm32SpiGeneral2RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN;
			}
			//收到长度低位
			else if((zIhuBspStm32SpiGeneral2RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN) && (zIhuBspStm32SpiGeneral2RxCount == 4))
			{
				zIhuBspStm32SpiGeneral2RxLen = ((zIhuBspStm32SpiGeneral2RxBuff[2] <<8) + zIhuBspStm32SpiGeneral2RxBuff[3]);
				//CHECKSUM及判定
				if ((zIhuBspStm32SpiGeneral2RxBuff[1] == (zIhuBspStm32SpiGeneral2RxBuff[0] ^ zIhuBspStm32SpiGeneral2RxBuff[2]^zIhuBspStm32SpiGeneral2RxBuff[3])) &&\
					(zIhuBspStm32SpiGeneral2RxLen < IHU_BSP_STM32_SPI2_GENERAL_REC_MAX_LEN-4))
				zIhuBspStm32SpiGeneral2RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_BODY;
			}
			//收到BODY位
			else if((zIhuBspStm32SpiGeneral2RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_BODY) && (zIhuBspStm32SpiGeneral2RxLen > 1))
			{
				zIhuBspStm32SpiGeneral2RxLen--;
			}
			//收到BODY最后一位
			else if((zIhuBspStm32SpiGeneral2RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_BODY) && (zIhuBspStm32SpiGeneral2RxLen == 1))
			{
				zIhuBspStm32SpiGeneral2RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE;
				zIhuBspStm32SpiGeneral2RxLen = 0;
				zIhuBspStm32SpiGeneral2RxCount = 0;
				//发送数据到上层SPILEO模块
				memset(&snd, 0, sizeof(msg_struct_spileo_l2frame_rcv_t));
				memcpy(snd.data, &zIhuBspStm32SpiGeneral2RxBuff[4], ((zIhuBspStm32SpiGeneral2RxBuff[2]<<8)+zIhuBspStm32SpiGeneral2RxBuff[3]));
				snd.length = sizeof(msg_struct_spileo_l2frame_rcv_t);
				ihu_message_send(MSG_ID_SPI_L2FRAME_RCV, TASK_ID_SPILEO, TASK_ID_VMFO, &snd, snd.length);		
			}
			//差错情况
			else{
				zIhuBspStm32SpiGeneral2RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE;
				zIhuBspStm32SpiGeneral2RxLen = 0;
				zIhuBspStm32SpiGeneral2RxCount = 0;
			}
		}
		//重新设置中断		
		HAL_SPI_Receive_IT(&IHU_BSP_STM32_SPI_IAU_HANDLER, &zIhuSpiRxBuffer[IHU_BSP_STM32_SPI_IAU_HANDLER_ID-1], 1);
  }
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)	
//  if(SpiHandle==&IHU_BSP_STM32_SPI_RFID522_HANDLER)
//  {
//		zIhuBspStm32SpiGeneral2RxBuff[zIhuBspStm32SpiGeneral2RxCount] = zIhuSpiRxBuffer[IHU_BSP_STM32_SPI_RFID522_HANDLER_ID-1];
//		zIhuBspStm32SpiGeneral2RxCount++;
//		if (zIhuBspStm32SpiGeneral2RxCount >= IHU_BSP_STM32_SPI2_GENERAL_REC_MAX_LEN)
//			zIhuBspStm32SpiGeneral2RxCount = 0;
//		HAL_SPI_Receive_IT(&IHU_BSP_STM32_SPI_RFID522_HANDLER, &zIhuSpiRxBuffer[IHU_BSP_STM32_SPI_RFID522_HANDLER_ID-1], 1);
//  }
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
//  if(SpiHandle==&IHU_BSP_STM32_SPI_AD_SCALE_HANDLER)
//  {
//		zIhuBspStm32SpiGeneral2RxBuff[zIhuBspStm32SpiGeneral2RxCount] = zIhuSpiRxBuffer[IHU_BSP_STM32_SPI_AD_SCALE_HANDLER_ID-1];
//		zIhuBspStm32SpiGeneral2RxCount++;
//		if (zIhuBspStm32SpiGeneral2RxCount >= IHU_BSP_STM32_SPI2_GENERAL_REC_MAX_LEN)
//			zIhuBspStm32SpiGeneral2RxCount = 0;
//		HAL_SPI_Receive_IT(&IHU_BSP_STM32_SPI_AD_SCALE_HANDLER, &zIhuSpiRxBuffer[IHU_BSP_STM32_SPI_AD_SCALE_HANDLER_ID-1], 1);
//  }
#else
	#error Un-correct constant definition
#endif
}




