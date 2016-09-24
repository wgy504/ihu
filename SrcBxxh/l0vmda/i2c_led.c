/**
 ****************************************************************************************
 *
 * @file i2c_led.c
 *
 * @brief eeprom driver over i2c interface.
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

#include "i2c_led.h"

// macros
#define SEND_I2C_COMMAND(X) SetWord16(I2C_DATA_CMD_REG, (X))
#define WAIT_WHILE_I2C_FIFO_IS_FULL() while( (GetWord16(I2C_STATUS_REG) & TFNF) == 0 )
#define WAIT_UNTIL_I2C_FIFO_IS_EMPTY() while( (GetWord16(I2C_STATUS_REG) & TFE) == 0 )
#define WAIT_UNTIL_NO_MASTER_ACTIVITY() while( (GetWord16(I2C_STATUS_REG) & MST_ACTIVITY) !=0 )
#define WAIT_FOR_RECEIVED_BYTE() while(GetWord16(I2C_RXFLR_REG) == 0)

//static uint8_t mem_address_size;    // 2 byte address is used or not.
static uint8_t i2c_dev_address;     // Device addres

/*
PCA9551 Default Value is 01 (OFF) for each LED
*/
static uint8_t pca9551_ls0 = 0x55;
static uint8_t pca9551_ls1 = 0x55;
static uint8_t led_init_or_not = false; 

/* DELAYS */
#define LED_BLINK_ONCE_DELAY_LOOP		5000


///**
// ****************************************************************************************
// * @brief Initialize I2C controller as a master for EEPROM handling.
// ****************************************************************************************
// */
//void i2c_led_init(uint16_t dev_address, uint8_t speed, uint8_t address_mode, uint8_t address_size)
//{
//    mem_address_size = address_size;
//    SetBits16(CLK_PER_REG, I2C_ENABLE, 1);                                        // enable  clock for I2C 
//    SetWord16(I2C_ENABLE_REG, 0x0);                                               // Disable the I2C controller	
//    SetWord16(I2C_CON_REG, I2C_MASTER_MODE | I2C_SLAVE_DISABLE | I2C_RESTART_EN); // Slave is disabled
//    SetBits16(I2C_CON_REG, I2C_SPEED, speed);                                     // Set speed
//    SetBits16(I2C_CON_REG, I2C_10BITADDR_MASTER, address_mode);                   // Set addressing mode
//    SetWord16(I2C_TAR_REG, dev_address & 0x3FF);                                  // Set Slave device address
//    SetWord16(I2C_ENABLE_REG, 0x1);                                               // Enable the I2C controller
//    while( (GetWord16(I2C_STATUS_REG) & 0x20) != 0 );                             // Wait for I2C master FSM to be IDLE
//    i2c_dev_address = dev_address;
//}


///**
// ****************************************************************************************
// * @brief Disable I2C controller and clock
// ****************************************************************************************
// */
//void i2c_led_release(void)
//{	
//    SetWord16(I2C_ENABLE_REG, 0x0);                             // Disable the I2C controller	
//    SetBits16(CLK_PER_REG, I2C_ENABLE, 0);                      // Disable clock for I2C
//}


///**
// ****************************************************************************************
// * @brief Polls until I2C eeprom is ready
// ****************************************************************************************
// */
//void i2c_wait_until_eeprom_ready(void)
//{
//    uint16_t abort_SR_Status; // TX Abort Source Register
//    // Polling until EEPROM ACK to detect busy period
//    do {
//        SEND_I2C_COMMAND(0x08);                                 // Make a dummy access
//        WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                         // Wait until Tx FIFO is empty
//        WAIT_UNTIL_NO_MASTER_ACTIVITY();                        // Wait until no master activity   
//        abort_SR_Status = GetWord16(I2C_TX_ABRT_SOURCE_REG);    // Read the Tx abort source register
//        GetWord16(I2C_CLR_TX_ABRT_REG);                         // Clear the Tx abort flag
//    } while( (abort_SR_Status & ABRT_7B_ADDR_NOACK) != 0 );     // Repeat if not ACK    
//}


