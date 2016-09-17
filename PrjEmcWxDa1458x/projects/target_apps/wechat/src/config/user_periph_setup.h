/**
 ****************************************************************************************
 *
 * @file user_periph_setup.h
 *
 * @brief Peripherals setup header file. 
 *
 * Copyright (C) 2012. Dialog Semiconductor Ltd, unpublished work. This computer 
 * program includes Confidential, Proprietary Information and is a Trade Secret of 
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited 
 * unless authorized in writing. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */
#ifndef _USER_PERIPH_SETUP_H_
#define _USER_PERIPH_SETUP_H_
 
/*
 * INCLUDE FILES
 ****************************************************************************************
 */

    #include "rwip_config.h"
    #include "global_io.h"
    #include "arch.h"
    #include "da1458x_periph_setup.h"
    #include "i2c_eeprom.h"
		#include "da1458x_config_basic.h"
		
/*
 * DEFINES
 ****************************************************************************************
 */

//*** <<< Use Configuration Wizard in Context Menu >>> ***

// <o> DK selection <0=> As in da1458x_periph_setup.h <1=> Basic <2=> Pro <3=> Expert
    #define HW_CONFIG (0)

    #define HW_CONFIG_BASIC_DK  ((HW_CONFIG==0 && SDK_CONFIG==1) || HW_CONFIG==1)
    #define HW_CONFIG_PRO_DK    ((HW_CONFIG==0 && SDK_CONFIG==2) || HW_CONFIG==2)
    #define HW_CONFIG_EXPERT_DK ((HW_CONFIG==0 && SDK_CONFIG==3) || HW_CONFIG==3)

//*** <<< end of configuration section >>>    ***


/****************************************************************************************/ 
/* i2c eeprom configuration                                                             */
/****************************************************************************************/ 
#define I2C_EEPROM_SIZE   0x20000         // EEPROM size in bytes
#define I2C_EEPROM_PAGE   256             // EEPROM's page size in bytes
#define I2C_SPEED_MODE    I2C_FAST        // 1: standard mode (100 kbits/s), 2: fast mode (400 kbits/s)
#define I2C_ADDRESS_MODE  I2C_7BIT_ADDR   // 0: 7-bit addressing, 1: 10-bit addressing
//#define I2C_ADDRESS_SIZE  I2C_2BYTES_ADDR // 0: 8-bit memory address, 1: 16-bit memory address, 3: 24-bit memory address

/****************************************************************************************/ 
/* SPI FLASH configuration                                                             */
/****************************************************************************************/
#define SPI_FLASH_DEFAULT_SIZE  131072    // SPI Flash memory size in bytes
#define SPI_FLASH_DEFAULT_PAGE  256
#define SPI_SECTOR_SIZE         4096

#ifndef __DA14583__
		//Modifed for PEM2/PEM2.1
		#define SPI_EN_GPIO_PORT    GPIO_PORT_2
		#define SPI_EN_GPIO_PIN     GPIO_PIN_1
    #define SPI_CLK_GPIO_PORT   GPIO_PORT_0
    #define SPI_CLK_GPIO_PIN    GPIO_PIN_5
    #define SPI_DO_GPIO_PORT    GPIO_PORT_0
    #define SPI_DO_GPIO_PIN     GPIO_PIN_6
    #define SPI_DI_GPIO_PORT    GPIO_PORT_0
    #define SPI_DI_GPIO_PIN     GPIO_PIN_4

#else // DA14583

    #define SPI_EN_GPIO_PORT    GPIO_PORT_2
    #define SPI_EN_GPIO_PIN     GPIO_PIN_3

    #define SPI_CLK_GPIO_PORT   GPIO_PORT_2
    #define SPI_CLK_GPIO_PIN    GPIO_PIN_0

    #define SPI_DO_GPIO_PORT    GPIO_PORT_2
    #define SPI_DO_GPIO_PIN     GPIO_PIN_9

    #define SPI_DI_GPIO_PORT    GPIO_PORT_2
    #define SPI_DI_GPIO_PIN     GPIO_PIN_4

#endif

/*
 * DEFINES
 ****************************************************************************************
 */

/* Enable WKUPCT. Required by wkupct_quadec driver. */
#define WKUP_ENABLED

//#define HW_CONFIG_BASIC_DK

#if defined(HW_CONFIG_BASIC_DK)
    #define GPIO_ALERT_LED_PORT     GPIO_PORT_1
    #define GPIO_ALERT_LED_PIN      GPIO_PIN_0

    #define USE_PUSH_BUTTON         1
    #define GPIO_BUTTON_PORT        GPIO_PORT_1
    #define GPIO_BUTTON_PIN         GPIO_PIN_1

    #define USE_BAT_LEVEL_ALERT     0
    #define GPIO_BAT_LED_PORT       GPIO_PORT_1
    #define GPIO_BAT_LED_PIN        GPIO_PIN_2
