/**
 ****************************************************************************************
 *
 * @file periph_setup.c
 *
 * @brief Peripherals setup and initialization. 
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
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration
#include "user_periph_setup.h"            // peripheral configuration
#include "global_io.h"
#include "gpio.h"
#include "uart.h"                    // UART initialization
#include "mpbledemo2.h"
#include "spi_flash.h"

/**
 ****************************************************************************************
 * @brief Each application reserves its own GPIOs here.
 *
 * @return void
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
* Globally reserved GPIOs reservation
*/

/*
* Application specific GPIOs reservation. Used only in Development mode (#if DEVELOPMENT_DEBUG)
    
i.e.  
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);    //Reserve P_01 as Generic Purpose I/O
*/
#ifdef CFG_PRINTF_UART2
	RESERVE_GPIO(UART2_TX, GPIO_UART2_TX_PORT,  GPIO_UART2_TX_PIN, PID_UART2_TX);
	RESERVE_GPIO(UART2_RX, GPIO_UART2_RX_PORT,  GPIO_UART2_RX_PIN, PID_UART2_RX);
#endif
	
	//RESERVE ADC == PEM2.x以后才能完整支持
	RESERVE_GPIO(ADC_EMC, ADC0_EMC_PORT,  ADC0_EMC_PIN, PID_ADC);
	RESERVE_GPIO(ADC_TMR, ADC1_TMR_PORT,  ADC1_TMR_PIN, PID_ADC);
	RESERVE_GPIO(ADC_THM, ADC2_THM_PORT,  ADC2_THM_PIN, PID_ADC);
	RESERVE_GPIO(ADC_RSV, ADC3_RSV_PORT,  ADC3_RSV_PIN, PID_ADC);

	//RESERVE SPI FLASH == PEM2.x以后才能完整支持
	RESERVE_GPIO(FLASH_EN, FLASH_SPI_EN_PORT,  FLASH_SPI_EN_PIN, PID_SPI_EN);
	RESERVE_GPIO(FLASH_CLK, FLASH_SPI_CLK_PORT,  FLASH_SPI_CLK_PIN, PID_SPI_CLK);
	RESERVE_GPIO(FLASH_DO, FLASH_SPI_DO_PORT,  FLASH_SPI_DO_PIN, PID_SPI_DO);
	RESERVE_GPIO(FLASH_DI, FLASH_SPI_DI_PORT,  FLASH_SPI_DI_PIN, PID_SPI_DI);
	
	//RESERVE LED  == PEM2.x以后才能完整支持
	RESERVE_GPIO(LED_SCL, LED_SCL_PORT,  LED_SCL_PIN, PID_I2C_SCL);
	RESERVE_GPIO(LED_SDA, LED_SDA_PORT,  LED_SDA_PIN, PID_I2C_SDA);  
	RESERVE_GPIO(LED_RESET, LED_RESET_PORT,  LED_RESET_PIN, PID_GPIO);
	
	//RESERVE UART1
	RESERVE_GPIO(UART_TX, UART1_TX_PORT,  UART1_TX_PIN, PID_UART1_TX);
	RESERVE_GPIO(UART_TX, UART1_RX_PORT,  UART1_RX_PIN, PID_UART1_RX);
	RESERVE_GPIO(UART_RTS, UART1_RTS_PORT,  UART1_RTS_PIN, PID_UART1_RTSN);
	RESERVE_GPIO(UART_CTS, UART1_CTS_PORT,  UART1_CTS_PIN, PID_UART1_CTSN);
	
	//Battery Management
	RESERVE_GPIO(BM_VIN, BM_VIN_OK_PORT,  BM_VIN_OK_PIN, PID_GPIO);
	
}
#endif //DEVELOPMENT_DEBUG

/**
 ****************************************************************************************
 * @brief Map port pins
 *
 * The Uart and SPI port pins and GPIO ports are mapped
 ****************************************************************************************
 */
