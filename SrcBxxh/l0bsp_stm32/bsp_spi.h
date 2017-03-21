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
	#error Un-correct constant definition
#endif

//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名
//NC522, MF522模块
//本地定义的交换矩阵	
	#define BSP_STM32_SPI_MF522_CS_PORT       			CUBEMX_PIN_F2_SPI2_CS_GPIO_Port
	#define BSP_STM32_SPI_MF522_CS_PIN        			CUBEMX_PIN_F2_SPI2_CS_Pin
	#define BSP_STM32_SPI_MF522_CE_PORT      			 	CUBEMX_PIN_F2_SPI2_CE_GPIO_Port
	#define BSP_STM32_SPI_MF522_CE_PIN        			CUBEMX_PIN_F2_SPI2_CE_Pin
	#define BSP_STM32_SPI_MF522_CLK_PORT      			CUBEMX_PIN_F2_SPI2_SCK_GPIO_Port
	#define BSP_STM32_SPI_MF522_CLK_PIN       			CUBEMX_PIN_F2_SPI2_SCK_Pin
	#define BSP_STM32_SPI_MF522_MISO_PORT     			CUBEMX_PIN_F2_SPI2_MISO_GPIO_Port
	#define BSP_STM32_SPI_MF522_MISO_PIN      			CUBEMX_PIN_F2_SPI2_MISO_Pin
	#define BSP_STM32_SPI_MF522_MOSI_PORT     			CUBEMX_PIN_F2_SPI2_MOSI_GPIO_Port
	#define BSP_STM32_SPI_MF522_MOSI_PIN      			CUBEMX_PIN_F2_SPI2_MOSI_Pin
	#define BSP_STM32_SPI_MF522_IRQ_PORT      			CUBEMX_PIN_F2_SPI2_GPIO_RST_GPIO_Port
	#define BSP_STM32_SPI_MF522_IRQ_PIN       			CUBEMX_PIN_F2_SPI2_GPIO_RST_Pin
	#define BSP_STM32_SPI_MF522_RST_PORT      			CUBEMX_PIN_F2_SPI2_GPIO_RST_GPIO_Port
	#define BSP_STM32_SPI_MF522_RST_PIN       			CUBEMX_PIN_F2_SPI2_GPIO_RST_Pin
//有效的函数体
	#define BSP_STM32_SPI_MF522_SPI_CS_ENABLE()             HAL_GPIO_WritePin(BSP_STM32_SPI_MF522_CS_PORT, BSP_STM32_SPI_MF522_CS_PIN, GPIO_PIN_RESET)
	#define BSP_STM32_SPI_MF522_SPI_CS_DISABLE()            HAL_GPIO_WritePin(BSP_STM32_SPI_MF522_CS_PORT, BSP_STM32_SPI_MF522_CS_PIN, GPIO_PIN_SET)
	#define BSP_STM32_SPI_MF522_CE_LOW()                    HAL_GPIO_WritePin(BSP_STM32_SPI_MF522_CE_PORT, BSP_STM32_SPI_MF522_CE_PIN, GPIO_PIN_RESET)
	#define BSP_STM32_SPI_MF522_CE_HIGH()                   HAL_GPIO_WritePin(BSP_STM32_SPI_MF522_CE_PORT, BSP_STM32_SPI_MF522_CE_PIN, GPIO_PIN_SET)
  #define BSP_STM32_SPI_MF522_IRQ_PIN_READ()              HAL_GPIO_ReadPin(BSP_STM32_SPI_MF522_IRQ_PORT, BSP_STM32_SPI_MF522_IRQ_PIN)
	#define BSP_STM32_SPI_MF522_SPI_RESET_ENABLE()          HAL_GPIO_WritePin(BSP_STM32_SPI_MF522_RST_PORT, BSP_STM32_SPI_MF522_RST_PIN, GPIO_PIN_RESET)
	#define BSP_STM32_SPI_MF522_SPI_RESET_DISABLE()         HAL_GPIO_WritePin(BSP_STM32_SPI_MF522_RST_PORT, BSP_STM32_SPI_MF522_RST_PIN, GPIO_PIN_SET)
	
