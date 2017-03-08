/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
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

#define CUBEMX_PIN_F2_GPIO_DI_DHT11_Pin GPIO_PIN_2
#define CUBEMX_PIN_F2_GPIO_DI_DHT11_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_GPIO_DO_LED_POWER_Pin GPIO_PIN_3
#define CUBEMX_PIN_F2_GPIO_DO_LED_POWER_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_DCMI_D4_Pin GPIO_PIN_4
#define CUBEMX_PIN_F2_DCMI_D4_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_DCMI_D6_Pin GPIO_PIN_5
#define CUBEMX_PIN_F2_DCMI_D6_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_DCMI_D7_Pin GPIO_PIN_6
#define CUBEMX_PIN_F2_DCMI_D7_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_GPIO_DI_FALL_SNESOR_Pin GPIO_PIN_13
#define CUBEMX_PIN_F2_GPIO_DI_FALL_SNESOR_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_RCC_OS32_IN_Pin GPIO_PIN_14
#define CUBEMX_PIN_F2_RCC_OS32_IN_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_RCC_OSC32_OUT_Pin GPIO_PIN_15
#define CUBEMX_PIN_F2_RCC_OSC32_OUT_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_RCC_OSC_IN_Pin GPIO_PIN_0
#define CUBEMX_PIN_F2_RCC_OSC_IN_GPIO_Port GPIOH
#define CUBEMX_PIN_F2_RCC_OSC_OUT_Pin GPIO_PIN_1
#define CUBEMX_PIN_F2_RCC_OSC_OUT_GPIO_Port GPIOH
#define CUBEMX_PIN_F2_ADC1_BATTERY_SNESOR_Pin GPIO_PIN_0
#define CUBEMX_PIN_F2_ADC1_BATTERY_SNESOR_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_GPIO_LED_207VC_TEST_Pin GPIO_PIN_2
#define CUBEMX_PIN_F2_GPIO_LED_207VC_TEST_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_SPI2_MOSI_Pin GPIO_PIN_3
#define CUBEMX_PIN_F2_SPI2_MOSI_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_SYS_WKUP_Pin GPIO_PIN_0
#define CUBEMX_PIN_F2_SYS_WKUP_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_USART2_TX_RFID_Pin GPIO_PIN_2
#define CUBEMX_PIN_F2_USART2_TX_RFID_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_USART2_RX_RFID_Pin GPIO_PIN_3
#define CUBEMX_PIN_F2_USART2_RX_RFID_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_GPIO_DO_LED_WORK_STATE_Pin GPIO_PIN_4
#define CUBEMX_PIN_F2_GPIO_DO_LED_WORK_STATE_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_GPIO_DO_LED_COMMU_Pin GPIO_PIN_5
#define CUBEMX_PIN_F2_GPIO_DO_LED_COMMU_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_DCMI_PIXCK_Pin GPIO_PIN_6
#define CUBEMX_PIN_F2_DCMI_PIXCK_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI1_TRIGGER_Pin GPIO_PIN_4
#define CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI1_TRIGGER_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_GPIO_DI_DOOR1_RESTRICTION_Pin GPIO_PIN_5
#define CUBEMX_PIN_F2_GPIO_DI_DOOR1_RESTRICTION_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI2_TONGUE_Pin GPIO_PIN_0
#define CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI2_TONGUE_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_GPIO_DO_LOCK1_DO1_Pin GPIO_PIN_1
#define CUBEMX_PIN_F2_GPIO_DO_LOCK1_DO1_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_GPIO_DI_DOOR3_RESTRICTION_Pin GPIO_PIN_7
#define CUBEMX_PIN_F2_GPIO_DI_DOOR3_RESTRICTION_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_GPIO_DO_LOCK3_DO1_Pin GPIO_PIN_8
#define CUBEMX_PIN_F2_GPIO_DO_LOCK3_DO1_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_GPIO_DI_LOCK3_DI2_TONGUE_Pin GPIO_PIN_9
#define CUBEMX_PIN_F2_GPIO_DI_LOCK3_DI2_TONGUE_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_GPIO_DI_LOCK3_DI1_TRIGGER_Pin GPIO_PIN_10
#define CUBEMX_PIN_F2_GPIO_DI_LOCK3_DI1_TRIGGER_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_GPIO_DI_DOOR2_RESTRICTION_Pin GPIO_PIN_11
#define CUBEMX_PIN_F2_GPIO_DI_DOOR2_RESTRICTION_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_GPIO_DO_LOCK2_DO1_Pin GPIO_PIN_12
#define CUBEMX_PIN_F2_GPIO_DO_LOCK2_DO1_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_GPIO_DI_LOCK2_DI2_TONGUE_Pin GPIO_PIN_13
#define CUBEMX_PIN_F2_GPIO_DI_LOCK2_DI2_TONGUE_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_GPIO_DI_LOCK2_DI1_TRIGGER_Pin GPIO_PIN_14
#define CUBEMX_PIN_F2_GPIO_DI_LOCK2_DI1_TRIGGER_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_GPIO_DI_SHAKE_SENSOR_Pin GPIO_PIN_15
#define CUBEMX_PIN_F2_GPIO_DI_SHAKE_SENSOR_GPIO_Port GPIOE
#define CUBEMX_PIN_F2_SPI2_SCK_Pin GPIO_PIN_10
#define CUBEMX_PIN_F2_SPI2_SCK_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_USART3_RX_PRINT_Pin GPIO_PIN_11
#define CUBEMX_PIN_F2_USART3_RX_PRINT_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_SPI2_CS_Pin GPIO_PIN_12
#define CUBEMX_PIN_F2_SPI2_CS_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_SPI2_MISO_Pin GPIO_PIN_14
#define CUBEMX_PIN_F2_SPI2_MISO_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_USART3_TX_PRINT_Pin GPIO_PIN_8
#define CUBEMX_PIN_F2_USART3_TX_PRINT_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_SPI2_GPIO_RST_Pin GPIO_PIN_9
#define CUBEMX_PIN_F2_SPI2_GPIO_RST_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_GPIO_DI_DOOR4_RESTRICTION_Pin GPIO_PIN_10
#define CUBEMX_PIN_F2_GPIO_DI_DOOR4_RESTRICTION_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_GPIO_DO_LOCK4_DO1_Pin GPIO_PIN_11
#define CUBEMX_PIN_F2_GPIO_DO_LOCK4_DO1_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_GPIO_DI_LOCK4_DI2_TONGUE_Pin GPIO_PIN_12
#define CUBEMX_PIN_F2_GPIO_DI_LOCK4_DI2_TONGUE_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_GPIO_DI_LOCK4_DI1_TRIGGER_Pin GPIO_PIN_13
#define CUBEMX_PIN_F2_GPIO_DI_LOCK4_DI1_TRIGGER_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_GPIO_DO_BEEP_Pin GPIO_PIN_14
#define CUBEMX_PIN_F2_GPIO_DO_BEEP_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_GPIO_DI_SMOKE_SENSOR_Pin GPIO_PIN_15
#define CUBEMX_PIN_F2_GPIO_DI_SMOKE_SENSOR_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_DCMI_D0_Pin GPIO_PIN_6
#define CUBEMX_PIN_F2_DCMI_D0_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_USART1_RX_GPRS_Pin GPIO_PIN_7
#define CUBEMX_PIN_F2_USART1_RX_GPRS_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_DCMI_D2_Pin GPIO_PIN_8
#define CUBEMX_PIN_F2_DCMI_D2_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_DCMI_D3_Pin GPIO_PIN_9
#define CUBEMX_PIN_F2_DCMI_D3_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_RCC_MCO_1_Pin GPIO_PIN_8
#define CUBEMX_PIN_F2_RCC_MCO_1_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_USART1_TX_GPRS_Pin GPIO_PIN_9
#define CUBEMX_PIN_F2_USART1_TX_GPRS_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_USART1_RX_GPRSA10_Pin GPIO_PIN_10
#define CUBEMX_PIN_F2_USART1_RX_GPRSA10_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_SYS_JTMS_SWIDO_Pin GPIO_PIN_13
#define CUBEMX_PIN_F2_SYS_JTMS_SWIDO_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_SYS_JTCK_SWCLK_Pin GPIO_PIN_14
#define CUBEMX_PIN_F2_SYS_JTCK_SWCLK_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_SYS_JDTI_Pin GPIO_PIN_15
#define CUBEMX_PIN_F2_SYS_JDTI_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_UART4_TX_BLE_Pin GPIO_PIN_10
#define CUBEMX_PIN_F2_UART4_TX_BLE_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_UART4_RX_BLE_Pin GPIO_PIN_11
#define CUBEMX_PIN_F2_UART4_RX_BLE_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_GPIO_DI_WATER_SENSOR_Pin GPIO_PIN_0
#define CUBEMX_PIN_F2_GPIO_DI_WATER_SENSOR_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_DO_BLE_ATCMD_Pin GPIO_PIN_3
#define CUBEMX_PIN_F2_DO_BLE_ATCMD_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_DO_SENSOR_PWR_Pin GPIO_PIN_4
#define CUBEMX_PIN_F2_DO_SENSOR_PWR_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_DO_RFID_PWR_Pin GPIO_PIN_5
#define CUBEMX_PIN_F2_DO_RFID_PWR_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_DO_BLE_PWR_Pin GPIO_PIN_6
#define CUBEMX_PIN_F2_DO_BLE_PWR_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_DO_GPRSMOD_PWR_Pin GPIO_PIN_7
#define CUBEMX_PIN_F2_DO_GPRSMOD_PWR_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_SYS_JTDO_SWO_Pin GPIO_PIN_3
#define CUBEMX_PIN_F2_SYS_JTDO_SWO_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_SYS_JTRST_Pin GPIO_PIN_4
#define CUBEMX_PIN_F2_SYS_JTRST_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_DCMI_D5_Pin GPIO_PIN_6
#define CUBEMX_PIN_F2_DCMI_D5_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_I2C_SCL_Pin GPIO_PIN_8
#define CUBEMX_PIN_F2_I2C_SCL_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_I2C_SA_Pin GPIO_PIN_9
#define CUBEMX_PIN_F2_I2C_SA_GPIO_Port GPIOB
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
