    /**
 ****************************************************************************************
 *
 * @file i2c_led.h
 *
 * @brief eeprom driver over i2c interface header file.
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

/*******************************************************************************************/
/* Configuration of EEPROM IC's that have been tested                                      */
/* Define one of the following set of directives in your application                       */
/* (not in the body of the driver)                                                         */
/* a) Microchip 24LC02 (2 kBit - 8 byte page)                                              */
/*      #define i2c_led_SIZE   0x100            // EEPROM size in bytes                 */
/*      #define i2c_led_PAGE   8                // EEPROM's page size in bytes          */
/*      #define I2C_SPEED_MODE    I2C_FAST         // fast mode (400 kbits/s)              */
/*      #define I2C_ADDRESS_MODE  I2C_7BIT_ADDR    // 7-bit addressing                     */
/*      #define I2C_ADDRESS_SIZE  I2C_1BYTE_ADDR   // 8-bit memory address                 */
/* b) Microchip 24LC64 (64 kBit - 32 byte page)                                            */
/*      #define i2c_led_SIZE   0x2000           // EEPROM size in bytes                 */
/*      #define i2c_led_PAGE   32               // EEPROM's page size in bytes          */
/*      #define I2C_SPEED_MODE    I2C_FAST         // fast mode (400 kbits/s)              */
/*      #define I2C_ADDRESS_MODE  I2C_7BIT_ADDR    // 7-bit addressing                     */
/*      #define I2C_ADDRESS_SIZE  I2C_2BYTES_ADDR  // 16-bit memory address                */
/* c) ST M24M01 (1 MBit - 256 byte page)                                                   */
/*      #define i2c_led_SIZE   0x20000           // EEPROM size in bytes                */
/*      #define i2c_led_PAGE   256               // EEPROM's page size in bytes         */
/*      #define I2C_SPEED_MODE    I2C_FAST          // fast mode (400 kbits/s)             */
/*      #define I2C_ADDRESS_MODE  I2C_7BIT_ADDR     // 7-bit addressing                    */
/*      #define I2C_ADDRESS_SIZE  I2C_2BYTES_ADDR   // 16-bit memory address               */
/*                                                                                         */
/* Refer to EEPROM's datasheet to define I2C_SLAVE_ADDRESS                                 */
/* i2c_led_init(I2C_SLAVE_ADDRESS, I2C_SPEED_MODE, I2C_ADDRESS_MODE, I2C_2BYTES_ADDR);  */
/*******************************************************************************************/

#include "vmlayer.h"

#ifndef _i2c_led_H
#define _i2c_led_H

#define i2c_led_SIZE   0x100            // EEPROM size in bytes                 
#define i2c_led_PAGE   8                // EEPROM's page size in bytes          
#define I2C_SPEED_MODE    I2C_FAST         // fast mode (400 kbits/s)              
#define I2C_ADDRESS_MODE  I2C_7BIT_ADDR    // 7-bit addressing                     
//#define I2C_ADDRESS_SIZE  I2C_1BYTE_ADDR   // 8-bit memory address       

// Select EEPROM characteristics
#define I2C_EEPROM_SIZE   0x20000         // EEPROM size in bytes
#define I2C_EEPROM_PAGE   256             // EEPROM's page size in bytes
#define I2C_SLAVE_ADDRESS 0x60            // Set slave device address  => 1100 000X for PCA9551
#define I2C_SPEED_MODE    I2C_FAST        // Speed mode: I2C_STANDARD (100 kbits/s), I2C_FAST (400 kbits/s)
#define I2C_ADDRESS_MODE  I2C_7BIT_ADDR   // Addressing mode: {I2C_7BIT_ADDR, I2C_10BIT_ADDR}
//#define I2C_ADDRESS_SIZE  I2C_2BYTES_ADDR // Address width: {I2C_1BYTE_ADDR, I2C_2BYTES_ADDR, I2C_3BYTES_ADDR}

//MYC
#define 	LED_ID_0										0
#define 	LED_ID_1										1
#define 	LED_ID_2										2
#define 	LED_ID_3										3
#define 	LED_ID_4										4
#define 	LED_ID_5										5
#define 	LED_ID_6										6
#define 	LED_ID_7										7

//MYC
#define 	LED_MODE_ON									0
#define 	LED_MODE_OFF								1
#define 	LED_MODE_BLINK_LOW_SPEED		2
#define		LED_MODE_BLINK_HIGH_SPEED		3

//MYC
#define		LED_BLINK_ONE_FOUTH_HZ_PSC	151  	//(0.25Hz)
#define		LED_BLINK_HALF_HZ_PSC				75   	//(0.5Hz)
#define		LED_BLINK_1HZ_PSC						37   	//(1Hz)
#define		LED_BLINK_2HZ_PSC						18		//(2Hz)
#define		LED_BLINK_3HZ_PSC						12		//(3Hz)
#define		LED_BLINK_4HZ_PSC						8			//(4Hz)
#define		LED_BLINK_5HZ_PSC						6			//(5Hz)

//MYC
#define		LED_BLINK_DUTYCYCLE_50_PWM	128

//MYC
//#define		LED_BLINK_LOW_SPEED					LED_BLINK_1HZ
//#define		LED_BLINK_HIGH_SPEED				LED_BLINK_4HZ

//MYC
#define		PCA9551_CTL_REG_INPUT				0x00
#define		PCA9551_CTL_REG_PSC0				0x01
#define		PCA9551_CTL_REG_PWM0				0x02
#define		PCA9551_CTL_REG_PSC1				0x03
#define		PCA9551_CTL_REG_PWM1				0x04
#define		PCA9551_CTL_REG_LS0					0x05
#define		PCA9551_CTL_REG_LS1					0x06