///**
// ********************************************************************m********************
// * @brief Send I2C EEPROM's memory address
// *
// * @param[in] address of the I2C EEPROM memory
// ****************************************************************************************
// */
//void i2c_send_address(uint32_t address_to_send)
//{
//    switch(mem_address_size)
//    {
//        case I2C_2BYTES_ADDR:
//            SetWord16(I2C_ENABLE_REG, 0x0);  
//            SetWord16(I2C_TAR_REG, i2c_dev_address | ((address_to_send & 0x10000) >> 16));  // Set Slave device address
//            SetWord16(I2C_ENABLE_REG, 0x1);
//            while( (GetWord16(I2C_STATUS_REG) & 0x20) != 0 );   // Wait for I2C master FSM to be IDLE
//            SEND_I2C_COMMAND( (address_to_send >> 8) & 0xFF );  // Set address MSB, write access        
//        break;
//        
//        case I2C_3BYTES_ADDR:
//            SEND_I2C_COMMAND( (address_to_send >> 16) & 0xFF);  // Set address MSB, write access
//            SEND_I2C_COMMAND( (address_to_send >> 8) & 0xFF);   // Set address MSB, write access        
//        break;
//    }

//    SEND_I2C_COMMAND(address_to_send & 0xFF);                   // Set address LSB, write access    
//}



///**
// ****************************************************************************************
// * @brief Read single byte from I2C EEPROM.
// *
// * @param[in] Memory address to read the byte from.
// *
// * @return Read byte.
// ****************************************************************************************
// */
//uint8_t i2c_led_read_byte(uint32_t address)
//{
//    i2c_wait_until_eeprom_ready();
//    i2c_send_address(address);  
//    
//    WAIT_WHILE_I2C_FIFO_IS_FULL();                  // Wait if Tx FIFO is full
//    SEND_I2C_COMMAND(0x0100);                       // Set R/W bit to 1 (read access)
//    WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until I2C Tx FIFO empty
//    WAIT_FOR_RECEIVED_BYTE();                       // Wait for received data
//    return (0xFF & GetWord16(I2C_DATA_CMD_REG));    // Get received byte
//}



///**
// ****************************************************************************************
// * @brief Read single series of bytes from I2C EEPROM (for driver's internal use)
// *
// * @param[in] p    Memory address to read the series of bytes from (all in the same page)
// * @param[in] size count of bytes to read (must not cross page)
// ****************************************************************************************
// */
//static void read_data_single(uint8_t **p, uint32_t address, uint32_t size)
//{
//    int j;
//    
//    i2c_send_address(address);
//                
//    for (j = 0; j < size; j++)
//    {    
//        WAIT_WHILE_I2C_FIFO_IS_FULL();              // Wait if Tx FIFO is full
//        SEND_I2C_COMMAND(0x0100);                   // Set read access for <size> times
//    }
//    
//    // Critical section
//    GLOBAL_INT_DISABLE();
//    
//    // Get the received data
//    for (j = 0; j < size; j++)                                         
//    {
//        WAIT_FOR_RECEIVED_BYTE();                   // Wait for received data
//        **p =(0xFF & GetWord16(I2C_DATA_CMD_REG));  // Get the received byte
//        (*p)++;
//    }
//    // End of critical section
//    GLOBAL_INT_RESTORE();
//}



///**
// ****************************************************************************************
// * @brief Reads data from I2C EEPROM to memory position of given pointer.
// *
// * @param[in] rd_data_ptr     Read data pointer.
// * @param[in] address         Starting memory address.
// * @param[in] size            Size of the data to be read.
// *
// * @return Bytes that were actually read (due to memory size limitation).
// ****************************************************************************************
// */
//uint32_t i2c_led_read_data(uint8_t *rd_data_ptr, uint32_t address, uint32_t size)
//{
//    uint32_t tmp_size;
//    uint32_t bytes_read; 

