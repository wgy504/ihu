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
long counter_can_tx_nok = 0;
long counter_can_tx_total = 0;
long counter_can_rx_nok = 0;
long counter_can_rx_total = 0;
#define TEST_PRINT_PERIOD_COUNTER		500

extern long number_of_wmc_combin_timeout;

int ihu_bsp_stm32_can_send_data(uint8_t* buff, uint16_t len)
{ 
	//这里是帧处理的过程，未来待完善数据的发送接收处理过程
	uint16_t CanFrameNumber = 0;
	uint16_t CanTotalFrameLen = 0;
	uint16_t CanCurrentFrameLen = 0;
	uint16_t CanLastFrameLen = (len % 8);
	uint16_t i = 0;
  HAL_StatusTypeDef status;
	
//	static CanTxMsgTypeDef        TxMessage;
//	static CanRxMsgTypeDef        RxMessage;
//	hcan1.pTxMsg = &TxMessage;
//	hcan1.pRxMsg = &RxMessage;
	
	IHU_BSP_STM32_CAN_IAU_HANDLER.pTxMsg->StdId = AWS_CAN_ID;  //CAN ID for AWS
	IHU_BSP_STM32_CAN_IAU_HANDLER.pTxMsg->ExtId = AWS_CAN_ID;
  IHU_BSP_STM32_CAN_IAU_HANDLER.pTxMsg->RTR = CAN_RTR_DATA;
  IHU_BSP_STM32_CAN_IAU_HANDLER.pTxMsg->IDE = CAN_ID_EXT;
	
//	IhuDebugPrint("CANVELA: CAN Send %d bytes [%02X %02X %02X %02X %02X %02X %02X %02X]\n", \
//				len, buff[0], buff[1], buff[2], buff[3], buff[4], buff[5], buff[6], buff[7]);
	
	for(;;) // 8 byte is the max frame lenth for CAN
	{
		if((len - CanTotalFrameLen) >= 8)
		{
			CanCurrentFrameLen = 8;
		}
		else
		{
			CanCurrentFrameLen = CanLastFrameLen;
		}
		/* Fill msg length */
		hcan1.pTxMsg->DLC = CanCurrentFrameLen;
		/* Fill msg body */
		memcpy( hcan1.pTxMsg->Data, &buff[CanTotalFrameLen], CanCurrentFrameLen);

		/* CAN Transmit need to disable Interrupt */
		taskENTER_CRITICAL();
		status = HAL_CAN_Transmit(&hcan1, IHU_BSP_STM32_CAN_TX_MAX_DELAY);
		taskEXIT_CRITICAL();
		counter_can_tx_total++;
		/* CAN Transmit need to disable Interrupt */
		
		
		if (status == HAL_OK)
		{
			CanTotalFrameLen = CanTotalFrameLen + CanCurrentFrameLen;  //Counter how many has been sent
			/*IhuDebugPrint("CANVELA: ihu_bsp_stm32_can_send_data: send %d bytes [%02X %02X %02X %02X %02X %02X %02X %02X]\n", \
			hcan1.pTxMsg->DLC, \
			hcan1.pTxMsg->Data[0], hcan1.pTxMsg->Data[1], \
			hcan1.pTxMsg->Data[2], hcan1.pTxMsg->Data[3], \
			hcan1.pTxMsg->Data[4], hcan1.pTxMsg->Data[5], \
			hcan1.pTxMsg->Data[6], hcan1.pTxMsg->Data[7]); */
			
			ihu_bsp_stm32_led_work_state_f2board_toggle();
			if(CanTotalFrameLen == len) 
			{
				//IhuDebugPrint("CANVELA: ihu_bsp_stm32_can_send_data: CanTotalFrameLen(%d)== len(%d)\n", CanTotalFrameLen, len);
				break; //if all has been sent, break the loop, and exit with normal exit.
			}
			
			//osDelay(1); //make sure we do not break ther OS.
			//IhuDebugPrint("CANVELA: ihu_bsp_stm32_can_send_data: Sent CanTotalFrameLen(%d)\n", CanTotalFrameLen);
		}
		else
		{
			counter_can_tx_nok++;
			IhuErrorPrint("CANVELA: HAL_CAN_Transmit NOK: status=%d, total_nok=%d, MCR=0x%x, MSR=0x%x, TSR=0x%x, ESR=0x%x\n", 
											status, counter_can_tx_nok, hcan1.Instance->MCR, hcan1.Instance->MSR, 
											hcan1.Instance->TSR, hcan1.Instance->ESR);
			
			//TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//TODO: !!!!!!!!!!! CAN Link Faifure NEED TO BE CONSIDERRED !!!!!!!!!!
			//TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			return BSP_FAILURE;
		}
		if( 0 == (counter_can_tx_total % TEST_PRINT_PERIOD_COUNTER))
		{
			printf("%d:T+:%d,%d,R:%d,%d,W:%d\r\n",osKernelSysTick(), counter_can_tx_nok, counter_can_tx_total, counter_can_rx_nok, counter_can_rx_total, number_of_wmc_combin_timeout);				
		}
		
	}
	return BSP_SUCCESS;
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
IHU_HUITP_L2FRAME_Desc_t g_can_packet_desc[2];
uint8_t g_can_rx_buffer[BFSC_CAN_MAX_RX_BUF_SIZE];

//#define IhuDebugPrint(fmt, ...) 

/**
  * @brief  Transmission  complete callback in non blocking mode 
  * @param  CanHandle: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* CanHandle)
{
	HAL_StatusTypeDef status;
	
	IHU_HUITP_L2FRAME_Desc_t *frame_desc;
  //HAL_StatusTypeDef status;
  
	if(CanHandle->Instance == CAN1)
		frame_desc = &g_can_packet_desc[0];
	else
		frame_desc = &g_can_packet_desc[1];
/*
  printf("ExtId 0x%x length %d, data: 0x%08x 0x%08x\n", 
  		CanHandle->pRxMsg->ExtId,
  		CanHandle->pRxMsg->DLC,
  		*(uint32_t *)(&CanHandle->pRxMsg->Data[0]),
  		*(uint32_t *)(&CanHandle->pRxMsg->Data[4])
  		);
*/
	/* PAY ATTENTION THAT THE MASK */
  //if(CanHandle->pRxMsg->StdId == WMC_CAN_ID || (CanHandle->pRxMsg->ExtId) == WMC_CAN_ID)
	if( ((CanHandle->pRxMsg->ExtId & AWS_TO_WMC_CAN_ID_PREFIX) == AWS_TO_WMC_CAN_ID_PREFIX) &&
		  ( (CanHandle->pRxMsg->ExtId & (1<<WMC_CAN_ID_SUFFIX)) == (1<<WMC_CAN_ID_SUFFIX)))
  
  {
  	l2packet_rx_bytes(frame_desc, CanHandle->pRxMsg->Data, CanHandle->pRxMsg->DLC);
  }
	
	/* Receive again */
	status = HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0);
	counter_can_rx_total++;
	
	if(status != HAL_OK)
	{
		counter_can_rx_nok++;
		printf("HAL_CAN_Receive_IT() NOK, status=%d, total_nok=%d, MCR=%d, MSR=%d, TSR=%d, ESR=%d\n", 
		    status, counter_can_rx_nok, hcan1.Instance->MCR, hcan1.Instance->MSR, 
				hcan1.Instance->TSR, hcan1.Instance->ESR);
		
	}
	else
	{
			ihu_bsp_stm32_led_commu_f2board_toggle();
	}
	
  if (0 == ((counter_can_rx_total) % TEST_PRINT_PERIOD_COUNTER))
	{
			printf("%d:T:%d,%d,R+:%d,%d(ID:0x%08X),W:%d\r\n",osKernelSysTick(), counter_can_tx_nok, counter_can_tx_total, counter_can_rx_nok, counter_can_rx_total, CanHandle->pRxMsg->ExtId, number_of_wmc_combin_timeout);				
	}

}

