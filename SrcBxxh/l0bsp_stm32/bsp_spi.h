#ifndef  __BSP_STM32_SPI_H
#define  __BSP_STM32_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////////////
#include "stm32f2xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "sysdim.h"
#include "vmfreeoslayer.h"
#include "bsp_usart.h"
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#include "commsgccl.h"
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	#include "commsgbfsc.h"
#else
#endif

//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名

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
typedef struct IHU_HUITP_L2FRAME_STD_SPI_frame_header
{
  uint8_t start;
  uint8_t chksum;
  uint16_t len;
}IHU_HUITP_L2FRAME_STD_SPI_frame_header_t;
#define IHU_BSP_STM32_SPI_RX_STATE_START 0
#define IHU_BSP_STM32_SPI_RX_STATE_HEADER 1
#define IHU_BSP_STM32_SPI_RX_STATE_BODY 2
#define IHU_BSP_STM32_SPI_EXT_BORAD_START_CHAR 0xFE
#define IHU_BSP_STM32_SPI_EXT_BOARD_PADDING_CH 0x00

//Global APIs, defined by XPH
//需要将func_bsp_spi_slave_hw_init的工作内容融入到ihu_bsp_stm32_spi_slave_hw_init中去
int func_bsp_spi_slave_hw_init(int is_clock_phase_1edge, int is_clock_polarity_high);
int func_bsp_spi_start_transmit(SPI_HandleTypeDef *hspi, uint8_t *tx_buffer, uint16_t size);
int func_bsp_spi_start_receive(SPI_HandleTypeDef *hspi, uint8_t *rx_buffer, uint16_t size);

//ZJL DEFINITION
//常量定义
//MAX_IHU_MSG_BODY_LENGTH-2是因为发送到上层SPILEO的数据缓冲区受到消息结构msg_struct_spileo_l2frame_rcv_t的影响
#define IHU_BSP_STM32_SPI_IAU_REC_MAX_LEN 					IHU_MSG_BODY_L2FRAME_MAX_LEN	//最大接收数据长度
#define IHU_BSP_STM32_SPI1_PRESENT_REC_MAX_LEN 			IHU_MSG_BODY_L2FRAME_MAX_LEN	//最大接收数据长度

//发送和接受数据的延迟时间长度
#define IHU_BSP_STM32_SPI_TX_MAX_DELAY 						100
#define IHU_BSP_STM32_SPI_RX_MAX_DELAY 						100

//交换矩阵
#define IHU_BSP_STM32_SPI1_PRESENT_HANDLER					hspi1
#define IHU_BSP_STM32_SPI1_PRESENT_HANDLER_ID  			1
#define IHU_BSP_STM32_SPI_IAU_HANDLER								hspi2
#define IHU_BSP_STM32_SPI_IAU_HANDLER_ID  					2

//全局函数
extern int ihu_bsp_stm32_spi_slave_hw_init(void);

extern int ihu_bsp_stm32_spi_spare1_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_spi_spare1_rcv_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_spi_iau_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_spi_iau_rcv_data(uint8_t* buff, uint16_t len);

//重载接收函数，以便通过IT中断方式搞定接收通信，否则需要通过轮询或者单独线程搞定，更加麻烦
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *SpiHandle);


//local API
static void BSP_SPI_tx_isr(SPI_HandleTypeDef *hspi);
static void BSP_SPI_rx_isr(SPI_HandleTypeDef *hspi);
static void BSP_SPI_tx_complete(SPI_HandleTypeDef *hspi);
static void BSP_SPI_rx_complete(SPI_HandleTypeDef *hspi);


#ifdef __cplusplus
}
#endif
#endif //__BSP_STM32_SPI_H


