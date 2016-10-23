#include "spi_itf.h"
#include "bsp.h"
 
///////////////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////////////

//static void spileo_tx_isr(SPI_HandleTypeDef *hspi);
//static void spileo_rx_isr(SPI_HandleTypeDef *hspi);

/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;

uint8_t spileo_gen_chksum(l2spileo_msgheader_t *pMsgHeader)
{
	uint8_t *pData = (uint8_t *)pMsgHeader;
	
	return pData[0]^pData[2]^pData[3];
}

static void spileo_rx_isr(SPI_HandleTypeDef *hspi)
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
	  l2spileo_msgheader_t *pMsgHeader = (l2spileo_msgheader_t *)hspi->pRxBuffPtr;
	  
	  hspi->pRxBuffPtr[hspi->RxXferCount++] = hspi->Instance->DR;

	  switch(hspi->RxState)
	  {

	  case SPI_RX_STATE_START:
		if(pMsgHeader->start != EXT_BORAD_START_CHAR)
		{
		  // not synchonrized, reset the RxXferCount
		  hspi->RxXferCount = 0;
		}
		else
		  hspi->RxState = SPI_RX_STATE_HEADER;
		break;
		
	  case SPI_RX_STATE_HEADER:
		if(hspi->RxXferCount == sizeof(l2spileo_msgheader_t))
		{
		  if(spileo_gen_chksum(pMsgHeader) != pMsgHeader->chksum)
		  {
			hspi->RxXferCount = 0;
			hspi->RxState = SPI_RX_STATE_START;
		  }
		  else
		  {
			hspi->RxState = SPI_RX_STATE_BODY;
		  }
		}
		break;
		
	  case SPI_RX_STATE_BODY:
		if(hspi->RxXferCount == pMsgHeader->len)
		{
		  hspi->RxState = SPI_RX_STATE_START;
		  if(hspi->RxCompleteCallback)
			hspi->RxCompleteCallback(hspi);
		}

		if(hspi->RxXferCount == hspi->RxXferSize)
		{
		  hspi->RxXferCount = 0;
		  hspi->RxState = SPI_RX_STATE_START;
		}
		break;
	  }
	}
}

static void spileo_tx_isr(SPI_HandleTypeDef *hspi)
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
	
	/* if there isn't more data to transmit, disable TXE interrupt */
	if(hspi->TxXferCount == hspi->TxXferSize)
	{
	  __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE));
	  __HAL_SPI_CLEAR_OVRFLAG(hspi);

	  // call user's callback after the transfer is complete
	  if(hspi->TxCompleteCallback)
	    hspi->TxCompleteCallback(hspi);
	}
}


/**
  * @brief  This function handles SPI interrupt request.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval HAL status
  */
void HAL_SPI_IRQHandler()
{
  uint32_t tmp1 = 0, tmp2 = 0, tmp3 = 0;
  SPI_HandleTypeDef *hspi = &SpiHandle;

  tmp1 = __HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE);
  tmp2 = __HAL_SPI_GET_IT_SOURCE(hspi, SPI_IT_RXNE);
  tmp3 = __HAL_SPI_GET_FLAG(hspi, SPI_FLAG_OVR);
  /* SPI in mode Receiver and Overrun not occurred ---------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET) && (tmp3 == RESET))
  {
    hspi->RxISR(hspi);
    return;
  } 

  tmp1 = __HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE);
  tmp2 = __HAL_SPI_GET_IT_SOURCE(hspi, SPI_IT_TXE);
  /* SPI in mode Transmitter ---------------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    hspi->TxISR(hspi);
    return;
  }

  if(__HAL_SPI_GET_IT_SOURCE(hspi, SPI_IT_ERR) != RESET)
  {
    // journal_user_defined(10, (0x3<<30)|hspi->Instance->SR); // puhuix
    /* SPI CRC error interrupt occurred ---------------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR) != RESET)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_CRC;
      __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
    }
    /* SPI Mode Fault error interrupt occurred --------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_MODF) != RESET)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_MODF;
      __HAL_SPI_CLEAR_MODFFLAG(hspi);
    }
    
    /* SPI Overrun error interrupt occurred -----------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_OVR) != RESET)
    {
      if(hspi->State != HAL_SPI_STATE_BUSY_TX)
      {
        hspi->ErrorCode |= HAL_SPI_ERROR_OVR;
        __HAL_SPI_CLEAR_OVRFLAG(hspi);      
      }
    }

    /* SPI Frame error interrupt occurred -------------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, 0x00000100/* SPI_FLAG_FRE */) != RESET)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_FRE;
      __HAL_SPI_CLEAR_FREFLAG(hspi);
    }

    /* Call the Error call Back in case of Errors */
    if(hspi->ErrorCode!=HAL_SPI_ERROR_NONE)
    {
      hspi->State = HAL_SPI_STATE_READY;
      IhuErrorPrint("SPI ErrorCode: 0x%08x\n", hspi->ErrorCode);
    }
  }
}


