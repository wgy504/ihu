/**
 ****************************************************************************************
 *
 * @file gprs.h
 *
 * @brief GPRS module control
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */

#ifndef L1VUCOS_GPRS_H_
#define L1VUCOS_GPRS_H_

#include "stm32f2xx.h"
#include "string.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "vmucoslayer.h"
#include "bsp.h"


#define Buf1_Max 200 					  //串口1缓存长度
#define Buf2_Max 200 					  //串口2缓存长度

/*************  本地变量声明	**************/
u16 USART_RX_STA;
u8 Uart1_Buf[Buf1_Max];//串口1数据缓存区
u8 Uart2_Buf[Buf2_Max];//串口2数据缓存区
u8 First_Int = 0,shijian=0;
u16 Times=0;      //延时变量
u8 Time_UART1=0;  //串口1计时器
u8 Time_UART2=0;  //串口2计时器
u8 Time_miao=0;

u8 Timer0_start=0;	//定时器0延时启动计数器
u8 Uart2_Start=0;	  //串口2开始接收数据
u8 Uart2_End=0;	    //串口2接收数据结束

/*************	本地函数声明	**************/
void GPIO_config(void);//GPIO端口配置
void Timer0Init(void);//定时器0初始化
void CLR_Buf2(void);//清除串口2接收缓存
u8 GSM_send_cmd(u8 *cmd,u8 *ack,u8 wait_time);//模块指令发送函数
void GSM_test(void);//模块测试主程序
void GSM_mtest(void);//模块信息检测
u8 GSM_gsminfo(void);//GSM信息显示(信号质量,电池电量,日期时间)
u8 GSM_call_test(void);//拨号测试程序
u8 GSM_sms_test(void);//短信测试程序
u8 GSM_gprs_test(void);//GPRS数据传输测试程序
u8 change_str_Data(u8 *p,u8 len);//字符转整型
void change_Data_str(int n, char str[]);//整型转字符
void Swap(char *ch1, char *ch2);
void change_hex_str(u8 dest[],u8 src[],u8 len);//十六进制转字符串
u8 GSM_jz_test(void);//基站定位
u8 GSM_tts_test(void);//TTS文本语音测试程序
void USART_PRINT_Send_Len(char *s,u8 len);
void USART_PRINT_Data(char byte);
void USART_PRINT_SendString(char* s);
void SPS_GPRS_Data(char byte);

//Working Procedure
int GPRS_control_gsm_test(void);


/*************  外部函数和变量声明*****************/
void SPS_GPRS_clear_receive_buffer(void);
u8 SPS_GPRS_Find_char(char *a);
u8 SPS_GPRS_send_AT_command(char *cmd, char *ack, u16 wait_time);
void SPS_GPRS_SendString(char* s);
#define SPS_GPRS_SendLR() SPS_GPRS_SendString("\r\n")
void SPS_GPRS_Wait_CREG(void);
void SPS_GPRS_Set_ATE0(void);
void SPS_GPRS_connect_server(void);
void SPS_GPRS_Rec_Server_Data(void);
void SPS_GPRS_data_connection_and_receive_process(void);


#endif /* L1VUCOS_GPRS_H_ */

