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

#include "stm32f2xx_hal.h"
#include "vmfreeoslayer.h"

#define TIM_USART_GPRS_ID 		TIM2
#define SPS_GPRS_TTS_MAX_len 200 //定义最多播放的字节数
#define SPS_GPRS_REC_MAXLEN 200	//最大接收数据长度
#define SPS_RFID_REC_MAXLEN 200	//最大接收数据长度
#define SPS_BLE_REC_MAXLEN 200	//最大接收数据长度
#define SPS_SPARE1_REC_MAXLEN 200	//最大接收数据长度
#define SPS_PRINT_REC_MAXLEN 200	//最大接收数据长度


//工作调用流
extern void SPS_GPRS_GSM_working_procedure_selection(UINT8 option);
extern UINT8 SPS_GPRS_GSM_gsm_info_procedure(void);//GSM信息显示(信号质量,电池电量,日期时间)
extern UINT8 SPS_GPRS_GSM_module_procedure(void);//模块信息检测
extern UINT8 SPS_GPRS_GSM_call_procedure(void);//拨号测试程序
extern UINT8 SPS_GPRS_GSM_sms_procedure(void); //短信工作流程
extern UINT8 SPS_GPRS_GSM_gprs_procedure(void);//GPRS数据传输测试程序
extern UINT8 SPS_GPRS_GSM_bs_procedure(void);//基站定位
extern UINT8 SPS_GPRS_GSM_tts_procedure(void);//TTS文本语音测试程序


//循环工作流函数
UINT8 SPS_GPRS_gsm_loop_test_main(void);
void SPS_GPRS_GSM_test_loop(void);//模块测试主程序
UINT8 SPS_GPRS_GSM_gsm_test_info(void);//GSM信息显示(信号质量,电池电量,日期时间)
UINT8 SPS_GPRS_GSM_mtest(void);//模块信息检测
UINT8 SPS_GPRS_GSM_call_test(void);//拨号测试程序
UINT8 SPS_GPRS_GSM_sms_test(void);//短信测试程序
UINT8 SPS_GPRS_GSM_gprs_test(void);//GPRS数据传输测试程序
UINT8 SPS_GPRS_GSM_jz_test(void);//基站定位
UINT8 SPS_GPRS_GSM_tts_test(void);//TTS文本语音测试程序
void SPS_GPRS_connect_server(void);
void SPS_GPRS_Rec_Server_Data(void);
void SPS_GPRS_data_connection_and_receive_process(void);


//核心函数
void SPS_GPRS_clear_receive_buffer(void);
UINT8 SPS_GPRS_send_AT_command(UINT8 *cmd, UINT8 *ack, UINT16 wait_time);
void SPS_GPRS_SendString(char* s);
#define SPS_GPRS_SendLR() SPS_GPRS_SendString("\r\n")
void SPS_GPRS_Wait_CREG(void);
void SPS_GPRS_Set_ATE0(void);
void SPS_GPRS_Data_byte_send(char byte);
extern void TIM_USART_GPRS_IRQHandler(void);

//公共函数
UINT8 SPS_GPRS_Find_char(char *a);
UINT8 SPS_GPRS_change_str_Data(UINT8 *p,UINT8 len);//字符转整型
void SPS_GPRS_change_Data_str(int n, char str[]);//整型转字符
void SPS_GPRS_Swap(char *ch1, char *ch2);
void SPS_GPRS_change_hex_str(UINT8 dest[],UINT8 src[],UINT8 len);//十六进制转字符串


//要被去掉的函数
void USART_PRINT_Send_Len(char *s,UINT8 len);
void USART_PRINT_Data(char byte);
void USART_PRINT_SendString(char* s);
extern void USART_PRINT_IRQHandler(void);

//Working Procedure



#endif /* L1VUCOS_MOD_GPRS_H_ */

