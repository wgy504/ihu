/**
 ****************************************************************************************
 *
 * @file mod_rfid.c
 *
 * @brief RFID module control
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */
 
#include "vmmw_rfid.h"

//全局变量，引用外部
extern int8_t 	zIhuBspStm32SpsRfidRxBuff[IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN];			//串口BLE数据接收缓冲区 
extern int8_t 	zIhuBspStm32SpsRfidRxState;																					//串口RFID接收状态
extern int16_t	zIhuBspStm32SpsRfidRxCount;																					//当前接收数据的字节数 	  
extern int16_t zIhuBspStm32SpsRfidRxLen;

/*******************************************************************************
* 函数名 : func_rfidmod_uart_send_AT_command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、发送等待时间(单位：S)
* 输出   : 
* 返回   : 1:正常  -1:错误
* 注意   : 
* OPSTAT func_rfidmod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, uint8_t wait_time)  
* 这里的发送，只有成功返回ACK对应的回复时，才算成功
*******************************************************************************/
OPSTAT func_rfidmod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, uint16_t wait_time) //in Second
{
	int res;
	
	//等待的时间长度，到底是以tick为单位的，还是以ms为单位的？经过验证，都是以ms为单位的，所以不用担心！！！
	uint32_t tickTotal = wait_time * 1000 / IHU_BSP_STM32_SPS_RX_MAX_DELAY;

	//清理接收缓冲区
	func_rfidmod_uart_clear_receive_buffer();
	ihu_l1hd_sps_rfid_send_data((uint8_t *)cmd, strlen((char*)cmd));
	func_rfidmod_uart_send_LR();	
	
	res = IHU_FAILURE;
	while((tickTotal > 0) && (res == IHU_FAILURE))
	{
		ihu_usleep(IHU_BSP_STM32_SPS_RX_MAX_DELAY); //这里的周期就是以绝对ms为单位的
		tickTotal--;
		if(strstr((const char*)zIhuBspStm32SpsRfidRxBuff, (char*)ack)==NULL)
			 res = IHU_FAILURE;
		else
			 res = IHU_SUCCESS;
	}
	return res;
}

/*******************************************************************************
* 函数名 : func_rfidmod_uart_clear_receive_buffer
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void func_rfidmod_uart_clear_receive_buffer(void)
{
	uint16_t k;
	for(k=0;k<IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpsRfidRxBuff[k] = 0x00;
	}
  zIhuBspStm32SpsRfidRxCount = 0;               //接收字符串的起始存储位置
}

/*******************************************************************************
* 函数名  : USART_RFID_SendString
* 描述    : USART_RFID发送字符串
* 输入    : *s字符串指针
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void func_rfidmod_uart_send_string(char* s)
{
	while(*s != '\0')//检测字符串结束符
	{
		ihu_l1hd_sps_rfid_send_data((uint8_t *)s++, 1);
	}
}