#elif defined(HW_CONFIG_PRO_DK)
    #define GPIO_ALERT_LED_PORT     GPIO_PORT_1
    #define GPIO_ALERT_LED_PIN      GPIO_PIN_0

    #define USE_PUSH_BUTTON         1
    #define GPIO_BUTTON_PORT        GPIO_PORT_1
    #define GPIO_BUTTON_PIN         GPIO_PIN_1

    #define USE_BAT_LEVEL_ALERT     0
    #define GPIO_BAT_LED_PORT       GPIO_PORT_1
    #define GPIO_BAT_LED_PIN        GPIO_PIN_2
#else // HW_CONFIG_EXPERT_DK
    #define GPIO_ALERT_LED_PORT     GPIO_PORT_1
    #define GPIO_ALERT_LED_PIN      GPIO_PIN_0

    #define USE_PUSH_BUTTON         1
    #define GPIO_BUTTON_PORT        GPIO_PORT_1
    #define GPIO_BUTTON_PIN         GPIO_PIN_1

    #define USE_BAT_LEVEL_ALERT     1
    #define GPIO_BAT_LED_PORT       GPIO_PORT_1
    #define GPIO_BAT_LED_PIN        GPIO_PIN_0
#endif

//#ifdef CFG_PRINTF_UART2
//    #define  GPIO_UART2_TX_PORT     GPIO_PORT_0
//    #define  GPIO_UART2_TX_PIN      GPIO_PIN_4
//    
//    #define  GPIO_UART2_RX_PORT     GPIO_PORT_0
//    #define  GPIO_UART2_RX_PIN      GPIO_PIN_5
//#endif

//Modied for PEM2.0/PEM2.1 and other new PEMs
#ifdef CFG_PRINTF_UART2
    #define  GPIO_UART2_TX_PORT     GPIO_PORT_1
    #define  GPIO_UART2_TX_PIN      GPIO_PIN_2
    #define  GPIO_UART2_RX_PORT     GPIO_PORT_1
    #define  GPIO_UART2_RX_PIN      GPIO_PIN_3
#endif

//ZJL: PEM2 is totally different configuration
//#if (IHU_EMCWX_CURRENT_HW_PEM == IHU_EMCWX_CFG_HW_PEM1_0)
//	#define UART1_TX_PORT   GPIO_PORT_0
//	#define UART1_TX_PIN    GPIO_PIN_6
//	#define UART1_RX_PORT   GPIO_PORT_0
//	#define UART1_RX_PIN    GPIO_PIN_7
//	#define UART1_RTS_PORT  GPIO_PORT_3
//	#define UART1_RTS_PIN   GPIO_PIN_1
//	#define UART1_CTS_PORT  GPIO_PORT_3
//	#define UART1_CTS_PIN   GPIO_PIN_2
//	#define UART2_TX_PORT   GPIO_PORT_2
//	#define UART2_TX_PIN    GPIO_PIN_3
//	#define UART2_RX_PORT   GPIO_PORT_2
//	#define UART2_RX_PIN    GPIO_PIN_4
//	#define BEEP_PORT  			GPIO_PORT_2
//	#define BEEP_PIN   			GPIO_PIN_0
//	//MYC test begin, no useful so far
//	#define RELAY_PORT 			GPIO_PORT_2
//	#define RELAY_PIN  			GPIO_PIN_3
//	#define RGB_LED_R_PORT  GPIO_PORT_2
//	#define RGB_LED_R_PIN   GPIO_PIN_4
//	#define RGB_LED_G_PORT  GPIO_PORT_2
//	#define RGB_LED_G_PIN   GPIO_PIN_5
//	#define RGB_LED_B_PORT  GPIO_PORT_2
//	#define RGB_LED_B_PIN   GPIO_PIN_6
//	//MYC test end

//#elif (IHU_EMCWX_CURRENT_HW_PEM == IHU_EMCWX_CFG_HW_PEM1_1)
//	#define UART1_TX_PORT   GPIO_PORT_1
//	#define UART1_TX_PIN    GPIO_PIN_1
//	#define UART1_RX_PORT   GPIO_PORT_1
//	#define UART1_RX_PIN    GPIO_PIN_2
//	#define UART1_RTS_PORT  GPIO_PORT_2
//	#define UART1_RTS_PIN   GPIO_PIN_1
//	#define UART1_CTS_PORT  GPIO_PORT_2
//	#define UART1_CTS_PIN   GPIO_PIN_2
//	#define UART2_TX_PORT   GPIO_PORT_0
//	#define UART2_TX_PIN    GPIO_PIN_4
//	#define UART2_RX_PORT   GPIO_PORT_0
//	#define UART2_RX_PIN    GPIO_PIN_5
//	#define BEEP_PORT  			GPIO_PORT_1
//	#define BEEP_PIN   			GPIO_PIN_0

