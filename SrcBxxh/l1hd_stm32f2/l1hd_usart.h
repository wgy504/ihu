#ifndef __L1HD_USART_H
#define __L1HD_USART_H

#include "stm32f2xx.h"
#include "stdio.h"  
#include "string.h"  
#include "vmucoslayer.h"
#include "bsp.h"

#define SPS_GPRS_REC_MAXLEN 200	//最大接收数据长度
#define SPS_RFID_REC_MAXLEN 200	//最大接收数据长度
#define SPS_BLE_REC_MAXLEN 200	//最大接收数据长度
#define SPS_SPARE1_REC_MAXLEN 200	//最大接收数据长度
#define SPS_PRINT_REC_MAXLEN 200	//最大接收数据长度

//USART1 - GPRS，用于GPRS模块的串口控制
#define USART_GPRS                        USART1
#define USART_GPRS_CLK_ENABLE()						RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
#define USART_GPRS_TX_GPIO_PORT		 				GPIOA
#define USART_GPRS_TX_GPIO_PIN 						GPIO_Pin_9
#define USART_GPRS_TX_GPIO_CLK           	RCC_AHB1Periph_GPIOA
#define USART_GPRS_TX_SOURCE             	GPIO_PinSource9
#define USART_GPRS_TX_AF                 	GPIO_AF_USART1
#define USART_GPRS_RX_GPIO_PORT		 				GPIOA
#define USART_GPRS_RX_GPIO_PIN 						GPIO_Pin_10
#define USART_GPRS_RX_GPIO_CLK           	RCC_AHB1Periph_GPIOA
#define USART_GPRS_RX_SOURCE             	GPIO_PinSource10
#define USART_GPRS_RX_AF                 	GPIO_AF_USART1
#define USART_GPRS_IRQn										USART1_IRQn
#define USART_GPRS_TX_ENABLE()						RCC_AHB1PeriphClockCmd(USART_GPRS_TX_GPIO_CLK, ENABLE)
#define USART_GPRS_RX_ENABLE()						RCC_APB1PeriphClockCmd(USART_GPRS_RX_GPIO_CLK, ENABLE)
#define USART_GPRS_INT_VECTOR							BSP_INT_ID_USART1

//USART2 - RFID，用于RFID模块的串口控制
#define USART_RFID                       	USART2
#define USART_RFID_CLK_ENABLE()						RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE)
#define USART_RFID_TX_GPIO_PORT		 				GPIOA
#define USART_RFID_TX_GPIO_PIN 						GPIO_Pin_2
#define USART_RFID_TX_GPIO_CLK           	RCC_AHB1Periph_GPIOA
#define USART_RFID_TX_SOURCE             	GPIO_PinSource2
#define USART_RFID_TX_AF                 	GPIO_AF_USART2
#define USART_RFID_RX_GPIO_PORT		 				GPIOA
#define USART_RFID_RX_GPIO_PIN 						GPIO_Pin_3
#define USART_RFID_RX_GPIO_CLK           	RCC_AHB1Periph_GPIOA
#define USART_RFID_RX_SOURCE             	GPIO_PinSource3
#define USART_RFID_RX_AF                 	GPIO_AF_USART2
#define USART_RFID_IRQn										USART2_IRQn
#define USART_RFID_TX_ENABLE()						RCC_AHB1PeriphClockCmd(USART_RFID_TX_GPIO_CLK, ENABLE)
#define USART_RFID_RX_ENABLE()						RCC_APB1PeriphClockCmd(USART_RFID_RX_GPIO_CLK, ENABLE)
#define USART_RFID_INT_VECTOR							BSP_INT_ID_USART2

//USART3 - 缺省留作打印口，跟系统缺省的PC10/PC11不在同一个接口
#define USART_PRINT                       USART3
#define USART_PRINT_CLK_ENABLE()					RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE)
#define USART_PRINT_TX_GPIO_PORT		 			GPIOB
#define USART_PRINT_TX_GPIO_PIN 					GPIO_Pin_10
#define USART_PRINT_TX_GPIO_CLK           RCC_AHB1Periph_GPIOB
#define USART_PRINT_TX_SOURCE             GPIO_PinSource10
#define USART_PRINT_TX_AF                 GPIO_AF_USART3
#define USART_PRINT_RX_GPIO_PORT		 			GPIOB
#define USART_PRINT_RX_GPIO_PIN 					GPIO_Pin_11
#define USART_PRINT_RX_GPIO_CLK           RCC_AHB1Periph_GPIOB
#define USART_PRINT_RX_SOURCE             GPIO_PinSource11
#define USART_PRINT_RX_AF                 GPIO_AF_USART3
#define USART_PRINT_IRQn									USART3_IRQn
#define USART_PRINT_TX_ENABLE()						RCC_AHB1PeriphClockCmd(USART_PRINT_TX_GPIO_CLK, ENABLE)
#define USART_PRINT_RX_ENABLE()						RCC_APB1PeriphClockCmd(USART_PRINT_RX_GPIO_CLK, ENABLE)
#define USART_PRINT_INT_VECTOR						BSP_INT_ID_USART3

