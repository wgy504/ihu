/**
  ******************************************************************************
  * 文件名程: bsp_can.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2016-10-04
  * 功    能: 板载调试串口底层驱动程序：默认使用CAN1
  ******************************************************************************
  * 
  ******************************************************************************
*/

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_can.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
//从MAIN.x中继承过来的函数
extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern uint8_t zIhuCanRxBuffer[2];

//从SCYCB项目中继承过来的全局变量，待定
int8_t IHU_BSP_STM32_CAN_IAU_R_Buff[IHU_BSP_STM32_CAN_IAU_REC_MAX_LEN];			//数据接收缓冲区 
int8_t IHU_BSP_STM32_CAN_IAU_R_State=0;																	//接收状态
int16_t IHU_BSP_STM32_CAN_IAU_R_Count=0;																//当前接收数据的字节数
int16_t IHU_BSP_STM32_CAN_IAU_R_Len=0;
int8_t IHU_BSP_STM32_CAN_SPARE1_R_Buff[IHU_BSP_STM32_CAN_SPARE1_REC_MAX_LEN];	//数据接收缓冲区 
int8_t IHU_BSP_STM32_CAN_SPARE1_R_State=0;												//接收状态
int16_t IHU_BSP_STM32_CAN_SPARE1_R_Count=0;										//当前接收数据的字节数 	  
int16_t IHU_BSP_STM32_CAN_SPARE1_R_Len=0;


/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

int ihu_bsp_stm32_can_slave_hw_init(void)
{
	uint16_t k;
	for(k=0;k<IHU_BSP_STM32_CAN_IAU_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		IHU_BSP_STM32_CAN_IAU_R_Buff[k] = 0x00;
	}
  IHU_BSP_STM32_CAN_IAU_R_Count = 0;               //接收字符串的起始存储位置
	IHU_BSP_STM32_CAN_IAU_R_State = 0;
	IHU_BSP_STM32_CAN_IAU_R_Len = 0;

	for(k=0;k<IHU_BSP_STM32_CAN_SPARE1_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		IHU_BSP_STM32_CAN_SPARE1_R_Buff[k] = 0x00;
	}
  IHU_BSP_STM32_CAN_SPARE1_R_Count = 0;               //接收字符串的起始存储位置
	IHU_BSP_STM32_CAN_SPARE1_R_State = 0;
	IHU_BSP_STM32_CAN_SPARE1_R_Len = 0;	
	
	return BSP_SUCCESS;
}