void app_can_loopback_callback(IHU_HUITP_L2FRAME_Desc_t *pdesc)
{
	CAN_HandleTypeDef* CanHandle;
	int ret = 0;

	//assert(pdesc);
	//assert(CanHandle);

	CanHandle = (CAN_HandleTypeDef* )pdesc->UserData;
/*
	printf("CAN ISR: L2Packet %d bytes, first: 0x%02x %02x last: 0x%02x %02x\n", 
		pdesc->RxXferCount,
		CanHandle->pRxMsg->Data[0], CanHandle->pRxMsg->Data[1],
		CanHandle->pRxMsg->Data[6], CanHandle->pRxMsg->Data[7]);
*/
	//Forward to TASK_ID_CANVELA
	ret = ihu_message_send_isr(MSG_ID_CAN_L2FRAME_RCV, TASK_ID_CANVELA, TASK_ID_CANVELA, g_can_rx_buffer, pdesc->RxXferCount);
	if (ret == IHU_FAILURE){
		//zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		printf("CAN ISR: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);
		return;
	}
	//printf("CAN ISR: Send message OK, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);
	
	//bsp_can_transmit(CanHandle, pdesc->pRxBuffPtr, pdesc->RxXferCount, 10);
}

int bsp_can_start_rx(CAN_HandleTypeDef* CanHandle, void (*app_rx_callback)(), uint8_t *pRxBuffPtr, uint16_t rxBufferSize, void *user_data)
{
	IHU_HUITP_L2FRAME_Desc_t *frame_desc;
	HAL_StatusTypeDef status;
	
	if(CanHandle->Instance == CAN1)
		frame_desc = &g_can_packet_desc[0];
	else
		frame_desc = &g_can_packet_desc[1];
	
	memset(frame_desc, 0, sizeof(IHU_HUITP_L2FRAME_Desc_t));
	memset(pRxBuffPtr, 0, rxBufferSize);
	
	frame_desc->RxState = IHU_L2PACKET_RX_STATE_START;
	frame_desc->pRxBuffPtr = pRxBuffPtr;
	frame_desc->RxBuffSize = rxBufferSize;
	frame_desc->RxXferCount = 0;
	frame_desc->app_rx_callback = app_rx_callback;
	frame_desc->UserData = user_data;
	
	status = HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0);
	if(status != HAL_OK)
	{
		/* Reception Error */
    //printf("HAL_CAN_Receive_IT() error... status = %d\n", status);
		return (1);
	}
	else
	{
			ihu_bsp_stm32_led_commu_f2board_toggle();
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
    taskENTER_CRITICAL();
		status = HAL_CAN_Transmit(CanHandle, timeout);
    taskEXIT_CRITICAL();
		if(status == HAL_OK)
		{
			length -= translen;
			buffer += translen;
		}
		else
		{
			IhuErrorPrint("bsp_can_transmit() status=%d\n", status);
			break;
		}
	}

	return length;
}

