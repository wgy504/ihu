/*----------------------------------------Copyright (c)---------------------------------------------------                              
*                                        http://www.39board.com
*
*      39board工作室提供的所有ST、TI、Freescale、Altera等平台的例程、文档、方案等资料信息旨在为学习者提供
*  学习参考，若使用者在使用过程中因任何原因造成直接或间接的损失，本工作室不承担任何责任。
*  
*
*
*                                                                             —————39board工作室
*
*----------------------------------------------------------------------------------------------------------
* File name:            spi_flash.h
* Author:               alvan 
* Version:              V1.0
* Date:                 2014-01-02
* Description:          SPI_FLASH驱动头文件
* Function List:		
* History:    		
*-----------------------------------------------------------------------------------------------------------
*/

#ifndef  __L1HD_SPI_FLASH_H
#define  __L1HD_SPI_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

	/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
	
#define W25Q10 	                            0x10 	
#define W25Q20 	                            0x11
#define W25Q40 	                            0x12	
#define W25Q80                              0x13 	
#define W25Q16                              0x14
#define W25Q32 	                            0x15
#define W25Q64 	                            0x16
#define W25Q128 	                        0x17	


#define SPI_FLASH_PAGE_SIZE                 0x100

#define SPI_FLASH_WRITE_ENABLE              0x06 
#define SPI_FLASH_WRITE_DISABLE  	        0x04 
#define SPI_FLASH_READ_STATUS_REG 	        0x05 
#define SPI_FLASH_WRITE_STATUS_REG 	       	0x01 
#define SPI_FLASH_READ_DATA                 0x03 
#define SPI_FLASH_FAST_READ_DATA            0x0B 
#define SPI_FLASH_FAST_READ_DUAL_OUTPUT     0x3B 
#define SPI_FLASH_PAGE_PROGRAM              0x02 
#define SPI_FLASH_BLOCK_ERASE 	            0xD8 
#define SPI_FLASH_SECTOR_ERASE              0x20 
#define SPI_FLASH_CHIP_ERASE                0xC7 
#define SPI_FLASH_POWER_DOWN                0xB9 
#define SPI_FLASH_DEVICE_ID 	            0xAB 
#define SPI_FLASH_MANUFACT_DEVICE_ID        0x90 
#define SPI_FLASH_JEDEC_DEVICE_ID 	        0x9F 


/* M25P FLASH SPI Interface pins  */  				   
#define sFLASH_SPI                           SPI1
#define sFLASH_SPI_CLK                       RCC_APB2Periph_SPI1
#define sFLASH_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define sFLASH_SPI_SCK_PIN                   GPIO_Pin_5
#define sFLASH_SPI_SCK_GPIO_PORT             GPIOA
#define sFLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define sFLASH_SPI_SCK_SOURCE                GPIO_PinSource5
#define sFLASH_SPI_SCK_AF                    GPIO_AF_SPI1

#define sFLASH_SPI_MISO_PIN                  GPIO_Pin_6
#define sFLASH_SPI_MISO_GPIO_PORT            GPIOA
#define sFLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define sFLASH_SPI_MISO_SOURCE               GPIO_PinSource6
#define sFLASH_SPI_MISO_AF                   GPIO_AF_SPI1

#define sFLASH_SPI_MOSI_PIN                  GPIO_Pin_7
#define sFLASH_SPI_MOSI_GPIO_PORT            GPIOA
#define sFLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define sFLASH_SPI_MOSI_SOURCE               GPIO_PinSource7
#define sFLASH_SPI_MOSI_AF                   GPIO_AF_SPI1

#define sFLASH_CS_PIN                        GPIO_Pin_4
#define sFLASH_CS_GPIO_PORT                  GPIOA
#define sFLASH_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOA


void  spi_flash_init (void);

u8 spi_flash_read_device_ID (void);

void spi_flash_sector_erase (u32 sector_addr);

void spi_flash_block_erase (u32 block_addr);

void spi_flash_chip_erase (void);

void spi_flash_write (u32 write_addr, u8* p_buffer, u16 num_byte_to_write);

void spi_flash_read (u32 read_addr, u8* p_buffer, u16 num_byte_to_read);




#ifdef __cplusplus
}
#endif

#endif