////#elif (IHU_EMCWX_CURRENT_HW_PEM == IHU_EMCWX_CFG_HW_PEM2_0)
//#ifdef IHU_EMCWX_CFG_HW_PEM2_0_PRESENT
//	#define FLASH_SPI_DI_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DI_PIN    GPIO_PIN_6
//	#define FLASH_SPI_CLK_PORT  GPIO_PORT_0
//	#define FLASH_SPI_CLK_PIN   GPIO_PIN_5
//	#define FLASH_SPI_DO_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DO_PIN    GPIO_PIN_4
//	#define FLASH_SPI_EN_PORT   GPIO_PORT_2
//	#define FLASH_SPI_EN_PIN    GPIO_PIN_1
//	#define UART1_TX_PORT   GPIO_PORT_2
//	#define UART1_TX_PIN    GPIO_PIN_2
//	#define UART1_RX_PORT   GPIO_PORT_2
//	#define UART1_RX_PIN    GPIO_PIN_3
//	#define UART1_RTS_PORT  GPIO_PORT_2
//	#define UART1_RTS_PIN   GPIO_PIN_4
//	#define UART1_CTS_PORT  GPIO_PORT_0
//	#define UART1_CTS_PIN   GPIO_PIN_7
//	//#define UART2_TX_PORT   GPIO_PORT_0 //MYC
//	//#define UART2_TX_PIN    GPIO_PIN_7  //MYC
//	//#define UART2_RX_PORT   GPIO_PORT_2 //MYC
//	//#define UART2_RX_PIN    GPIO_PIN_4  //MYC
//	#define UART2_TX_PORT   GPIO_PORT_1 //ZJL
//	#define UART2_TX_PIN    GPIO_PIN_2  //ZJL
//	#define UART2_RX_PORT   GPIO_PORT_1 //ZJL
//	#define UART2_RX_PIN    GPIO_PIN_3  //ZJL
//	#define BEEP_PORT  			GPIO_PORT_1 //MYC
//	#define BEEP_PIN   			GPIO_PIN_0  //MYC
//	#define LED_SCL_PORT  	GPIO_PORT_2
//	#define LED_SCL_PIN   	GPIO_PIN_8
//	#define LED_SDA_PORT  	GPIO_PORT_2
//	#define LED_SDA_PIN   	GPIO_PIN_9
//	#define LED_RESET_PORT  GPIO_PORT_2
//	#define LED_RESET_PIN   GPIO_PIN_7
//	#define BM_VIN_OK_PORT  GPIO_PORT_2
//	#define BM_VIN_OK_PIN   GPIO_PIN_0
//	#define ADC0_EMC_PORT  	GPIO_PORT_0
//	#define ADC0_EMC_PIN   	GPIO_PIN_0
//	#define ADC1_TMR_PORT  	GPIO_PORT_0
//	#define ADC1_TMR_PIN   	GPIO_PIN_1
//	#define ADC2_THM_PORT  	GPIO_PORT_0
//	#define ADC2_THM_PIN   	GPIO_PIN_2
//	#define ADC3_RSV_PORT  	GPIO_PORT_0
//	#define ADC3_RSV_PIN   	GPIO_PIN_3
//	#define VCC_OUT_EN_PORT GPIO_PORT_2
//	#define VCC_OUT_EN_PIN  GPIO_PIN_6
//	#define DB15_RSV1_PORT  GPIO_PORT_1
//	#define DB15_RSV1_PIN   GPIO_PIN_0
//	#define DB15_RSV2_PORT  GPIO_PORT_1
//	#define DB15_RSV2_PIN   GPIO_PIN_1
//	#define DB15_RSV3_PORT  GPIO_PORT_2
//	#define DB15_RSV3_PIN   GPIO_PIN_5
//#endif

