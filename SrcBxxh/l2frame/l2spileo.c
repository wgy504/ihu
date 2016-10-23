/**
 ****************************************************************************************
 *
 * @file l2spileo.c
 *
 * @brief L2 SPILEO
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
 #include "l2spileo.h"
 #include "bsp.h"
 
///////////////////////////////////////////////////////////////////////////////////////
// redefine macros
///////////////////////////////////////////////////////////////////////////////////////
#ifdef SPI_IT_TXE
#undef SPI_IT_TXE
#endif
#define SPI_IT_TXE                   SPI_CR2_TXEIE

#ifdef SPI_IT_RXNE
#undef SPI_IT_RXNE
#endif
#define SPI_IT_RXNE                  SPI_CR2_RXNEIE

#ifdef SPI_IT_ERR
#undef SPI_IT_ERR
#endif
#define SPI_IT_ERR                   SPI_CR2_ERRIE

#ifdef SPI_FLAG_RXNE
#undef SPI_FLAG_RXNE
#endif
#define SPI_FLAG_RXNE                SPI_SR_RXNE

#ifdef SPI_FLAG_TXE
#undef SPI_FLAG_TXE
#endif
#define SPI_FLAG_TXE                 SPI_SR_TXE

#ifdef SPI_FLAG_OVR
#undef SPI_FLAG_OVR
#endif
#define SPI_FLAG_OVR                 SPI_SR_OVR

#ifdef SPI_FLAG_BSY
#undef SPI_FLAG_BSY
#endif
#define SPI_FLAG_BSY                 SPI_SR_BSY

///////////////////////////////////////////////////////////////////////////////////////

/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;
#define SPILEO_BUF_SIZE 256
uint8_t spileo_rx_buffer[SPILEO_BUF_SIZE];
uint8_t spileo_tx_buffer[SPILEO_BUF_SIZE];

int spileo_start_transmit(SPI_HandleTypeDef *hspi, uint8_t *tx_buffer, uint16_t size);
int spileo_start_receive(SPI_HandleTypeDef *hspi, uint8_t *rx_buffer, uint16_t size);

uint8_t spileo_gen_chksum(l2spileo_msgheader_t *pMsgHeader)
{
	uint8_t *pData = (uint8_t *)pMsgHeader;
	
	return pData[0]^pData[2]^pData[3];
}

static void spileo_rx_isr(SPI_HandleTypeDef *hspi)
{
  uint16_t tmp;
  
  if(hspi->RxXferCount >= hspi->RxXferSize)
	{
	  // buffer is full or Rx isn't started, drop the data
	  tmp = hspi->Instance->DR;
	  UNUSED(tmp);
	}
	else
	{
	  l2spileo_msgheader_t *pMsgHeader = (l2spileo_msgheader_t *)hspi->pRxBuffPtr;
	  
	  hspi->pRxBuffPtr[hspi->RxXferCount++] = hspi->Instance->DR;

	  switch(hspi->RxState)
	  {

	  case SPI_RX_STATE_START:
		if(pMsgHeader->start != EXT_BORAD_START_CHAR)
		{
		  // not synchonrized, reset the RxXferCount
		  hspi->RxXferCount = 0;
		}
		else
		  hspi->RxState = SPI_RX_STATE_HEADER;
		break;
		
	  case SPI_RX_STATE_HEADER:
		if(hspi->RxXferCount == sizeof(l2spileo_msgheader_t))
		{
		  if(spileo_gen_chksum(pMsgHeader) != pMsgHeader->chksum)
		  {
			hspi->RxXferCount = 0;
			hspi->RxState = SPI_RX_STATE_START;
		  }
		  else
		  {
			hspi->RxState = SPI_RX_STATE_BODY;
		  }
		}
		else if(hspi->RxXferCount > sizeof(l2spileo_msgheader_t))
		{
			/* error case, just reset */
			hspi->RxXferCount = 0;
			hspi->RxState = SPI_RX_STATE_START;
		}
		break;
		
	  case SPI_RX_STATE_BODY:
		if(hspi->RxXferCount == pMsgHeader->len)
		{
		  if(hspi->RxCompleteCallback)
				hspi->RxCompleteCallback(hspi);
			hspi->RxState = SPI_RX_STATE_START;
			hspi->RxXferCount = 0;
		}

		if(hspi->RxXferCount >= hspi->RxXferSize)
		{
		  hspi->RxXferCount = 0;
		  hspi->RxState = SPI_RX_STATE_START;
		}
		break;
	  }
	}
}

