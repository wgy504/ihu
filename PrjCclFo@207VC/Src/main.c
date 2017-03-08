/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f2xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "vmfreeoslayer.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

DCMI_HandleTypeDef hdcmi;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osMessageQId myQueue01Handle;
osTimerId myTimer01Handle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t zIhuUartRxBuffer[6];
uint8_t zIhuSpiRxBuffer[2];
uint8_t zIhuI2cRxBuffer[2];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_UART4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_DCMI_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI2_Init(void);
void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void Callback01(void const * argument);
static void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_RTC_Init();
  MX_DCMI_Init();
  MX_ADC1_Init();
  MX_SPI2_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();

  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1,&zIhuUartRxBuffer[0],1);	
  HAL_UART_Receive_IT(&huart2,&zIhuUartRxBuffer[1],1);
  HAL_UART_Receive_IT(&huart3,&zIhuUartRxBuffer[2],1);
  HAL_UART_Receive_IT(&huart4,&zIhuUartRxBuffer[3],1);
  //HAL_UART_Receive_IT(&huart5,&zIhuUartRxBuffer[4],1);
  //HAL_UART_Receive_IT(&huart6,&zIhuUartRxBuffer[5],1);
  //HAL_SPI_Receive_IT(&hspi1,&zIhuSpiRxBuffer[0],1);
  HAL_SPI_Receive_IT(&hspi2,&zIhuSpiRxBuffer[1],1);
#if (BSP_STM32_I2C_WORK_MODE_CHOICE == BSP_STM32_I2C_WORK_MODE_IAU)
  HAL_I2C_Slave_Receive_IT(&hi2c1, &zIhuI2cRxBuffer[0], 1);
#elif (BSP_STM32_I2C_WORK_MODE_CHOICE == BSP_STM32_I2C_WORK_MODE_MPU6050)
	HAL_I2C_Master_Receive_IT(&hi2c1, (uint16_t)IHU_BSP_STM32_I2C_MPU6050_SENSOR_SLAVE_ADDRESS, &zIhuI2cRxBuffer[0], 1);	
#elif (BSP_STM32_I2C_WORK_MODE_CHOICE == BSP_STM32_I2C_WORK_MODE_CCL_SENSOR)
	HAL_I2C_Master_Receive_IT(&hi2c1, (uint16_t)IHU_BSP_STM32_I2C_CCL_SENSOR_SLAVE_ADDRESS, &zIhuI2cRxBuffer[0], 1);	
#endif
  //HAL_CAN_Receive_IT(&hcan1, 0);
  //HAL_CAN_Receive_IT(&hcan2, 1);
	
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of myTimer01 */
  osTimerDef(myTimer01, Callback01);
  myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityNormal, 0, 512);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of myQueue01 */
  osMessageQDef(myQueue01, 16, uint16_t);
  myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
	//Self defined main task entry
	ihu_vm_main();
	
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV8;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_4);

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/** NVIC Configuration
*/
static void MX_NVIC_Init(void)
{
  /* I2C1_EV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
  /* I2C1_ER_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USART3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* UART4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UART4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(UART4_IRQn);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/* DCMI init function */
static void MX_DCMI_Init(void)
{

  hdcmi.Instance = DCMI;
  hdcmi.Init.SynchroMode = DCMI_SYNCHRO_EMBEDDED;
  hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_FALLING;
  hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
  hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  hdcmi.Init.SyncroCode.FrameEndCode = 0;
  hdcmi.Init.SyncroCode.FrameStartCode = 0;
  hdcmi.Init.SyncroCode.LineStartCode = 0;
  hdcmi.Init.SyncroCode.LineEndCode = 0;
  hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
  if (HAL_DCMI_Init(&hdcmi) != HAL_OK)
  {
    Error_Handler();
  }

}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

}

/* RTC init function */
static void MX_RTC_Init(void)
{

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

    /**Initialize RTC Only 
    */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initialize RTC and set the Time and Date 
    */
  sTime.Hours = 0x21;
  sTime.Minutes = 0x20;
  sTime.Seconds = 0x40;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x2;
  sDate.Year = 0x17;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

}

/* SPI2 init function */
static void MX_SPI2_Init(void)
{

  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }

}