////#elif (IHU_EMCWX_CURRENT_HW_PEM == IHU_EMCWX_CFG_HW_PEM2_1)
//#ifdef IHU_EMCWX_CFG_HW_PEM2_1_PRESENT
//	#define FLASH_SPI_DI_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DI_PIN    GPIO_PIN_6
//	#define FLASH_SPI_CLK_PORT  GPIO_PORT_0
//	#define FLASH_SPI_CLK_PIN   GPIO_PIN_5
//	#define FLASH_SPI_DO_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DO_PIN    GPIO_PIN_4
//	#define FLASH_SPI_EN_PORT   GPIO_PORT_2
//	#define FLASH_SPI_EN_PIN    GPIO_PIN_1
//	#define UART1_TX_PORT   GPIO_PORT_2
//	#define UART1_TX_PIN    GPIO_PIN_2
//	#define UART1_RX_PORT   GPIO_PORT_2
//	#define UART1_RX_PIN    GPIO_PIN_3
//	#define UART1_RTS_PORT  GPIO_PORT_2
//	#define UART1_RTS_PIN   GPIO_PIN_4
//	#define UART1_CTS_PORT  GPIO_PORT_0
//	#define UART1_CTS_PIN   GPIO_PIN_7
//	#define UART2_TX_PORT   GPIO_PORT_1
//	#define UART2_TX_PIN    GPIO_PIN_2 
//	#define UART2_RX_PORT   GPIO_PORT_1
//	#define UART2_RX_PIN    GPIO_PIN_3 
//	#define BEEP_PORT  			GPIO_PORT_1
//	#define BEEP_PIN   			GPIO_PIN_0
//	#define LED_SCL_PORT  	GPIO_PORT_2
//	#define LED_SCL_PIN   	GPIO_PIN_8
//	#define LED_SDA_PORT  	GPIO_PORT_2
//	#define LED_SDA_PIN   	GPIO_PIN_9
//	#define LED_RESET_PORT  GPIO_PORT_2
//	#define LED_RESET_PIN   GPIO_PIN_7
//	#define BM_VIN_OK_PORT  GPIO_PORT_2
//	#define BM_VIN_OK_PIN   GPIO_PIN_0
//	#define ADC0_EMC_PORT  	GPIO_PORT_0
//	#define ADC0_EMC_PIN   	GPIO_PIN_0
//	#define ADC1_TMR_PORT  	GPIO_PORT_0
//	#define ADC1_TMR_PIN   	GPIO_PIN_1
//	#define ADC2_THM_PORT  	GPIO_PORT_0
//	#define ADC2_THM_PIN   	GPIO_PIN_2
//	#define ADC3_RSV_PORT  	GPIO_PORT_0
//	#define ADC3_RSV_PIN   	GPIO_PIN_3
//	#define VCC_OUT_EN_PORT GPIO_PORT_2
//	#define VCC_OUT_EN_PIN  GPIO_PIN_5  //changed comparing with PEM2.0
//	#define DB15_RSV1_PORT  GPIO_PORT_1
//	#define DB15_RSV1_PIN   GPIO_PIN_0
//	#define DB15_RSV2_PORT  GPIO_PORT_1
//	#define DB15_RSV2_PIN   GPIO_PIN_1
//	#define DB15_RSV3_PORT  GPIO_PORT_2
//	#define DB15_RSV3_PIN   GPIO_PIN_6  //changed comparing with PEM2.0
//#endif

////#elif (IHU_EMCWX_CURRENT_HW_PEM == IHU_EMCWX_CFG_HW_PEM3)
//#ifdef IHU_EMCWX_CFG_HW_PEM3_PRESENT
//	#define FLASH_SPI_DI_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DI_PIN    GPIO_PIN_6
//	#define FLASH_SPI_CLK_PORT  GPIO_PORT_0
//	#define FLASH_SPI_CLK_PIN   GPIO_PIN_5
//	#define FLASH_SPI_DO_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DO_PIN    GPIO_PIN_4
//	#define FLASH_SPI_EN_PORT   GPIO_PORT_2
//	#define FLASH_SPI_EN_PIN    GPIO_PIN_1
//	#define UART1_TX_PORT   GPIO_PORT_2
//	#define UART1_TX_PIN    GPIO_PIN_2
//	#define UART1_RX_PORT   GPIO_PORT_2
//	#define UART1_RX_PIN    GPIO_PIN_3
//	#define UART1_RTS_PORT  GPIO_PORT_2
//	#define UART1_RTS_PIN   GPIO_PIN_4
//	#define UART1_CTS_PORT  GPIO_PORT_0
//	#define UART1_CTS_PIN   GPIO_PIN_7
//	#define UART2_TX_PORT   GPIO_PORT_1
//	#define UART2_TX_PIN    GPIO_PIN_2 
//	#define UART2_RX_PORT   GPIO_PORT_1
//	#define UART2_RX_PIN    GPIO_PIN_3 
//	#define BEEP_PORT  			GPIO_PORT_1
//	#define BEEP_PIN   			GPIO_PIN_0
//	#define LED_SCL_PORT  	GPIO_PORT_2
//	#define LED_SCL_PIN   	GPIO_PIN_8
//	#define LED_SDA_PORT  	GPIO_PORT_2
//	#define LED_SDA_PIN   	GPIO_PIN_9
//	#define LED_RESET_PORT  GPIO_PORT_2
//	#define LED_RESET_PIN   GPIO_PIN_7
//	#define BM_VIN_OK_PORT  GPIO_PORT_2
//	#define BM_VIN_OK_PIN   GPIO_PIN_0
//	#define ADC0_EMC_PORT  	GPIO_PORT_0
//	#define ADC0_EMC_PIN   	GPIO_PIN_0
//	#define ADC1_TMR_PORT  	GPIO_PORT_0
//	#define ADC1_TMR_PIN   	GPIO_PIN_1
//	#define ADC2_THM_PORT  	GPIO_PORT_0
//	#define ADC2_THM_PIN   	GPIO_PIN_2
//	#define ADC3_RSV_PORT  	GPIO_PORT_0
//	#define ADC3_RSV_PIN   	GPIO_PIN_3
//	#define VCC_OUT_EN_PORT GPIO_PORT_2
//	#define VCC_OUT_EN_PIN  GPIO_PIN_5
//	#define DB15_RSV1_PORT  GPIO_PORT_1
//	#define DB15_RSV1_PIN   GPIO_PIN_0
//	#define DB15_RSV2_PORT  GPIO_PORT_1
//	#define DB15_RSV2_PIN   GPIO_PIN_1
//	#define DB15_RSV3_PORT  GPIO_PORT_2
//	#define DB15_RSV3_PIN   GPIO_PIN_6
//#endif