static void spileo_tx_isr(SPI_HandleTypeDef *hspi)
{
  if(hspi->TxXferCount >= hspi->TxXferSize)
	{
	  // no pending data, send pading
	  hspi->Instance->DR = EXT_BOARD_PADDING_CH;
	}
	else
	{
	  hspi->Instance->DR = hspi->pTxBuffPtr[hspi->TxXferCount++];
	}
	
	/* if there isn't more data to transmit, disable TXE interrupt (must...) */
	if(hspi->TxXferCount == hspi->TxXferSize)
	{
	  __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE));
	  __HAL_SPI_CLEAR_OVRFLAG(hspi);

	  // call user's callback after the transfer is complete
	  if(hspi->TxCompleteCallback)
	    hspi->TxCompleteCallback(hspi);
	}
}


/**
  * @brief  This function handles SPI interrupt request.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval HAL status
  */
void HAL_SPI_IRQHandler()
{
  uint32_t tmp1 = 0, tmp2 = 0, tmp3 = 0;
  SPI_HandleTypeDef *hspi = &SpiHandle;

  tmp1 = __HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE);
  tmp2 = __HAL_SPI_GET_IT_SOURCE(hspi, SPI_IT_RXNE);
  tmp3 = __HAL_SPI_GET_FLAG(hspi, SPI_FLAG_OVR);
	/*
	IhuDebugPrint("HAL_SPI_IRQHandler: SR 0x%08x CR2 0x%08x\n", 
		__HAL_SPI_GET_FLAG(hspi, 0xFFFF), 
		__HAL_SPI_GET_IT_SOURCE(hspi, 0xFFFF));
	*/
	
  /* SPI in mode Receiver and Overrun not occurred ---------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET) && (tmp3 == RESET))
  {
    hspi->RxISR(hspi);
    return;
  }

  tmp1 = __HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE);
  tmp2 = __HAL_SPI_GET_IT_SOURCE(hspi, SPI_IT_TXE);
  /* SPI in mode Transmitter ---------------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    hspi->TxISR(hspi);
    return;
  }

  if(__HAL_SPI_GET_IT_SOURCE(hspi, SPI_IT_ERR) != RESET)
  {
    // journal_user_defined(10, (0x3<<30)|hspi->Instance->SR); // puhuix
    /* SPI CRC error interrupt occurred ---------------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR) != RESET)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_CRC;
      __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
    }
    /* SPI Mode Fault error interrupt occurred --------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_MODF) != RESET)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_MODF;
      __HAL_SPI_CLEAR_MODFFLAG(hspi);
    }
    
    /* SPI Overrun error interrupt occurred -----------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_OVR) != RESET)
    {
      if(hspi->State != HAL_SPI_STATE_BUSY_TX)
      {
        hspi->ErrorCode |= HAL_SPI_ERROR_OVR;
        __HAL_SPI_CLEAR_OVRFLAG(hspi);      
      }
    }

    /* SPI Frame error interrupt occurred -------------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, 0x00000100/* SPI_FLAG_FRE */) != RESET)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_FRE;
      __HAL_SPI_CLEAR_FREFLAG(hspi);
    }

    /* Call the Error call Back in case of Errors */
    if(hspi->ErrorCode!=HAL_SPI_ERROR_NONE)
    {
      hspi->State = HAL_SPI_STATE_READY;
      IhuErrorPrint("SPI ErrorCode: 0x%08x\n\r", hspi->ErrorCode);
    }
  }
}