//    if (size == 0)
//        return 0;
//    
//    if (size > i2c_led_SIZE - address)           // Check for max bytes to be read from a (MAX_SIZE x 8) EEPROM
//    {
//        tmp_size = i2c_led_SIZE - address;
//        bytes_read = tmp_size;
//    }
//    else
//    {
//        tmp_size = size;
//        bytes_read = size;
//    }

//    i2c_wait_until_eeprom_ready();

//    // Read 32 bytes at a time
//    while (tmp_size >= 32)
//    {
//        read_data_single(&rd_data_ptr, address, 32);
//            
//        address += 32;                              // Update base address for read
//        tmp_size -= 32;                             // Update tmp_size for bytes remaining to be read
//    }

//    if (tmp_size)
//        read_data_single(&rd_data_ptr, address, tmp_size);

//    return bytes_read;
//}

///**
// ****************************************************************************************
// * @brief Write single byte to I2C EEPROM.
// *
// * @param[in] address     Memory position to write the byte to.
// * @param[in] wr_data     Byte to be written.
// ****************************************************************************************
// */
//void i2c_led_write_byte(uint32_t address, uint8_t wr_data)
//{
//    i2c_wait_until_eeprom_ready();
//    i2c_send_address(address);
//        
//    WAIT_WHILE_I2C_FIFO_IS_FULL();                  // Wait if I2C Tx FIFO is full
//    SEND_I2C_COMMAND(wr_data & 0xFF);               // Send write data
//    WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
//    
//    WAIT_UNTIL_NO_MASTER_ACTIVITY();                // wait until no master activity 
//}


///**
// ****************************************************************************************
// * @brief Writes page to I2C EEPROM.
// *
// * @param[in] address         Starting address of memory page.
// * @param[in] wr_data_ptr     Pointer to the first of bytes to be written.
// * @param[in] size            Size of the data to be written (MUST BE LESS OR EQUAL TO i2c_led_PAGE).
// *
// * @return                    Count of bytes that were actually written
// ****************************************************************************************
// */
//uint16_t i2c_led_write_page(uint8_t *wr_data_ptr, uint32_t address, uint16_t size)
//{
//	uint16_t feasible_size;
//    uint16_t bytes_written = 0;

//    if (address < i2c_led_SIZE)
//    {
//        // max possible write size without crossing page boundary
//        feasible_size = i2c_led_PAGE - (address % i2c_led_PAGE);
//        
//        if (size < feasible_size)                                                                    
//            feasible_size = size;                   // adjust limit accordingly
//        
//        i2c_wait_until_eeprom_ready();
//        
//        // Critical section
//        GLOBAL_INT_DISABLE();
//        
//        i2c_send_address(address);
//        
//        do
//        {
//            WAIT_WHILE_I2C_FIFO_IS_FULL();          // Wait if I2c Tx FIFO is full
//            SEND_I2C_COMMAND(*wr_data_ptr & 0xFF);  // Send write data
//            wr_data_ptr++;
//            feasible_size--;
//            bytes_written++;
//        } 
//        while (feasible_size != 0);

//        // End of critical section
//        GLOBAL_INT_RESTORE();
//        
//        WAIT_UNTIL_I2C_FIFO_IS_EMPTY();             // Wait until Tx FIFO is empty
//        WAIT_UNTIL_NO_MASTER_ACTIVITY();            // Wait until no master activity
//    }
//    
//    return bytes_written;
//}

///**
// ****************************************************************************************
// * @brief Writes data to I2C EEPROM.
// *
// * @param[in] address         Starting address of the write process.
// * @param[in] wr_data_ptr     Pointer to the first of bytes to be written.
// * @param[in] size            Size of the data to be written.
// *
// * @return Bytes that were actually written (due to memory size limitation).
// ****************************************************************************************
// */
//uint32_t i2c_led_write_data(uint8_t *wr_data_ptr, uint32_t address, uint32_t size)
//{
//	uint32_t bytes_written; 
//	uint32_t feasible_size = size;
//    uint32_t bytes_left_to_send;  