//#else
//	#define FLASH_SPI_DI_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DI_PIN    GPIO_PIN_6
//	#define FLASH_SPI_CLK_PORT  GPIO_PORT_0
//	#define FLASH_SPI_CLK_PIN   GPIO_PIN_5
//	#define FLASH_SPI_DO_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DO_PIN    GPIO_PIN_4
//	#define FLASH_SPI_EN_PORT   GPIO_PORT_2
//	#define FLASH_SPI_EN_PIN    GPIO_PIN_1
//	#define UART1_TX_PORT   GPIO_PORT_2
//	#define UART1_TX_PIN    GPIO_PIN_2
//	#define UART1_RX_PORT   GPIO_PORT_2
//	#define UART1_RX_PIN    GPIO_PIN_3
//	#define UART1_RTS_PORT  GPIO_PORT_2
//	#define UART1_RTS_PIN   GPIO_PIN_4
//	#define UART1_CTS_PORT  GPIO_PORT_0
//	#define UART1_CTS_PIN   GPIO_PIN_7
//	#define UART2_TX_PORT   GPIO_PORT_1
//	#define UART2_TX_PIN    GPIO_PIN_2 
//	#define UART2_RX_PORT   GPIO_PORT_1
//	#define UART2_RX_PIN    GPIO_PIN_3 
//	#define BEEP_PORT  			GPIO_PORT_1
//	#define BEEP_PIN   			GPIO_PIN_0
//	#define LED_SCL_PORT  	GPIO_PORT_2
//	#define LED_SCL_PIN   	GPIO_PIN_8
//	#define LED_SDA_PORT  	GPIO_PORT_2
//	#define LED_SDA_PIN   	GPIO_PIN_9
//	#define LED_RESET_PORT  GPIO_PORT_2
//	#define LED_RESET_PIN   GPIO_PIN_7
//	#define BM_VIN_OK_PORT  GPIO_PORT_2
//	#define BM_VIN_OK_PIN   GPIO_PIN_0
//	#define ADC0_EMC_PORT  	GPIO_PORT_0
//	#define ADC0_EMC_PIN   	GPIO_PIN_0
//	#define ADC1_TMR_PORT  	GPIO_PORT_0
//	#define ADC1_TMR_PIN   	GPIO_PIN_1
//	#define ADC2_THM_PORT  	GPIO_PORT_0
//	#define ADC2_THM_PIN   	GPIO_PIN_2
//	#define ADC3_RSV_PORT  	GPIO_PORT_0
//	#define ADC3_RSV_PIN   	GPIO_PIN_3
//	#define VCC_OUT_EN_PORT GPIO_PORT_2
//	#define VCC_OUT_EN_PIN  GPIO_PIN_5
//	#define DB15_RSV1_PORT  GPIO_PORT_1
//	#define DB15_RSV1_PIN   GPIO_PIN_0
//	#define DB15_RSV2_PORT  GPIO_PORT_1
//	#define DB15_RSV2_PIN   GPIO_PIN_1
//	#define DB15_RSV3_PORT  GPIO_PORT_2
//	#define DB15_RSV3_PIN   GPIO_PIN_6
//	//IHU_EMCWX_CFG_HW_PEM3
//#endif //IHU_EMCWX_CURRENT_HW_PEM



//#if (IHU_EMCWX_CURRENT_HW_PEM == IHU_EMCWX_CFG_HW_PEM1_0)
//	#define UART1_TX_PORT   GPIO_PORT_0
//	#define UART1_TX_PIN    GPIO_PIN_6
//	#define UART1_RX_PORT   GPIO_PORT_0
//	#define UART1_RX_PIN    GPIO_PIN_7
//	#define UART1_RTS_PORT  GPIO_PORT_3
//	#define UART1_RTS_PIN   GPIO_PIN_1
//	#define UART1_CTS_PORT  GPIO_PORT_3
//	#define UART1_CTS_PIN   GPIO_PIN_2
//	#define UART2_TX_PORT   GPIO_PORT_2
//	#define UART2_TX_PIN    GPIO_PIN_3
//	#define UART2_RX_PORT   GPIO_PORT_2
//	#define UART2_RX_PIN    GPIO_PIN_4
//	#define BEEP_PORT  			GPIO_PORT_2
//	#define BEEP_PIN   			GPIO_PIN_0
//	//MYC test begin, no useful so far
//	#define RELAY_PORT 			GPIO_PORT_2
//	#define RELAY_PIN  			GPIO_PIN_3
//	#define RGB_LED_R_PORT  GPIO_PORT_2
//	#define RGB_LED_R_PIN   GPIO_PIN_4
//	#define RGB_LED_G_PORT  GPIO_PORT_2
//	#define RGB_LED_G_PIN   GPIO_PIN_5
//	#define RGB_LED_B_PORT  GPIO_PORT_2
//	#define RGB_LED_B_PIN   GPIO_PIN_6
//	//MYC test end