static void spileo_rx_complete(SPI_HandleTypeDef *hspi)
{
/*
  IhuDebugPrint("spi RX complete (%d): packet 0x%08x 0x%08x 0x%08x\n\r", 
                hspi->RxXferCount, *(uint32_t *)&hspi->pRxBuffPtr[0], *(uint32_t *)&hspi->pRxBuffPtr[4], *(uint32_t *)&hspi->pRxBuffPtr[8]);
*/

  if(*(uint16_t *)(&spileo_rx_buffer[4]) == 0xEEEE)
  {
    /* for loopback test */
		l2spileo_msgheader_t *pMsgHeader = (l2spileo_msgheader_t *)spileo_rx_buffer;
		uint16_t msglen = pMsgHeader->len > SPILEO_BUF_SIZE? SPILEO_BUF_SIZE: pMsgHeader->len;
		
		memcpy(spileo_tx_buffer, spileo_rx_buffer, msglen);
		spileo_start_transmit(hspi, spileo_tx_buffer, msglen);
  }
	else
	{
    /* notify the application */
  }
}

int spileo_start_receive(SPI_HandleTypeDef *hspi, uint8_t *rx_buffer, uint16_t size)
{
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();

  /* set RX buffer */
  hspi->RxState = SPI_RX_STATE_START;
  hspi->RxXferCount = 0;
  hspi->RxXferSize = size;
  hspi->pRxBuffPtr = rx_buffer;

  /* set RX complete callback */
  hspi->RxCompleteCallback = spileo_rx_complete;

  CPU_CRITICAL_EXIT();
  
  /* enable RXNE interrupt */
  __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_RXNE | SPI_IT_ERR));

  /* Check if the SPI is already enabled */ 
  if((hspi->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
  {
    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);
  }
  
  return 0;
}

static void spileo_tx_complete(SPI_HandleTypeDef *hspi)
{
/*
	IhuDebugPrint("spi TX complete: packet 0x%08x 0x%08x size %d\n\r", 
                *(uint32_t *)&hspi->pTxBuffPtr[0], *(uint32_t *)&hspi->pTxBuffPtr[4], hspi->TxXferCount);
*/
}

int spileo_start_transmit(SPI_HandleTypeDef *hspi, uint8_t *tx_buffer, uint16_t size)
{
  CPU_SR_ALLOC();

  if(hspi->TxXferCount < hspi->TxXferSize)
  {
  	IhuErrorPrint("SPI transmit ongoing...\n");
  	return -1;
  }

  CPU_CRITICAL_ENTER();
  
  hspi->pTxBuffPtr = tx_buffer;
  hspi->TxXferCount = 0;
  hspi->TxXferSize = size;

  hspi->TxCompleteCallback = spileo_tx_complete;
  
  CPU_CRITICAL_EXIT();

  /* enable TXE interrupt */
  __HAL_SPI_ENABLE_IT(hspi, SPI_IT_TXE);

  /* Check if the SPI is already enabled */ 
  if((hspi->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
  {
    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);
  }

  return 0;
}

/**
  * @brief SPI MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - NVIC configuration for SPI interrupt request enable
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  if (hspi->Instance == SPIx)
  {
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
	  RCC_AHB1PeriphClockCmd(SPIx_SCK_RCC_GPIO, ENABLE);
	  RCC_AHB1PeriphClockCmd(SPIx_MISO_RCC_GPIO, ENABLE);
	  RCC_AHB1PeriphClockCmd(SPIx_MOSI_RCC_GPIO, ENABLE);
	
    /* Enable SPI clock */
    SPIx_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.GPIO_Pin       = SPIx_SCK_PIN;
    GPIO_InitStruct.GPIO_Mode      = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType     = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd      = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed     = GPIO_Speed_50MHz;
    GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);
		GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN_SOURCE, SPIx_SCK_AF);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.GPIO_Pin = SPIx_MISO_PIN;
    GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);
		GPIO_PinAFConfig(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN_SOURCE, SPIx_MISO_AF);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.GPIO_Pin = SPIx_MOSI_PIN;
    GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);
		GPIO_PinAFConfig(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN_SOURCE, SPIx_MOSI_AF);

    /*##-3- Configure the NVIC for SPI #########################################*/
    
  }
}

