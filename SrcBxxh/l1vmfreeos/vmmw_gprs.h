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
#include "l1comdef_freeos.h"
#include "huixmlcodec.h"

//全局使用的常量定义
#define IHU_VMMW_GPRSMOD_TTS_MAX_LEN 200 //定义最多播放的字节数
#define IHU_VMMW_GPRSMOD_REPEAT_CNT 3

//不同潜在模组的定义，未来如果有AT COMMAND不一样的地方，将依靠这个进行规避
#define IHU_VMWM_GPRSMOD_MODULE_SIM800A 1
#define IHU_VMWM_GPRSMOD_MODULE_SIM900 2
#define IHU_VMWM_GPRSMOD_MODULE_A6 3
#define IHU_VMWM_GPRSMOD_MODULE_ZTE 4
#define IHU_VMWM_GPRSMOD_CURRENT_CONFIG  IHU_VMWM_GPRSMOD_MODULE_SIM800A  //当前硬件模组配置为SIM800A

//常量控制
#define IHU_VMWM_GPRSMOD_RECEIVE_DATA_TRANSFER_TO_LOWER_CASE_YES 1
#define IHU_VMWM_GPRSMOD_RECEIVE_DATA_TRANSFER_TO_LOWER_CASE_NO 2
#define IHU_VMWM_GPRSMOD_RECEIVE_DATA_TRANSFER_TO_LOWER_CASE_SET IHU_VMWM_GPRSMOD_RECEIVE_DATA_TRANSFER_TO_LOWER_CASE_NO


/* 字符串转义的写法
\0 空字符(NULL) 0x00 
\a 响铃(BEL) 0x07 
\b 退格(BS) ，将当前位置移到前一列 0x08 
\f 换页(FF)，将当前位置移到下页开头 0x0C
\n 换行(LF) ，将当前位置移到下一行开头 0x0A 
\r 回车(CR) ，将当前位置移到本行开头 0x0D 
\t 水平制表(HT) （跳到下一个TAB位置）0x09
\v 垂直制表(VT) 0x0B 
\\ 代表一个反斜线字符''\' 0x5C(92)
\' 代表一个单引号（撇号）字符 0x27(39)
\" 代表一个双引号字符 0x22(34)
\? 代表一个问号 0x3F(63) 
\ddd 1～3位八进制数所代表的字符 
\xhh 1～2位十六进制数所代表的字符
*/
#define IHU_VMWM_GPRSMOD_STRING_CHAR_NULL '\0'  //0x00, 结束符
#define IHU_VMWM_GPRSMOD_STRING_CHAR_CR '\r'  //0x0D, 回车符，carriage return
#define IHU_VMWM_GPRSMOD_STRING_CHAR_LF '\n'  //0x0A, 换行符，line feed
#define IHU_VMWM_GPRSMOD_STRING_CHAR_EOF '\32'  //0x1A, 文件结束符，短信等文件的结束符，'\x1A'是16进制写法，'\32'是8进制写法
#define IHU_VMWM_GPRSMOD_STRING_CHAR_SPACE '\x20'  //0x20, 文件结束符，短信等文件的结束符, '\x20'是16进制写法，'\40'是8进制写法

//向上提供全局统一服务的入口
//目前主要用了HTTP，其它的等待以后其它机会再测试，但都通过正式文档将其实现了
	extern OPSTAT ihu_vmmw_gprsmod_call_perform(char *calledNumber);
	extern OPSTAT ihu_vmmw_gprsmod_sms_transmit_with_confirm(char *calledNumber, char *input);
	extern OPSTAT ihu_vmmw_gprsmod_http_data_transmit_with_receive(char *input, int16_t inlen, char *output, uint16_t *outlen);   //往后台发送的POST功能
	extern OPSTAT ihu_vmmw_gprsmod_tcp_text_data_transmit_with_receive(char *input, int16_t inlen, char *output, uint16_t *outlen);  //往后台发送的POST功能
	extern OPSTAT ihu_vmmw_gprsmod_tcp_u8_data_transmit_with_receive(int8_t *input, int16_t inlen, int8_t *output, uint16_t *outlen);  //往后台发送的POST功能
	extern OPSTAT ihu_vmmw_gprsmod_udp_text_data_transmit_with_receive(char *input, int16_t inlen, char *output, uint16_t *outlen);  //往后台发送的POST功能
	extern OPSTAT ihu_vmmw_gprsmod_udp_u8_data_transmit_with_receive(int8_t *input, int16_t inlen, int8_t *output, uint16_t *outlen);    //往后台发送的POST功能
	extern OPSTAT ihu_vmmw_gprsmod_ftp_data_transmit_with_receive(int8_t *output, uint16_t *outlen);   //从后台GET数据的功能
	extern OPSTAT ihu_vmmw_gprsmod_email_data_transmit_with_receive(char *emailTo, char *emailFrom, char *emailTitle, char *emailContent, int16_t inlen); //往后台发送的POST功能
	extern OPSTAT ihu_vmmw_gprsmod_bs_position_perform(StrVmmwGprsmodBasestationPosition_t *output);
	extern OPSTAT ihu_vmmw_gprsmod_tts_perform(char *input, int16_t inlen);
	extern int16_t ihu_vmmw_gprsmod_get_rssi_value(void);

//本地函数：目前以SIM800A为例子。如果不是SIM800A，则需要另换一套程序
//由于目前只有SIM800A，所有的程序都以这个为基础。如果未来需要支持第二种以上的模组，本程序待改进
//改进的方案将采用#define配置的方式
OPSTAT func_gprsmod_module_session_init(void);
OPSTAT func_gprsmod_module_info_retrieve(void);//模块信息检测
OPSTAT func_gprsmod_gsm_info_retrieve(void);//SIM800A信息显示(信号质量,电池电量,日期时间)
OPSTAT func_gprsmod_send_AT_command(uint8_t *cmd, uint8_t *ack, UINT16 wait_time);  //秒级！！！
void func_gprsmod_clear_receive_buffer(void);
void func_gprsmod_send_string(char* s);
#define func_gprsmod_send_LR() func_gprsmod_send_string("\r\n")
void func_gprsmod_wait_CREG(void);
void func_grpsmod_set_ATE0(void);

//公共函数，暂时得以保留
OPSTAT func_gprsmod_find_char(char *a);
OPSTAT func_gprsmod_convert_str_to_int(uint8_t *p,uint8_t len);//字符转整型
void func_gprsmod_convert_int_to_str(int n, char str[]);//整型转字符
void func_gprsmod_two_char_swap(char *ch1, char *ch2);
void func_gprsmod_convert_hex_to_str(uint8_t dest[],uint8_t src[],uint8_t len);//十六进制转字符串

//高级定义，简化程序的可读性
#define IHU_ERROR_PRINT_GPRSMOD	zIhuRunErrCnt[TASK_ID_VMFO]++; IhuErrorPrint
//#define IHU_ERROR_PRINT_GPRSMOD(...)	({zIhuRunErrCnt[TASK_ID_VMFO]++;  ((void (*)(const char *, ...))IhuErrorPrint)(__VA_ARGS__);  return IHU_FAILURE;})


#endif /* L1FREERTOS_MOD_GPRS_H_ */



