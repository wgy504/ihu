#ifndef __BSP_STM32_USART_H
#define __BSP_STM32_USART_H

#ifdef __cplusplus
extern "C" {
#endif
	
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f2xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "sysdim.h"
#include "vmfreeoslayer.h"
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#include "commsgccl.h"
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	#include "commsgbfsc.h"
#else
	#error Un-correct constant definition
#endif

//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名

//IHU_SYSDIM_MSG_BODY_LEN_MAX-1是因为发送到上层SPSVIRGO的数据缓冲区受到消息结构msg_struct_spsvirgo_l2frame_rcv_t的影响
//最大接收数据长度，实际处理的时候，我们得要知道，CHAR因为'\0'的原因会少一个字符
//由于往上发送带有U16_LEN的长度域，不得不将长度再缩小一个字节，这样最大-2
#define IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN 				IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX
#define IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN 				IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX
#define IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN 				IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX
#define IHU_BSP_STM32_SPS_PRINT_REC_MAX_LEN 			IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX
#define IHU_BSP_STM32_SPS_SPARE1_REC_MAX_LEN 			IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX
#define IHU_BSP_STM32_SPS_SPARE2_REC_MAX_LEN 			IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX

//HUITP L2FRAME帧结构
typedef struct IHU_HUITP_L2FRAME_STD_UART_frame_header
{
  uint8_t start;
  uint8_t chksum;
  uint16_t len;
}IHU_HUITP_L2FRAME_STD_UART_frame_header_t;
#define IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE 0
#define IHU_HUITP_L2FRAME_STD_RX_STATE_START 1
#define IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM 2
#define IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN 3
#define IHU_HUITP_L2FRAME_STD_RX_STATE_BODY 4
#define IHU_HUITP_L2FRAME_STD_RX_START_FLAG_CHAR 0xFE

//发送和接受数据的延迟时间长度
#define BSP_SUCCESS 0
#define BSP_FAILURE -1
#define IHU_BSP_STM32_SPS_TX_MAX_DELAY 100
#define IHU_BSP_STM32_SPS_RX_MAX_DELAY 100

//本地定义的交换矩阵
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#define IHU_BSP_STM32_UART_GPRS_HANDLER					huart2  //根据最新板子修改
	#define IHU_BSP_STM32_UART_GPRS_HANDLER_ID  		2
  #define IHU_BSP_STM32_UART_RFID_HANDLER					huart1  //其实是不用的，而使用了SPI接口
	#define IHU_BSP_STM32_UART_RFID_HANDLER_ID  		1
	#define IHU_BSP_STM32_UART_PRINT_HANDLER				huart3
	#define IHU_BSP_STM32_UART_PRINT_HANDLER_ID  		3
	#define IHU_BSP_STM32_UART_BLE_HANDLER					huart1  //根据最新板子修改
	#define IHU_BSP_STM32_UART_BLE_HANDLER_ID  			1
	#define IHU_BSP_STM32_UART_SPARE1_HANDLER				huart1
	#define IHU_BSP_STM32_UART_SPARE1_HANDLER_ID  	5
	#define IHU_BSP_STM32_UART6_PRESENT_HANDLER			huart6
	#define IHU_BSP_STM32_UART6_PRESENT_HANDLER_ID  6

#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	#define IHU_BSP_STM32_UART_GPRS_HANDLER					huart1
	#define IHU_BSP_STM32_UART_GPRS_HANDLER_ID  		1
	#define IHU_BSP_STM32_UART_RFID_HANDLER					huart2
	#define IHU_BSP_STM32_UART_RFID_HANDLER_ID  		2
	#define IHU_BSP_STM32_UART_PRINT_HANDLER				huart3
	#define IHU_BSP_STM32_UART_PRINT_HANDLER_ID  		3
	#define IHU_BSP_STM32_UART_BLE_HANDLER					huart1 //huart4, MYC update for BFSC 2017/03/19
	#define IHU_BSP_STM32_UART_BLE_HANDLER_ID  			4
	#define IHU_BSP_STM32_UART_SPARE1_HANDLER				huart1 //huart5, MYC update for BFSC 2017/03/19
	#define IHU_BSP_STM32_UART_SPARE1_HANDLER_ID  	5
	#define IHU_BSP_STM32_UART6_PRESENT_HANDLER			huart6
	#define IHU_BSP_STM32_UART6_PRESENT_HANDLER_ID  6
#endif



/*蓝牙地址，数字形式，分NAP，UAP，LAP段*/			
#define BLTDEV_MAX_NUM  5
typedef  struct 
{
	uint16_t  NAP;
	uint8_t 	UAP;
	uint32_t  LAP;
}BLTAddr;
typedef  struct 
{
	uint8_t num;		//扫描到的蓝牙设备数量	
	BLTAddr addr[BLTDEV_MAX_NUM];	//蓝牙设备地址，数字形式
	char unpraseAddr[BLTDEV_MAX_NUM][50];	//蓝牙设备地址，字符串形式，方便扫描时和连接时使用
	char name[BLTDEV_MAX_NUM][50];	//蓝牙设备的名字
}BLTDev;

enum
{
  HC05_DEFAULT_TIMEOUT = 200,
  HC05_INQUIRY_DEFAULT_TIMEOUT = 10000,
  HC05_PAIRING_DEFAULT_TIMEOUT = 10000,
  HC05_PASSWORD_MAXLEN = 16,
  HC05_PASSWORD_BUFSIZE = HC05_PASSWORD_MAXLEN + 1,
  HC05_NAME_MAXLEN = 32,
  HC05_NAME_BUFSIZE = HC05_NAME_MAXLEN + 1,
  HC05_ADDRESS_MAXLEN = 14,
  HC05_ADDRESS_BUFSIZE = HC05_ADDRESS_MAXLEN + 1,
};
		
/* 宏定义 --------------------------------------------------------------------*/
//#define HC05_USART          	          USART2
//#define HC05_EN_GPIO_CLK() 	            __HAL_RCC_GPIOF_CLK_ENABLE()		/* GPIO端口时钟 */
//#define HC05_EN_GPIO_PORT    	          GPIOF			              /* GPIO端口 */
//#define HC05_EN_GPIO_PIN		            GPIO_PIN_11		          /* 连接到HC05 EN引脚的GPIO */
//#define HC05_EN_HIGHT()		              HAL_GPIO_WritePin(HC05_EN_GPIO_PORT,HC05_EN_GPIO_PIN,GPIO_PIN_SET);	
//#define HC05_EN_LOW()				            HAL_GPIO_WritePin(HC05_EN_GPIO_PORT,HC05_EN_GPIO_PIN,GPIO_PIN_RESET);	


//全局函数
extern int ihu_bsp_stm32_sps_slave_hw_init(void);
extern int ihu_bsp_stm32_sps_print_fputc(int ch, FILE *f);
extern int ihu_bsp_stm32_sps_print_fgetc(FILE * f);
extern int ihu_bsp_stm32_sps_print_send_data(char *s);
extern int ihu_bsp_stm32_sps_print_rcv_data(char *s, uint16_t len);
extern int ihu_bsp_stm32_sps_print_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout);
extern int ihu_bsp_stm32_sps_gprs_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_gprs_rcv_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_gprs_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout);
extern int ihu_bsp_stm32_sps_rfid_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_rfid_rcv_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_rfid_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout);
extern int ihu_bsp_stm32_sps_ble_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_ble_rcv_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_ble_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout);
extern int ihu_bsp_stm32_sps_spare1_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_spare1_rcv_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_spare1_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout);
extern int ihu_bsp_stm32_sps_spare2_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_spare2_rcv_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_sps_spare2_rcv_data_timeout(uint8_t* buff, uint16_t len, uint32_t timeout);

//重载接收函数，以便通过IT中断方式搞定接收通信，否则需要通过轮询或者单独线程搞定，更加麻烦
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle);


//Local APIs


#ifdef __cplusplus
}
#endif
#endif  /* __BSP_STM32_USART_H */