/**
  * @brief  Initializes the SPI according to the specified parameters 
  *         in the SPI_InitTypeDef and create the associated handle.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
{
  /* Check the SPI handle allocation */
  if(hspi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_SPI_MODE(hspi->Init.Mode));
  assert_param(IS_SPI_DIRECTION_MODE(hspi->Init.Direction));
  assert_param(IS_SPI_DATASIZE(hspi->Init.DataSize));
  assert_param(IS_SPI_CPOL(hspi->Init.CLKPolarity));
  assert_param(IS_SPI_CPHA(hspi->Init.CLKPhase));
  assert_param(IS_SPI_NSS(hspi->Init.NSS));
  assert_param(IS_SPI_BAUDRATE_PRESCALER(hspi->Init.BaudRatePrescaler));
  assert_param(IS_SPI_FIRST_BIT(hspi->Init.FirstBit));
  //assert_param(IS_SPI_TIMODE(hspi->Init.TIMode));
  //assert_param(IS_SPI_CRC_CALCULATION(hspi->Init.CRCCalculation));
  assert_param(IS_SPI_CRC_POLYNOMIAL(hspi->Init.CRCPolynomial));

  if(hspi->State == HAL_SPI_STATE_RESET)
  {
    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
    HAL_SPI_MspInit(hspi);
  }
  
  hspi->State = HAL_SPI_STATE_BUSY;

  /* Disable the selected SPI peripheral */
  __HAL_SPI_DISABLE(hspi);

  /*----------------------- SPIx CR1 & CR2 Configuration ---------------------*/
  /* Configure : SPI Mode, Communication Mode, Data size, Clock polarity and phase, NSS management,
  Communication speed, First bit and CRC calculation state */
  hspi->Instance->CR1 = (hspi->Init.SPI_Mode | hspi->Init.SPI_Direction | hspi->Init.SPI_DataSize |
                         hspi->Init.SPI_CPOL | hspi->Init.SPI_CPHA | (hspi->Init.SPI_NSS & SPI_CR1_SSM) |
                         hspi->Init.SPI_BaudRatePrescaler | hspi->Init.SPI_FirstBit);

  /* Configure : NSS management */
  hspi->Instance->CR2 = (((hspi->Init.SPI_NSS >> 16) & SPI_CR2_SSOE));

  /*---------------------------- SPIx CRCPOLY Configuration ------------------*/
  /* Configure : CRC Polynomial */
  hspi->Instance->CRCPR = hspi->Init.SPI_CRCPolynomial;

  /* Activate the SPI mode (Make sure that I2SMOD bit in I2SCFGR register is reset) */
  hspi->Instance->I2SCFGR &= (uint32_t)(~SPI_I2SCFGR_I2SMOD);

  hspi->ErrorCode = HAL_SPI_ERROR_NONE;
  hspi->State = HAL_SPI_STATE_READY;
  
  return HAL_OK;
}


