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
//当使用串口时
extern int8_t 	zIhuBspStm32SpsRfidRxBuff[IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN];			//串口RFID数据接收缓冲区 
extern int16_t	zIhuBspStm32SpsRfidRxCount;																					//当前接收数据的字节数 	  
//当使用SPI接口时
#if (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI1)
	extern int8_t 	zIhuBspStm32SpiGeneral1RxBuff[IHU_BSP_STM32_SPI1_GENERAL_REC_MAX_LEN];				//SPI数据接收缓冲区 
	extern int16_t 	zIhuBspStm32SpiGeneral1RxCount;																							//当前接收数据的字节数 	  
#elif (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI2)
	extern int8_t 	zIhuBspStm32SpiGeneral2RxBuff[IHU_BSP_STM32_SPI2_GENERAL_REC_MAX_LEN];				//SPI数据接收缓冲区 
	extern int16_t 	zIhuBspStm32SpiGeneral2RxCount;																							//当前接收数据的字节数
#endif

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
	func_rfidmod_clear_receive_buffer();
	ihu_l1hd_sps_rfid_send_data((uint8_t *)cmd, strlen((char*)cmd));
	func_rfidmod_send_LR();	
	
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

//in Second
//使用#if的方式，是因为SPI可能根本不存在，但USART是强迫存在的VMMW模块，不得已，不然可以使用if-else-end
OPSTAT func_rfidmod_wait_command_fb(uint8_t *ack, uint16_t wait_time)
{
	int ret = 0;
	
	//等待的时间长度，到底是以tick为单位的，还是以ms为单位的？经过验证，都是以ms为单位的，所以不用担心！！！
	uint32_t tickTotal = wait_time * 1000 / IHU_BSP_STM32_SPS_RX_MAX_DELAY;

	//清理接收缓冲区
	ret = IHU_FAILURE;

	while((tickTotal > 0) && (ret == IHU_FAILURE))
	{
		ihu_usleep(IHU_BSP_STM32_SPS_RX_MAX_DELAY); //这里的周期就是以绝对ms为单位的
		tickTotal--;

#if (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPS_RFID)
		if(strstr((const char*)zIhuBspStm32SpsRfidRxBuff, (char*)ack)==NULL)
			 ret = IHU_FAILURE;
		else
			 ret = IHU_SUCCESS;
#elif (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI1)
		if(strstr((const char*)zIhuBspStm32SpiGeneral1RxBuff, (char*)ack)==NULL)
			 ret = IHU_FAILURE;
		else
			 ret = IHU_SUCCESS;
#elif (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI2)
		if(strstr((const char*)zIhuBspStm32SpiGeneral2RxBuff, (char*)ack)==NULL)
			 ret = IHU_FAILURE;
		else
			 ret = IHU_SUCCESS;
#else
	#error Un-correct constant definition
#endif
	}
	return ret;
}


/*******************************************************************************
* 函数名 : func_rfidmod_clear_receive_buffer
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
//使用#if的方式，是因为SPI可能根本不存在，但USART是强迫存在的VMMW模块，不得已，不然可以使用if-else-end
void func_rfidmod_clear_receive_buffer(void)
{
	uint16_t k;
#if (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPS_RFID)
	for(k=0;k<IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpsRfidRxBuff[k] = 0x00;
	}
	zIhuBspStm32SpsRfidRxCount = 0;               //接收字符串的起始存储位置
#elif (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI1)
	for(k=0;k<IHU_BSP_STM32_SPI1_GENERAL_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpiGeneral1RxBuff[k] = 0x00;
	}
	zIhuBspStm32SpiGeneral1RxCount = 0;               //接收字符串的起始存储位置
#elif (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI2)
	for(k=0;k<IHU_BSP_STM32_SPI2_GENERAL_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpiGeneral2RxBuff[k] = 0x00;
	}
	zIhuBspStm32SpiGeneral2RxCount = 0;               //接收字符串的起始存储位置
#else
	#error Un-correct constant definition
#endif
}

/*******************************************************************************
* 函数名  : USART_RFID_SendString
* 描述    : USART_RFID发送字符串
* 输入    : *s字符串指针
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void func_rfidmod_send_string(char* s)
{
	while(*s != '\0')//检测字符串结束符
	{
		ihu_l1hd_sps_rfid_send_data((uint8_t *)s++, 1);
	}
}


//发送命令
OPSTAT ihu_vmmw_rfidmod_spi_send_command(uint8_t *command)
{
	return IHU_SUCCESS;
}

//读取RFID卡的数据，或者说扫描ID
OPSTAT ihu_vmmw_rfidmod_spi_read_id(uint8_t *output)
{
	return IHU_SUCCESS;
}