//    if (address >= i2c_led_SIZE)                 // address is not it the EEPROM address space    
//        return 0;
//   
//	// limit to the maximum count of bytes that can be written to an (i2c_led_SIZE x 8) EEPROM
//	if (size > i2c_led_SIZE - address)
//        feasible_size = i2c_led_SIZE - address;
// 
//    bytes_left_to_send = feasible_size;
//    bytes_written = 0;
//    
//    while (bytes_left_to_send)
//    {
//		uint16_t cnt = i2c_led_write_page(wr_data_ptr + bytes_written, address + bytes_written, bytes_left_to_send);
//        bytes_written += cnt;  
//        bytes_left_to_send -= cnt;
//    }
//    
//    return bytes_written;
//}

void vmda1458x_i2c_led_init(uint16_t dev_address, uint8_t speed)
{
    
		uint32_t i;

		GPIO_SetInactive(LED_RESET_PORT, LED_RESET_PIN);
		for(i = 0; i < 10000; i++);
		GPIO_SetActive(LED_RESET_PORT, LED_RESET_PIN);
		for(i = 0; i < 1000; i++);
//		GPIO_SetInactive(LED_RESET_PORT, LED_RESET_PIN);
	
		SetBits16(CLK_PER_REG, I2C_ENABLE, 1);                                        // enable  clock for I2C 
    SetWord16(I2C_ENABLE_REG, 0x0);                                               // Disable the I2C controller	
    SetWord16(I2C_CON_REG, I2C_MASTER_MODE | I2C_SLAVE_DISABLE | I2C_RESTART_EN); // Slave is disabled
    SetBits16(I2C_CON_REG, I2C_SPEED, speed);                                     // Set speed
    SetBits16(I2C_CON_REG, I2C_10BITADDR_MASTER, I2C_ADDRESS_MODE);               // Set addressing mode  MYC 7bit for LED 
    SetWord16(I2C_TAR_REG, dev_address & 0x3FF);                                  // Set Slave device address
    SetWord16(I2C_ENABLE_REG, 0x1);                                               // Enable the I2C controller
    while( (GetWord16(I2C_STATUS_REG) & 0x20) != 0 );                             // Wait for I2C master FSM to be IDLE
    i2c_dev_address = dev_address;
	
		GLOBAL_INT_DISABLE();
		
		/* START TO PROGRAM BLINK SPEED REGISTER */
		/* COMTROL REGSTERS */
		/*
		#define		PCA9551_CTL_REG_INPUT				0x00
		#define		PCA9551_CTL_REG_PSC0				0x01
		#define		PCA9551_CTL_REG_PWM0				0x02
		#define		PCA9551_CTL_REG_PSC1				0x03
		#define		PCA9551_CTL_REG_PWM1				0x04
		#define		PCA9551_CTL_REG_LS0					0x05
		#define		PCA9551_CTL_REG_LS1					0x06
		*/
		
		/* !!!!!!! REGISTER 1 !!!!!!! */
		/* #define		PCA9551_CTL_REG_PSC0				0x01 */
		SetWord16(I2C_ENABLE_REG, 0x0);  
		SetWord16(I2C_TAR_REG, i2c_dev_address);  // Set Slave device address
		SetWord16(I2C_ENABLE_REG, 0x1);
		while( (GetWord16(I2C_STATUS_REG) & 0x20) != 0 );   // Wait for I2C master FSM to be IDLE
		
		SEND_I2C_COMMAND(PCA9551_CTL_REG_PSC0);  // Set address MSB, write access
		WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
		
		SEND_I2C_COMMAND(LED_BLINK_ONE_FOUTH_HZ_PSC);   // Set address MSB, write access  
		WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
		
		/* !!!!!!! REGISTER 2 !!!!!!! */
		/* #define		PCA9551_CTL_REG_PWM0				0x02 */
		SetWord16(I2C_ENABLE_REG, 0x0);  
		SetWord16(I2C_TAR_REG, i2c_dev_address);  // Set Slave device address
		SetWord16(I2C_ENABLE_REG, 0x1);
		while( (GetWord16(I2C_STATUS_REG) & 0x20) != 0 );   // Wait for I2C master FSM to be IDLE
		
		SEND_I2C_COMMAND(PCA9551_CTL_REG_PWM0);  // Set address MSB, write access
		WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
		
		SEND_I2C_COMMAND(LED_BLINK_DUTYCYCLE_50_PWM);   // Set address MSB, write access  
		WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
		
		/* !!!!!!! REGISTER 3 !!!!!!! */
		/* #define		PCA9551_CTL_REG_PSC1				0x03 */
		SetWord16(I2C_ENABLE_REG, 0x0);  
		SetWord16(I2C_TAR_REG, i2c_dev_address);  // Set Slave device address
		SetWord16(I2C_ENABLE_REG, 0x1);
		while( (GetWord16(I2C_STATUS_REG) & 0x20) != 0 );   // Wait for I2C master FSM to be IDLE
		
		SEND_I2C_COMMAND(PCA9551_CTL_REG_PSC1);  // Set address MSB, write access
		WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
		
		SEND_I2C_COMMAND(LED_BLINK_HALF_HZ_PSC);   // Set address MSB, write access  
		WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
		
		/* !!!!!!! REGISTER 4 !!!!!!! */
		/* #define		PCA9551_CTL_REG_PWM1				0x04 */
		SetWord16(I2C_ENABLE_REG, 0x0);  
		SetWord16(I2C_TAR_REG, i2c_dev_address);  // Set Slave device address
		SetWord16(I2C_ENABLE_REG, 0x1);
		while( (GetWord16(I2C_STATUS_REG) & 0x20) != 0 );   // Wait for I2C master FSM to be IDLE
		
	  SEND_I2C_COMMAND(PCA9551_CTL_REG_PWM1);  // Set address MSB, write access
		WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
		
		SEND_I2C_COMMAND(LED_BLINK_DUTYCYCLE_50_PWM);   // Set address MSB, write access  
		WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
		
		/* END TO PROGRAM BLINK SPEED REGISTER */
		
		
		WAIT_UNTIL_NO_MASTER_ACTIVITY();                // wait until no master activity 	
		
		/* record the status for whether the I2C for LED has been initialized */
		led_init_or_not = true;

		GLOBAL_INT_RESTORE();

 }

