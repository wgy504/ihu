/**
 ****************************************************************************************
 *
 * @file mod_gprs.h
 *
 * @brief GPRS module control
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */

#ifndef L1FREERTOS_MOD_GPRS_H_
#define L1FREERTOS_MOD_GPRS_H_

#include "stm32f2xx_hal.h"
#include "vmfreeoslayer.h"
#include "bsp_usart.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"

//全局使用的常亮定义
#define IHU_TIM_USART_GPRS_ID 		TIM2
#define IHU_GPRS_UART_TTS_MAX_LEN 200 //定义最多播放的字节数
#define IHU_GPRS_UART_REPEAT_CNT 3
#define IHU_GPRS_SMS_TEST_CONTENT   "THIS IS ZZZ TEST MESSAGE!"
#define IHU_GPRS_SIM800A_CALLED_NUMBER "+8613701629240"
#define IHU_GPRS_SIM800A_DTMF_TEST "X"
#define IHU_GPRS_SIM800A_GPRS_STREAM "TCP"   //"UDP"
#define IHU_GPRS_SIM800A_GPRS_CONTENT "THIS iS A PURE TEST!"

//工作调用流
extern OPSTAT GPRS_UART_GSM_working_procedure_selection(uint8_t option, uint8_t sub_opt);
extern OPSTAT GPRS_UART_SIM800A_gsm_info_procedure(void);//SIM800A信息显示(信号质量,电池电量,日期时间)
extern OPSTAT GPRS_UART_SIM800A_module_procedure(void);//模块信息检测
extern OPSTAT GPRS_UART_SIM800A_call_procedure(uint8_t sub_opt);//拨号测试程序
extern OPSTAT GPRS_UART_SIM800A_sms_procedure(void); //短信工作流程
extern OPSTAT GPRS_UART_SIM800A_gprs_procedure(uint8_t sub_opt);//GPRS数据传输测试程序
extern OPSTAT GPRS_UART_SIM800A_bs_procedure(void);//基站定位
extern OPSTAT GPRS_UART_SIM800A_tts_procedure(void);//TTS文本语音测试程序


//循环工作流函数
OPSTAT GPRS_UART_gsm_loop_test_main(void);
void GPRS_UART_SIM800A_test_loop(void);//模块测试主程序
OPSTAT GPRS_UART_SIM800A_gsm_test_info(void);//SIM800A信息显示(信号质量,电池电量,日期时间)
OPSTAT GPRS_UART_SIM800A_mtest(void);//模块信息检测
OPSTAT GPRS_UART_SIM800A_call_test(void);//拨号测试程序
OPSTAT GPRS_UART_SIM800A_sms_test(void);//短信测试程序
OPSTAT GPRS_UART_SIM800A_gprs_test(void);//GPRS数据传输测试程序
OPSTAT GPRS_UART_SIM800A_jz_test(void);//基站定位
OPSTAT GPRS_UART_SIM800A_tts_test(void);//TTS文本语音测试程序
void GPRS_UART_connect_server(void);
void GPRS_UART_Rec_Server_Data(void);
void GPRS_UART_data_connection_and_receive_process(void);


//核心函数
void GPRS_UART_clear_receive_buffer(void);
OPSTAT GPRS_UART_send_AT_command(uint8_t *cmd, uint8_t *ack, UINT16 wait_time);  //秒级！！！
void GPRS_UART_SendString(char* s);
#define GPRS_UART_SendLR() GPRS_UART_SendString("\r\n")
void GPRS_UART_Wait_CREG(void);
void GPRS_UART_Set_ATE0(void);
void GPRS_UART_Data_byte_send(char byte);
extern void TIM_USART_GPRS_IRQHandler(void);

//公共函数
OPSTAT GPRS_UART_Find_char(char *a);
OPSTAT GPRS_UART_change_str_Data(uint8_t *p,uint8_t len);//字符转整型
void GPRS_UART_change_Data_str(int n, char str[]);//整型转字符
void GPRS_UART_Swap(char *ch1, char *ch2);
void GPRS_UART_change_hex_str(uint8_t dest[],uint8_t src[],uint8_t len);//十六进制转字符串


//要被去掉的函数
void USART_PRINT_Send_Len(char *s,uint8_t len);
void USART_PRINT_Data(char byte);
void USART_PRINT_SendString(char* s);
extern void USART_PRINT_IRQHandler(void);

//Working Procedure



#endif /* L1FREERTOS_MOD_GPRS_H_ */

