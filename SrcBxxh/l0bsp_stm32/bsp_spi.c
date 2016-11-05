#include <string.h>
#include "bsp_spi.h"
#include "vmfreeoslayer.h"

///////////////////////////////////////////////////////////////////////////////////////

/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;
#define SPILEO_BUF_SIZE 256
uint8_t spileo_rx_buffer[SPILEO_BUF_SIZE];
uint8_t spileo_tx_buffer[SPILEO_BUF_SIZE];

uint32_t spileo_RxState;

static void spileo_tx_isr(SPI_HandleTypeDef *hspi);
static void spileo_rx_isr(SPI_HandleTypeDef *hspi);
static void spileo_tx_complete(SPI_HandleTypeDef *hspi);
static void spileo_rx_complete(SPI_HandleTypeDef *hspi);

uint8_t spileo_gen_chksum(spileo_msgheader_t *pMsgHeader)
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
	  spileo_msgheader_t *pMsgHeader = (spileo_msgheader_t *)hspi->pRxBuffPtr;
	  
	  hspi->pRxBuffPtr[hspi->RxXferCount++] = hspi->Instance->DR;

	  switch(spileo_RxState)
	  {

	  case SPI_RX_STATE_START:
		if(pMsgHeader->start != EXT_BORAD_START_CHAR)
		{
		  // not synchonrized, reset the RxXferCount
		  hspi->RxXferCount = 0;
		}
		else
		  spileo_RxState = SPI_RX_STATE_HEADER;
		break;
		
	  case SPI_RX_STATE_HEADER:
		if(hspi->RxXferCount == sizeof(spileo_msgheader_t))
		{
		  if(spileo_gen_chksum(pMsgHeader) != pMsgHeader->chksum)
		  {
			hspi->RxXferCount = 0;
			spileo_RxState = SPI_RX_STATE_START;
		  }
		  else
		  {
			spileo_RxState = SPI_RX_STATE_BODY;
		  }
		}
		else if(hspi->RxXferCount > sizeof(spileo_msgheader_t))
		{
			/* error case, just reset */
			hspi->RxXferCount = 0;
			spileo_RxState = SPI_RX_STATE_START;
		}
		break;
		
	  case SPI_RX_STATE_BODY:
		if(hspi->RxXferCount == pMsgHeader->len)
		{
			// call user's callback after the receive is complete
			spileo_rx_complete(hspi);
			spileo_RxState = SPI_RX_STATE_START;
			hspi->RxXferCount = 0;
		}

		if(hspi->RxXferCount >= hspi->RxXferSize)
		{
		  hspi->RxXferCount = 0;
		  spileo_RxState = SPI_RX_STATE_START;
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
	
	/* if there isn't more data to transmit, disable TXE interrupt (must...) */
	if(hspi->TxXferCount == hspi->TxXferSize)
	{
	  __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE));
	  __HAL_SPI_CLEAR_OVRFLAG(hspi);

	  // call user's callback after the transfer is complete
		spileo_tx_complete(hspi);
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

static void spileo_rx_complete(SPI_HandleTypeDef *hspi)
{
/*
  IhuDebugPrint("spi RX complete (%d): packet 0x%08x 0x%08x 0x%08x\n\r", 
                hspi->RxXferCount, *(uint32_t *)&hspi->pRxBuffPtr[0], *(uint32_t *)&hspi->pRxBuffPtr[4], *(uint32_t *)&hspi->pRxBuffPtr[8]);
*/

  if(*(uint16_t *)(&spileo_rx_buffer[4]) == 0xEEEE)
  {
    /* for loopback test */
		spileo_msgheader_t *pMsgHeader = (spileo_msgheader_t *)spileo_rx_buffer;
		uint16_t msglen = pMsgHeader->len > SPILEO_BUF_SIZE? SPILEO_BUF_SIZE: pMsgHeader->len;
		
		memcpy(spileo_tx_buffer, spileo_rx_buffer, msglen);
		spileo_start_transmit(hspi, spileo_tx_buffer, msglen);
  }
	else
	{
    /* notify the application */
  }
}

int spileo_start_receive(SPI_HandleTypeDef *hspi, uint8_t *rx_buffer, uint16_t size)
{
  OS_ENTER_CRITICAL_SECTION();

  /* set RX buffer */
  spileo_RxState = SPI_RX_STATE_START;
  hspi->RxXferCount = 0;
  hspi->RxXferSize = size;
  hspi->pRxBuffPtr = rx_buffer;

  OS_LEAVE_CRITICAL_SECTION();
  
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

static void spileo_tx_complete(SPI_HandleTypeDef *hspi)
{
/*
	IhuDebugPrint("spi TX complete: packet 0x%08x 0x%08x size %d\n\r", 
                *(uint32_t *)&hspi->pTxBuffPtr[0], *(uint32_t *)&hspi->pTxBuffPtr[4], hspi->TxXferCount);
*/
}

int spileo_start_transmit(SPI_HandleTypeDef *hspi, uint8_t *tx_buffer, uint16_t size)
{
  if(hspi->TxXferCount < hspi->TxXferSize)
  {
  	IhuErrorPrint("SPI transmit ongoing...\n");
  	return -1;
  }

  OS_ENTER_CRITICAL_SECTION();
  
  hspi->pTxBuffPtr = tx_buffer;
  hspi->TxXferCount = 0;
  hspi->TxXferSize = size;
  
  OS_LEAVE_CRITICAL_SECTION();

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


int spileo_slave_hw_init(int is_clock_phase_1edge, int is_clock_polarity_high)
{
	SPI_HandleTypeDef *hspi = &SpiHandle;
	
	/******************************************************************************
	SPI init
	******************************************************************************/
	/*##-1- Configure the SPI peripheral #######################################*/
	/* Set the SPI parameters */
	hspi->Instance = SPIx;

  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi->Init.Direction		   = SPI_DIRECTION_2LINES;
  hspi->Init.CLKPhase		   = is_clock_phase_1edge?SPI_PHASE_1EDGE:SPI_PHASE_2EDGE;
  hspi->Init.CLKPolarity	   = is_clock_polarity_high?SPI_POLARITY_HIGH:SPI_POLARITY_LOW;
  hspi->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  hspi->Init.CRCPolynomial	   = 7;
  hspi->Init.DataSize		   = SPI_DATASIZE_8BIT;
  hspi->Init.FirstBit		   = SPI_FIRSTBIT_MSB;
  hspi->Init.NSS			   = SPI_NSS_SOFT;
  hspi->Init.TIMode 		   = SPI_TIMODE_DISABLE;

  hspi->Init.Mode = SPI_MODE_SLAVE;
	
	// Please don't use STM32 offical function for receive or transmit
	// Instead, use spileo_start_receive() and spileo_start_transmit()
	hspi->RxISR = spileo_rx_isr;
	hspi->TxISR = spileo_tx_isr;
	
	if(HAL_SPI_Init(hspi) != HAL_OK)
	{
		/* Initialization Error */
		IhuErrorPrint("HAL_SPI_Init() failed.\n");
		return -1;
	}

	/* attach SPI interrupt */
	// BSP_IntVectSet(BSP_INT_ID_SPI2, HAL_SPI_IRQHandler);
	// BSP_IntEn(BSP_INT_ID_SPI2);

	spileo_start_receive(hspi, spileo_rx_buffer, SPILEO_BUF_SIZE);
	
	IhuDebugPrint("spileo_slave_hw_init() done.\n");
	return 0;
}

