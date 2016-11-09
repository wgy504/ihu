//

#ifndef  __BSP_SPI_H
#define  __BSP_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////////////
#include "stm32f2xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "sysdim.h"
#include "vmfreeoslayer.h"
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#include "commsgccl.h"
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	#include "commsgbfsc.h"
#else
#endif

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
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI2_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI2_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_13
#define SPIx_SCK_GPIO_PORT               GPIOB
#define SPIx_SCK_AF                      GPIO_AF5_SPI2
#define SPIx_MISO_PIN                    GPIO_PIN_14
#define SPIx_MISO_GPIO_PORT              GPIOB
#define SPIx_MISO_AF                     GPIO_AF5_SPI2
#define SPIx_MOSI_PIN                    GPIO_PIN_15
#define SPIx_MOSI_GPIO_PORT              GPIOB
#define SPIx_MOSI_AF                     GPIO_AF5_SPI2

/* Definition for SPIx's NVIC */
#define SPIx_IRQn                        SPI2_IRQn

//GENERAL FRAME帧结构
typedef struct BSP_SPI_msgheader
{
  uint8_t start;
  uint8_t chksum;
  uint16_t len;
}BSP_SPI_msgheader_t;
#define SPI_RX_STATE_START 0
#define SPI_RX_STATE_HEADER 1
#define SPI_RX_STATE_BODY 2
#define EXT_BORAD_START_CHAR 0xFE
#define EXT_BOARD_PADDING_CH 0x00

//Global APIs
int BSP_SPI_slave_hw_init(int is_clock_phase_1edge, int is_clock_polarity_high);
int BSP_SPI_start_transmit(SPI_HandleTypeDef *hspi, uint8_t *tx_buffer, uint16_t size);
int BSP_SPI_start_receive(SPI_HandleTypeDef *hspi, uint8_t *rx_buffer, uint16_t size);


//ZJL DEFINITION
//常量定义
//MAX_IHU_MSG_BODY_LENGTH-1是因为发送到上层SPILEO的数据缓冲区受到消息结构msg_struct_spileo_l2frame_rcv_t的影响
#define BSP_STM32_SPI_IAU_REC_MAXLEN 			MAX_IHU_MSG_BODY_LENGTH-1	//最大接收数据长度
#define BSP_STM32_SPI_SPARE1_REC_MAXLEN 	MAX_IHU_MSG_BODY_LENGTH-1	//最大接收数据长度
#define BSP_STM32_SPI_SPARE1			hspi1
#define BSP_STM32_SPI_SPARE1_ID  	1
#define BSP_STM32_SPI_IAU					hspi2
#define BSP_STM32_SPI_IAU_ID  		2
#define SPI_TX_MAX_DELAY_DURATION 100
#define SPI_RX_MAX_DELAY_DURATION 100
extern int BSP_STM32_SPI_SPARE1_SendData(uint8_t* buff, uint16_t len);
extern int BSP_STM32_SPI_SPARE1_RcvData(uint8_t* buff, uint16_t len);
extern int BSP_STM32_SPI_IAU_SendData(uint8_t* buff, uint16_t len);
extern int BSP_STM32_SPI_IAU_RcvData(uint8_t* buff, uint16_t len);
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *SpiHandle);

#ifdef __cplusplus
}
#endif

#endif //__BSP_SPI_H


