/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  *
  * Copyright (c) 2016 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define GPIO_DI_AXIS_3D_SNESOR_Pin GPIO_PIN_13
#define GPIO_DI_AXIS_3D_SNESOR_GPIO_Port GPIOC
#define GPIO_DI_LOCK_RESV2_Pin GPIO_PIN_1
#define GPIO_DI_LOCK_RESV2_GPIO_Port GPIOH
#define GPIO_DO_LOCK_DO1_Pin GPIO_PIN_2
#define GPIO_DO_LOCK_DO1_GPIO_Port GPIOC
#define GPIO_DI_TEMPATURE_SENSOR_Pin GPIO_PIN_3
#define GPIO_DI_TEMPATURE_SENSOR_GPIO_Port GPIOC
#define ADC1_IN1_TEST1_Pin GPIO_PIN_1
#define ADC1_IN1_TEST1_GPIO_Port GPIOA
#define USART2_TX_RFID_Pin GPIO_PIN_2
#define USART2_TX_RFID_GPIO_Port GPIOA
#define USART2_RX_RFID_Pin GPIO_PIN_3
#define USART2_RX_RFID_GPIO_Port GPIOA
#define SPI1_CS_FLASH_Pin GPIO_PIN_4
#define SPI1_CS_FLASH_GPIO_Port GPIOA
#define SPI1_SCK_FLASH_Pin GPIO_PIN_5
#define SPI1_SCK_FLASH_GPIO_Port GPIOA
#define SPI1_MISO_FLASH_Pin GPIO_PIN_6
#define SPI1_MISO_FLASH_GPIO_Port GPIOA
#define SPI1_MOSI_FLASH_Pin GPIO_PIN_7
#define SPI1_MOSI_FLASH_GPIO_Port GPIOA
#define GPIO_DI_HUMIDITY_SENSOR_Pin GPIO_PIN_4
#define GPIO_DI_HUMIDITY_SENSOR_GPIO_Port GPIOC
#define GPIO_DI_DOOR_RESTRICTION_Pin GPIO_PIN_5
#define GPIO_DI_DOOR_RESTRICTION_GPIO_Port GPIOC
#define GPIO_DI_LOCK_DI2_STATUS_Pin GPIO_PIN_0
#define GPIO_DI_LOCK_DI2_STATUS_GPIO_Port GPIOB
#define GPIO_DI_LOCK_DI1_TRIGGER_Pin GPIO_PIN_1
#define GPIO_DI_LOCK_DI1_TRIGGER_GPIO_Port GPIOB
#define GPIO_DI_VIBRATION_SENSOR_Pin GPIO_PIN_2
#define GPIO_DI_VIBRATION_SENSOR_GPIO_Port GPIOB
#define USART3_TX_PRINT_Pin GPIO_PIN_10
#define USART3_TX_PRINT_GPIO_Port GPIOB
#define USART3_RX_PRINT_Pin GPIO_PIN_11
#define USART3_RX_PRINT_GPIO_Port GPIOB
#define SPI2_SCK_IAU_Pin GPIO_PIN_13
#define SPI2_SCK_IAU_GPIO_Port GPIOB
#define SPI2_MISO_IAU_Pin GPIO_PIN_14
#define SPI2_MISO_IAU_GPIO_Port GPIOB
#define SPI2_MOSI_IAU_Pin GPIO_PIN_15
#define SPI2_MOSI_IAU_GPIO_Port GPIOB
#define GPIO_DI_SMOKE_SENSOR_Pin GPIO_PIN_8
#define GPIO_DI_SMOKE_SENSOR_GPIO_Port GPIOC
#define GPIO_DI_LOCK_RESV1_Pin GPIO_PIN_9
#define GPIO_DI_LOCK_RESV1_GPIO_Port GPIOC
#define GPIO_DI_WATER_SENSOR_Pin GPIO_PIN_8
#define GPIO_DI_WATER_SENSOR_GPIO_Port GPIOA
#define USART1_TX_GPRS_Pin GPIO_PIN_9
#define USART1_TX_GPRS_GPIO_Port GPIOA
#define USART1_RX_GPRS_Pin GPIO_PIN_10
#define USART1_RX_GPRS_GPIO_Port GPIOA
#define UART4_TX_BLE_Pin GPIO_PIN_10
#define UART4_TX_BLE_GPIO_Port GPIOC
#define UART4_RX_BLE_Pin GPIO_PIN_11
#define UART4_RX_BLE_GPIO_Port GPIOC
#define UART5_TX_SPARE1_Pin GPIO_PIN_12
#define UART5_TX_SPARE1_GPIO_Port GPIOC
#define UART5_RX_SPARE1_Pin GPIO_PIN_2
#define UART5_RX_SPARE1_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