/* UART4 init function */
static void MX_UART4_Init(void)
{

  huart4.Instance = UART4;
  huart4.Init.BaudRate = 38400;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PA8   ------> RCC_MCO_1
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pins : CUBEMX_PIN_F2_GPIO_DI_DHT11_Pin CUBEMX_PIN_F2_GPIO_DI_DOOR3_RESTRICTION_Pin CUBEMX_PIN_F2_GPIO_DI_LOCK3_DI2_TONGUE_Pin CUBEMX_PIN_F2_GPIO_DI_DOOR2_RESTRICTION_Pin 
                           CUBEMX_PIN_F2_GPIO_DI_LOCK2_DI2_TONGUE_Pin CUBEMX_PIN_F2_GPIO_DI_SHAKE_SENSOR_Pin */
  GPIO_InitStruct.Pin = CUBEMX_PIN_F2_GPIO_DI_DHT11_Pin|CUBEMX_PIN_F2_GPIO_DI_DOOR3_RESTRICTION_Pin|CUBEMX_PIN_F2_GPIO_DI_LOCK3_DI2_TONGUE_Pin|CUBEMX_PIN_F2_GPIO_DI_DOOR2_RESTRICTION_Pin 
                          |CUBEMX_PIN_F2_GPIO_DI_LOCK2_DI2_TONGUE_Pin|CUBEMX_PIN_F2_GPIO_DI_SHAKE_SENSOR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : CUBEMX_PIN_F2_GPIO_DO_LED_POWER_Pin CUBEMX_PIN_F2_GPIO_DO_LOCK3_DO1_Pin CUBEMX_PIN_F2_GPIO_DO_LOCK2_DO1_Pin */
  GPIO_InitStruct.Pin = CUBEMX_PIN_F2_GPIO_DO_LED_POWER_Pin|CUBEMX_PIN_F2_GPIO_DO_LOCK3_DO1_Pin|CUBEMX_PIN_F2_GPIO_DO_LOCK2_DO1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : CUBEMX_PIN_F2_GPIO_DI_FALL_SNESOR_Pin CUBEMX_PIN_F2_GPIO_DI_DOOR1_RESTRICTION_Pin */
  GPIO_InitStruct.Pin = CUBEMX_PIN_F2_GPIO_DI_FALL_SNESOR_Pin|CUBEMX_PIN_F2_GPIO_DI_DOOR1_RESTRICTION_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : CUBEMX_PIN_F2_GPIO_LED_207VC_TEST_Pin */
  GPIO_InitStruct.Pin = CUBEMX_PIN_F2_GPIO_LED_207VC_TEST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CUBEMX_PIN_F2_GPIO_LED_207VC_TEST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CUBEMX_PIN_F2_GPIO_DO_LED_WORK_STATE_Pin CUBEMX_PIN_F2_GPIO_DO_LED_COMMU_Pin */
  GPIO_InitStruct.Pin = CUBEMX_PIN_F2_GPIO_DO_LED_WORK_STATE_Pin|CUBEMX_PIN_F2_GPIO_DO_LED_COMMU_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI1_TRIGGER_Pin */
  GPIO_InitStruct.Pin = CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI1_TRIGGER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI1_TRIGGER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI2_TONGUE_Pin */
  GPIO_InitStruct.Pin = CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI2_TONGUE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI2_TONGUE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CUBEMX_PIN_F2_GPIO_DO_LOCK1_DO1_Pin */
  GPIO_InitStruct.Pin = CUBEMX_PIN_F2_GPIO_DO_LOCK1_DO1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CUBEMX_PIN_F2_GPIO_DO_LOCK1_DO1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CUBEMX_PIN_F2_GPIO_DI_LOCK3_DI1_TRIGGER_Pin CUBEMX_PIN_F2_GPIO_DI_LOCK2_DI1_TRIGGER_Pin */
  GPIO_InitStruct.Pin = CUBEMX_PIN_F2_GPIO_DI_LOCK3_DI1_TRIGGER_Pin|CUBEMX_PIN_F2_GPIO_DI_LOCK2_DI1_TRIGGER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : CUBEMX_PIN_F2_SPI2_GPIO_RST_Pin CUBEMX_PIN_F2_GPIO_DO_LOCK4_DO1_Pin CUBEMX_PIN_F2_GPIO_DO_BEEP_Pin CUBEMX_PIN_F2_DO_BLE_ATCMD_Pin 
                           CUBEMX_PIN_F2_DO_SENSOR_PWR_Pin CUBEMX_PIN_F2_DO_RFID_PWR_Pin CUBEMX_PIN_F2_DO_BLE_PWR_Pin CUBEMX_PIN_F2_DO_GPRSMOD_PWR_Pin */
  GPIO_InitStruct.Pin = CUBEMX_PIN_F2_SPI2_GPIO_RST_Pin|CUBEMX_PIN_F2_GPIO_DO_LOCK4_DO1_Pin|CUBEMX_PIN_F2_GPIO_DO_BEEP_Pin|CUBEMX_PIN_F2_DO_BLE_ATCMD_Pin 
                          |CUBEMX_PIN_F2_DO_SENSOR_PWR_Pin|CUBEMX_PIN_F2_DO_RFID_PWR_Pin|CUBEMX_PIN_F2_DO_BLE_PWR_Pin|CUBEMX_PIN_F2_DO_GPRSMOD_PWR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : CUBEMX_PIN_F2_GPIO_DI_DOOR4_RESTRICTION_Pin CUBEMX_PIN_F2_GPIO_DI_LOCK4_DI2_TONGUE_Pin CUBEMX_PIN_F2_GPIO_DI_SMOKE_SENSOR_Pin CUBEMX_PIN_F2_GPIO_DI_WATER_SENSOR_Pin */
  GPIO_InitStruct.Pin = CUBEMX_PIN_F2_GPIO_DI_DOOR4_RESTRICTION_Pin|CUBEMX_PIN_F2_GPIO_DI_LOCK4_DI2_TONGUE_Pin|CUBEMX_PIN_F2_GPIO_DI_SMOKE_SENSOR_Pin|CUBEMX_PIN_F2_GPIO_DI_WATER_SENSOR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : CUBEMX_PIN_F2_GPIO_DI_LOCK4_DI1_TRIGGER_Pin */
  GPIO_InitStruct.Pin = CUBEMX_PIN_F2_GPIO_DI_LOCK4_DI1_TRIGGER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(CUBEMX_PIN_F2_GPIO_DI_LOCK4_DI1_TRIGGER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CUBEMX_PIN_F2_RCC_MCO_1_Pin */
  GPIO_InitStruct.Pin = CUBEMX_PIN_F2_RCC_MCO_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(CUBEMX_PIN_F2_RCC_MCO_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, CUBEMX_PIN_F2_GPIO_DO_LED_POWER_Pin|CUBEMX_PIN_F2_GPIO_DO_LOCK3_DO1_Pin|CUBEMX_PIN_F2_GPIO_DO_LOCK2_DO1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CUBEMX_PIN_F2_GPIO_LED_207VC_TEST_GPIO_Port, CUBEMX_PIN_F2_GPIO_LED_207VC_TEST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CUBEMX_PIN_F2_GPIO_DO_LED_WORK_STATE_Pin|CUBEMX_PIN_F2_GPIO_DO_LED_COMMU_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CUBEMX_PIN_F2_GPIO_DO_LOCK1_DO1_GPIO_Port, CUBEMX_PIN_F2_GPIO_DO_LOCK1_DO1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, CUBEMX_PIN_F2_SPI2_GPIO_RST_Pin|CUBEMX_PIN_F2_GPIO_DO_LOCK4_DO1_Pin|CUBEMX_PIN_F2_GPIO_DO_BEEP_Pin|CUBEMX_PIN_F2_DO_BLE_ATCMD_Pin 
                          |CUBEMX_PIN_F2_DO_SENSOR_PWR_Pin|CUBEMX_PIN_F2_DO_RFID_PWR_Pin|CUBEMX_PIN_F2_DO_BLE_PWR_Pin|CUBEMX_PIN_F2_DO_GPRSMOD_PWR_Pin, GPIO_PIN_RESET);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */ 
}

/* StartTask02 function */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask02 */
}

/* Callback01 function */
void Callback01(void const * argument)
{
  /* USER CODE BEGIN Callback01 */
  
  /* USER CODE END Callback01 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
/* USER CODE BEGIN Callback 0 */

/* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
/* USER CODE BEGIN Callback 1 */

/* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
//	IhuErrorPrint("VMFO: Fatal error occured, but still move forward as test purposed!");
//  while(1) 
//  {
//  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
