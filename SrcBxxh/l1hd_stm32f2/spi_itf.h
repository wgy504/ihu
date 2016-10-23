//

#ifndef  __SPI_ITF_H
#define  __SPI_ITF_H

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////////////
#include "stm32f2xx.h"
#include "stm32f2xx_spi.h"
	
/** 
  * @brief  HAL Status structures definition  
  */  
typedef enum 
{
  HAL_OK       = 0x00,
  HAL_ERROR    = 0x01,
  HAL_BUSY     = 0x02,
  HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;

/* SPI1: GPIOA.GPIO_PIN_7 conflict with ETH
         NSS  GPIOA.4, GPIOA.15
         SCK  GPIOA.5, GPIOB.3
         MISO GPIOA.6, GPIOB.4
         MOSI GPIOA.7, GPIOB.5
   SPI2: 
         NSS  GPIOB.12, GPIOI.0, GPIOB.9
         SCK  GPIOB.10, GPIOB.13, GPIOI.1
         MISO GPIOC.2, GPIOB.14, GPIOI.2
         MOSI GPIOC.3, GPIOB.15, GPIOI.3
         
   SPI3: GPIOC.10/11 are used by serial
         NSS  GPIOA.4, GPIOA.15
         SCK  GPIOC.10, GPIOB.3
         MISO GPIOC.11, GPIOB.4
         MOSI GPIOC.12, GPIOB.5

 Here use SPI2: PB13, PB14, PB15 and PB9 (NSS, not care)
*/
#define SPIx                             SPI2
#define SPIx_CLK_ENABLE()                RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE)

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_Pin_13
#define SPIx_SCK_GPIO_PORT               GPIOB
#define SPIx_SCK_AF                      GPIO_AF_SPI2
#define SPIx_MISO_PIN                    GPIO_Pin_14
#define SPIx_MISO_GPIO_PORT              GPIOB
#define SPIx_MISO_AF                     GPIO_AF_SPI2
#define SPIx_MOSI_PIN                    GPIO_Pin_15
#define SPIx_MOSI_GPIO_PORT              GPIOB
#define SPIx_MOSI_AF                     GPIO_AF_SPI2
#define SPIx_SCK_RCC_GPIO                RCC_AHB1Periph_GPIOB
#define SPIx_MISO_RCC_GPIO               RCC_AHB1Periph_GPIOB
#define SPIx_MOSI_RCC_GPIO               RCC_AHB1Periph_GPIOB

/* Definition for SPIx's NVIC */
#define SPIx_IRQn                        SPI2_IRQn


/**
  * @brief  HAL SPI State structure definition
  */
typedef enum
{
  HAL_SPI_STATE_RESET      = 0x00,  /*!< SPI not yet initialized or disabled                */
  HAL_SPI_STATE_READY      = 0x01,  /*!< SPI initialized and ready for use                  */
  HAL_SPI_STATE_BUSY       = 0x02,  /*!< SPI process is ongoing                             */
  HAL_SPI_STATE_BUSY_TX    = 0x12,  /*!< Data Transmission process is ongoing               */
  HAL_SPI_STATE_BUSY_RX    = 0x22,  /*!< Data Reception process is ongoing                  */
  HAL_SPI_STATE_BUSY_TX_RX = 0x32,  /*!< Data Transmission and Reception process is ongoing */
  HAL_SPI_STATE_ERROR      = 0x03   /*!< SPI error state                                    */
    
}HAL_SPI_StateTypeDef;

#define SPI_RX_STATE_START 0
#define SPI_RX_STATE_HEADER 1
#define SPI_RX_STATE_BODY 2

/** 
  * @brief  SPI handle Structure definition
  */
typedef struct __SPI_HandleTypeDef
{
  SPI_TypeDef                *Instance;    /* SPI registers base address */

  SPI_InitTypeDef            Init;         /* SPI communication parameters */

  uint8_t                    *pTxBuffPtr;  /* Pointer to SPI Tx transfer Buffer */

  uint16_t                   TxXferSize;   /* SPI Tx transfer size */
  
  uint16_t                   TxXferCount;  /* SPI Tx Transfer Counter */

  uint8_t                    *pRxBuffPtr;  /* Pointer to SPI Rx transfer Buffer */

  uint16_t                   RxXferSize;   /* SPI Rx transfer size */

  uint16_t                   RxXferCount;  /* SPI Rx Transfer Counter */

  uint32_t                   RxState;      /* SPI Rx State: START, HEADER, BODY */

  void                       (*RxISR)(struct __SPI_HandleTypeDef * hspi); /* function pointer on Rx ISR */

  void                       (*TxISR)(struct __SPI_HandleTypeDef * hspi); /* function pointer on Tx ISR */
  
  void                       (*RxCompleteCallback)(struct __SPI_HandleTypeDef * hspi); /* function pointer on Rx Complete */

  void                       (*TxCompleteCallback)(struct __SPI_HandleTypeDef * hspi); /* function pointer on Tx Complete */

  __IO HAL_SPI_StateTypeDef  State;        /* SPI communication state */

  __IO uint32_t              ErrorCode;    /* SPI Error code */

}SPI_HandleTypeDef;

/** @defgroup SPI_Error_Code SPI Error Code
  * @brief    SPI Error Code 
  * @{
  */ 
#define HAL_SPI_ERROR_NONE         ((uint32_t)0x00000000)   /*!< No error             */
#define HAL_SPI_ERROR_MODF         ((uint32_t)0x00000001)   /*!< MODF error           */
#define HAL_SPI_ERROR_CRC          ((uint32_t)0x00000002)   /*!< CRC error            */
#define HAL_SPI_ERROR_OVR          ((uint32_t)0x00000004)   /*!< OVR error            */
#define HAL_SPI_ERROR_FRE          ((uint32_t)0x00000008)   /*!< FRE error            */
#define HAL_SPI_ERROR_DMA          ((uint32_t)0x00000010)   /*!< DMA transfer error   */
#define HAL_SPI_ERROR_FLAG         ((uint32_t)0x00000020)   /*!< Flag: RXNE,TXE, BSY  */

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif
/* Exported macro ------------------------------------------------------------*/
/** @defgroup SPI_Exported_Macros SPI Exported Macros
  * @{
  */
/** @brief Reset SPI handle state
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @retval None
  */
#define __HAL_SPI_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_SPI_STATE_RESET)

/** @brief  Enable or disable the specified SPI interrupts.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @param  __INTERRUPT__: specifies the interrupt source to enable or disable.
  *         This parameter can be one of the following values:
  *            @arg SPI_IT_TXE: Tx buffer empty interrupt enable
  *            @arg SPI_IT_RXNE: RX buffer not empty interrupt enable
  *            @arg SPI_IT_ERR: Error interrupt enable
  * @retval None
  */
#define __HAL_SPI_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->CR2 |= (__INTERRUPT__))
#define __HAL_SPI_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((__HANDLE__)->Instance->CR2 &= (~(__INTERRUPT__)))

/** @brief  Check if the specified SPI interrupt source is enabled or disabled.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @param  __INTERRUPT__: specifies the SPI interrupt source to check.
  *          This parameter can be one of the following values:
  *             @arg SPI_IT_TXE: Tx buffer empty interrupt enable
  *             @arg SPI_IT_RXNE: RX buffer not empty interrupt enable
  *             @arg SPI_IT_ERR: Error interrupt enable
  * @retval The new state of __IT__ (TRUE or FALSE).
  */
#define __HAL_SPI_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->CR2 & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Check whether the specified SPI flag is set or not.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg SPI_FLAG_RXNE: Receive buffer not empty flag
  *            @arg SPI_FLAG_TXE: Transmit buffer empty flag
  *            @arg SPI_FLAG_CRCERR: CRC error flag
  *            @arg SPI_FLAG_MODF: Mode fault flag
  *            @arg SPI_FLAG_OVR: Overrun flag
  *            @arg SPI_FLAG_BSY: Busy flag
  *            @arg SPI_FLAG_FRE: Frame format error flag  
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_SPI_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__))