/*******************************************************************************
* 函数名  : CAN_SendData
* 描述    : CAN_IAU发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
int ihu_bsp_stm32_can_send_data(uint8_t* buff, uint16_t len)
{ 
	//这里是帧处理的过程，未来待完善数据的发送接收处理过程	
	if (HAL_CAN_Transmit(&IHU_BSP_STM32_CAN_IAU_HANDLER, IHU_BSP_STM32_CAN_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int ihu_bsp_stm32_can_rcv_data(uint8_t* buff, uint16_t len)
{    
	//这里是帧处理的过程，未来待完善数据的发送接收处理过程	
	if (HAL_CAN_Receive(&IHU_BSP_STM32_CAN_IAU_HANDLER, IHU_BSP_STM32_CAN_IAU_HANDLER_ID-1, IHU_BSP_STM32_CAN_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}



/*******************************************************************************
* 函数名  : CAN_SendData
* 描述    : CAN_SPARE1发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
int ihu_bsp_stm32_can_spare1_send_data(uint8_t* buff, uint16_t len)
{    
	//这里是帧处理的过程，未来待完善数据的发送接收处理过程	
	if (HAL_CAN_Transmit(&IHU_BSP_STM32_CAN_SPARE1_HANDLER, IHU_BSP_STM32_CAN_TX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int ihu_bsp_stm32_can_spare1_rcv_data(uint8_t* buff, uint16_t len)
{    
	//这里是帧处理的过程，未来待完善数据的发送接收处理过程	
	if (HAL_CAN_Receive(&IHU_BSP_STM32_CAN_SPARE1_HANDLER, IHU_BSP_STM32_CAN_SPARE1_HANDLER_ID-1, IHU_BSP_STM32_CAN_RX_MAX_DELAY) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

/**
  * 串口接口完成回调函数的处理
  * 为什么需要重新执行HAL_CAN_Receive_IT，待确定
  */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *CanHandle)
{
	uint8_t res = 0;
	msg_struct_canvela_l2frame_rcv_t snd;
  if(CanHandle==&IHU_BSP_STM32_CAN_IAU_HANDLER)
  {
		//这里是帧处理的过程，未来待完善数据的接收处理过程
		//res = zIhuCanRxBuffer[IHU_BSP_STM32_CAN_IAU_HANDLER_ID-1];
		IHU_BSP_STM32_CAN_IAU_R_Buff[IHU_BSP_STM32_CAN_IAU_R_Count++] = res;
		if (IHU_BSP_STM32_CAN_IAU_R_Count >= IHU_BSP_STM32_CAN_IAU_REC_MAX_LEN)
			IHU_BSP_STM32_CAN_IAU_R_Count = 0;

		//为了IDLE状态下提高效率，直接分解为IDLE和ELSE
		if (IHU_BSP_STM32_CAN_IAU_R_State == IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE)
		{
			//只有满足这么苛刻的条件，才算找到了帧头
			if ((res == IHU_HUITP_L2FRAME_STD_RX_START_FLAG_CHAR) && (IHU_BSP_STM32_CAN_IAU_R_Count == 1))
			IHU_BSP_STM32_CAN_IAU_R_State = IHU_HUITP_L2FRAME_STD_RX_STATE_START;
		}
		else
		{
			//收到CHECKSUM
			if((IHU_BSP_STM32_CAN_IAU_R_State == IHU_HUITP_L2FRAME_STD_RX_STATE_START) && (IHU_BSP_STM32_CAN_IAU_R_Count == 2))
			{
				IHU_BSP_STM32_CAN_IAU_R_State = IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM;
			}
			//收到长度高位
			else if((IHU_BSP_STM32_CAN_IAU_R_State == IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM) && (IHU_BSP_STM32_CAN_IAU_R_Count == 3))
			{
				IHU_BSP_STM32_CAN_IAU_R_State = IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN;
			}
			//收到长度低位
			else if((IHU_BSP_STM32_CAN_IAU_R_State == IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN) && (IHU_BSP_STM32_CAN_IAU_R_Count == 4))
			{
				IHU_BSP_STM32_CAN_IAU_R_Len = ((IHU_BSP_STM32_CAN_IAU_R_Buff[2] <<8) + IHU_BSP_STM32_CAN_IAU_R_Buff[3]);
				//CHECKSUM及判定
				if ((IHU_BSP_STM32_CAN_IAU_R_Buff[1] == (IHU_BSP_STM32_CAN_IAU_R_Buff[0] ^ IHU_BSP_STM32_CAN_IAU_R_Buff[2]^IHU_BSP_STM32_CAN_IAU_R_Buff[3])) &&\
					(IHU_BSP_STM32_CAN_IAU_R_Len < IHU_BSP_STM32_CAN_IAU_REC_MAX_LEN-4))
				IHU_BSP_STM32_CAN_IAU_R_State = IHU_HUITP_L2FRAME_STD_RX_STATE_BODY;
			}
			//收到BODY位
			else if((IHU_BSP_STM32_CAN_IAU_R_State == IHU_HUITP_L2FRAME_STD_RX_STATE_BODY) && (IHU_BSP_STM32_CAN_IAU_R_Len > 1))
			{
				IHU_BSP_STM32_CAN_IAU_R_Len--;
			}
			//收到BODY最后一位
			else if((IHU_BSP_STM32_CAN_IAU_R_State == IHU_HUITP_L2FRAME_STD_RX_STATE_BODY) && (IHU_BSP_STM32_CAN_IAU_R_Len == 1))
			{
				IHU_BSP_STM32_CAN_IAU_R_State = IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE;
				IHU_BSP_STM32_CAN_IAU_R_Len = 0;
				IHU_BSP_STM32_CAN_IAU_R_Count = 0;
				//发送数据到上层CANARIES模块
				memset(&snd, 0, sizeof(msg_struct_canvela_l2frame_rcv_t));
				memcpy(snd.data, &IHU_BSP_STM32_CAN_IAU_R_Buff[4], ((IHU_BSP_STM32_CAN_IAU_R_Buff[2]<<8)+IHU_BSP_STM32_CAN_IAU_R_Buff[3]));
				snd.length = sizeof(msg_struct_canvela_l2frame_rcv_t);				
				ihu_message_send(MSG_ID_SPS_L2FRAME_RCV, TASK_ID_CANVELA, TASK_ID_VMFO, &snd, snd.length);				
			}
			//差错情况
			else{
				IHU_BSP_STM32_CAN_IAU_R_State = IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE;
				IHU_BSP_STM32_CAN_IAU_R_Len = 0;
				IHU_BSP_STM32_CAN_IAU_R_Count = 0;
			}
		}
		//重新设置中断
		HAL_CAN_Receive_IT(&IHU_BSP_STM32_CAN_IAU_HANDLER, IHU_BSP_STM32_CAN_IAU_HANDLER_ID-1);
  }
  else if(CanHandle==&IHU_BSP_STM32_CAN_SPARE1_HANDLER)
  {
		//这里是帧处理的过程，未来待完善数据的接收处理过程		
		//IHU_BSP_STM32_CAN_SPARE1_R_Buff[IHU_BSP_STM32_CAN_SPARE1_R_Count] = zIhuCanRxBuffer[IHU_BSP_STM32_CAN_SPARE1_HANDLER_ID-1];
		IHU_BSP_STM32_CAN_SPARE1_R_Count++;
		if (IHU_BSP_STM32_CAN_SPARE1_R_Count >= IHU_BSP_STM32_CAN_SPARE1_REC_MAX_LEN)
			IHU_BSP_STM32_CAN_SPARE1_R_Count = 0;
		HAL_CAN_Receive_IT(&IHU_BSP_STM32_CAN_SPARE1_HANDLER, IHU_BSP_STM32_CAN_SPARE1_HANDLER_ID);
  }
}