#include <stdint.h>

//enum I2C_SPEED_MODES{
//  I2C_STANDARD = 1,
//  I2C_FAST,
//};

//enum I2C_ADDRESS_MODES{
//  I2C_7BIT_ADDR,
//  I2C_10BIT_ADDR,
//};

//enum I2C_ADRESS_BYTES_COUNT{
//  I2C_1BYTE_ADDR,
//  I2C_2BYTES_ADDR,
//  I2C_3BYTES_ADDR,
//};

/**
 ****************************************************************************************
 * @brief Initialize I2C controller as a master for EEPROM handling.
 ****************************************************************************************
 */
void i2c_led_init(uint16_t dev_address, uint8_t speed, uint8_t address_mode, uint8_t address_size);
void bxxh_i2c_led_init(uint16_t dev_address, uint8_t speed);
void bxxh_led_set(uint8_t ledId, uint8_t mode);


/**
 ****************************************************************************************
 * @brief Disable I2C controller and clock
 ****************************************************************************************
 */
void i2c_led_release(void);

/**
 ****************************************************************************************
 * @brief Polls until I2C eeprom is ready
 ****************************************************************************************
 */
void i2c_wait_until_eeprom_ready(void);

/**
 ****************************************************************************************
 * @brief Read single byte from I2C EEPROM.
 *
 * @param[in] Memory address to read the byte from.
 *
 * @return Read byte.
 ****************************************************************************************
 */
uint8_t i2c_led_read_byte(uint32_t address);

/**
 ****************************************************************************************
 * @brief Reads data from I2C EEPROM to memory position of given pointer.
 *
 * @param[in] rd_data_ptr     Read data pointer.
 * @param[in] address         Starting memory address.
 * @param[in] size            Size of the data to be read.
 *
 * @return Bytes that were actually read (due to memory size limitation).
 ****************************************************************************************
 */
uint32_t i2c_led_read_data(uint8_t *rd_data_ptr, uint32_t address, uint32_t size);

/**
 ****************************************************************************************
 * @brief Write single byte to I2C EEPROM.
 *
 * @param[in] address     Memory position to write the byte to.
 * @param[in] wr_data     Byte to be written.
 ****************************************************************************************
 */
void i2c_led_write_byte(uint32_t address, uint8_t wr_data);

/**
 ****************************************************************************************
 * @brief Writes page to I2C EEPROM.
 *
 * @param[in] address         Starting address of memory page (MUST BE MULTIPLE OF i2c_led_PAGE).
 * @param[in] wr_data_ptr     Pointer to the first of bytes to be written.
 * @param[in] size            Size of the data to be written (MUST BE LESS OR EQUAL TO i2c_led_PAGE).
 *
 * @return Bytes that were actually written (due to memory size limitation).
 ****************************************************************************************
 */
uint16_t i2c_led_write_page(uint8_t *wr_data_ptr, uint32_t address, uint16_t size);

/**
 ****************************************************************************************
 * @brief Writes data to I2C EEPROM.
 *
 * @param[in] address         Starting address of the write process.
 * @param[in] wr_data_ptr     Pointer to the first of bytes to be written.
 * @param[in] size            Size of the data to be written.
 *
 * @return Bytes that were actually written (due to memory size limitation).
 ****************************************************************************************
 */
uint32_t i2c_led_write_data (uint8_t *wr_data_ptr, uint32_t address, uint32_t size);


/* !!!!!!!!! I2C LED APIs !!!!!!!!! */
/* !!!!!!!! MYC 2015/11/08 !!!!!!!!*/
//MYC
//#define 	LED_ID_0										0
//#define 	LED_ID_1										1
// ......
//#define 	LED_ID_7										7

////MYC
//#define 	LED_MODE_ON									0
//#define 	LED_MODE_OFF								1
//#define 	LED_MODE_BLINK_LOW_SPEED		2
//#define		LED_MODE_BLINK_HIGH_SPEED		3



extern void bxxh_i2c_led_init(uint16_t dev_address, uint8_t speed);
extern void bxxh_led_set(uint8_t ledId, uint8_t mode);
extern void bxxh_led_on(uint8_t ledId);
extern void bxxh_led_off(uint8_t ledId);
extern void bxxh_led_blink_once_off_on(uint8_t ledId);
extern void bxxh_led_blink_once_on_off(uint8_t ledId);
extern void bxxh_led_blinking_high_speed(uint8_t ledId);
extern void bxxh_led_blinking_low_speed(uint8_t ledId);

//为IHU上层提供服务
enum IHU_LED_BLINK_MODE_SET
{
	IHU_LED_BLINK_MODE_MIN = 0,
	IHU_LED_BLINK_MODE_ALWAYS_ON,
	IHU_LED_BLINK_MODE_ALWAYS_OFF,
	IHU_LED_BLINK_MODE_ONCE_OFF_ON,
	IHU_LED_BLINK_MODE_ONCE_ON_OFF,
	IHU_LED_BLINK_MODE_HIGH_SPEED,
	IHU_LED_BLINK_MODE_LOW_SPEED,
	IHU_LED_BLINK_MODE_MAX,
};


extern void ihu_i2c_led_init(uint16_t dev_address, uint8_t speed);
extern void ihu_led_set(uint8_t ledId, uint8_t mode);
extern void ihu_led_blink_enable(UINT8 ledId, UINT8 mode);

#endif // _i2c_led_H