void vmda1458x_led_set(uint8_t ledId, uint8_t mode)
{
		////MYC
		//#define 	LED_ID_0										0
		// ......................................
		//#define 	LED_ID_6										6
		//#define 	LED_ID_7										7

		////MYC
		//#define 	LED_MODE_ON									0
		//#define 	LED_MODE_OFF								1
		//#define 	LED_MODE_BLINK_LOW_SPEED		2
		//#define		LED_MODE_BLINK_HIGH_SPEED		3

		uint32_t i;
		GLOBAL_INT_DISABLE();
		SetWord16(I2C_ENABLE_REG, 0x0);  
		SetWord16(I2C_TAR_REG, i2c_dev_address);  // Set Slave device address
		SetWord16(I2C_ENABLE_REG, 0x1);
		while( (GetWord16(I2C_STATUS_REG) & 0x20) != 0 );   // Wait for I2C master FSM to be IDLE
		
		if((ledId <= LED_ID_3))
		{
				pca9551_ls0 = pca9551_ls0 & (~(3 << ( ledId << 1 ) ) );
				pca9551_ls0 = pca9551_ls0 | (mode << ( ledId << 1 ) );
			
				SEND_I2C_COMMAND(0x05);  // Set address MSB, write access
				WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty

				for(i = 0; i < 10; i++);
				SEND_I2C_COMMAND(pca9551_ls0);  // Set address MSB, write access
				WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty

		}			
		else if ( (ledId >= LED_ID_4 ) && (ledId <= LED_ID_7) )
		{
			
				pca9551_ls1 = pca9551_ls1 & (~(3 << ( (ledId-4) << 1 ) ) );
				pca9551_ls1 = pca9551_ls1 | (mode << ( (ledId-4) << 1 ) );
			
				SEND_I2C_COMMAND(0x06);  // Set address MSB, write access
				WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
			
				for(i = 0; i < 10; i++);
				SEND_I2C_COMMAND( pca9551_ls1 );  // Set address MSB, write access
				WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
		}
		else
		{
				return;
		}
		
		GLOBAL_INT_RESTORE();
		
		return;
}