//#elif (IHU_EMCWX_CURRENT_HW_PEM == IHU_EMCWX_CFG_HW_PEM1_1)
//	#define UART1_TX_PORT   GPIO_PORT_1
//	#define UART1_TX_PIN    GPIO_PIN_1
//	#define UART1_RX_PORT   GPIO_PORT_1
//	#define UART1_RX_PIN    GPIO_PIN_2
//	#define UART1_RTS_PORT  GPIO_PORT_2
//	#define UART1_RTS_PIN   GPIO_PIN_1
//	#define UART1_CTS_PORT  GPIO_PORT_2
//	#define UART1_CTS_PIN   GPIO_PIN_2
//	#define UART2_TX_PORT   GPIO_PORT_0
//	#define UART2_TX_PIN    GPIO_PIN_4
//	#define UART2_RX_PORT   GPIO_PORT_0
//	#define UART2_RX_PIN    GPIO_PIN_5
//	#define BEEP_PORT  			GPIO_PORT_1
//	#define BEEP_PIN   			GPIO_PIN_0

//#elif (IHU_EMCWX_CURRENT_HW_PEM == IHU_EMCWX_CFG_HW_PEM2_0)
//	#define FLASH_SPI_DI_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DI_PIN    GPIO_PIN_6
//	#define FLASH_SPI_CLK_PORT  GPIO_PORT_0
//	#define FLASH_SPI_CLK_PIN   GPIO_PIN_5
//	#define FLASH_SPI_DO_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DO_PIN    GPIO_PIN_4
//	#define FLASH_SPI_EN_PORT   GPIO_PORT_2
//	#define FLASH_SPI_EN_PIN    GPIO_PIN_1
//	#define UART1_TX_PORT   GPIO_PORT_2
//	#define UART1_TX_PIN    GPIO_PIN_2
//	#define UART1_RX_PORT   GPIO_PORT_2
//	#define UART1_RX_PIN    GPIO_PIN_3
//	#define UART1_RTS_PORT  GPIO_PORT_2
//	#define UART1_RTS_PIN   GPIO_PIN_4
//	#define UART1_CTS_PORT  GPIO_PORT_0
//	#define UART1_CTS_PIN   GPIO_PIN_7
//	//#define UART2_TX_PORT   GPIO_PORT_0 //MYC
//	//#define UART2_TX_PIN    GPIO_PIN_7  //MYC
//	//#define UART2_RX_PORT   GPIO_PORT_2 //MYC
//	//#define UART2_RX_PIN    GPIO_PIN_4  //MYC
//	#define UART2_TX_PORT   GPIO_PORT_1 //ZJL
//	#define UART2_TX_PIN    GPIO_PIN_2  //ZJL
//	#define UART2_RX_PORT   GPIO_PORT_1 //ZJL
//	#define UART2_RX_PIN    GPIO_PIN_3  //ZJL
//	#define BEEP_PORT  			GPIO_PORT_1 //MYC
//	#define BEEP_PIN   			GPIO_PIN_0  //MYC
//	#define LED_SCL_PORT  	GPIO_PORT_2
//	#define LED_SCL_PIN   	GPIO_PIN_8
//	#define LED_SDA_PORT  	GPIO_PORT_2
//	#define LED_SDA_PIN   	GPIO_PIN_9
//	#define LED_RESET_PORT  GPIO_PORT_2
//	#define LED_RESET_PIN   GPIO_PIN_7
//	#define BM_VIN_OK_PORT  GPIO_PORT_2
//	#define BM_VIN_OK_PIN   GPIO_PIN_0
//	#define ADC0_EMC_PORT  	GPIO_PORT_0
//	#define ADC0_EMC_PIN   	GPIO_PIN_0
//	#define ADC1_TMR_PORT  	GPIO_PORT_0
//	#define ADC1_TMR_PIN   	GPIO_PIN_1
//	#define ADC2_THM_PORT  	GPIO_PORT_0
//	#define ADC2_THM_PIN   	GPIO_PIN_2
//	#define ADC3_RSV_PORT  	GPIO_PORT_0
//	#define ADC3_RSV_PIN   	GPIO_PIN_3
//	#define VCC_OUT_EN_PORT GPIO_PORT_2
//	#define VCC_OUT_EN_PIN  GPIO_PIN_6
//	#define DB15_RSV1_PORT  GPIO_PORT_1
//	#define DB15_RSV1_PIN   GPIO_PIN_0
//	#define DB15_RSV2_PORT  GPIO_PORT_1
//	#define DB15_RSV2_PIN   GPIO_PIN_1
//	#define DB15_RSV3_PORT  GPIO_PORT_2
//	#define DB15_RSV3_PIN   GPIO_PIN_5

