/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2012; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*                                             I/O Expander
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                           STM3220G-EVAL
*                                         Evaluation Board
*
* Filename      : bsp_ioe.h
* Version       : V1.00
* Programmer(s) : SL
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*
*           (2) This file and its dependencies requires IAR v6.20 or later to be compiled.
*
*********************************************************************************************************
*/

#ifndef  BSP_IOE_PRESENT
#define  BSP_IOE_PRESENT

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <bsp.h>
#include  <stm32f2xx_i2c.h>

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

typedef struct
{
    CPU_INT16U  TouchDetected;
    CPU_INT16U  X;
    CPU_INT16U  Y;
    CPU_INT16U  Z;
}TS_STATE;


typedef enum
{
    BitReset = 0,
    BitSet   = 1
}IOE_BitValue;


typedef enum
{
    IOE_OK = 0,
    IOE_FAILURE,
    IOE_TIMEOUT,
    PARAM_ERROR,
    IOE1_NOT_OPERATIONAL,
    IOE2_NOT_OPERATIONAL
}IOE_Status;


typedef enum
{
    IOE_DMA_TX = 0,
    IOE_DMA_RX = 1
}IOE_DMADirection;


/*
*********************************************************************************************************
*                                             REGISTERS
*********************************************************************************************************
*/

#define IOE_REG_SYS_CTRL1                               0x03
#define IOE_REG_SYS_CTRL2                               0x04
#define IOE_REG_SPI_CFG                                 0x08
#define IOE_REG_INT_CTRL                                0x09
#define IOE_REG_INT_EN                                  0x0A
#define IOE_REG_INT_STA                                 0x0B
#define IOE_REG_GPIO_INT_EN                             0x0C
#define IOE_REG_GPIO_INT_STA                            0x0D

#define IOE_REG_GPIO_SET_PIN                            0x10
#define IOE_REG_GPIO_CLR_PIN                            0x11
#define IOE_REG_GPIO_MP_STA                             0x12
#define IOE_REG_GPIO_DIR                                0x13
#define IOE_REG_GPIO_ED                                 0x14
#define IOE_REG_GPIO_RE                                 0x15
#define IOE_REG_GPIO_FE                                 0x16
#define IOE_REG_GPIO_AF                                 0x17

#define IOE_REG_ADC_INT_EN                              0x0E
#define IOE_REG_ADC_INT_STA                             0x0F
#define IOE_REG_ADC_CTRL1                               0x20
#define IOE_REG_ADC_CTRL2                               0x21
#define IOE_REG_ADC_CAPT                                0x22
#define IOE_REG_ADC_DATA_CH0                            0x30
#define IOE_REG_ADC_DATA_CH1                            0x32
#define IOE_REG_ADC_DATA_CH2                            0x34
#define IOE_REG_ADC_DATA_CH3                            0x36
#define IOE_REG_ADC_DATA_CH4                            0x38
#define IOE_REG_ADC_DATA_CH5                            0x3A
#define IOE_REG_ADC_DATA_CH6                            0x3B
#define IOE_REG_ADC_DATA_CH7                            0x3C

#define IOE_REG_TSC_CTRL                                0x40
#define IOE_REG_TSC_CFG                                 0x41
#define IOE_REG_WDM_TR_X                                0x42
#define IOE_REG_WDM_TR_Y                                0x44
#define IOE_REG_WDM_BL_X                                0x46
#define IOE_REG_WDM_BL_Y                                0x48
#define IOE_REG_FIFO_TH                                 0x4A
#define IOE_REG_FIFO_STA                                0x4B
#define IOE_REG_FIFO_SIZE                               0x4C
#define IOE_REG_TSC_DATA_X                              0x4D
#define IOE_REG_TSC_DATA_Y                              0x4F
#define IOE_REG_TSC_DATA_Z                              0x51
#define IOE_REG_TSC_DATA_XYZ                            0x52
#define IOE_REG_TSC_FRACT_XYZ                           0x56
#define IOE_REG_TSC_DATA                                0x57
#define IOE_REG_TSC_I_DRIVE                             0x58
#define IOE_REG_TSC_SHIELD                              0x59