/*
** FSM of the SPILEO
*/
FsmStateItem_t FsmSpileo[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_spileo_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_spileo_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_spileo_restart},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_SPILEO_INITED,         					fsm_spileo_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_SPILEO_INITED,         					fsm_spileo_stop_rcv},

	//Task level actived status
  {MSG_ID_COM_RESTART,        						FSM_STATE_SPILEO_ACTIVED,         					fsm_spileo_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_SPILEO_ACTIVED,         					fsm_spileo_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_SPILEO_ACTIVED,         				  fsm_spileo_time_out},
  {MSG_ID_SPI_DL_DATA_PUSH_FWD,						FSM_STATE_SPILEO_ACTIVED,         					fsm_spileo_dl_data_push_fwd},
  {MSG_ID_SPI_DL_CTRL_CMD_REQ,						FSM_STATE_SPILEO_ACTIVED,         					fsm_spileo_dl_ctrl_cmd_req},
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_spileo_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_SPILEO, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("SPILEO: Error Set FSM State at fsm_spileo_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_spileo_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//串行会送INIT_FB给VM，不然消息队列不够深度，此为节省内存机制
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM。由于消息队列不够深，所以不再回送FB证实。
//		msg_struct_com_init_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
//		snd.length = sizeof(msg_struct_com_init_fb_t);
//		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_SPILEO, &snd, snd.length);
//		if (ret == IHU_FAILURE){
//			IhuErrorPrint("SPILEO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_SPILEO], zIhuTaskNameList[src_id]);
//			return IHU_FAILURE;
//		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_SPILEO, FSM_STATE_SPILEO_INITED) == IHU_FAILURE){
		IhuErrorPrint("SPILEO: Error Set FSM State!\n");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_spileo_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("SPILEO: Error initialize interface!\n");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_SPILEO] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SPILEO, FSM_STATE_SPILEO_ACTIVED) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_SPILEO]++;
		IhuErrorPrint("SPILEO: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//启动本地定时器，如果有必要
	//测试性启动周期性定时器
	ret = ihu_timer_start(TASK_ID_SPILEO, TIMER_ID_1S_SPILEO_PERIOD_SCAN, zIhuSysEngPar.timer.spileoPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_SPILEO]++;
		IhuErrorPrint("SPILEO: Error start timer!\n");
		return IHU_FAILURE;
	}	
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("SPILEO: Enter FSM_STATE_SPILEO_ACTIVE status, Keeping refresh here!\n");
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_spileo_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("SPILEO: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_spileo_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_spileo_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_SPILEO)){
		zIhuRunErrCnt[TASK_ID_SPILEO]++;
		IhuErrorPrint("SPILEO: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SPILEO, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_SPILEO]++;
		IhuErrorPrint("SPILEO: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_spileo_dl_data_push_fwd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_SPILEO)){
		zIhuRunErrCnt[TASK_ID_SPILEO]++;
		IhuErrorPrint("SPILEO: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	
	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_spileo_dl_ctrl_cmd_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_SPILEO)){
		zIhuRunErrCnt[TASK_ID_SPILEO]++;
		IhuErrorPrint("SPILEO: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_spileo_hw_init(void)
{
	SPI_HandleTypeDef *hspi = &SpiHandle;
	
	/******************************************************************************
	SPI init
	******************************************************************************/
	/*##-1- Configure the SPI peripheral #######################################*/
	/* Set the SPI parameters */
	hspi->Instance = SPIx;

	SPI_StructInit(&hspi->Init);
	hspi->Init.SPI_NSS = SPI_NSS_Soft;
	hspi->Init.SPI_CPHA = SPI_CPHA_1Edge;
	hspi->Init.SPI_CPOL = SPI_CPOL_High;

	hspi->RxISR = spileo_rx_isr;
	hspi->TxISR = spileo_tx_isr;
	
	if(HAL_SPI_Init(hspi) != HAL_OK)
	{
		/* Initialization Error */
		IhuErrorPrint("HAL_SPI_Init() failed.\n");
		return ERROR;
	}

	/* attach SPI interrupt */
	BSP_IntVectSet(BSP_INT_ID_SPI2, HAL_SPI_IRQHandler);
	BSP_IntEn(BSP_INT_ID_SPI2);
	
	spileo_start_receive(hspi, spileo_rx_buffer, SPILEO_BUF_SIZE);
	
	IhuDebugPrint("func_spileo_hw_init() done.\n");

	return IHU_SUCCESS;
}

OPSTAT fsm_spileo_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("SPILEO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_SPILEO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuRunErrCnt[TASK_ID_SPILEO]是否超限
	if (zIhuRunErrCnt[TASK_ID_SPILEO] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuRunErrCnt[TASK_ID_SPILEO] = zIhuRunErrCnt[TASK_ID_SPILEO] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_SPILEO, TASK_ID_SPILEO, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_SPILEO]++;
			IhuErrorPrint("SPILEO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_SPILEO], zIhuTaskNameList[TASK_ID_SPILEO]);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_SPILEO_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_SPILEO) != FSM_STATE_SPILEO_ACTIVED){
			ret = FsmSetState(TASK_ID_SPILEO, FSM_STATE_SPILEO_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_SPILEO]++;
				IhuErrorPrint("SPILEO: Error Set FSM State!\n");
				return IHU_FAILURE;
			}//FsmSetState
		}
		func_spileo_time_out_period_scan();
	}

	return IHU_SUCCESS;
}

void func_spileo_time_out_period_scan(void)
{
	IhuDebugPrint("SPILEO: Time Out Test!\n");
}



