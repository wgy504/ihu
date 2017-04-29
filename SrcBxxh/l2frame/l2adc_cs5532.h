#ifndef CS5532_H
#define CS5532_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f2xx_hal.h"
	
/* Definition for SPIx clock resources */
#define SPIx                             SPI3
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI3_CLK_ENABLE()
#define DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI3_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI3_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_10
#define SPIx_SCK_GPIO_PORT               GPIOC
#define SPIx_SCK_AF                      GPIO_AF6_SPI3
#define SPIx_MISO_PIN                    GPIO_PIN_11
#define SPIx_MISO_GPIO_PORT              GPIOC
#define SPIx_MISO_AF                     GPIO_AF6_SPI3
#define SPIx_MOSI_PIN                    GPIO_PIN_12
#define SPIx_MOSI_GPIO_PORT              GPIOC
#define SPIx_MOSI_AF                     GPIO_AF6_SPI3

#define MISO_SDO        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)			
#define Clr_CS5532()    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET)
#define Set_CS5532()    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET)

/*转换速率　　　0～9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240*/
extern unsigned char  CollectionFF;          /*采样频率，默认0x09*/


/*通道0增益     0～6   0:1  1:2  2:4  3:8  4:16  5:32  6:64*/ 
extern unsigned char  Chan0Gain;              /*默认0x02*/

extern SPI_HandleTypeDef SpiHandle;
/**********************************************************************
***CS5532 ADC初始化
**输入参数：IntEnable: 是否中断使能
***********************************************************************/  
extern void CS5532Init(void);

/****************************************************************************
* 名称：ReadSeriesADValue()
* 功能：读连续转换AD值。
* 入口参数：无
* 出口参数：读出的AD数据值
* 说明：
****************************************************************************/
extern  unsigned int  ReadSeriesADValue(void);

/*******************************************************
**SD0引脚配置为GPIO模式
********************************************************/
extern   void         CS5532_SD0_ConfigGpio(void);

/*******************************************************
**SDO引脚配置为SPI模式
********************************************************/
extern   void         CS5532_SDO_ConfigSpi(void);

/*******************************************************
**判断转换是否完成.1：完成。0:未完成
********************************************************/
extern unsigned char   ReadWheChanOk(void);

#ifdef __cplusplus
    }
#endif

#endif