int spileo_receive(uint8_t *rx_buffer, uint16_t size)
{
  SPI_HandleTypeDef *hspi = &SpiHandle;
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();
  
  hspi->RxState = SPI_RX_STATE_START;
  hspi->RxXferCount = 0;
  hspi->RxXferSize = size;
  hspi->pRxBuffPtr = rx_buffer;
  
  CPU_CRITICAL_EXIT();
  
  return 0;
}

int spileo_transmit(uint8_t *tx_buffer, uint16_t size)
{
  SPI_HandleTypeDef *hspi = &SpiHandle;
  CPU_SR_ALLOC();

  if(hspi->TxXferCount < hspi->TxXferSize)
  {
  	IhuErrorPrint("SPI transmit ongoing...\n");
  	return -1;
  }

  CPU_CRITICAL_ENTER();
  
  hspi->pTxBuffPtr = tx_buffer;
  hspi->TxXferCount = 0;
  hspi->TxXferSize = size;

  CPU_CRITICAL_EXIT();

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
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  if (hspi->Instance == SPIx)
  {
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
	  RCC_AHB1PeriphClockCmd(SPIx_SCK_RCC_GPIO, ENABLE);
	  RCC_AHB1PeriphClockCmd(SPIx_MISO_RCC_GPIO, ENABLE);
	  RCC_AHB1PeriphClockCmd(SPIx_MOSI_RCC_GPIO, ENABLE);
	
    /* Enable SPI clock */
    SPIx_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.GPIO_Pin       = SPIx_SCK_PIN;
    GPIO_InitStruct.GPIO_Mode      = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType     = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd      = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed     = GPIO_Speed_50MHz;
    GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);
		GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN, SPIx_SCK_AF);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.GPIO_Pin = SPIx_MISO_PIN;
    GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);
		GPIO_PinAFConfig(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN, SPIx_MISO_AF);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.GPIO_Pin = SPIx_MOSI_PIN;
    GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);
		GPIO_PinAFConfig(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN, SPIx_MOSI_AF);

    /*##-3- Configure the NVIC for SPI #########################################*/
    
  }
}

/**
  * @brief  Initializes the SPI according to the specified parameters 
  *         in the SPI_InitTypeDef and create the associated handle.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
{
  /* Check the SPI handle allocation */
  if(hspi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_SPI_MODE(hspi->Init.Mode));
  assert_param(IS_SPI_DIRECTION_MODE(hspi->Init.Direction));
  assert_param(IS_SPI_DATASIZE(hspi->Init.DataSize));
  assert_param(IS_SPI_CPOL(hspi->Init.CLKPolarity));
  assert_param(IS_SPI_CPHA(hspi->Init.CLKPhase));
  assert_param(IS_SPI_NSS(hspi->Init.NSS));
  assert_param(IS_SPI_BAUDRATE_PRESCALER(hspi->Init.BaudRatePrescaler));
  assert_param(IS_SPI_FIRST_BIT(hspi->Init.FirstBit));
  //assert_param(IS_SPI_TIMODE(hspi->Init.TIMode));
  //assert_param(IS_SPI_CRC_CALCULATION(hspi->Init.CRCCalculation));
  assert_param(IS_SPI_CRC_POLYNOMIAL(hspi->Init.CRCPolynomial));

  if(hspi->State == HAL_SPI_STATE_RESET)
  {
    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
    HAL_SPI_MspInit(hspi);
  }
  
  hspi->State = HAL_SPI_STATE_BUSY;

  /* Disable the selected SPI peripheral */
  __HAL_SPI_DISABLE(hspi);

  /*----------------------- SPIx CR1 & CR2 Configuration ---------------------*/
  /* Configure : SPI Mode, Communication Mode, Data size, Clock polarity and phase, NSS management,
  Communication speed, First bit and CRC calculation state */
  hspi->Instance->CR1 = (hspi->Init.SPI_Mode | hspi->Init.SPI_Direction | hspi->Init.SPI_DataSize |
                         hspi->Init.SPI_CPOL | hspi->Init.SPI_CPHA | (hspi->Init.SPI_NSS & SPI_CR1_SSM) |
                         hspi->Init.SPI_BaudRatePrescaler | hspi->Init.SPI_FirstBit);

  /* Configure : NSS management */
  hspi->Instance->CR2 = (((hspi->Init.SPI_NSS >> 16) & SPI_CR2_SSOE));

  /*---------------------------- SPIx CRCPOLY Configuration ------------------*/
  /* Configure : CRC Polynomial */
  hspi->Instance->CRCPR = hspi->Init.SPI_CRCPolynomial;

  /* Activate the SPI mode (Make sure that I2SMOD bit in I2SCFGR register is reset) */
  hspi->Instance->I2SCFGR &= (uint32_t)(~SPI_I2SCFGR_I2SMOD);

  hspi->ErrorCode = HAL_SPI_ERROR_NONE;
  hspi->State = HAL_SPI_STATE_READY;
  
  return HAL_OK;
}







