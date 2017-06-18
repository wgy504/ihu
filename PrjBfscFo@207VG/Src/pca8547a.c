#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_i2c.h"

#include "pca8547a.h"

#define PCA8574A_I2C_HANDLE (&hi2c2)
#define PCA8547A_I2C_TIMEOUT (100)
#define PCA8574A_DEVADR (0x38<<1)

extern I2C_HandleTypeDef hi2c2;

void PCA8574A_init(void)
{
	//PCA8574A_write(0);
}


int32_t PCA8574A_write(uint8_t val)
{
	HAL_StatusTypeDef status;
	
	__disable_irq();
	status = HAL_I2C_Master_Transmit(PCA8574A_I2C_HANDLE, PCA8574A_DEVADR, &val, 1, PCA8547A_I2C_TIMEOUT);
  __enable_irq();
	
	return status;
}


int32_t PCA8574A_read(uint8_t *val)
{
	HAL_StatusTypeDef status;
  
	__disable_irq();
	status = HAL_I2C_Master_Receive(PCA8574A_I2C_HANDLE, PCA8574A_DEVADR, val, 1, PCA8547A_I2C_TIMEOUT);
	__enable_irq();
	
	return status;
}