//#elif (IHU_EMCWX_CURRENT_HW_PEM == IHU_EMCWX_CFG_HW_PEM2_1)
	#define FLASH_SPI_DI_PORT   GPIO_PORT_0
	#define FLASH_SPI_DI_PIN    GPIO_PIN_6
	#define FLASH_SPI_CLK_PORT  GPIO_PORT_0
	#define FLASH_SPI_CLK_PIN   GPIO_PIN_5
	#define FLASH_SPI_DO_PORT   GPIO_PORT_0
	#define FLASH_SPI_DO_PIN    GPIO_PIN_4
	#define FLASH_SPI_EN_PORT   GPIO_PORT_2
	#define FLASH_SPI_EN_PIN    GPIO_PIN_1
	#define UART1_TX_PORT   GPIO_PORT_2
	#define UART1_TX_PIN    GPIO_PIN_2
	#define UART1_RX_PORT   GPIO_PORT_2
	#define UART1_RX_PIN    GPIO_PIN_3
	#define UART1_RTS_PORT  GPIO_PORT_2
	#define UART1_RTS_PIN   GPIO_PIN_4
	#define UART1_CTS_PORT  GPIO_PORT_0
	#define UART1_CTS_PIN   GPIO_PIN_7
	#define UART2_TX_PORT   GPIO_PORT_1
	#define UART2_TX_PIN    GPIO_PIN_2 
	#define UART2_RX_PORT   GPIO_PORT_1
	#define UART2_RX_PIN    GPIO_PIN_3 
	#define BEEP_PORT  			GPIO_PORT_1
	#define BEEP_PIN   			GPIO_PIN_0
	#define LED_SCL_PORT  	GPIO_PORT_2
	#define LED_SCL_PIN   	GPIO_PIN_8
	#define LED_SDA_PORT  	GPIO_PORT_2
	#define LED_SDA_PIN   	GPIO_PIN_9
	#define LED_RESET_PORT  GPIO_PORT_2
	#define LED_RESET_PIN   GPIO_PIN_7
	#define BM_VIN_OK_PORT  GPIO_PORT_2
	#define BM_VIN_OK_PIN   GPIO_PIN_0
	#define ADC0_EMC_PORT  	GPIO_PORT_0
	#define ADC0_EMC_PIN   	GPIO_PIN_0
	#define ADC1_TMR_PORT  	GPIO_PORT_0
	#define ADC1_TMR_PIN   	GPIO_PIN_1
	#define ADC2_THM_PORT  	GPIO_PORT_0
	#define ADC2_THM_PIN   	GPIO_PIN_2
	#define ADC3_RSV_PORT  	GPIO_PORT_0
	#define ADC3_RSV_PIN   	GPIO_PIN_3
	#define VCC_OUT_EN_PORT GPIO_PORT_2
	#define VCC_OUT_EN_PIN  GPIO_PIN_5  //changed comparing with PEM2.0
	#define DB15_RSV1_PORT  GPIO_PORT_1
	#define DB15_RSV1_PIN   GPIO_PIN_0
	#define DB15_RSV2_PORT  GPIO_PORT_1
	#define DB15_RSV2_PIN   GPIO_PIN_1
	#define DB15_RSV3_PORT  GPIO_PORT_2
	#define DB15_RSV3_PIN   GPIO_PIN_6  //changed comparing with PEM2.0
	#define LMV_SHDN_PORT		GPIO_PORT_2
	#define LMV_SHDN_PIN		GPIO_PIN_6

//#elif (IHU_EMCWX_CURRENT_HW_PEM == IHU_EMCWX_CFG_HW_PEM3)
//	#define FLASH_SPI_DI_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DI_PIN    GPIO_PIN_6
//	#define FLASH_SPI_CLK_PORT  GPIO_PORT_0
//	#define FLASH_SPI_CLK_PIN   GPIO_PIN_5
//	#define FLASH_SPI_DO_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DO_PIN    GPIO_PIN_4
//	#define FLASH_SPI_EN_PORT   GPIO_PORT_2
//	#define FLASH_SPI_EN_PIN    GPIO_PIN_1
//	#define UART1_TX_PORT   GPIO_PORT_2
//	#define UART1_TX_PIN    GPIO_PIN_2
//	#define UART1_RX_PORT   GPIO_PORT_2
//	#define UART1_RX_PIN    GPIO_PIN_3
//	#define UART1_RTS_PORT  GPIO_PORT_2
//	#define UART1_RTS_PIN   GPIO_PIN_4
//	#define UART1_CTS_PORT  GPIO_PORT_0
//	#define UART1_CTS_PIN   GPIO_PIN_7
//	#define UART2_TX_PORT   GPIO_PORT_1
//	#define UART2_TX_PIN    GPIO_PIN_2 
//	#define UART2_RX_PORT   GPIO_PORT_1
//	#define UART2_RX_PIN    GPIO_PIN_3 
//	#define BEEP_PORT  			GPIO_PORT_1
//	#define BEEP_PIN   			GPIO_PIN_0
//	#define LED_SCL_PORT  	GPIO_PORT_2
//	#define LED_SCL_PIN   	GPIO_PIN_8
//	#define LED_SDA_PORT  	GPIO_PORT_2
//	#define LED_SDA_PIN   	GPIO_PIN_9
//	#define LED_RESET_PORT  GPIO_PORT_2
//	#define LED_RESET_PIN   GPIO_PIN_7
//	#define BM_VIN_OK_PORT  GPIO_PORT_2
//	#define BM_VIN_OK_PIN   GPIO_PIN_0
//	#define ADC0_EMC_PORT  	GPIO_PORT_0
//	#define ADC0_EMC_PIN   	GPIO_PIN_0
//	#define ADC1_TMR_PORT  	GPIO_PORT_0
//	#define ADC1_TMR_PIN   	GPIO_PIN_1
//	#define ADC2_THM_PORT  	GPIO_PORT_0
//	#define ADC2_THM_PIN   	GPIO_PIN_2
//	#define ADC3_RSV_PORT  	GPIO_PORT_0
//	#define ADC3_RSV_PIN   	GPIO_PIN_3
//	#define VCC_OUT_EN_PORT GPIO_PORT_2
//	#define VCC_OUT_EN_PIN  GPIO_PIN_5
//	#define DB15_RSV1_PORT  GPIO_PORT_1
//	#define DB15_RSV1_PIN   GPIO_PIN_0
//	#define DB15_RSV2_PORT  GPIO_PORT_1
//	#define DB15_RSV2_PIN   GPIO_PIN_1
//	#define DB15_RSV3_PORT  GPIO_PORT_2
//	#define DB15_RSV3_PIN   GPIO_PIN_6

