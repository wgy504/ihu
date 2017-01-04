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

#define CUBEMX_PIN_F2_RCC_OSC32_IN_Pin GPIO_PIN_14
#define CUBEMX_PIN_F2_RCC_OSC32_IN_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_RCC_OSC32_OUT_Pin GPIO_PIN_15
#define CUBEMX_PIN_F2_RCC_OSC32_OUT_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_STS_WKUP_Pin GPIO_PIN_0
#define CUBEMX_PIN_F2_STS_WKUP_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_ADC1_TEST1_Pin GPIO_PIN_1
#define CUBEMX_PIN_F2_ADC1_TEST1_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_USART2_TX_485_Pin GPIO_PIN_2
#define CUBEMX_PIN_F2_USART2_TX_485_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_USART2_RX_485_Pin GPIO_PIN_3
#define CUBEMX_PIN_F2_USART2_RX_485_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_MOTO_CTRL_Pin GPIO_PIN_4
#define CUBEMX_PIN_F2_MOTO_CTRL_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_BOOT1_Pin GPIO_PIN_2
#define CUBEMX_PIN_F2_BOOT1_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_SPI2_CS_ADC_Pin GPIO_PIN_12
#define CUBEMX_PIN_F2_SPI2_CS_ADC_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_SPI2_SCK_ADC_Pin GPIO_PIN_13
#define CUBEMX_PIN_F2_SPI2_SCK_ADC_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_SPI2_MISO_ADC_Pin GPIO_PIN_14
#define CUBEMX_PIN_F2_SPI2_MISO_ADC_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_SPI2_MOSI_ADC_Pin GPIO_PIN_15
#define CUBEMX_PIN_F2_SPI2_MOSI_ADC_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_USART3_TX_PRINT_Pin GPIO_PIN_8
#define CUBEMX_PIN_F2_USART3_TX_PRINT_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_USART3_RX_PRINT_Pin GPIO_PIN_9
#define CUBEMX_PIN_F2_USART3_RX_PRINT_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_USART2_485_CTRL_Pin GPIO_PIN_12
#define CUBEMX_PIN_F2_USART2_485_CTRL_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_GPIO_DO_BEEP_Pin GPIO_PIN_13
#define CUBEMX_PIN_F2_GPIO_DO_BEEP_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_GPIO_DO_LED_SERV3_Pin GPIO_PIN_14
#define CUBEMX_PIN_F2_GPIO_DO_LED_SERV3_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_GPIO_DO_LED_SERV2_Pin GPIO_PIN_15
#define CUBEMX_PIN_F2_GPIO_DO_LED_SERV2_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_GPIO_DO_LED_SERV1_Pin GPIO_PIN_6
#define CUBEMX_PIN_F2_GPIO_DO_LED_SERV1_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_GPIO_DO_LED_WORK_STATE_Pin GPIO_PIN_7
#define CUBEMX_PIN_F2_GPIO_DO_LED_WORK_STATE_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_GPIO_DO_LED_COMMU_Pin GPIO_PIN_8
#define CUBEMX_PIN_F2_GPIO_DO_LED_COMMU_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_GPIO_DO_LED_POWER_Pin GPIO_PIN_9
#define CUBEMX_PIN_F2_GPIO_DO_LED_POWER_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_ADC_CLK_Pin GPIO_PIN_8
#define CUBEMX_PIN_F2_ADC_CLK_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_USART1_TX_DEBUG_Pin GPIO_PIN_9
#define CUBEMX_PIN_F2_USART1_TX_DEBUG_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_USART1_RX_DEBUG_Pin GPIO_PIN_10
#define CUBEMX_PIN_F2_USART1_RX_DEBUG_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_SYS_JTMS_SWDIO_Pin GPIO_PIN_13
#define CUBEMX_PIN_F2_SYS_JTMS_SWDIO_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_SYS_JTCK_SWCLK_Pin GPIO_PIN_14
#define CUBEMX_PIN_F2_SYS_JTCK_SWCLK_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_SYS_JTDI_Pin GPIO_PIN_15
#define CUBEMX_PIN_F2_SYS_JTDI_GPIO_Port GPIOA
#define CUBEMX_PIN_F2_UART4_TX_BLE_Pin GPIO_PIN_10
#define CUBEMX_PIN_F2_UART4_TX_BLE_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_UART4_RX_BLE_Pin GPIO_PIN_11
#define CUBEMX_PIN_F2_UART4_RX_BLE_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_UART5_TX_SPARE1_Pin GPIO_PIN_12
#define CUBEMX_PIN_F2_UART5_TX_SPARE1_GPIO_Port GPIOC
#define CUBEMX_PIN_F2_CAN1_RX_Pin GPIO_PIN_0
#define CUBEMX_PIN_F2_CAN1_RX_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_CAN1_TX_Pin GPIO_PIN_1
#define CUBEMX_PIN_F2_CAN1_TX_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_UART5_RX_SPARE1_Pin GPIO_PIN_2
#define CUBEMX_PIN_F2_UART5_RX_SPARE1_GPIO_Port GPIOD
#define CUBEMX_PIN_F2_SYS_JTDO_SWO_Pin GPIO_PIN_3
#define CUBEMX_PIN_F2_SYS_JTDO_SWO_GPIO_Port GPIOB
#define CUBEMX_PIN_F2_SYS_JTRST_Pin GPIO_PIN_4
#define CUBEMX_PIN_F2_SYS_JTRST_GPIO_Port GPIOB
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