//NRF24L01
//本地定义的交换矩阵	
	#define BSP_STM32_SPI_NRF24L01_CS_PORT       CUBEMX_PIN_F2_SPI2_CS_GPIO_Port
	#define BSP_STM32_SPI_NRF24L01_CS_PIN        CUBEMX_PIN_F2_SPI2_CS_Pin
	#define BSP_STM32_SPI_NRF24L01_CE_PORT       CUBEMX_PIN_F2_SPI2_CE_GPIO_Port
	#define BSP_STM32_SPI_NRF24L01_CE_PIN        CUBEMX_PIN_F2_SPI2_CE_Pin
	#define BSP_STM32_SPI_NRF24L01_CLK_PORT      CUBEMX_PIN_F2_SPI2_SCK_GPIO_Port
	#define BSP_STM32_SPI_NRF24L01_CLK_PIN       CUBEMX_PIN_F2_SPI2_SCK_Pin
	#define BSP_STM32_SPI_NRF24L01_MISO_PORT     CUBEMX_PIN_F2_SPI2_MISO_GPIO_Port
	#define BSP_STM32_SPI_NRF24L01_MISO_PIN      CUBEMX_PIN_F2_SPI2_MISO_Pin
	#define BSP_STM32_SPI_NRF24L01_MOSI_PORT     CUBEMX_PIN_F2_SPI2_MOSI_GPIO_Port
	#define BSP_STM32_SPI_NRF24L01_MOSI_PIN      CUBEMX_PIN_F2_SPI2_MOSI_Pin
	#define BSP_STM32_SPI_NRF24L01_IRQ_PORT      CUBEMX_PIN_F2_SPI2_GPIO_RST_GPIO_Port
	#define BSP_STM32_SPI_NRF24L01_IRQ_PIN       CUBEMX_PIN_F2_SPI2_GPIO_RST_Pin
	#define BSP_STM32_SPI_NRF24L01_RST_PORT      CUBEMX_PIN_F2_SPI2_GPIO_RST_GPIO_Port
	#define BSP_STM32_SPI_NRF24L01_RST_PIN       CUBEMX_PIN_F2_SPI2_GPIO_RST_Pin
//有效的函数体
	#define BSP_STM32_SPI_NRF24L01_SPI_CS_ENABLE()             HAL_GPIO_WritePin(BSP_STM32_SPI_NRF24L01_CS_PORT, BSP_STM32_SPI_NRF24L01_CS_PIN, GPIO_PIN_RESET)
	#define BSP_STM32_SPI_NRF24L01_SPI_CS_DISABLE()            HAL_GPIO_WritePin(BSP_STM32_SPI_NRF24L01_CS_PORT, BSP_STM32_SPI_NRF24L01_CS_PIN, GPIO_PIN_SET)
	#define BSP_STM32_SPI_NRF24L01_CE_LOW()                    HAL_GPIO_WritePin(BSP_STM32_SPI_NRF24L01_CE_PORT, BSP_STM32_SPI_NRF24L01_CE_PIN, GPIO_PIN_RESET)
	#define BSP_STM32_SPI_NRF24L01_CE_HIGH()                   HAL_GPIO_WritePin(BSP_STM32_SPI_NRF24L01_CE_PORT, BSP_STM32_SPI_NRF24L01_CE_PIN, GPIO_PIN_SET)
  #define BSP_STM32_SPI_NRF24L01_IRQ_PIN_READ()              HAL_GPIO_ReadPin(BSP_STM32_SPI_NRF24L01_IRQ_PORT, BSP_STM32_SPI_NRF24L01_IRQ_PIN)

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
//IHU_SYSDIM_MSG_BODY_LEN_MAX-2是因为发送到上层SPILEO的数据缓冲区受到消息结构msg_struct_spileo_l2frame_rcv_t的影响
#define IHU_BSP_STM32_SPI2_GENERAL_REC_MAX_LEN 					IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX	//最大接收数据长度
#define IHU_BSP_STM32_SPI1_GENERAL_REC_MAX_LEN 			    IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX	//最大接收数据长度

