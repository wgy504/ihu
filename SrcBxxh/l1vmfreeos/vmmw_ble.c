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
 
#include "vmmw_ble.h"

//全局变量，引用外部
extern int8_t zIhuBspStm32SpsBleRxBuff[IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN];			//串口BLE数据接收缓冲区 
extern int8_t zIhuBspStm32SpsBleRxState;																				//串口BLE接收状态
extern int16_t zIhuBspStm32SpsBleRxCount;																				//当前接收数据的字节数 	  
extern int16_t zIhuBspStm32SpsBleRxLen;

/*******************************************************************************
* 函数名 : func_blemod_uart_send_AT_command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、发送等待时间(单位：S)
* 输出   : 
* 返回   : 1:正常  -1:错误
* 注意   : 
* OPSTAT func_blemod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, uint8_t wait_time)  
* 这里的发送，只有成功返回ACK对应的回复时，才算成功
*******************************************************************************/
OPSTAT func_blemod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, uint16_t wait_time) //in Second
{
	int res;
	
	//等待的时间长度，到底是以tick为单位的，还是以ms为单位的？经过验证，都是以ms为单位的，所以不用担心！！！
	uint32_t tickTotal = wait_time * 1000 / IHU_BSP_STM32_SPS_RX_MAX_DELAY;

	//清理接收缓冲区
	func_blemod_uart_clear_receive_buffer();
	ihu_l1hd_sps_ble_send_data((uint8_t *)cmd, strlen((char*)cmd));
	func_blemod_uart_send_LR();	
	
	res = IHU_FAILURE;
	while((tickTotal > 0) && (res == IHU_FAILURE))
	{
		ihu_usleep(IHU_BSP_STM32_SPS_RX_MAX_DELAY); //这里的周期就是以绝对ms为单位的
		tickTotal--;
		if(strstr((const char*)zIhuBspStm32SpsBleRxBuff, (char*)ack)==NULL)
			 res = IHU_FAILURE;
		else
			 res = IHU_SUCCESS;
	}
	return res;
}

/*******************************************************************************
* 函数名 : func_blemod_uart_clear_receive_buffer
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void func_blemod_uart_clear_receive_buffer(void)
{
	uint16_t k;
	for(k=0;k<IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpsBleRxBuff[k] = 0x00;
	}
  zIhuBspStm32SpsBleRxCount = 0;               //接收字符串的起始存储位置
}

/*******************************************************************************
* 函数名  : USART_BLE_SendString
* 描述    : USART_BLE发送字符串
* 输入    : *s字符串指针
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void func_blemod_uart_send_string(char* s)
{
	while(*s != '\0')//检测字符串结束符
	{
		ihu_l1hd_sps_ble_send_data((uint8_t *)s++, 1);
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
OPSTAT ihu_vmmw_blemod_hc05_uart_fetch_mac_addr_procedure(void)
{
	uint8_t repeatCnt = IHU_BLE_UART_REPEAT_CNT;
//	uint8_t temp[50];
//	uint8_t loc=0;
	
	//设置BLE模块拉高的工作状态
	ihu_l1hd_dido_f2board_ble_power_ctrl_on();
	
	func_blemod_uart_clear_receive_buffer();
	//最大循环次数
	while((repeatCnt > 0) && (func_blemod_uart_send_AT_command((uint8_t*)"AT", (uint8_t*)"OK", 2) != IHU_SUCCESS))//查询是否应到AT指令
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
	func_blemod_uart_clear_receive_buffer();
	if (func_blemod_uart_send_AT_command((uint8_t*)"AT+VERSION?", (uint8_t*)"OK", 2) == IHU_SUCCESS) {
		if(strstr((const char*)zIhuBspStm32SpsBleRxBuff, "+VERSION:") != NULL){
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: BLE Version = [%s]!\n", zIhuBspStm32SpsBleRxBuff);
		}
	}else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: BLE inquery version failure!\n");
		return IHU_FAILURE;
	}	
	
	//获取地址
	func_blemod_uart_clear_receive_buffer();
	if (func_blemod_uart_send_AT_command((uint8_t*)"AT+ADDR?", (uint8_t*)"OK", 2) == IHU_SUCCESS) {
		if(strstr((const char*)zIhuBspStm32SpsBleRxBuff, "+ADDR::") != NULL){
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: BLE Address = [%s]!\n", zIhuBspStm32SpsBleRxBuff);
		}
	}else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: BLE fetch address failure!\n");
		return IHU_FAILURE;
	}
	
	//如何将获得的地址返回给上层，待上层需求

	//设置BLE模块拉低的蓝牙正常工作状态
	ihu_l1hd_dido_f2board_ble_power_ctrl_off();

	return IHU_SUCCESS;
}
