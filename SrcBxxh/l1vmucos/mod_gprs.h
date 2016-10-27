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

#ifndef L1VUCOS_MOD_GPRS_H_
#define L1VUCOS_MOD_GPRS_H_

#include "stm32f2xx.h"
#include "string.h"
#include "l1hd_usart.h"
#include "l1hd_led.h"
#include "l1hd_timer.h"
#include "vmucoslayer.h"
#include "bsp.h"

#define TIM_USART_GPRS_ID 		TIM2
#define SPS_GPRS_TTS_MAX_len 200 //定义最多播放的字节数


//工作流函数
u8 SPS_GPRS_gsm_test_main(void);
void SPS_GPRS_GSM_test_loop(void);//模块测试主程序
u8 SPS_GPRS_GSM_gsminfo(void);//GSM信息显示(信号质量,电池电量,日期时间)
u8 SPS_GPRS_GSM_mtest(void);//模块信息检测
u8 SPS_GPRS_GSM_call_test(void);//拨号测试程序
u8 SPS_GPRS_GSM_sms_test(void);//短信测试程序
u8 SPS_GPRS_GSM_gprs_test(void);//GPRS数据传输测试程序
u8 SPS_GPRS_GSM_jz_test(void);//基站定位
u8 SPS_GPRS_GSM_tts_test(void);//TTS文本语音测试程序
void SPS_GPRS_connect_server(void);
void SPS_GPRS_Rec_Server_Data(void);
void SPS_GPRS_data_connection_and_receive_process(void);


//核心函数
void SPS_GPRS_clear_receive_buffer(void);
u8 SPS_GPRS_send_AT_command(u8 *cmd, u8 *ack, u16 wait_time);
void SPS_GPRS_SendString(char* s);
#define SPS_GPRS_SendLR() SPS_GPRS_SendString("\r\n")
void SPS_GPRS_Wait_CREG(void);
void SPS_GPRS_Set_ATE0(void);
void SPS_GPRS_Data_byte_send(char byte);
extern void TIM_USART_GPRS_IRQHandler(void);

//公共函数
u8 SPS_GPRS_Find_char(char *a);
u8 SPS_GPRS_change_str_Data(u8 *p,u8 len);//字符转整型
void SPS_GPRS_change_Data_str(int n, char str[]);//整型转字符
void SPS_GPRS_Swap(char *ch1, char *ch2);
void SPS_GPRS_change_hex_str(u8 dest[],u8 src[],u8 len);//十六进制转字符串


//要被去掉的函数
void USART_PRINT_Send_Len(char *s,u8 len);
void USART_PRINT_Data(char byte);
void USART_PRINT_SendString(char* s);
extern void USART_PRINT_IRQHandler(void);

//Working Procedure



#endif /* L1VUCOS_MOD_GPRS_H_ */