//#else
//	#define FLASH_SPI_DI_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DI_PIN    GPIO_PIN_6
//	#define FLASH_SPI_CLK_PORT  GPIO_PORT_0
//	#define FLASH_SPI_CLK_PIN   GPIO_PIN_5
//	#define FLASH_SPI_DO_PORT   GPIO_PORT_0
//	#define FLASH_SPI_DO_PIN    GPIO_PIN_4
//	#define FLASH_SPI_EN_PORT   GPIO_PORT_2
//	#define FLASH_SPI_EN_PIN    GPIO_PIN_1
//	#define UART1_TX_PORT   GPIO_PORT_2
//	#define UART1_TX_PIN    GPIO_PIN_2
//	#define UART1_RX_PORT   GPIO_PORT_2
//	#define UART1_RX_PIN    GPIO_PIN_3
//	#define UART1_RTS_PORT  GPIO_PORT_2
//	#define UART1_RTS_PIN   GPIO_PIN_4
//	#define UART1_CTS_PORT  GPIO_PORT_0
//	#define UART1_CTS_PIN   GPIO_PIN_7
//	#define UART2_TX_PORT   GPIO_PORT_1
//	#define UART2_TX_PIN    GPIO_PIN_2 
//	#define UART2_RX_PORT   GPIO_PORT_1
//	#define UART2_RX_PIN    GPIO_PIN_3 
//	#define BEEP_PORT  			GPIO_PORT_1
//	#define BEEP_PIN   			GPIO_PIN_0
//	#define LED_SCL_PORT  	GPIO_PORT_2
//	#define LED_SCL_PIN   	GPIO_PIN_8
//	#define LED_SDA_PORT  	GPIO_PORT_2
//	#define LED_SDA_PIN   	GPIO_PIN_9
//	#define LED_RESET_PORT  GPIO_PORT_2
//	#define LED_RESET_PIN   GPIO_PIN_7
//	#define BM_VIN_OK_PORT  GPIO_PORT_2
//	#define BM_VIN_OK_PIN   GPIO_PIN_0
//	#define ADC0_EMC_PORT  	GPIO_PORT_0
//	#define ADC0_EMC_PIN   	GPIO_PIN_0
//	#define ADC1_TMR_PORT  	GPIO_PORT_0
//	#define ADC1_TMR_PIN   	GPIO_PIN_1
//	#define ADC2_THM_PORT  	GPIO_PORT_0
//	#define ADC2_THM_PIN   	GPIO_PIN_2
//	#define ADC3_RSV_PORT  	GPIO_PORT_0
//	#define ADC3_RSV_PIN   	GPIO_PIN_3
//	#define VCC_OUT_EN_PORT GPIO_PORT_2
//	#define VCC_OUT_EN_PIN  GPIO_PIN_5
//	#define DB15_RSV1_PORT  GPIO_PORT_1
//	#define DB15_RSV1_PIN   GPIO_PIN_0
//	#define DB15_RSV2_PORT  GPIO_PORT_1
//	#define DB15_RSV2_PIN   GPIO_PIN_1
//	#define DB15_RSV3_PORT  GPIO_PORT_2
//	#define DB15_RSV3_PIN   GPIO_PIN_6
//	//IHU_EMCWX_CFG_HW_PEM3
//#endif //IHU_EMCWX_CURRENT_HW_PEM



/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
 
void periph_init(void);

void GPIO_reservations(void);



#endif //_USER_PERIPH_SETUP_H_