#define IOE_REG_TEMP_CTRL                               0x60
#define IOE_REG_TEMP_DATA                               0x61
#define IOE_REG_TEMP_TH                                 0x62

#define IOE_GIT_GPIO                                    0x80
#define IOE_GIT_ADC                                     0x40
#define IOE_GIT_TEMP                                    0x20
#define IOE_GIT_FE                                      0x10
#define IOE_GIT_FF                                      0x08
#define IOE_GIT_FOV                                     0x04
#define IOE_GIT_FTH                                     0x02
#define IOE_GIT_TOUCH                                   0x01

#define IOE_1_ADDR                                      0x82
#define IOE_2_ADDR                                      0x88

#define IOE_ADC_FCT                                     0x01
#define IOE_TS_FCT                                      0x02
#define IOE_IO_FCT                                      0x04
#define IOE_TEMPSENS_FCT                                0x08

#define AUDIO_RESET_PIN                                 0x04
#define MII_INT_PIN                                     0x01
#define VBAT_DIV_PIN                                    0x01
#define MEMS_INT1_PIN                                   0x08
#define MEMS_INT2_PIN                                   0x04

#define JOY_IO_PINS                     (CPU_INT32U)(0x08 | 0x10 | 0x20 | 0x40 | 0x80)
#define TOUCH_IO_ALL                    (CPU_INT32U)(0x02 | 0x04 | 0x08 | 0x10)
#define IO1_IN_ALL_PINS                 (CPU_INT32U)(MEMS_INT1_PIN | MEMS_INT2_PIN)
#define IO2_IN_ALL_PINS                 (CPU_INT32U)(JOY_IO_PINS)
#define IO1_OUT_ALL_PINS                (CPU_INT32U)(VBAT_DIV_PIN)
#define IO2_OUT_ALL_PINS                (CPU_INT32U)(AUDIO_RESET_PIN | MII_INT_PIN)

#define Direction_IN                                    0x00
#define Direction_OUT                                   0x01

#define EDGE_FALLING                                    0x01
#define EDGE_RISING                                     0x02

#define IOE_DMA_CLK                      RCC_AHB1Periph_DMA1
#define IOE_DMA_CHANNEL                  DMA_Channel_1
#define IOE_DMA_TX_STREAM                DMA1_Stream6
#define IOE_DMA_RX_STREAM                DMA1_Stream0
#define IOE_DMA_TX_TCFLAG                DMA_FLAG_TCIF6
#define IOE_DMA_RX_TCFLAG                DMA_FLAG_TCIF0

#define IOE_I2C                          I2C1
#define IOE_I2C_CLK                      RCC_APB1Periph_I2C1
#define IOE_I2C_SCL_PIN                  GPIO_Pin_6
#define IOE_I2C_SCL_GPIO_PORT            GPIOB
#define IOE_I2C_SCL_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define IOE_I2C_SCL_SOURCE               GPIO_PinSource6
#define IOE_I2C_SCL_AF                   GPIO_AF_I2C1

#define IOE_I2C_SDA_PIN                  GPIO_Pin_9
#define IOE_I2C_SDA_GPIO_PORT            GPIOB
#define IOE_I2C_SDA_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define IOE_I2C_SDA_SOURCE               GPIO_PinSource9
#define IOE_I2C_SDA_AF                   GPIO_AF_I2C1

#define IOE_I2C_DR                       ((CPU_INT32U)0x40005410)

#define IOE_IT_PIN                       GPIO_Pin_2
#define IOE_IT_GPIO_PORT                 GPIOI
#define IOE_IT_GPIO_CLK                  RCC_AHB1Periph_GPIOI
#define IOE_IT_EXTI_PORT_SOURCE          EXTI_PortSourceGPIOI
#define IOE_IT_EXTI_PIN_SOURCE           EXTI_PinSource2
#define IOE_IT_EXTI_LINE                 EXTI_Line2
#define IOE_IT_EXTI_IRQn                 EXTI2_IRQn

#define IOE_TimeoutUserCallback()        IOE_TIMEOUT
#define TIMEOUT_MAX                      0x3000

#define I2C_SPEED                        100000

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void         BSP_IOE_Init        (void);

TS_STATE*    BSP_IOE_TS_GetState (void);


#endif

