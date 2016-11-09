/**
 ****************************************************************************************
 *
 * @file mod_ble.c
 *
 * @brief BLE module control
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */
 
#include "mod_hal_ble.h"

//全局变量，引用外部
extern int8_t BSP_STM32_SPS_BLE_R_Buff[BSP_STM32_SPS_BLE_REC_MAXLEN];			//串口BLE数据接收缓冲区 
extern int8_t BSP_STM32_SPS_BLE_R_State;												//串口BLE接收状态
extern int16_t BSP_STM32_SPS_BLE_R_Count;					//当前接收数据的字节数 	  


/*******************************************************************************
* 函数名 : BLE_UART_send_AT_command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、发送等待时间(单位：S)
* 输出   : 
* 返回   : 1:正常  -1:错误
* 注意   : 
* OPSTAT BLE_UART_send_AT_command(uint8_t *cmd, uint8_t *ack, uint8_t wait_time)  
* 这里的发送，只有成功返回ACK对应的回复时，才算成功
*******************************************************************************/
OPSTAT BLE_UART_send_AT_command(uint8_t *cmd, uint8_t *ack, uint16_t wait_time) //in Second
{
	int res;
	
	//等待的时间长度，到底是以tick为单位的，还是以ms为单位的？经过验证，都是以ms为单位的，所以不用担心！！！
	uint32_t tickTotal = wait_time * 1000 / SPS_UART_RX_MAX_DELAY_DURATION;

	//清理接收缓冲区
	BLE_UART_clear_receive_buffer();
	BSP_STM32_SPS_BLE_SendData((uint8_t *)cmd, strlen((char*)cmd));
	BLE_UART_SendLR();	
	
	res = IHU_FAILURE;
	while((tickTotal > 0) && (res == IHU_FAILURE))
	{
		ihu_usleep(SPS_UART_RX_MAX_DELAY_DURATION); //这里的周期就是以绝对ms为单位的
		tickTotal--;
		if(strstr((const char*)BSP_STM32_SPS_BLE_R_Buff, (char*)ack)==NULL)
			 res = IHU_FAILURE;
		else
			 res = IHU_SUCCESS;
	}
	return res;
}

/*******************************************************************************
* 函数名 : BLE_UART_clear_receive_buffer
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void BLE_UART_clear_receive_buffer(void)
{
	uint16_t k;
	for(k=0;k<BSP_STM32_SPS_BLE_REC_MAXLEN;k++)      //将缓存内容清零
	{
		BSP_STM32_SPS_BLE_R_Buff[k] = 0x00;
	}
  BSP_STM32_SPS_BLE_R_Count = 0;               //接收字符串的起始存储位置
}

/*******************************************************************************
* 函数名  : USART_BLE_SendString
* 描述    : USART_BLE发送字符串
* 输入    : *s字符串指针
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void BLE_UART_SendString(char* s)
{
	while(*s != '\0')//检测字符串结束符
	{
		BSP_STM32_SPS_BLE_SendData((uint8_t *)s++, 1);
	}
}

/*******************************************************************************
* 函数名 : BLE_UART_fetch_mac_add_procedure
* 描述   : 获取BLE MAC地址测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
OPSTAT BLE_HC05_UART_fetch_mac_add_procedure(void)
{
	uint8_t repeatCnt = IHU_BLE_UART_REPEAT_CNT;
//	uint8_t temp[50];
//	uint8_t loc=0;
	
	//设置BLE模块拉高的工作状态
	BLE_UART_PORT_ENABLE();
	
	BLE_UART_clear_receive_buffer();
	//最大循环次数
	while((repeatCnt > 0) && (BLE_UART_send_AT_command((uint8_t*)"AT", (uint8_t*)"OK", 2) != IHU_SUCCESS))//查询是否应到AT指令
	{
		repeatCnt--;
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Not detect BLE module, trying to reconnecting!\n");
		}
		ihu_usleep(200);
		if (repeatCnt == 0){
			zIhuRunErrCnt[TASK_ID_VMFO]++;
			IhuErrorPrint("VMFO: BLE detect failure!\n");
			return IHU_FAILURE;
		}
	}
	
	//查阅版本
	BLE_UART_clear_receive_buffer();
	if (BLE_UART_send_AT_command((uint8_t*)"AT+VERSION?", (uint8_t*)"OK", 2) == IHU_SUCCESS) {
		if(strstr((const char*)BSP_STM32_SPS_BLE_R_Buff, "+VERSION:") != NULL){
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: BLE Version = [%s]!\n", BSP_STM32_SPS_BLE_R_Buff);
		}
	}else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: BLE inquery version failure!\n");
		return IHU_FAILURE;
	}	
	
	//获取地址
	BLE_UART_clear_receive_buffer();
	if (BLE_UART_send_AT_command((uint8_t*)"AT+ADDR?", (uint8_t*)"OK", 2) == IHU_SUCCESS) {
		if(strstr((const char*)BSP_STM32_SPS_BLE_R_Buff, "+ADDR::") != NULL){
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: BLE Address = [%s]!\n", BSP_STM32_SPS_BLE_R_Buff);
		}
	}else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: BLE fetch address failure!\n");
		return IHU_FAILURE;
	}
	
	//如何将获得的地址返回给上层，待上层需求

	//设置BLE模块拉低的蓝牙正常工作状态
	BLE_UART_PORT_DISABLE();

	return IHU_SUCCESS;
}