void bsp_can_config_filter(CAN_HandleTypeDef* CanHandle, uint32_t can_id)
{
	CAN_FilterConfTypeDef  sFilterConfig;
	
	IhuErrorPrint("bsp_can_config_filter() can_id = 0x%08x\r\n", can_id);
	
	/* ref to 1) http://m.blog.csdn.net/article/details?id=52317532 */
	/* ref to 2) Figure 305. Filter bank scale configuration - register organization  */
	/*            in DocID15403 Rev 7, Page 792, Reference Manuel 0033*/
	/* 32 Bit Mask Mode */
	/* FilterIdHigh & FilterIdLow => ID after bit Mask */
	/* FilterMaskIdHigh & FilterMaskIdLow => Mask */
	/* For both ID and Mask, low 3 bit are [bit2=IDE], [bit1=RTR], [bit0=0] */
	/* IDE => Identifier Extension Bit, StdId (0) or ExtId (1) */
	/* RTR => Remote Transmission Request, Data Frame (0), Remote Frame (1) */
    
	/*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterNumber = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = ((can_id <<3) >> 16) & 0xFFFF;
  sFilterConfig.FilterIdLow = ((can_id <<3) & 0xFFFF) | 0x04;   // Extend Frame & Data Frame //
  sFilterConfig.FilterMaskIdHigh = ((((can_id <<3) >> 16) & 0xFFFF));
  sFilterConfig.FilterMaskIdLow = (((can_id <<3) & 0xFFFF)) | 0x04 ;  // Extend Frame & Data Frame //
  sFilterConfig.FilterFIFOAssignment = 0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.BankNumber = 14;
	
	if(HAL_CAN_ConfigFilter(CanHandle, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
		IhuErrorPrint("bsp_can_filter() NOK\r\n");
    return;
  }
}

void bsp_can_init(CAN_HandleTypeDef* CanHandle, uint32_t std_id)
{
	CAN_FilterConfTypeDef  sFilterConfig;
	static CanTxMsgTypeDef        TxMessage;
  static CanRxMsgTypeDef        RxMessage;
	
	CanHandle->pTxMsg = &TxMessage;
	CanHandle->pRxMsg = &RxMessage;
	
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
    return;
  }
	
	/*##-3- Configure Transmission process #####################################*/
	CanHandle->pTxMsg->StdId = AWS_CAN_ID;
  CanHandle->pTxMsg->ExtId = AWS_CAN_ID;
  CanHandle->pTxMsg->RTR = CAN_RTR_DATA;
  CanHandle->pTxMsg->IDE = CAN_ID_EXT;
  CanHandle->pTxMsg->DLC = 0;
	
	bsp_can_start_rx(CanHandle, app_can_loopback_callback, g_can_rx_buffer, BFSC_CAN_MAX_RX_BUF_SIZE, CanHandle);
}

////MYC Forward the complete/valid CAN Frame from CAN Interrupt to CAN Task
////MSG_ID_CAN_L2FRAME_RCV
//OPSTAT complete_wmc_control_frame_forward_canvela(void *p_msg, UINT16 msg_len)
//{
//	int ret = 0;
//	msg_struct_canvela_l2frame_rcv_t rcv;

//	ret = ihu_message_send(MSG_ID_CAN_L3BFSC_CMD_CTRL, TASK_ID_BFSC, TASK_ID_CANVELA, p_msg, msg_len);
//	if (ret == IHU_FAILURE){
//		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
//		IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);
//		return IHU_FAILURE;
//	}	
//	//返回
//	return IHU_SUCCESS;
//}

