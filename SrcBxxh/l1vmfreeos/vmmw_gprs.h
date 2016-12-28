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

//全局使用的常量定义
#define IHU_VMMW_GPRSMOD_TIM_CHAN_ID 		TIM2
#define IHU_VMMW_GPRSMOD_TTS_MAX_LEN 200 //定义最多播放的字节数
#define IHU_VMMW_GPRSMOD_REPEAT_CNT 3
#define IHU_VMMW_GPRSMOD_TEST_SMS_CONTENT   "THIS IS ZZZ TEST MESSAGE!"
#define IHU_VMMW_GPRSMOD_TEST_CALLED_NUMBER "+8613701629240"
#define IHU_VMMW_GPRSMOD_TEST_DTMF_KEY "X"
#define IHU_VMMW_GPRSMOD_STREAM_CHOICE "TCP"   //"UDP"
#define IHU_VMMW_GPRSMOD_STEAM_CONTENT "THIS iS A PURE TEST!"

//向上提供全局统一服务的入口
extern OPSTAT ihu_vmmw_gprsmod_gsm_working_procedure_selection(uint8_t option, uint8_t sub_opt);
extern OPSTAT ihu_vmmw_gprsmod_gsm_info_procedure(void);//SIM800A信息显示(信号质量,电池电量,日期时间)
extern OPSTAT ihu_vmmw_gprsmod_module_procedure(void);//模块信息检测
extern OPSTAT ihu_vmmw_gprsmod_call_procedure(uint8_t sub_opt);//拨号测试程序
extern OPSTAT ihu_vmmw_gprsmod_sms_procedure(void); //短信工作流程
extern OPSTAT ihu_vmmw_gprsmod_gprs_procedure(uint8_t sub_opt);//GPRS数据传输测试程序
extern OPSTAT ihu_vmmw_gprsmod_bs_procedure(void);//基站定位
extern OPSTAT ihu_vmmw_gprsmod_tts_procedure(void);//TTS文本语音测试程序

//本地函数：目前以SIM800A为例子。如果不是SIM800A，则需要另换一套程序
OPSTAT func_gprsmod_sim800a_gsm_loop_test_main(void);
OPSTAT func_gprsmod_sim800a_test_loop(void);//模块测试主程序
OPSTAT func_gprsmod_sim800a_gsm_test_info(void);//SIM800A信息显示(信号质量,电池电量,日期时间)
OPSTAT func_gprsmod_sim800a_mtest(void);//模块信息检测
OPSTAT func_gprsmod_sim800a_call_test(void);//拨号测试程序
OPSTAT func_gprsmod_sim800a_sms_test(void);//短信测试程序
OPSTAT func_gprsmod_sim800a_gprs_test(void);//GPRS数据传输测试程序
OPSTAT func_gprsmod_sim800a_jz_test(void);//基站定位
OPSTAT func_gprsmod_sim800a_tts_test(void);//TTS文本语音测试程序
void func_gprsmod_sim800a_connect_server(void);
void func_gprsmod_sim800a_rec_server_data(void);
void func_gprsmod_sim800a_data_connection_and_receive_process(void);

//核心函数：跟采用哪一种GPRS模组没有太大关系
OPSTAT func_gprsmod_send_AT_command(uint8_t *cmd, uint8_t *ack, UINT16 wait_time);  //秒级！！！
void   func_gprsmod_clear_receive_buffer(void);
void   func_gprsmod_wait_CREG(void);
void   func_grpsmod_set_ATE0(void);
void   func_gprsmod_data_byte_send(char byte);
void   func_gprsmod_send_string(char* s);
#define func_gprsmod_send_LR() func_gprsmod_send_string("\r\n")

//公共函数，暂时得以保留
OPSTAT func_gprsmod_find_char(char *a);
OPSTAT func_gprsmod_convert_str_to_int(uint8_t *p,uint8_t len);//字符转整型
void   func_gprsmod_convert_int_to_str(int n, char str[]);//整型转字符
void   func_gprsmod_two_char_swap(char *ch1, char *ch2);
void   func_gprsmod_convert_hex_to_str(uint8_t dest[],uint8_t src[],uint8_t len);//十六进制转字符串

//要被去掉的函数，待完善
void func_gprsmod_print_send_len(char *s, uint8_t len);
void func_gprsmod_print_data(char byte);
void func_gprsmod_print_send_string(char* s);
//extern void USART_PRINT_IRQHandler(void);
//extern void TIM_USART_GPRS_IRQHandler(void);

#endif /* L1FREERTOS_MOD_GPRS_H_ */

