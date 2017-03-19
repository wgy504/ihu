/**
 ****************************************************************************************
 *
 * @file l2adc_cs5532.h
 *
 * @brief L2 ADC API for CS5532/34-BS from Cirrus Logic,
 *
 * BXXH team
 * Created by MYC, 20170206
 *
 ****************************************************************************************
 */
 
#ifndef L2ADC_CS5532_H
#define L2ADC_CS5532_H

#include <stdint.h>
#include "stm32f2xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define       CS5532_SPI_PORT           0
#define       CS5532_SPI_PIN_SCLK       (1UL << 15)
#define       CS5532_SPI_PIN_MISO       (1UL << 17)
#define       CS5532_SPI_PIN_MOSI       (1UL << 18)
#define       CS5532_SPI_PIN_CS         (1UL << 16)
/*转换速率　　　0～9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240*/
extern unsigned char  CollectionFF;          /*采样频率，默认0x09*/


/*通道0增益     0～6   0:1  1:2  2:4  3:8  4:16  5:32  6:64*/ 
extern unsigned char  Chan0Gain;              /*默认0x02*/


/**********************************************************************
***CS5532 ADC初始化
**输入参数：IntEnable: 是否中断使能
***********************************************************************/  
extern void CS5532Init();

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