//UART4 - BLE，用于BLE模块的串口控制，跟I2C2以及ETH相冲突，同时更系统缺省打印口也相冲突
#define UART_BLE                       		UART4
#define UART_BLE_CLK_ENABLE()							RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE)
#define UART_BLE_TX_GPIO_PORT		 					GPIOC
#define UART_BLE_TX_GPIO_PIN 							GPIO_Pin_10
#define UART_BLE_TX_GPIO_CLK           		RCC_AHB1Periph_GPIOC
#define UART_BLE_TX_SOURCE             		GPIO_PinSource10
#define UART_BLE_TX_AF                 		GPIO_AF_UART4
#define UART_BLE_RX_GPIO_PORT		 					GPIOC
#define UART_BLE_RX_GPIO_PIN 							GPIO_Pin_11
#define UART_BLE_RX_GPIO_CLK           		RCC_AHB1Periph_GPIOC
#define UART_BLE_RX_SOURCE             		GPIO_PinSource11
#define UART_BLE_RX_AF                 		GPIO_AF_UART4
#define UART_BLE_IRQn											UART4_IRQn
#define UART_BLE_TX_ENABLE()							RCC_AHB1PeriphClockCmd(UART_BLE_TX_GPIO_CLK, ENABLE)
#define UART_BLE_RX_ENABLE()							RCC_APB1PeriphClockCmd(UART_BLE_RX_GPIO_CLK, ENABLE)
#define UART_BLE_INT_VECTOR								BSP_INT_ID_USART4

//UART5 - 空缺
#define UART_SPARE1                       UART5
#define UART_SPARE1_CLK_ENABLE()					RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE)
#define UART_SPARE1_TX_GPIO_PORT		 			GPIOC
#define UART_SPARE1_TX_GPIO_PIN 					GPIO_Pin_12
#define UART_SPARE1_TX_GPIO_CLK           RCC_AHB1Periph_GPIOC
#define UART_SPARE1_TX_SOURCE             GPIO_PinSource12
#define UART_SPARE1_TX_AF                 GPIO_AF_UART5
#define UART_SPARE1_RX_GPIO_PORT		 			GPIOD
#define UART_SPARE1_RX_GPIO_PIN 					GPIO_Pin_2
#define UART_SPARE1_RX_GPIO_CLK           RCC_AHB1Periph_GPIOD
#define UART_SPARE1_RX_SOURCE             GPIO_PinSource2
#define UART_SPARE1_RX_AF                 GPIO_AF_UART5
#define UART_SPARE1_IRQn									UART5_IRQn
#define UART_SPARE1_TX_ENABLE()						RCC_AHB1PeriphClockCmd(UART_SPARE1_TX_GPIO_CLK, ENABLE)
#define UART_SPARE1_RX_ENABLE()						RCC_APB1PeriphClockCmd(UART_SPARE1_RX_GPIO_CLK, ENABLE)
#define UART_SPARE1_INT_VECTOR						BSP_INT_ID_USART5

//USART6 - 空缺

//API函数
extern void SPS_GPRS_Init_Config(u32 bound);
extern void SPS_GPRS_SendData(vs8* buff, u16 len);
void SPS_GPRS_IRQHandler(void);
extern void SPS_RFID_Init_Config(u32 bound);
extern void SPS_RFID_SendData(vs8* buff, u16 len);
void SPS_RFID_IRQHandler(void);
extern void SPS_BLE_Init_Config(u32 bound);
extern void SPS_BLE_SendData(vs8* buff, u16 len);
void SPS_BLE_IRQHandler(void);
extern void SPS_SPARE1_Init_Config(u32 bound);
extern void SPS_SPARE1_SendData(vs8* buff, u16 len);
void SPS_SPARE1_IRQHandler(void);

//本地函数
int sps_fputc(int ch, FILE *f);
int sps_fgetc(FILE *f);

//Global functions
//char *strstr( char *str, char * substr );

#endif


