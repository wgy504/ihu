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

//从MAIN.x中继承过来的函数
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
extern CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2; //MAIN中未定义，这里重新定义是为了复用
#endif

extern uint8_t zIhuCanRxBuffer[2];

//本地全局变量
int8_t zIhuBspStm32CanIauRxBuff[IHU_BSP_STM32_CAN_IAU_REC_MAX_LEN];			//数据接收缓冲区 
int8_t zIhuBspStm32CanIauRxState=0;																	//接收状态
int16_t zIhuBspStm32CanIauRxCount=0;																//当前接收数据的字节数
int16_t zIhuBspStm32CanIauRxLen=0;
int8_t zIhuBspStm32CanSpare1RxBuff[IHU_BSP_STM32_CAN_SPARE1_REC_MAX_LEN];	//数据接收缓冲区 
int8_t zIhuBspStm32CanSpare1RxState=0;												//接收状态
int16_t zIhuBspStm32CanSpare1RxCount=0;										//当前接收数据的字节数 	  
int16_t zIhuBspStm32CanSpare1RxLen=0;


/* 函数体 --------------------------------------------------------------------*/

int ihu_bsp_stm32_can_slave_hw_init(void)
{
	uint16_t k;
	for(k=0;k<IHU_BSP_STM32_CAN_IAU_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32CanIauRxBuff[k] = 0x00;
	}
  zIhuBspStm32CanIauRxCount = 0;               //接收字符串的起始存储位置
	zIhuBspStm32CanIauRxState = 0;
	zIhuBspStm32CanIauRxLen = 0;

	for(k=0;k<IHU_BSP_STM32_CAN_SPARE1_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32CanSpare1RxBuff[k] = 0x00;
	}
  zIhuBspStm32CanSpare1RxCount = 0;               //接收字符串的起始存储位置
	zIhuBspStm32CanSpare1RxState = 0;
	zIhuBspStm32CanSpare1RxLen = 0;	
	
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
void HAL_CAN_RxCpltCallback1111(CAN_HandleTypeDef *CanHandle)   // Puhuix: rename the function....
{
	uint8_t res = 0;
	msg_struct_canvela_l2frame_rcv_t snd;
  if(CanHandle==&IHU_BSP_STM32_CAN_IAU_HANDLER)
  {
		//这里是帧处理的过程，未来待完善数据的接收处理过程
		//res = zIhuCanRxBuffer[IHU_BSP_STM32_CAN_IAU_HANDLER_ID-1];
		zIhuBspStm32CanIauRxBuff[zIhuBspStm32CanIauRxCount++] = res;
		if (zIhuBspStm32CanIauRxCount >= IHU_BSP_STM32_CAN_IAU_REC_MAX_LEN)
			zIhuBspStm32CanIauRxCount = 0;

		//为了IDLE状态下提高效率，直接分解为IDLE和ELSE
		if (zIhuBspStm32CanIauRxState == IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE)
		{
			//只有满足这么苛刻的条件，才算找到了帧头
			if ((res == IHU_HUITP_L2FRAME_STD_RX_START_FLAG_CHAR) && (zIhuBspStm32CanIauRxCount == 1))
			zIhuBspStm32CanIauRxState = IHU_HUITP_L2FRAME_STD_RX_STATE_START;
		}
		else
		{
			//收到CHECKSUM
			if((zIhuBspStm32CanIauRxState == IHU_HUITP_L2FRAME_STD_RX_STATE_START) && (zIhuBspStm32CanIauRxCount == 2))
			{
				zIhuBspStm32CanIauRxState = IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM;
			}
			//收到长度高位
			else if((zIhuBspStm32CanIauRxState == IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM) && (zIhuBspStm32CanIauRxCount == 3))
			{
				zIhuBspStm32CanIauRxState = IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN;
			}
			//收到长度低位
			else if((zIhuBspStm32CanIauRxState == IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN) && (zIhuBspStm32CanIauRxCount == 4))
			{
				zIhuBspStm32CanIauRxLen = ((zIhuBspStm32CanIauRxBuff[2] <<8) + zIhuBspStm32CanIauRxBuff[3]);
				//CHECKSUM及判定
				if ((zIhuBspStm32CanIauRxBuff[1] == (zIhuBspStm32CanIauRxBuff[0] ^ zIhuBspStm32CanIauRxBuff[2]^zIhuBspStm32CanIauRxBuff[3])) &&\
					(zIhuBspStm32CanIauRxLen < IHU_BSP_STM32_CAN_IAU_REC_MAX_LEN-4))
				zIhuBspStm32CanIauRxState = IHU_HUITP_L2FRAME_STD_RX_STATE_BODY;
			}
			//收到BODY位
			else if((zIhuBspStm32CanIauRxState == IHU_HUITP_L2FRAME_STD_RX_STATE_BODY) && (zIhuBspStm32CanIauRxLen > 1))
			{
				zIhuBspStm32CanIauRxLen--;
			}
			//收到BODY最后一位
			else if((zIhuBspStm32CanIauRxState == IHU_HUITP_L2FRAME_STD_RX_STATE_BODY) && (zIhuBspStm32CanIauRxLen == 1))
			{
				zIhuBspStm32CanIauRxState = IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE;
				zIhuBspStm32CanIauRxLen = 0;
				zIhuBspStm32CanIauRxCount = 0;
				//发送数据到上层CANARIES模块
				memset(&snd, 0, sizeof(msg_struct_canvela_l2frame_rcv_t));
				memcpy(snd.data, &zIhuBspStm32CanIauRxBuff[4], ((zIhuBspStm32CanIauRxBuff[2]<<8)+zIhuBspStm32CanIauRxBuff[3]));
				snd.length = sizeof(msg_struct_canvela_l2frame_rcv_t);				
				ihu_message_send(MSG_ID_SPS_L2FRAME_RCV, TASK_ID_CANVELA, TASK_ID_VMFO, &snd, snd.length);				
			}
			//差错情况
			else{
				zIhuBspStm32CanIauRxState = IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE;
				zIhuBspStm32CanIauRxLen = 0;
				zIhuBspStm32CanIauRxCount = 0;
			}
		}
		//重新设置中断
		HAL_CAN_Receive_IT(&IHU_BSP_STM32_CAN_IAU_HANDLER, IHU_BSP_STM32_CAN_IAU_HANDLER_ID-1);
  }
  else if(CanHandle==&IHU_BSP_STM32_CAN_SPARE1_HANDLER)
  {
		//这里是帧处理的过程，未来待完善数据的接收处理过程		
		//zIhuBspStm32CanSpare1RxBuff[zIhuBspStm32CanSpare1RxCount] = zIhuCanRxBuffer[IHU_BSP_STM32_CAN_SPARE1_HANDLER_ID-1];
		zIhuBspStm32CanSpare1RxCount++;
		if (zIhuBspStm32CanSpare1RxCount >= IHU_BSP_STM32_CAN_SPARE1_REC_MAX_LEN)
			zIhuBspStm32CanSpare1RxCount = 0;
		HAL_CAN_Receive_IT(&IHU_BSP_STM32_CAN_SPARE1_HANDLER, IHU_BSP_STM32_CAN_SPARE1_HANDLER_ID);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//  CAN interface by puhuix
/////////////////////////////////////////////////////////////////////////////////////////////////
#include "l2packet.h"

#define BFSC_CAN_MAX_RX_BUF_SIZE 256
IHU_HUITP_L2FRAME_Desc_t g_can_packet_desc;
uint8_t g_can_rx_buffer[BFSC_CAN_MAX_RX_BUF_SIZE];

/**
  * @brief  Configures the CAN.
  * @param  CanHandle
  * @retval None
  */
static void CAN_Config(CAN_HandleTypeDef *CanHandle)
{
  CAN_FilterConfTypeDef  sFilterConfig;
  static CanTxMsgTypeDef        TxMessage;
  static CanRxMsgTypeDef        RxMessage;
  
  /*##-1- Configure the CAN peripheral #######################################*/
  CanHandle->Instance = CAN1;
  CanHandle->pTxMsg = &TxMessage;
  CanHandle->pRxMsg = &RxMessage;
    
  CanHandle->Init.TTCM = DISABLE;
  CanHandle->Init.ABOM = DISABLE;
  CanHandle->Init.AWUM = DISABLE;
  CanHandle->Init.NART = DISABLE;
  CanHandle->Init.RFLM = DISABLE;
  CanHandle->Init.TXFP = DISABLE;
  CanHandle->Init.Mode = CAN_MODE_NORMAL;
  CanHandle->Init.SJW = CAN_SJW_1TQ;       // set baudrate as 500kbps
  CanHandle->Init.BS1 = CAN_BS1_12TQ;
  CanHandle->Init.BS2 = CAN_BS2_2TQ;
  CanHandle->Init.Prescaler = 4;
  
  if(HAL_CAN_Init(CanHandle) != HAL_OK)
  {
    /* Initialization Error */
    IhuDebugPrint("HAL_CAN_Init() error.\n");
	return;
  }

  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterNumber = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = 0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.BankNumber = 14;
  
  if(HAL_CAN_ConfigFilter(CanHandle, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    IhuDebugPrint("HAL_CAN_ConfigFilter() error.\n");
	return;
  }
      
  /*##-3- Configure Transmission process #####################################*/
  CanHandle->pTxMsg->StdId = 0x321;
  CanHandle->pTxMsg->ExtId = 0x01;
  CanHandle->pTxMsg->RTR = CAN_RTR_DATA;
  CanHandle->pTxMsg->IDE = CAN_ID_STD;
  CanHandle->pTxMsg->DLC = 2;
}

/**
  * @brief  Transmission  complete callback in non blocking mode 
  * @param  CanHandle: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* CanHandle)
{
	IhuDebugPrint("stdId 0x%x length %d, data: 0x%08x 0x%08x\n", 
		CanHandle->pRxMsg->StdId,
		CanHandle->pRxMsg->DLC,
		*(uint32_t *)(&CanHandle->pRxMsg->Data[0]),
		*(uint32_t *)(&CanHandle->pRxMsg->Data[4])
		);
	
	l2packet_rx_bytes(&g_can_packet_desc, CanHandle->pRxMsg->Data, CanHandle->pRxMsg->DLC);
	
	/* Receive again */
	if(HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0) != HAL_OK)
	{
		/* Reception Error */
		IhuDebugPrint("HAL_CAN_Receive_IT() error.\n");
	}
}

int bsp_can_start_rx(CAN_HandleTypeDef* CanHandle, void (*rx_complete_callback)(), void *user_data)
{
	g_can_packet_desc.RxState = IHU_L2PACKET_RX_STATE_START;
	g_can_packet_desc.pRxBuffPtr = g_can_rx_buffer;
	g_can_packet_desc.RxBuffSize = BFSC_CAN_MAX_RX_BUF_SIZE;
	g_can_packet_desc.RxXferCount = 0;
	g_can_packet_desc.rx_complete_callback = rx_complete_callback;
	g_can_packet_desc.UserData = user_data;
	
	if(HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0) != HAL_OK)
	{
		/* Reception Error */
		return (1);
	}
	
	return 0;
}

/* return the size of transmitted */
uint32_t bsp_can_transmit(CAN_HandleTypeDef* CanHandle, uint8_t *buffer, uint32_t length, uint32_t timeout)
{
	uint8_t translen;
	HAL_StatusTypeDef status;
	
	while(length > 0)
	{
		translen = (length > 8)?8:length;
		CanHandle->pTxMsg->DLC = translen;
		memcpy(CanHandle->pTxMsg->Data, buffer, translen);
		status = HAL_CAN_Transmit(CanHandle, timeout);
		if(status == HAL_OK)
		{
			length -= translen;
			buffer += translen;
		}
		else
		{
			IhuDebugPrint("bsp_can_transmit() status=%d\n", status);
			break;
		}
	}

	return length;
}

void bsp_can_init(CAN_HandleTypeDef* CanHandle)
{
	CAN_Config(CanHandle);
}