//发送和接受数据的延迟时间长度
#define IHU_BSP_STM32_SPI_TX_MAX_DELAY 						100
#define IHU_BSP_STM32_SPI_RX_MAX_DELAY 						100
#define IHU_BSP_STM32_SPI_TX_RX_MAX_DELAY         0xFF


//SPI Handler指针交换矩阵
//SCYCB=>IAU功能接口
#define IHU_BSP_STM32_SPI_SPARE1_HANDLER						hspi1
#define IHU_BSP_STM32_SPI_SPARE1_HANDLER_ID  				1
#define IHU_BSP_STM32_SPI_IAU_HANDLER								hspi2
#define IHU_BSP_STM32_SPI_IAU_HANDLER_ID  					2
//BFSC=>ADC秤驱动接口
#define IHU_BSP_STM32_SPI_AD_SCALE_HANDLER					hspi2
#define IHU_BSP_STM32_SPI_AD_SCALE_HANDLER_ID  			2
//CCL=>RFID MF522使用了SPI2接口
#define IHU_BSP_STM32_SPI_RFID522_HANDLER						hspi2
#define IHU_BSP_STM32_SPI_RFID522_HANDLER_ID  			2
//CCL=>RFID NRF24L01使用了SPI2接口，且做为接收工作方式
#define IHU_BSP_STM32_SPI_RFID_NRF24L01_HANDLER			hspi2
#define IHU_BSP_STM32_SPI_RFID_NRF24L01_HANDLER_ID  2



//全局函数
extern int ihu_bsp_stm32_spi_slave_hw_init(void);
//SCYCB=>IAU功能接口
extern int ihu_bsp_stm32_spi_spare1_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_spi_spare1_rcv_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_spi_iau_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_spi_iau_rcv_data(uint8_t* buff, uint16_t len);
//BFSC=>ADC秤驱动接口
extern int ihu_bsp_stm32_spi_ad_scale_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_spi_ad_scale_rcv_data(uint8_t* buff, uint16_t len);
//CCL=>RFID NC522使用了SPI2接口
extern void ihu_bsp_stm32_spi_mf522_cs_enable(void);
extern void ihu_bsp_stm32_spi_mf522_cs_disable(void);
extern void ihu_bsp_stm32_spi_mf522_reset_enable(void);
extern void ihu_bsp_stm32_spi_mf522_reset_disable(void);
extern int ihu_bsp_stm32_spi_rfid522_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_spi_rfid522_rcv_data(uint8_t* buff, uint16_t len);
extern uint8_t ihu_bsp_stm32_spi_flash_read_byte(void);
extern uint8_t ihu_bsp_stm32_spi_flash_send_byte(uint8_t byte);
//CCL=>RFID NRF24L01使用了SPI2接口
extern void ihu_bsp_stm32_spi_nrf24l01_cs_enable(void);
extern void ihu_bsp_stm32_spi_nrf24l01_cs_disable(void);
extern void ihu_bsp_stm32_spi_nrf24l01_ce_low(void);
extern void ihu_bsp_stm32_spi_nrf24l01_ce_high(void);
extern uint8_t ihu_bsp_stm32_spi_nrf24l01_irq_read(void);
extern uint8_t ihu_bsp_stm32_spi_nrf24l01_read_write_byte(uint8_t byte);


//重载接收函数，以便通过IT中断方式搞定接收通信，否则需要通过轮询或者单独线程搞定，更加麻烦
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *SpiHandle);


//local API => XPH定义的标准操作函数
static void BSP_SPI_tx_isr(SPI_HandleTypeDef *hspi);
static void BSP_SPI_rx_isr(SPI_HandleTypeDef *hspi);
static void BSP_SPI_tx_complete(SPI_HandleTypeDef *hspi);
static void BSP_SPI_rx_complete(SPI_HandleTypeDef *hspi);


#ifdef __cplusplus
}
#endif
#endif //__BSP_STM32_SPI_H