void vmda1458x_led_on(uint8_t ledId)
{
		if( (false == led_init_or_not) || (ledId > LED_ID_7) )
		{				
				return;
		}
		
		vmda1458x_led_set(ledId, LED_MODE_ON);	
}

void vmda1458x_led_off(uint8_t ledId)
{
		if( (false == led_init_or_not) || (ledId > LED_ID_7) )
		{				
				return;
		}
		
		vmda1458x_led_set(ledId, LED_MODE_OFF);
	
}

void vmda1458x_led_blink_once_off_on(uint8_t ledId)
{
		uint32_t i;
		if( (false == led_init_or_not) || (ledId > LED_ID_7) )
		{				
				return;
		}
		
		vmda1458x_led_set(ledId, LED_MODE_OFF);
		
		for(i = 0; i < LED_BLINK_ONCE_DELAY_LOOP; i++);
		
		vmda1458x_led_set(ledId, LED_MODE_ON);
	
}

void vmda1458x_led_blink_once_on_off(uint8_t ledId)
{
		uint32_t i;
		if( (false == led_init_or_not) || (ledId > LED_ID_7) )
		{				
				return;
		}
		
		vmda1458x_led_set(ledId, LED_MODE_ON);
		
		for(i = 0; i < LED_BLINK_ONCE_DELAY_LOOP; i++);
		
		vmda1458x_led_set(ledId, LED_MODE_OFF);
	
}

void vmda14568x_led_blinking_high_speed(uint8_t ledId)
{
		if( (false == led_init_or_not) || (ledId > LED_ID_7) )
		{				
				return;
		}
		
		vmda1458x_led_set(ledId, LED_MODE_BLINK_HIGH_SPEED);
	
}

void vmda1458x_led_blinking_low_speed(uint8_t ledId)
{
		if( (false == led_init_or_not) || (ledId > LED_ID_7) )
		{				
				return;
		}
		
		vmda1458x_led_set(ledId, LED_MODE_BLINK_LOW_SPEED);	
	
}

//Change for VM system usage
void ihu_i2c_led_init(uint16_t dev_address, uint8_t speed)
{
	vmda1458x_i2c_led_init(dev_address, speed);
}

void ihu_led_set(uint8_t ledId, uint8_t mode)
{
	vmda1458x_led_set(ledId, mode);
}

//由于不输出差错，故而稍微有点怪异
void ihu_led_blink_enable(uint8_t ledId, uint8_t mode)
{
	if ((mode <=IHU_LED_BLINK_MODE_MIN) || (mode >=IHU_LED_BLINK_MODE_MAX)){
		return;
	}
	
	switch(mode)
	{
		case IHU_LED_BLINK_MODE_ALWAYS_ON:
			vmda1458x_led_on(ledId);
			break;
		
		case IHU_LED_BLINK_MODE_ALWAYS_OFF:
			vmda1458x_led_off(ledId);
			break;
		
		case IHU_LED_BLINK_MODE_ONCE_OFF_ON:
			vmda1458x_led_blink_once_off_on(ledId);
			break;
		
		case IHU_LED_BLINK_MODE_ONCE_ON_OFF:
			vmda1458x_led_blink_once_on_off(ledId);
			break;
		
		case IHU_LED_BLINK_MODE_HIGH_SPEED:
			vmda14568x_led_blinking_high_speed(ledId);
			break;
		
		case IHU_LED_BLINK_MODE_LOW_SPEED:
			vmda1458x_led_blinking_low_speed(ledId);
			break;
		
		default:
			break;
	}
	return;
}