/** @brief  Clear the SPI CRCERR pending flag.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @retval None
  */
#define __HAL_SPI_CLEAR_CRCERRFLAG(__HANDLE__) ((__HANDLE__)->Instance->SR = ~(SPI_FLAG_CRCERR))

/** @brief  Clear the SPI MODF pending flag.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral. 
  * @retval None
  */
#define __HAL_SPI_CLEAR_MODFFLAG(__HANDLE__)            \
  do{                                                   \
    __IO uint32_t tmpreg = 0x00;                        \
    tmpreg = (__HANDLE__)->Instance->SR;                \
    (__HANDLE__)->Instance->CR1 &= (~SPI_CR1_SPE);      \
    UNUSED(tmpreg);                                     \
  } while(0) 

/** @brief  Clear the SPI OVR pending flag.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral. 
  * @retval None
  */
#define __HAL_SPI_CLEAR_OVRFLAG(__HANDLE__)     \
  do{                                           \
    __IO uint32_t tmpreg = 0x00;                \
    tmpreg = (__HANDLE__)->Instance->DR;        \
    tmpreg = (__HANDLE__)->Instance->SR;        \
    UNUSED(tmpreg);                             \
  } while(0) 

/** @brief  Clear the SPI FRE pending flag.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @retval None
  */                                           
#define __HAL_SPI_CLEAR_FREFLAG(__HANDLE__)     \
  do{                                           \
  __IO uint32_t tmpreg = 0x00;                  \
  tmpreg = (__HANDLE__)->Instance->SR;          \
  UNUSED(tmpreg);                               \
  }while(0) 

/** @brief Enable SPI
  * @param __HANDLE__: specifies the SPI Handle.
  * @retval None
  */
#define __HAL_SPI_ENABLE(__HANDLE__) ((__HANDLE__)->Instance->CR1 |=  SPI_CR1_SPE)

/** @brief Disable SPI
  * @param __HANDLE__: specifies the SPI Handle.
  * @retval None
  */
#define __HAL_SPI_DISABLE(__HANDLE__) ((__HANDLE__)->Instance->CR1 &=  ~SPI_CR1_SPE)
/**
  * @}
  */

#define EXT_BORAD_START_CHAR 0xEF
#define EXT_BOARD_PADDING_CH 0x00

typedef struct l2spileo_msgheader
{
  uint8_t start;
  uint8_t chksum;
  uint16_t len;
}l2spileo_msgheader_t;

//Global APIs
extern void IhuDebugPrint(char *format, ...);
extern void IhuErrorPrint(char *format, ...);
static void spileo_tx_isr(SPI_HandleTypeDef *hspi);
static void spileo_rx_isr(SPI_HandleTypeDef *hspi);

#ifdef __cplusplus
}
#endif

#endif //__SPI_ITF_H


