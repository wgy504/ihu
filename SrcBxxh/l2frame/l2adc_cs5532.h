#ifndef CS5532_H
#define CS5532_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f2xx_hal.h"
	
#include "l3bfsc.h"
#include "l3bfsc_msg.h"
#include "main.h"
	
///* Definition for SPIx clock resources */
//#define SPIx                             SPI3
//#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI3_CLK_ENABLE()
//#define DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
//#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
//#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
//#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()

//#define SPIx_FORCE_RESET()               __HAL_RCC_SPI3_FORCE_RESET()
//#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI3_RELEASE_RESET()

///* Definition for SPIx Pins */
//#define SPIx_SCK_PIN                     GPIO_PIN_10
//#define SPIx_SCK_GPIO_PORT               GPIOC
//#define SPIx_SCK_AF                      GPIO_AF6_SPI3
//#define SPIx_MISO_PIN                    GPIO_PIN_11
//#define SPIx_MISO_GPIO_PORT              GPIOC
//#define SPIx_MISO_AF                     GPIO_AF6_SPI3
//#define SPIx_MOSI_PIN                    GPIO_PIN_12
//#define SPIx_MOSI_GPIO_PORT              GPIOC
//#define SPIx_MOSI_AF                     GPIO_AF6_SPI3

#define MISO_SDO        HAL_GPIO_ReadPin(CUBEMX_PIN_F2_ADCS_GPIO_Port, CUBEMX_PIN_F2_ADCS_Pin)			
#define Clr_CS5532()    HAL_GPIO_WritePin(CUBEMX_PIN_F2_ADCS_GPIO_Port, CUBEMX_PIN_F2_ADCS_Pin, GPIO_PIN_RESET)
#define Set_CS5532()    HAL_GPIO_WritePin(CUBEMX_PIN_F2_ADCS_GPIO_Port, CUBEMX_PIN_F2_ADCS_Pin, GPIO_PIN_SET)

//#define MISO_SDO        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)			
//#define Clr_CS5532()    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET)
//#define Set_CS5532()    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET)


/*转换速率　　　0～9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240*/
extern unsigned char  CollectionFF;          /*采样频率，默认0x09*/


/*通道0增益     0～6   0:1  1:2  2:4  3:8  4:16  5:32  6:64*/ 
extern unsigned char  Chan0Gain;              /*默认0x02*/

extern SPI_HandleTypeDef hspi3;

extern WeightSensorParamaters_t					zWeightSensorParam;

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

typedef struct WeightSensorCalirationKB
{
		uint32_t 	b;
		double 		k;
}WeightSensorCalirationKB_t;


#define WEIGHT_SENSOR_NOT_INIT      0
#define WEIGHT_SENSOR_HAD_INITED		1

/* Initialize Sensor */
uint32_t WeightSensorInit(WeightSensorParamaters_t *pwsp);

/* Reconfig Sensor */
uint32_t weightSensorConfig(WeightSensorParamaters_t *pwsp);

/* Zero Load Sensor */
uint32_t WeightSensorCalibrationZero(WeightSensorParamaters_t *pwsp);

/* Full/Nominal Load Sensor */
uint32_t WeightSensorCalibrationFull(WeightSensorParamaters_t *pwsp);

/* Read Load Sensor */
int32_t WeightSensorReadCurrent(WeightSensorParamaters_t *pwsp);

/* mapping table */
uint32_t SpsGainToBitwidthMapping(uint32_t gain_index, uint32_t wordrate_index);

#define 		WORDRATE_INDEX_NUMBER						10
#define 		GAIN_INDEX_NUMBER								7

#define			ADC_AMPLIFIER_GAIN_1X						0
#define			ADC_AMPLIFIER_GAIN_2X						1
#define			ADC_AMPLIFIER_GAIN_4X						2
#define			ADC_AMPLIFIER_GAIN_8X						3
#define			ADC_AMPLIFIER_GAIN_16X					4
#define			ADC_AMPLIFIER_GAIN_32X					5
#define			ADC_AMPLIFIER_GAIN_64X					6

#define			ADC_AMPLIFIER_WORDRATE_120SPS		0
#define			ADC_AMPLIFIER_WORDRATE_60SPS		1
#define			ADC_AMPLIFIER_WORDRATE_30SPS		2
#define			ADC_AMPLIFIER_WORDRATE_15SPS		3
#define			ADC_AMPLIFIER_WORDRATE_7_5SPS		4
#define			ADC_AMPLIFIER_WORDRATE_3840SPS	5
#define			ADC_AMPLIFIER_WORDRATE_1920SPS	6
#define			ADC_AMPLIFIER_WORDRATE_960SPS		7
#define			ADC_AMPLIFIER_WORDRATE_480SPS		8
#define			ADC_AMPLIFIER_WORDRATE_240SPS		9


//000 Gain = 1, (Input Span = [(VREF+)-(VREF-)]/1*A for unipolar).
//001 Gain = 2, (Input Span = [(VREF+)-(VREF-)]/2*A for unipolar).
//010 Gain = 4, (Input Span = [(VREF+)-(VREF-)]/4*A for unipolar).
//011 Gain = 8, (Input Span = [(VREF+)-(VREF-)]/8*A for unipolar).
//100 Gain = 16, (Input Span = [(VREF+)-(VREF-)]/16*A for unipolar).
//101 Gain = 32, (Input Span = [(VREF+)-(VREF-)]/32*A for unipolar).
//110 Gain = 64, (Input Span = [(VREF+)-(VREF-)]/64*A for unipolar).
																																								
//Bit WR (FRS = 0) WR (FRS = 1)
//0000 120 Sps 		100 Sps
//0001 60 Sps 		50 Sps
//0010 30 Sps 		25 Sps
//0011 15 Sps 		12.5 Sps
//0100 7.5 Sps 		6.25 Sps
//1000 3840 Sps 	3200 Sps
//1001 1920 Sps 	1600 Sps
//1010 960 Sps 		800 Sps
//1011 480 Sps 		400 Sps
//1100 240 Sps		200 Sps

//*      Gain    ??     0~6   0:1  1:2  2:4  3:8  4:16  5:32  6:64 
//*      WordRate  ????   0~9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240 

////////////////////////////////////////////////////////////////////////////////
#define WIGHT_SENSOR_CMD_TYPE_STOP 0
#define WIGHT_SENSOR_CMD_TYPE_START 1

typedef struct weight_sensor_cmd_s
{
  uint32_t valid:1;
  uint32_t type:7;
  uint32_t reserved:24;
}weight_sensor_cmd_t;

typedef struct weight_sensor_filter_s
{
  int32_t adc_filtered[2];
  uint32_t beta_num[2];
  uint32_t stable_thresh;
  uint32_t change_thresh;
}weight_sensor_filter_t;

int weight_sensor_send_cmd(uint32_t type);
int weight_sensor_recv_cmd(weight_sensor_cmd_t *command);
uint32_t weight_sensor_read_and_filtering(weight_sensor_filter_t *wsf);
void weight_sensor_task(void const *param);


#ifdef __cplusplus
    }
#endif

#endif