void set_pad_functions(void)        // set gpio port function mode
{
#ifdef CFG_PRINTF_UART2
	GPIO_ConfigurePin( GPIO_UART2_TX_PORT,  GPIO_UART2_TX_PIN, OUTPUT, PID_UART2_TX, false );
	GPIO_ConfigurePin( GPIO_UART2_RX_PORT,  GPIO_UART2_RX_PIN, INPUT, PID_UART2_RX, false );
#endif
/*
* Configure application ports.
i.e.    
    GPIO_ConfigurePin( GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false ); // Set P_01 as Generic purpose Output
*/
	
	//ADC PAD SET  == PEM2.x以后才能完整支持
	GPIO_ConfigurePin(ADC0_EMC_PORT, ADC0_EMC_PIN, INPUT, PID_ADC, false);
	GPIO_ConfigurePin(ADC1_TMR_PORT, ADC1_TMR_PIN, INPUT, PID_ADC, false);
	GPIO_ConfigurePin(ADC2_THM_PORT, ADC2_THM_PIN, INPUT, PID_ADC, false);
	GPIO_ConfigurePin(ADC3_RSV_PORT, ADC3_RSV_PIN, INPUT, PID_ADC, false);
	
	//FLASH_SPI PAD SET  == PEM2.x以后才能完整支持
	GPIO_ConfigurePin(FLASH_SPI_EN_PORT, FLASH_SPI_EN_PIN, OUTPUT, PID_SPI_EN, true);  //CS
	GPIO_ConfigurePin(FLASH_SPI_CLK_PORT, FLASH_SPI_CLK_PIN, OUTPUT, PID_SPI_CLK, false);
	GPIO_ConfigurePin(FLASH_SPI_DO_PORT, FLASH_SPI_DO_PIN, OUTPUT, PID_SPI_DO, false);
	GPIO_ConfigurePin(FLASH_SPI_DI_PORT, FLASH_SPI_DI_PIN, INPUT, PID_SPI_DI, false);	
	 
	//LED PAD SET  == PEM2.x以后才能完整支持
	GPIO_ConfigurePin(LED_SCL_PORT, LED_SCL_PIN, OUTPUT, PID_I2C_SCL, false);	//输入输出？？？
	GPIO_ConfigurePin(LED_SDA_PORT, LED_SDA_PIN, OUTPUT, PID_I2C_SDA, false);	//输入输出？？？
	GPIO_ConfigurePin(LED_RESET_PORT, LED_RESET_PIN, INPUT, PID_GPIO, false); //针对哪一个PID？
	
	//UART1 PAD SET
	GPIO_ConfigurePin(UART1_TX_PORT, UART1_TX_PIN, OUTPUT, PID_UART1_TX, false);	
	GPIO_ConfigurePin(UART1_RX_PORT, UART1_RX_PIN, INPUT, PID_UART1_RX, false);	
	GPIO_ConfigurePin(UART1_RTS_PORT, UART1_RTS_PIN, INPUT, PID_UART1_RTSN, false);	
	GPIO_ConfigurePin(UART1_RTS_PORT, UART1_CTS_PIN, OUTPUT, PID_UART1_CTSN, false);	

	//Battery Management PAD SET
	GPIO_ConfigurePin(BM_VIN_OK_PORT, BM_VIN_OK_PIN, INPUT, PID_GPIO, false);	
	
}


/**
 ****************************************************************************************
 * @brief Enable pad's and peripheral clocks assuming that peripherals' power domain is down. The Uart and SPi clocks are set.
 *
 * @return void
 ****************************************************************************************
 */
void periph_init(void) 
{
	//ZJL新增的，为了FLASH的启动
	// system init
	SetWord16(CLK_AMBA_REG, 0x00);                 // set clocks (hclk and pclk ) 16MHz
	SetWord16(SET_FREEZE_REG, FRZ_WDOG);            // stop watch dog    
	SetBits16(SYS_CTRL_REG, PAD_LATCH_EN,1);        // open pads
	SetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE,1);     // open debugger
	SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);       // exit peripheral power down	
	
	// Power up peripherals' power domain
	SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
	while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP)) ; 

	SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
	
	//rom patch
	patch_func();
	
	//Init pads
	set_pad_functions();

	if (0 != m_mpbledemo2_handler)
	{
			m_mpbledemo2_handler->m_data_init_peripheral_func();
	}
    
	#ifdef CFG_PRINTF_UART2
		SetBits16(CLK_PER_REG, UART2_ENABLE, 1);
		uart2_init(UART_BAUDRATE_115K2, 3); 
	#endif
    
  //Enable the pads
	SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, 1);
		
	//初始化ADC设备，只有ADC0被激活，所以这里只ENABLE一个通道ADC0
	adc_init(GP_ADC_SE, GP_ADC_SIGN, GP_ADC_ATTN3X);  //这里的参数配置，需要再实验研究，并查阅DATASHEET！！！
	adc_usDelay(20);
	adc_enable_channel(ADC_CHANNEL_P00);
	
	//初始化SPI_FLASH，从SPI_FLASH的官方例子中拷贝而来
	SPI_Pad_t spi_FLASH_CS_Pad;
	int8_t detected_spi_flash_device_index;	
	spi_FLASH_CS_Pad.pin = FLASH_SPI_EN_PIN;
	spi_FLASH_CS_Pad.port = FLASH_SPI_EN_PORT;
	// Enable SPI & SPI FLASH
	spi_init(&spi_FLASH_CS_Pad, SPI_MODE_8BIT, SPI_ROLE_MASTER, SPI_CLK_IDLE_POL_LOW, SPI_PHA_MODE_0, SPI_MINT_DISABLE, SPI_XTAL_DIV_8);
	detected_spi_flash_device_index = spi_flash_auto_detect();
	if(detected_spi_flash_device_index == SPI_FLASH_AUTO_DETECT_NOT_DETECTED)
	{
			// The device was not identified.
			// The default parameters are used (SPI_FLASH_SIZE, SPI_FLASH_PAGE)
			// Alternatively, an error can be asserted here.
			spi_flash_init(0x20000, 0x100);  //得根据实际的FLASH进行修改
	}
	
	//初始化LED
	ihu_i2c_led_init(I2C_SLAVE_ADDRESS, LED_BLINK_1HZ_PSC);
	
	//初始化UART1  == PEM2.x中暂时未用，这里只是为了程序编写的完整性，同时还可以进行必要的测试，但板子上并没有强制使用
	SetBits16(CLK_PER_REG, UART1_ENABLE, 1);        // enable clock - always @16MHz
  uart_init(UART_BAUDRATE_115K2, 3);	
	
	//初始化BM_VIN
	//电源直接使用uint8_t battery_get_lvl(uint8_t batt_type)，并没有初始化过程，它采用的ADC自身带的初始化
	
}
