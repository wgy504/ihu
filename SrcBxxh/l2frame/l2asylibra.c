/**
 ****************************************************************************************
 *
 * @file l2asylibra.c
 *
 * @brief L2 ASYLIBRA
 *
 * BXXH team
 * Created by ZJL, 20160127
 *
 ****************************************************************************************
 */
 
#include "l2asylibra.h"
 
/*
** FSM of the ASYLIBRA
*/
FsmStateItem_t FsmAsylibra[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_asylibra_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_asylibra_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_asylibra_restart},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_ASYLIBRA_INITED,         					fsm_asylibra_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_ASYLIBRA_INITED,         					fsm_asylibra_stop_rcv},  //消息逻辑的来源待定	
  {MSG_ID_VMDA_ASYLIBRA_BLE_CON,					FSM_STATE_ASYLIBRA_INITED,         					fsm_asylibra_ble_con_rcv},

  //Task level BLE connection
  {MSG_ID_COM_RESTART,        						FSM_STATE_ASYLIBRA_BLE_CONNECTED,         	fsm_asylibra_restart}, 	
  {MSG_ID_VMDA_ASYLIBRA_BLE_DISC,					FSM_STATE_ASYLIBRA_BLE_CONNECTED,         	fsm_asylibra_ble_disc_rcv},
  {MSG_ID_VMDA_ASYLIBRA_DATA_DL,					FSM_STATE_ASYLIBRA_BLE_CONNECTED,         	fsm_asylibra_ble_data_dl_rcv},
  {MSG_ID_COM_TIME_OUT,										FSM_STATE_ASYLIBRA_BLE_CONNECTED,         	fsm_asylibra_time_out},
	
  //Task level BLE Authrozed ok, after receive AUTH_RESP and then send out INIT
  {MSG_ID_COM_RESTART,        						FSM_STATE_ASYLIBRA_AUTH_OK,         				fsm_asylibra_restart},
  {MSG_ID_VMDA_ASYLIBRA_BLE_DISC,					FSM_STATE_ASYLIBRA_AUTH_OK,         				fsm_asylibra_ble_disc_rcv},
  {MSG_ID_VMDA_ASYLIBRA_DATA_DL,					FSM_STATE_ASYLIBRA_AUTH_OK,         				fsm_asylibra_ble_data_dl_rcv},
  {MSG_ID_COM_TIME_OUT,										FSM_STATE_ASYLIBRA_AUTH_OK,         				fsm_asylibra_time_out},
	
  //Task level BLE Init accomplished and data ready to transfer, after receive INIT_RESP
  {MSG_ID_COM_RESTART,        						FSM_STATE_ASYLIBRA_DATA_READY,         			fsm_asylibra_restart}, 	
  {MSG_ID_VMDA_ASYLIBRA_BLE_DISC,					FSM_STATE_ASYLIBRA_DATA_READY,         			fsm_asylibra_ble_disc_rcv},
  {MSG_ID_VMDA_ASYLIBRA_DATA_DL,					FSM_STATE_ASYLIBRA_DATA_READY,         			fsm_asylibra_ble_data_dl_rcv},
  {MSG_ID_EMC_ASYLIBRA_DATA_REQ,					FSM_STATE_ASYLIBRA_DATA_READY,         			fsm_asylibra_emc_data_req},	
	
  //Task level data send and wait for confirmation status
  {MSG_ID_COM_RESTART,        						FSM_STATE_ASYLIBRA_DATA_WF_CONF,         		fsm_asylibra_restart},
  {MSG_ID_VMDA_ASYLIBRA_BLE_DISC,					FSM_STATE_ASYLIBRA_DATA_WF_CONF,         		fsm_asylibra_ble_disc_rcv},
  {MSG_ID_VMDA_ASYLIBRA_DATA_DL,					FSM_STATE_ASYLIBRA_DATA_WF_CONF,         		fsm_asylibra_ble_data_dl_rcv},	
  {MSG_ID_COM_TIME_OUT,										FSM_STATE_ASYLIBRA_DATA_WF_CONF,         		fsm_asylibra_time_out},
	
	//备份过程，未来不需要
	//{MSG_ID_EMC_ASYLIBRA_DATA_REPORT,				FSM_STATE_ASYLIBRA_DATA_READY,         			fsm_asylibra_emc_data_report},	
	//{MSG_ID_EMC_ASYLIBRA_TIME_SYNC_REQ,			FSM_STATE_ASYLIBRA_DATA_READY,         			fsm_asylibra_emc_time_sync_req},
	//{MSG_ID_EMC_ASYLIBRA_EQU_INFO_SYNC,			FSM_STATE_ASYLIBRA_DATA_READY,         			fsm_asylibra_emc_equ_info_sync},
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
IhuAsylibraLinkCtrlTable_t zIhuAsyLinkCtx;

/*
 *
 *   状态转移机API函数过程
 *
 */

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_asylibra_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_IDLE) == FAILURE){
		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_IDLE State at fsm_asylibra_task_entry.");
	}
	//拉灯状态
	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_IDLE);
	return SUCCESS;
}

//任务模块初始化过程
OPSTAT fsm_asylibra_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	int ret=0;
	char strDebug[BX_PRINT_SZ];

	//串行会送INIT_FB给VMDA，不然消息队列不够深度，此为节省内存机制
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VMDA
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_ASYLIBRA, &snd, snd.length);
		if (ret == FAILURE){
			sprintf(strDebug, "ASYLIBRA: Send MSG_ID_COM_INIT_FB message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[src_id]);
			IhuErrorPrint(strDebug);
			return FAILURE;
		}
	}

	//初始化硬件接口
	if (func_asylibra_hw_init() == FAILURE){	
		IhuErrorPrint("ASYLIBRA: Error func_asylibra_hw_init initialize interface!");
		return FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_ASYLIBRA] = 0;
	memset(&zIhuAsyLinkCtx, 0, sizeof(IhuAsylibraLinkCtrlTable_t));
	
	uint8_t md5_type_id[16] = {0x79, 0xC1, 0x92, 0xD6, 0xE7, 0xEA, 0x6B, 0xB9, 0x7E, 0x1C, 0x2C, 0x44, 0xD3, 0xAB, 0x92, 0x43};
	uint8_t device_key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

	memcpy(zIhuAsyLinkCtx.authReq.md5_type_id, md5_type_id, sizeof(md5_type_id));
	memcpy(zIhuAsyLinkCtx.authReq.device_key, device_key, sizeof(device_key));
	strcpy(zIhuAsyLinkCtx.authReq.device_name, "WeChat_Device");
	strcpy(zIhuAsyLinkCtx.authReq.device_id, "Dev_001");
	strcpy(zIhuAsyLinkCtx.authReq.timezone, "UTC+8");
	strcpy(zIhuAsyLinkCtx.authReq.language, "zh-CN");	
	
	//收到初始化消息后，进入初始化状态, 设置状态机到目标状态, 
	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_INITED) == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_ASYLIBRA_INITED State!");
		return FAILURE;
	}
	//拉灯状态
	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_INITED);
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("ASYLIBRA: Enter FSM_STATE_ASYLIBRA_INITED status, Keeping refresh here!");
	}

	//返回
	return SUCCESS;
}

//模块RESTART过程
//需要在模块运行过程中的某些关键差错地方，设置RESTART机制，未来待进一步研究和完善
OPSTAT fsm_asylibra_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{	
	IhuErrorPrint("ASYLIBRA: Internal error counter reach DEAD level, SW-RESTART soon!");
	fsm_asylibra_init(0, 0, NULL, 0);
	
	return SUCCESS;
}

OPSTAT fsm_asylibra_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_ASYLIBRA)){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Wrong fsm_asylibra_stop_rcv input paramters!");
		return FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_IDLE) == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_IDLE State!");
		return FAILURE;
	}
	//拉灯状态
	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_IDLE);
	
	//返回
	return SUCCESS;
}

OPSTAT fsm_asylibra_ble_con_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	int ret=0;
	char strDebug[BX_PRINT_SZ];
	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_ASYLIBRA)){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Wrong fsm_asylibra_ble_con_rcv input paramters!");
		return FAILURE;
	}
	
	//收消息
	msg_struct_vmda_asylibra_ble_con_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_vmda_asylibra_ble_con_t));
	if (param_len > sizeof(msg_struct_vmda_asylibra_ble_con_t)){
		IhuErrorPrint("ASYLIBRA: Receive MSG_ID_VMDA_ASY_BLE_CON message error!");
		zIhuRunErrCnt[TASK_ID_VMDASHELL]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_BLE_CONNECTED) == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_ASYLIBRA_BLE_CONNECTED State!");
		return FAILURE;
	}
	//拉灯状态
	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_BLE_CONNECTED);

	//业务逻辑处理
	//触发AUTH发送消息，并启动定时器，一旦超时将恢复到初始状态
	//这里有一个问题：BLE_CON收到后，底层BLE的链路能坚持多久？AUTH过程超时后，该继续重发，还是等待底层的第二次BLE_CON消息来临？需要测试得知！！！
	//如果等待BLE_CON来启动第二次AUTH过程，则应该回到FSM_STATE_ASYLIBRA_INITED，否则就应该待在FSM_STATE_ASYLIBRA_BLE_CONNECTED继续重试

	//这里的这种状态机非常精妙！！！
	//还有一个问题：AUTH的发送需要等待底层BLE的一个钩子，以便更加安全的发送AUTH，由于目前该钩子指示还未找到，临时采用的方式就是等待超时
	//这里的处理并没有采用超时处理方式，而采用主循环延迟处理的方式，本来就是一种等待超时的方式
	
	//链路zIhuAsySeqId重置，协议规定从1开始
	zIhuAsyLinkCtx.seqId = 1;
	
	//发送AUTH消息，利用发送消息的dataBuf当缓冲区，节省内存的消耗
	//使用MSG_ID_ASYLIBRA_VMDA_DATA_UL消息发送给VMDA
	msg_struct_vmda_asylibra_data_ul_t snd;
	memset(&snd, 0, sizeof(msg_struct_vmda_asylibra_data_ul_t));
	if (func_asylibra_pack_auth_req(snd.dataBuf, snd.length) == FAILURE){
		IhuErrorPrint(strDebug);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		//恢复到空闲状态，等待下次链接的到来
		FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_INITED);
		//拉灯状态
		ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_INITED);		
		return FAILURE;		
	}
	ret = ihu_message_send(MSG_ID_ASYLIBRA_VMDA_DATA_UL, TASK_ID_VMDASHELL, TASK_ID_ASYLIBRA, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_VMDA_DATA_UL message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_VMDASHELL]);
		IhuErrorPrint(strDebug);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
	
	//启动定时器，以便判定AUTH_RESP消息的到来是否超时
	//如果超时，只要在BLE_CON状态下，可以重新尝试，待完成的消息
	ret = ihu_timer_start(TASK_ID_ASYLIBRA, TIMER_ID_1S_ASYLIBRA_AUTH_RESP_WAIT, ASYLIBRA_TIMER_DURATION_AUTH_RESP_WAIT, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error ihu_timer_start start timer!");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_BLE_CONNECTED) == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_ASYLIBRA_BLE_CONNECTED State!");
		return FAILURE;
	}	
	//拉灯状态
	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_BLE_CONNECTED);
	
	//返回
	return SUCCESS;
}

OPSTAT fsm_asylibra_ble_disc_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{	
	int ret=0;
	char strDebug[BX_PRINT_SZ];
	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_ASYLIBRA)){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Wrong fsm_asylibra_ble_disc_rcv input paramters!");
		return FAILURE;
	}

	//收消息
	msg_struct_vmda_asylibra_ble_disc_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_vmda_asylibra_ble_disc_t));
	if (param_len > sizeof(msg_struct_vmda_asylibra_ble_disc_t)){
		IhuErrorPrint("ASYLIBRA: Receive MSG_ID_VMDA_ASY_BLE_DISC message error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//通知L3断链
	msg_struct_asylibra_emc_disc_t snd;
	memset(&snd, 0, sizeof(msg_struct_asylibra_emc_disc_t));
	snd.length = sizeof(msg_struct_asylibra_emc_disc_t);
	ret = ihu_message_send(MSG_ID_ASYLIBRA_EMC_DISC, TASK_ID_EMC, TASK_ID_ASYLIBRA, &snd, snd.length);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_EMC_BLE_DISC message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_EMC]);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_INITED) == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_ASYLIBRA_INITED State!");
		return FAILURE;
	}
	//拉灯状态
	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_INITED);	
	//返回
	return SUCCESS;
}

//下行数据从WX后台收到，这是个巨大的函数，其中可能包含各种可能性，需要分开处理。
//这个函数的入口，可能会在不同的状态机情形下，需要仔细并小心处理
//该函数的处理还涉及到链路SEQID的验证
//该函数涉及到定时器的处理，超时的处理机制问题
//为了节省内存，特别是消息数量，这个函数的处理决定不采用自己给自己发消息的形式，而直接进行函数调用，因为自我状态判断非常重要
OPSTAT fsm_asylibra_ble_data_dl_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_ASYLIBRA)){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Wrong fsm_asylibra_ble_disc_rcv input paramters!");
		return FAILURE;
	}

	//收消息
	msg_struct_vmda_asylibra_data_dl_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_vmda_asylibra_data_dl_t));
	if (param_len > sizeof(msg_struct_vmda_asylibra_data_dl_t)){
		IhuErrorPrint("ASYLIBRA: Receive MSG_ID_VMDA_ASYLIBRA_DATA_DL message error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//解码消息
	UINT16 nCmd=0;
	if (func_asylibra_unpack_ble_data_dl_head(rcv.dataBuf, rcv.length, nCmd) == FAILURE){
		IhuErrorPrint("ASYLIBRA: Unpack MSG_ID_VMDA_ASYLIBRA_DATA_DL message error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;		
	}

	//根据收到消息类型，进入不同的状态	
	if (nCmd == ECI_resp_auth){		
		if (func_asylibra_rcv_auth_resp_processing(&rcv.dataBuf[0], rcv.length) == FAILURE){
		IhuErrorPrint("ASYLIBRA: Processing AUTH_RESP message error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;					
		}
	}
	else if (nCmd == ECI_resp_init){
		if (func_asylibra_rcv_init_resp_processing(&rcv.dataBuf[0], rcv.length) == FAILURE){
		IhuErrorPrint("ASYLIBRA: Processing INIT_RESP message error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;					
		}
	}
	else if (nCmd == ECI_resp_sendData){
		if (func_asylibra_rcv_data_resp_processing(&rcv.dataBuf[0], rcv.length) == FAILURE){
		IhuErrorPrint("ASYLIBRA: Processing DATA_RESP message error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;					
		}
	}
	else if (nCmd == ECI_push_recvData){
		if (func_asylibra_rcv_data_push_processing(&rcv.dataBuf[0], rcv.length) == FAILURE){
		IhuErrorPrint("ASYLIBRA: Processing DATA_PUSH message error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;					
		}
	}
	
	else if (nCmd == ECI_push_switchView){
		if (func_asylibra_rcv_push_switch_view_processing(&rcv.dataBuf[0], rcv.length) == FAILURE){
		IhuErrorPrint("ASYLIBRA: Processing PUSH_SWITCH_VIEW message error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;					
		}
	}

	else if (nCmd == ECI_push_switchBackgroud){
		if (func_asylibra_rcv_push_switch_background_processing(&rcv.dataBuf[0], rcv.length) == FAILURE){
		IhuErrorPrint("ASYLIBRA: Processing PUSH_SWITCH_BACKGROUND message error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;					
		}
	}	
	
	else if (nCmd == ECI_err_decode){
		if (func_asylibra_rcv_err_decode_processing(&rcv.dataBuf[0], rcv.length) == FAILURE){
		IhuErrorPrint("ASYLIBRA: Processing ERR_DECODE message error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;					
		}
	}		
	
	//差错情况，直接丢失该COMMAND，而不做状态转移
	else{
		IhuErrorPrint("ASYLIBRA: Wrong message with commandId received, not able to process!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;			
	}

	//状态根据不同情况分别处理，这里无法统一处理。拉灯亦然。
	//返回
	return SUCCESS;
}

//TIMEOUT超时消息收到
OPSTAT fsm_asylibra_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{	
	int ret=0;
	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_ASYLIBRA)){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Wrong fsm_asylibra_time_out input paramters!");
		return FAILURE;
	}

	//收消息
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if (param_len > sizeof(msg_struct_com_time_out_t)){
		IhuErrorPrint("ASYLIBRA: Receive MSG_ID_COM_TIME_OUT message error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//RESTART钩子机制先去掉，暂时不考虑
	
	//=>BLE_CON下，重发AUTH_REQ消息
	//=>AUTH_OK下，重发INIT_REQ消息
	//状态机可能不变，反正这里不处理，而在内部处理

	//AUTH_RESP时钟
	if ((rcv.timeId == TIMER_ID_1S_ASYLIBRA_AUTH_RESP_WAIT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_asylibra_time_out_auth_resp_wait();
	}

	//INIT_RESP时钟
	else if ((rcv.timeId == TIMER_ID_1S_ASYLIBRA_INIT_RESP_WAIT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_asylibra_time_out_init_resp_wait();
	}

	//DATA_RESP时钟
	else if ((rcv.timeId == TIMER_ID_1S_ASYLIBRA_DATA_RESP_WAIT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_asylibra_time_out_data_resp_wait();
	}
	
	//返回
	return ret;	
}

//收到L3的EMC数据后的处理过程
OPSTAT fsm_asylibra_emc_data_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{	
	int ret=0;
	char strDebug[BX_PRINT_SZ];

	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_ASYLIBRA)){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Wrong fsm_asylibra_emc_data_report input paramters!");
		return FAILURE;
	}

	//收消息
	msg_struct_emc_asylibra_data_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_emc_asylibra_data_req_t));
	if (param_len > sizeof(msg_struct_emc_asylibra_data_req_t)){
		IhuErrorPrint("ASYLIBRA: Receive MSG_ID_EMC_ASYLIBRA_DATA_REQ message error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//消息内容检查
	if ((rcv.l3dataLen < 2) || (rcv.l3dataLen > MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL)){
		IhuErrorPrint("ASYLIBRA: Error L3 data received during DATA_REQ send to cloud!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
	
	//发送数据到底层BLE
	//使用MSG_ID_ASYLIBRA_VMDA_DATA_UL消息发送给VMDA
	msg_struct_vmda_asylibra_data_ul_t snd;
	memset(&snd, 0, sizeof(msg_struct_vmda_asylibra_data_ul_t));
	
	//自定义L3数据格式，由L3和后台的L3分别对照着修改，非常方便
	//举例：0x20,0x0A,0x00,0x22,0x15,0x08,0x17,0x23,0x01,0x02,0x00,0x01
	
	SendDataRequest *request = (SendDataRequest *)malloc(sizeof(SendDataRequest));
	if (rcv.EmDeviceDataType == EDDT_wxDeviceHtmlChatView){
		request->has_type = true;
		request->type = rcv.EmDeviceDataType;		
	}
	request->data.data = rcv.l3Data;
	request->data.len = rcv.l3dataLen;
	epb_pack_send_data_request(request, (UINT8*)snd.dataBuf, snd.length);
	free(request);
	
	ret = ihu_message_send(MSG_ID_ASYLIBRA_VMDA_DATA_UL, TASK_ID_VMDASHELL, TASK_ID_ASYLIBRA, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_VMDA_DATA_UL message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_VMDASHELL]);
		IhuErrorPrint(strDebug);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
	
	//启动数据等待定时器
	ret = ihu_timer_start(TASK_ID_ASYLIBRA, TIMER_ID_1S_ASYLIBRA_DATA_RESP_WAIT, ASYLIBRA_TIMER_DURATION_DATA_RESP_WAIT, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error ihu_timer_start start timer!");
		return FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_DATA_WF_CONF) == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_ASYLIBRA_DATA_WF_CONF State!");
		return FAILURE;
	}	
	//拉灯状态
	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_DATA_WF_CONF);		

	//通知L3，链路进入了非重入状态
	//使用MSG_ID_ASYLIBRA_EMC_DLK_BLOCK消息发送给EMC
	msg_struct_asylibra_emc_dlk_block_t snd1;
	memset(&snd1, 0, sizeof(msg_struct_asylibra_emc_dlk_block_t));
	snd1.length = sizeof(msg_struct_asylibra_emc_dlk_block_t);
	ret = ihu_message_send(MSG_ID_ASYLIBRA_EMC_DLK_BLOCK, TASK_ID_EMC, TASK_ID_ASYLIBRA, &snd1, snd1.length);
	if (ret == FAILURE){
		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_VMDA_DATA_UL message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_EMC]);
		IhuErrorPrint(strDebug);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
	
	//返回
	return SUCCESS;
}




/*
 *
 *   本地API函数过程
 *
 */

//Local APIs
OPSTAT func_asylibra_hw_init(void)
{
	return SUCCESS;
}

//输入输出buf及len长度
//同时输出PACK是否成功
//该函数在缓冲区越界的保护由上一级来做，而不是在该函数内部来做
//先采用最原始的方式来pack函数，未来再完善
OPSTAT func_asylibra_pack_auth_req(INT8 *buf, UINT8 len)
{
#if 0	
	UINT8 TmpHigh, TmpLow;
	UINT16 tmp;
	UINT8 AuthReq[] = {0xFE,0x01,0x00,0x1A,0x27,0x11,0x00,0x01,0x0A,0x00,0x18,0x80,0x80,0x04,0x20,0x01,0x28,0x02,0x3A,0x06,0xD0,0x39,0x72,0xA5,0xEF,0x24};

	//入参检查
	if (buf == NULL){
		IhuErrorPrint("ASYLIBRA: Pack message func_asylibra_pack_auth_req error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
	
	//Magic
	*buf = IHU_ASY_HEADER_MAGICNUMBER;
	*(buf+1) = IHU_ASY_HEADER_BVERSION;	
		
	//Length
	len = sizeof(AuthReq);
	TmpHigh = (len>>8) & 0xFF;
	TmpLow = len & 0xFF;
	*(buf+2) = TmpHigh;
	*(buf+3) = TmpLow;
	//MessageType
	tmp = ECI_req_auth;
	TmpHigh = (tmp>>8) & 0xFF;
	TmpLow = tmp & 0xFF;	
	*(buf+4) = TmpHigh;
	*(buf+5) = TmpLow;	
	//SEQ设置为最新的发送值
	TmpHigh = (zIhuAsyLinkCtx.seqId >> 8) & 0xFF;
	TmpLow = zIhuAsyLinkCtx.seqId & 0xFF;
	*(buf+6) = TmpHigh;
	*(buf+7) = TmpLow;
	//设置BaseReq
	*(buf+8) = TAG_AuthRequest_BaseRequest;
	*(buf+9) = 0;
	//设置ProtoVersion
	*(buf+10) = TAG_AuthRequest_ProtoVersion;
	*(buf+11) = AuthReq[11];
	*(buf+12) = AuthReq[12];
	*(buf+13) = AuthReq[13];
	//设置TAG_AuthRequest_AuthProto
	*(buf+14) = TAG_AuthRequest_AuthProto;
	*(buf+15) = 0x01;	
	//设置TAG_AuthRequest_AuthMethod
	*(buf+16) = TAG_AuthRequest_AuthMethod;
	*(buf+17) = 0x02;		
	//设置MAC地址
	*(buf+18) = TAG_AuthRequest_MacAddress;
	*(buf+19) = 6;
	if (ihu_get_mac_addr((UINT8*)(buf+20)) == FAILURE){
		IhuErrorPrint("ASYLIBRA: Pack AUTH - get MAC address error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
#endif

	AuthRequest *request = (AuthRequest *)malloc(sizeof(AuthRequest));
	request->md5_device_type_and_device_id.data = zIhuAsyLinkCtx.authReq.md5_type_id;
	request->md5_device_type_and_device_id.len = sizeof(zIhuAsyLinkCtx.authReq.md5_type_id);
	request->proto_version = 0x010000;
	request->auth_proto = 1;
	request->auth_method = 1;

	auth_buf_t auth_buf;
  int auth_rand = rand();
	auth_buf.random =auth_rand;
	auth_buf.seq = zIhuAsyLinkCtx.seqId;
	uint32_t crc = 0;
	crc = crc32(crc, (uint8_t *)zIhuAsyLinkCtx.authReq.device_id, strlen(zIhuAsyLinkCtx.authReq.device_id));
	crc = crc32(crc, (uint8_t *)&auth_buf.random, 4);
	crc = crc32(crc, (uint8_t *)&auth_buf.seq, 4);
	auth_buf.crc = crc;

	AES_CTX c;
	int out_len = 0;
	uint8_t temp_buf[32];
	aes_cbc_encrypt_init(&c, zIhuAsyLinkCtx.authReq.device_key);
	aes_cbc_encrypt_final(&c, (uint8_t *)&auth_buf, sizeof(auth_buf), temp_buf, &out_len);
	request->aes_sign.data = temp_buf;
	request->aes_sign.len = out_len;
	request->has_time_zone = true;
	request->time_zone.str = zIhuAsyLinkCtx.authReq.timezone;
	request->time_zone.len = strlen(zIhuAsyLinkCtx.authReq.timezone);
	request->has_language = true;
	request->language.str = zIhuAsyLinkCtx.authReq.language;
	request->language.len = strlen(zIhuAsyLinkCtx.authReq.language);
	request->has_device_name = true;
	request->device_name.str = zIhuAsyLinkCtx.authReq.device_name;
	request->device_name.len = strlen(zIhuAsyLinkCtx.authReq.device_name);
	epb_pack_auth_request(request, (UINT8*)buf, len);
	free(request);
	
	return SUCCESS;
}

//输入输出buf及len长度
//同时输出PACK是否成功
//该函数在缓冲区越界的保护由上一级来做，而不是在该函数内部来做
//先采用最原始的方式来pack函数，未来再完善
//发送Init_req, 直接二进制码流， 生成消息结构
//原生编码 uint8_t InitReq[]   = {0xFE,0x01,0x00,0x13,0x27,0x13,0x00,0x02,0x0A,0x00,0x1A,0x04,0x30,0x37,0x31,0x35,0x12,0x01,0x7F};
//缩短编码 uint8_t InitReq[]   = {0xFE,0x01,0x00,0x13,0x27,0x13,0x00,0x02,0x0A,0x00,0x1A,0x04,0x30,0x37,0x31,0x35,0x12,0x01,0x00};
OPSTAT func_asylibra_pack_init_req(INT8 *buf, UINT8 len)
{
#if 0	
	UINT8 TmpHigh, TmpLow;
	UINT16 tmp;
	UINT8 InitReq[] = {0xFE,0x01,0x00,0x13,0x27,0x13,0x00,0x02,0x0A,0x00,0x1A,0x04,0x30,0x37,0x31,0x35,0x12,0x01,0x00};	

	//入参检查
	if (buf == NULL){
		IhuErrorPrint("ASYLIBRA: Pack message func_asylibra_pack_init_req error!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}

	//Magic
	*buf = IHU_ASY_HEADER_MAGICNUMBER;
	*(buf+1) = IHU_ASY_HEADER_BVERSION;
	//Length
	len = sizeof(InitReq);
	TmpHigh = (len>>8) & 0xFF;
	TmpLow = len & 0xFF;
	*(buf+2) = TmpHigh;
	*(buf+3) = TmpLow;
	//MessageType
	tmp = ECI_req_init;
	TmpHigh = (tmp>>8) & 0xFF;
	TmpLow = tmp & 0xFF;	
	*(buf+4) = TmpHigh;
	*(buf+5) = TmpLow;	
	//SEQ设置为最新的发送值
	TmpHigh = (zIhuAsyLinkCtx.seqId >> 8) & 0xFF;
	TmpLow = zIhuAsyLinkCtx.seqId & 0xFF;
	*(buf+6) = TmpHigh;
	*(buf+7) = TmpLow;
	//设置BaseReq
	*(buf+8) = TAG_AuthRequest_BaseRequest;
	*(buf+9) = 0;
	//设置InitReqChallenge
	//1A 04 30 37 31 35 => Challenge，为了做到足够好，这里应该生成随机数，然后在接收的InitResp中进行对比，以增强安全性
	//12 01 41 => RespFieldFilter	
	*(buf+10) = TAG_InitRequest_Challenge;
	*(buf+11) = 4;  //长度
	*(buf+12) = InitReq[12];
	*(buf+13) = InitReq[13];
	*(buf+14) = InitReq[14];
	*(buf+15) = InitReq[15];
	memcpy(zIhuAsyLinkCtx.initReqChallenge, buf+12, 4); //存入环境上下文	
	//TAG_InitRequest_RespFieldFilter
	//enum EmInitRespFieldFilter {
	//EIRFF_userNickName = 0x1;
	//EIRFF_platformType = 0x2;
	//EIRFF_model = 0x4;
	//EIRFF_os = 0x8;
	//EIRFF_time = 0x10;
	//EIRFF_timeZone = 0x20;
	//EIRFF_timeString = 0x40;
	//}	这些领域，全部设置为1，总数值=0x7F，从而所有参量都从InitResp中回送
	*(buf+16) = TAG_InitRequest_Challenge;
	*(buf+17) = 1; //长度
	//MYC 7F -> 00 !!!!!!! //为了减少InitResp的处理复杂度
	*(buf+18) = 0x00;
#endif

	InitRequest *request = (InitRequest *)malloc(sizeof(InitRequest));
	request->has_challenge = true;
	request->challenge.data = zIhuAsyLinkCtx.initReqChallenge;
	request->challenge.len = sizeof(zIhuAsyLinkCtx.initReqChallenge);
	request->has_resp_field_filter = true;
	zIhuAsyLinkCtx.resp_field_filter[0] = 0x7F;
	request->resp_field_filter.data = zIhuAsyLinkCtx.resp_field_filter;
	request->resp_field_filter.len = sizeof(zIhuAsyLinkCtx.resp_field_filter);		
	epb_pack_init_request(request, (UINT8*)buf, len);
	free(request);

	return SUCCESS;
}

//UNPACK接受到的消息
//暂时还是采用比较土的方式进行解码，未来再考虑优化
OPSTAT func_asylibra_unpack_ble_data_dl_head(INT8 *buf, UINT8 len, UINT16 nCmd)
{
	char strDebug[BX_PRINT_SZ];
	
	//入参检查
	if ((buf == NULL) ||(len > MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL)){
		sprintf(strDebug, "ASYLIBRA: Unpack DATA_DL message error with internal length, len=%d.", len);
		IhuErrorPrint(strDebug);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
	
	//解码消息头MAGIC/bVER
	if (((UINT8)*buf != IHU_ASY_HEADER_MAGICNUMBER) || ((UINT8)*(buf+1) != IHU_ASY_HEADER_BVERSION)){
		IhuErrorPrint("ASYLIBRA: Unpack DATA_DL message error with MAGIC/bVER segment.");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;		
	}	

	//解码消息头length
	UINT16 nLength = 0, t1=0, t2=0;	
	t1 = (UINT8)*(buf+2); t2 = (UINT8)*(buf+3);
	nLength = ((t1<<8) & 0xFF00) + (t2 & 0xFF);
	if ((nLength<=8) || (nLength > len)){
		IhuErrorPrint("ASYLIBRA: Unpack DATA_DL message with wrong length domain.");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;				
	}
	
	//解码消息头CommandId/SequenceId
	UINT16 nSeq = 0;
	t1 = (UINT8)*(buf+4); t2 = (UINT8)*(buf+5);
	nCmd = ((t1<<8) & 0xFF00) + (t2 & 0xFF);
	t1 = (UINT8)*(buf+6); t2 = (UINT8)*(buf+7);
	nSeq = ((t1<<8) & 0xFF00) + (t2 & 0xFF);
	//nSeq的握手及滑窗处理，这里没有详细的做处理，未来待完善
	
	//根据CommandId进行分别处理  //处理seqId，顺序增加
	
	//auth_resp = 20001
	if (nCmd == ECI_resp_auth){
		zIhuAsyLinkCtx.seqId++;
	}
	
	//init_resp = 20003
	else if (nCmd == ECI_resp_init){	
		zIhuAsyLinkCtx.seqId++;
	}
	
	//data_resp = 20002
	else if (nCmd == ECI_resp_sendData){
		//系统采用最简单的WAIT-STOP机制，意味着上一次的REQ对应的RESP没有收到，则下一次的REQ不能发出
		//至于由此导致的消息丢失等问题，全部由高层自行处理，链路层不保证		
		//处理seqId，顺序增加
		zIhuAsyLinkCtx.seqId++;
	} //nCmdid == 0x224E
	
	//data_push = 30001
	else if (nCmd == ECI_push_recvData){
		//判定SequenceId是否为0
		if (nSeq != 0){
			IhuErrorPrint("ASYLIBRA: Unpack DATA_DL/DataPUSH message with wrong SeqId.");
			zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
			return FAILURE;
		}
	}

	//ECI_push_switchView = 30002
	else if (nCmd == ECI_push_switchView){
		zIhuAsyLinkCtx.seqId++;
	}

	//ECI_push_switchBackgroud = 30003
	else if (nCmd == ECI_push_switchBackgroud){
		zIhuAsyLinkCtx.seqId++;
	}	
	
	//ECI_err_decode = 29999
	else if (nCmd == ECI_err_decode){
		zIhuAsyLinkCtx.seqId++;
	}	
	
	//无效CommandID，直接作废不处理
	else{
		IhuErrorPrint("ASYLIBRA: Unpack DATA_DL message with wrong CommandId.");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
	
	//返回
	return SUCCESS;
}

//等待AUTH_RESP回复的情形超时，条件合适继续重发
OPSTAT func_asylibra_time_out_auth_resp_wait(void)
{
	int ret=0;
	char strDebug[BX_PRINT_SZ];
	
	//超时，检查状态，合适就重发AUTH_REQ消息
	if (FsmGetState(TASK_ID_ASYLIBRA) == FSM_STATE_ASYLIBRA_BLE_CONNECTED){
		//使用MSG_ID_ASYLIBRA_VMDA_DATA_UL消息发送给VMDA
		msg_struct_vmda_asylibra_data_ul_t snd;
		memset(&snd, 0, sizeof(msg_struct_vmda_asylibra_data_ul_t));
		if (func_asylibra_pack_auth_req(snd.dataBuf, snd.length) == FAILURE){
			IhuErrorPrint(strDebug);
			zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
			//恢复到空闲状态，等待下次链接的到来
			FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_INITED);
			//拉灯状态
			ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_INITED);		
			return FAILURE;		
		}
		ret = ihu_message_send(MSG_ID_ASYLIBRA_VMDA_DATA_UL, TASK_ID_VMDASHELL, TASK_ID_ASYLIBRA, &snd, snd.length);
		if (ret == FAILURE){
			sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_VMDA_DATA_UL message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_VMDASHELL]);
			IhuErrorPrint(strDebug);
			zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
			return FAILURE;
		}
		
		//启动定时器，以便判定AUTH_RESP消息的到来是否超时
		//如果超时，只要在BLE_CON状态下，可以重新尝试，待完成的消息
		ret = ihu_timer_start(TASK_ID_ASYLIBRA, TIMER_ID_1S_ASYLIBRA_AUTH_RESP_WAIT, ASYLIBRA_TIMER_DURATION_AUTH_RESP_WAIT, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
			IhuErrorPrint("ASYLIBRA: Error ihu_timer_start start timer!");
			return FAILURE;
		}
	}
	
	//否则，抛弃处理
	else{
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		sprintf(strDebug, "ASYLIBRA: Receive TIME OUT message at wrong state=[%d], not FSM_STATE_ASYLIBRA_BLE_CONNECTED[3]!", FsmGetState(TASK_ID_ASYLIBRA));		
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	return SUCCESS;
}

//等待INIT_RESP回复的情形超时，条件合适继续重发
OPSTAT func_asylibra_time_out_init_resp_wait(void)
{
	int ret=0;
	char strDebug[BX_PRINT_SZ];
	
	//超时，检查状态，合适就重发INIT_REQ消息
	if (FsmGetState(TASK_ID_ASYLIBRA) == FSM_STATE_ASYLIBRA_AUTH_OK){
		//使用MSG_ID_ASYLIBRA_VMDA_DATA_UL消息发送给VMDA
		msg_struct_vmda_asylibra_data_ul_t snd;
		memset(&snd, 0, sizeof(msg_struct_vmda_asylibra_data_ul_t));
		if (func_asylibra_pack_init_req(snd.dataBuf, snd.length) == FAILURE){
			IhuErrorPrint(strDebug);
			zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
			//恢复到空闲状态，等待下次链接的到来
			FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_INITED);
			//拉灯状态
			ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_INITED);		
			return FAILURE;		
		}
		ret = ihu_message_send(MSG_ID_ASYLIBRA_VMDA_DATA_UL, TASK_ID_VMDASHELL, TASK_ID_ASYLIBRA, &snd, snd.length);
		if (ret == FAILURE){
			sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_VMDA_DATA_UL message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_VMDASHELL]);
			IhuErrorPrint(strDebug);
			zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
			return FAILURE;
		}
		
		//启动定时器，以便判定AUTH_RESP消息的到来是否超时
		//如果超时，只要在BLE_CON状态下，可以重新尝试，待完成的消息
		ret = ihu_timer_start(TASK_ID_ASYLIBRA, TIMER_ID_1S_ASYLIBRA_INIT_RESP_WAIT, ASYLIBRA_TIMER_DURATION_INIT_RESP_WAIT, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
			IhuErrorPrint("ASYLIBRA: Error ihu_timer_start start timer!");
			return FAILURE;
		}
	}
	
	//否则，抛弃处理
	else{
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		sprintf(strDebug, "ASYLIBRA: Receive TIME OUT message at wrong state=[%d], not FSM_STATE_ASYLIBRA_AITH_OK[4]!", FsmGetState(TASK_ID_ASYLIBRA));		
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	return SUCCESS;
}

//等待DATA_RESP回复的情形超时
OPSTAT func_asylibra_time_out_data_resp_wait(void)
{
	int ret=0;
	char strDebug[BX_PRINT_SZ];
	
	//超时检查状态，恢复到BLE连接状态，准备重新建立AIRSYNC远程链路
	if (FsmGetState(TASK_ID_ASYLIBRA) == FSM_STATE_ASYLIBRA_DATA_WF_CONF){
		//设置状态到FSM_STATE_ASYLIBRA_BLE_CONNECTED	
		if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_BLE_CONNECTED) == FAILURE){
			zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
			IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_ASYLIBRA_BLE_CONNECTED State!");
			return FAILURE;
		}	
		//拉灯状态
		ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_BLE_CONNECTED);		

		//再建立链路
		//使用MSG_ID_ASYLIBRA_VMDA_DATA_UL消息发送给VMDA
		msg_struct_vmda_asylibra_data_ul_t snd;
		memset(&snd, 0, sizeof(msg_struct_vmda_asylibra_data_ul_t));
		if (func_asylibra_pack_auth_req(snd.dataBuf, snd.length) == FAILURE){
			IhuErrorPrint(strDebug);
			zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
			//恢复到空闲状态，等待下次链接的到来
			FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_INITED);
			//拉灯状态
			ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_INITED);		
			return FAILURE;		
		}
		ret = ihu_message_send(MSG_ID_ASYLIBRA_VMDA_DATA_UL, TASK_ID_VMDASHELL, TASK_ID_ASYLIBRA, &snd, snd.length);
		if (ret == FAILURE){
			sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_VMDA_DATA_UL message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_VMDASHELL]);
			IhuErrorPrint(strDebug);
			zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
			return FAILURE;
		}

		//启动定时器，以便判定AUTH_RESP消息的到来是否超时
		//如果超时，只要在BLE_CON状态下，可以重新尝试，待完成的消息
		ret = ihu_timer_start(TASK_ID_ASYLIBRA, TIMER_ID_1S_ASYLIBRA_AUTH_RESP_WAIT, ASYLIBRA_TIMER_DURATION_AUTH_RESP_WAIT, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
			IhuErrorPrint("ASYLIBRA: Error ihu_timer_start start timer!");
			return FAILURE;
		}
		
	}//FSM_STATE_ASYLIBRA_DATA_WF_CONF
	
	//否则，严重错误，直接使用RESTART恢复
	else{
		IhuErrorPrint("ASYLIBRA: Fatal error occur, just restart whole module now...");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		fsm_asylibra_init(0, 0, NULL, 0);
		return FAILURE;
	}
	return SUCCESS;
}

//**** send auth response ****
//data len = 14
//data dump = FE 01 00 0E 4E 21 00 01 0A 02 08 00 12 00 
//收到AUTH_RESP的消息处理
OPSTAT func_asylibra_rcv_auth_resp_processing(INT8 *buf, UINT8 len)
{
	int ret=0;
	char strDebug[BX_PRINT_SZ];

	//入参检查
	if ((buf == NULL) || (len > MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL)){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Wrong func_asylibra_rcv_auth_resp_processing input paramters!");
		return FAILURE;
	}
	
	//消息解码，这里省略，未来再完善
	AuthResponse *response = epb_unpack_auth_response((UINT8*)buf, len);
	epb_unpack_auth_response_free(response);
	//主要逻辑是判断TAG_AuthResponse_BaseResponse和TAG_AuthResponse_AesSessionKey的正确性	
	
	//停止AUTH_RESP_WAIT时钟
	ret = ihu_timer_stop(TASK_ID_ASYLIBRA, TIMER_ID_1S_ASYLIBRA_AUTH_RESP_WAIT, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error ihu_timer_stop stop timer!\n");
		return FAILURE;
	}
	
	//判定状态，如果不再FSM_STATE_ASYLIBRA_BLE_CONNECTED则报错，并恢复到INITED状态，等待再次底层的连接请求从而进入BLE_CON状态
	if (FsmGetState(TASK_ID_ASYLIBRA) != FSM_STATE_ASYLIBRA_BLE_CONNECTED){
		FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_INITED);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Receive AUTH_RESP link message during wrong state!");
		//拉灯状态
		ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_INITED);
		return FAILURE;		
	}
	
	//发送INIT_REQ消息出去
	msg_struct_vmda_asylibra_data_ul_t snd;
	memset(&snd, 0, sizeof(msg_struct_vmda_asylibra_data_ul_t));
	if (func_asylibra_pack_init_req(snd.dataBuf, snd.length) == FAILURE){
		IhuErrorPrint(strDebug);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		//恢复到空闲状态，等待下次链接的到来
		FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_INITED);
		//拉灯状态
		ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_INITED);		
		return FAILURE;		
	}
	ret = ihu_message_send(MSG_ID_ASYLIBRA_VMDA_DATA_UL, TASK_ID_VMDASHELL, TASK_ID_ASYLIBRA, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_VMDA_DATA_UL message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_VMDASHELL]);
		IhuErrorPrint(strDebug);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
	
	//启动定时器，以便判定INIT_RESP消息的到来是否超时
	//如果超时，只要在AUTH_CON状态下，可以重新尝试，待完成的消息
	ret = ihu_timer_start(TASK_ID_ASYLIBRA, TIMER_ID_1S_ASYLIBRA_INIT_RESP_WAIT, ASYLIBRA_TIMER_DURATION_INIT_RESP_WAIT, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error ihu_timer_start start timer!");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_AUTH_OK) == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_ASYLIBRA_AUTH_OK State!");
		return FAILURE;
	}	
	//拉灯状态
	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_AUTH_OK);	
	
	//返回
	return SUCCESS;
}

//**** send init request response ****
//data len = 70
//data dump = FE 01 00 46 4E 23 00 02 0A 02 08 00 10 B4 24 18 F8 AC 01 20 B9 AE 94 85 06 5A 14 57 65 43 68 61 74 20 54 65 73 74 20 62 79 20 6D 61 74 74 21 8A 01 14 57 65 43 68 61 74 20 54 65 73 74 20 62 79 20 6D 61 74 74 21 
//收到INIT_RESP的消息处理
OPSTAT func_asylibra_rcv_init_resp_processing(INT8 *buf, UINT8 len)
{
	int ret=0;
	char strDebug[BX_PRINT_SZ];

	//入参检查
	if ((buf == NULL) || (len > MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL)){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Wrong func_asylibra_rcv_init_resp_processing input paramters!");
		return FAILURE;
	}
	
	//消息解码，这里省略，未来再完善
	InitResponse *response = epb_unpack_init_response((UINT8*)buf, len);
	epb_unpack_init_response_free(response);
	//接收到该消息，zIhuAsyLinkCtx.initRespFb大结构，里面可能包含丰富信息，值得仔细处理的

	//停止INIT_RESP_WAIT时钟
	ret = ihu_timer_stop(TASK_ID_ASYLIBRA, TIMER_ID_1S_ASYLIBRA_INIT_RESP_WAIT, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error ihu_timer_stop stop timer!\n");
		return FAILURE;
	}
	
	//判定状态，如果不再FSM_STATE_ASYLIBRA_AUTH_OK则报错，并恢复到INITED状态，等待再次底层的连接请求从而进入BLE_CON状态
	if (FsmGetState(TASK_ID_ASYLIBRA) != FSM_STATE_ASYLIBRA_AUTH_OK){
		FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_INITED);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Receive INIT_RESP link message during wrong state!");
		//拉灯状态
		ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_INITED);
		return FAILURE;		
	}
	
	//发送MSG_ID_ASYLIBRA_EMC_CON消息出去
	msg_struct_asylibra_emc_con_t snd;
	memset(&snd, 0, sizeof(msg_struct_asylibra_emc_con_t));
	snd.length = sizeof(msg_struct_asylibra_emc_con_t);
	ret = ihu_message_send(MSG_ID_ASYLIBRA_EMC_CON, TASK_ID_EMC, TASK_ID_ASYLIBRA, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_EMC_CON message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_EMC]);
		IhuErrorPrint(strDebug);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_DATA_READY) == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_ASYLIBRA_DATA_READY State!");
		return FAILURE;
	}	
	//拉灯状态
	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_DATA_READY);	
	
	//返回
	return SUCCESS;
}

//**** send SendData Response(echo request) ****
//data len = 36
//data dump = FE 01 00 24 4E 22 12 37 0A 02 08 00 12 16 0A 00 12 10 20 0E 00 06 00 01 02 03 04 05 01 02 03 04 05 06 18 10 
//收到DATA_RESP的消息处理
OPSTAT func_asylibra_rcv_data_resp_processing(INT8 *buf, UINT8 len)
{
	int ret=0;
	char strDebug[BX_PRINT_SZ];

	//入参检查
	if ((buf == NULL) || (len > MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL)){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Wrong func_asylibra_rcv_data_resp_processing input paramters!");
		return FAILURE;
	}

	//消息解码
	SendDataResponse *response = epb_unpack_send_data_response((UINT8*)buf, len);
	epb_unpack_send_data_response_free(response);
	
	//停止DATA_RESP_WAIT时钟
	ret = ihu_timer_stop(TASK_ID_ASYLIBRA, TIMER_ID_1S_ASYLIBRA_DATA_RESP_WAIT, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error ihu_timer_stop stop timer!\n");
		return FAILURE;
	}
	
	//判定状态，如果不再FSM_STATE_ASYLIBRA_DATA_WF_CONF则报错，并恢复到FSM_STATE_ASYLIBRA_DATA_READY
	if (FsmGetState(TASK_ID_ASYLIBRA) != FSM_STATE_ASYLIBRA_DATA_WF_CONF){
		FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_DATA_READY);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Receive DATA_RESP link message during wrong state!");
		//拉灯状态
		ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_DATA_READY);
		return FAILURE;
	}
	
	//业务逻辑处理
	//该过程数据SeqenceId和数据WAIT-STOP相关的滑窗过程，可用于上行数据发送的证实性
	//BLE采用单线方式，只能发送一个数据，一旦进入WAIT_FOR_CONFORM状态，不再能继续发送数据
	
	//通知L3链路进入可发送数据状态，Send back MSG_ID_ASYLIBRA_EMC_DLK_UNBLOCK to EMC
	msg_struct_asylibra_emc_dlk_unblock_t snd;
	memset(&snd, 0, sizeof(msg_struct_asylibra_emc_dlk_unblock_t));
	snd.length = sizeof(msg_struct_asylibra_emc_dlk_unblock_t);
	ret = ihu_message_send(MSG_ID_ASYLIBRA_EMC_DLK_UNBLOCK, TASK_ID_EMC, TASK_ID_ASYLIBRA, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_EMC_DLK_UNBLOCK message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_EMC]);
		IhuErrorPrint(strDebug);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_DATA_READY) == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_ASYLIBRA_DATA_READY State!");
		return FAILURE;
	}	
	//拉灯状态
	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_DATA_READY);	
	
	//返回
	return SUCCESS;
}

//**** send ManufactureSvr Push ****		
//data len = 22
//data dump = FE 01 00 16 75 31 00 00 0A 00 12 08 31 32 33 34 35 36 37 38 18 00
//**** send Html Push ****
//data len = 23
//data dump = FE 01 00 17 75 31 00 00 0A 00 12 08 31 32 33 34 35 36 37 38 18 91 4E 
//#define TAG_RecvDataPush_BasePush												0x0a
//#define TAG_RecvDataPush_Data														0x12
//#define TAG_RecvDataPush_Type														0x18
//收到DATA_PUSH的消息处理
OPSTAT func_asylibra_rcv_data_push_processing(INT8 *buf, UINT8 len)
{
	int ret=0;
	char strDebug[BX_PRINT_SZ];

	//入参检查
	if ((buf == NULL) || (len > MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL) || (len < 6)){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Wrong func_asylibra_rcv_data_push_processing input paramters!");
		return FAILURE;
	}

	//判定状态，如果不再FSM_STATE_ASYLIBRA_DATA_READY则报错，并恢复到FSM_STATE_ASYLIBRA_DATA_READY
	if (FsmGetState(TASK_ID_ASYLIBRA) != FSM_STATE_ASYLIBRA_DATA_READY){
		FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_DATA_READY);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Receive DATA_PUSH link message during wrong state!");
		//拉灯状态
		ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_DATA_READY);
		return FAILURE;
	}

	//消息解码及检查
	RecvDataPush *response = epb_unpack_recv_data_push((UINT8*)buf, len);
	if ((response->data.len <=0 || response->data.len > MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL)){
		IhuErrorPrint("ASYLIBRA: Receive DATA_PUSH with wrong data area!");
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}

	//业务逻辑处理：发送MSG_ID_ASYLIBRA_EMC_PUSH_CMD消息命令数据到L3/EMC模块
	msg_struct_asylibra_emc_push_cmd_t snd;
	memset(&snd, 0, sizeof(msg_struct_asylibra_emc_push_cmd_t));
	if (response->has_type == true) snd.EmDeviceDataType = response->type;
	memcpy(snd.l3Data, response->data.data, response->data.len);
	snd.l3dataLen = response->data.len;
	
	epb_unpack_recv_data_push_free(response);	
	
//	//增加JSAPI微信界面H5会话操作功能，就是一直看到的18 00 OR 18 02 27 11
//	//使用编解码函数将会更加准确，这里使用傻瓜解码方式，可能会出错的，特别是IE字段顺序被WXAPP调整的话
//	//这里的解码是没办法的办法，未来需要好好打造
//	UINT8 cmdTag1=0, cmdTag2=0, len1=0;
//	cmdTag1 = (UINT8)*buf;
//	if (cmdTag1 == TAG_RecvDataPush_Type){
//		if ((((UINT8)*(buf+1) << 8) & 0xFF00) + ((UINT8)*(buf+2) & 0xFF) == EDDT_wxDeviceHtmlChatView){
//			snd.EmDeviceDataType = EDDT_wxDeviceHtmlChatView;
//		}
//		else{
//			snd.EmDeviceDataType = EDDT_manufatureSvr;
//		}
//	}
//	else{
//		len1 = (UINT8)*(buf+1);
//		cmdTag2 = (UINT8)*(buf+len1+3);
//		if (cmdTag2 == TAG_RecvDataPush_Type){
//			if ((((UINT8)*(buf+len1+4) << 8) & 0xFF00) + ((UINT8)*(buf+len1+5) & 0xFF) == EDDT_wxDeviceHtmlChatView){
//				snd.EmDeviceDataType = EDDT_wxDeviceHtmlChatView;
//			}
//			else{
//				snd.EmDeviceDataType = EDDT_manufatureSvr;
//			}		
//		}
//		else{
//			snd.EmDeviceDataType = EDDT_manufatureSvr;
//		}
//	}
	snd.length = sizeof(msg_struct_asylibra_emc_push_cmd_t);	
	ret = ihu_message_send(MSG_ID_ASYLIBRA_EMC_PUSH_CMD, TASK_ID_EMC, TASK_ID_ASYLIBRA, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_EMC_DLK_UNBLOCK message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_EMC]);
		IhuErrorPrint(strDebug);
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		return FAILURE;
	}	
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_DATA_READY) == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_ASYLIBRA_DATA_READY State!");
		return FAILURE;
	}	
	//拉灯状态
	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_DATA_READY);		
	
	return SUCCESS;
}

//**** send EnterDeviceChatView Push ****
//data len = 14
//data dump = FE 01 00 0E 75 32 00 00 0A 00 10 01 18 01 
//**** send Exit ChatView Push ****
//data len = 14
//data dump = FE 01 00 0E 75 32 00 00 0A 00 10 02 18 01 
//**** send Enter HtmlChatView Push ****
//data len = 14
//data dump = FE 01 00 0E 75 32 00 00 0A 00 10 01 18 02 
//**** send Exit HtmlChatView Push ****
//data len = 14
//data dump = FE 01 00 0E 75 32 00 00 0A 00 10 02 18 02 
//收到PUSH_SWITCH_VIEW的消息处理
OPSTAT func_asylibra_rcv_push_switch_view_processing(INT8 *buf, UINT8 len)
{
	//入参检查
	if ((buf == NULL) || (len > MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL)){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Wrong func_asylibra_rcv_push_switch_view_processing input paramters!");
		return FAILURE;
	}

	//消息解码
	SwitchViewPush *response = epb_unpack_switch_view_push((UINT8*)buf, len);
	epb_unpack_switch_view_push_free(response);	
	
	//返回
	return SUCCESS;
}

//**** send enterBackground Push ****
//data len = 12
//data dump = FE 01 00 0C 75 33 00 00 0A 00 10 01 
//**** send enterForground Push ****
//data len = 12
//data dump = FE 01 00 0C 75 33 00 00 0A 00 10 02 
//**** send enterSleep Push ****
//data len = 12
//data dump = FE 01 00 0C 75 33 00 00 0A 00 10 03 
//收到PUSH_SWITCH_BACKGROUND的消息处理
OPSTAT func_asylibra_rcv_push_switch_background_processing(INT8 *buf, UINT8 len)
{
	//入参检查
	if ((buf == NULL) || (len > MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL)){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Wrong func_asylibra_rcv_push_switch_background_processing input paramters!");
		return FAILURE;
	}

	//消息解码
	SwitchBackgroudPush *response = epb_unpack_switch_backgroud_push((UINT8*)buf, len);
	epb_unpack_switch_backgroud_push_free(response);		
	
	//返回
	return SUCCESS;
}

//收到ERR_DECODE的消息处理
OPSTAT func_asylibra_rcv_err_decode_processing(INT8 *buf, UINT8 len)
{
	//入参检查
	if ((buf == NULL) || (len > MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL)){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Wrong func_asylibra_rcv_err_decode_processing input paramters!");
		return FAILURE;
	}

	//消息解码
	
	
	//返回
	return SUCCESS;
}








/*
 *
*   备份处理过程，未来很可能不需要
 *
 */

////收到L3的EMC数据后的处理过程
//OPSTAT fsm_asylibra_emc_data_report(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
//{	
//	int ret=0;
//	char strDebug[BX_PRINT_SZ];

//	//入参检查
//	if ((param_ptr == NULL) || (dest_id != TASK_ID_ASYLIBRA)){
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		IhuErrorPrint("ASYLIBRA: Wrong fsm_asylibra_emc_data_report input paramters!");
//		return FAILURE;
//	}

//	//收消息
//	msg_struct_emc_asylibra_data_report_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_emc_asylibra_data_report_t));
//	if (param_len > sizeof(msg_struct_emc_asylibra_data_report_t)){
//		IhuErrorPrint("ASYLIBRA: Receive MSG_ID_EMC_ASYLIBRA_DATA_REPORT message error!");
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		return FAILURE;
//	}
//	memcpy(&rcv, param_ptr, param_len);
//	
//	//发送数据到底层BLE
//	//使用MSG_ID_ASYLIBRA_VMDA_DATA_UL消息发送给VMDA
//	msg_struct_vmda_asylibra_data_ul_t snd;
//	memset(&snd, 0, sizeof(msg_struct_vmda_asylibra_data_ul_t));
//	
//	if (func_asylibra_pack_data_req(snd.dataBuf, snd.length, (INT8*)&rcv, rcv.length) == FAILURE){
//		IhuErrorPrint("ASYLIBRA: Pack DATA_REQ message error!");
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		//恢复到发送DATA_REQ之前的状态，等待瑕疵数据报文的到来
//		FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_DATA_READY);
//		//拉灯状态
//		ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_DATA_READY);		
//		return FAILURE;
//	}
//	ret = ihu_message_send(MSG_ID_ASYLIBRA_VMDA_DATA_UL, TASK_ID_VMDASHELL, TASK_ID_ASYLIBRA, &snd, snd.length);
//	if (ret == FAILURE){
//		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_VMDA_DATA_UL message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_VMDASHELL]);
//		IhuErrorPrint(strDebug);
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		return FAILURE;
//	}
//	
//	//启动数据等待定时器
//	ret = ihu_timer_start(TASK_ID_ASYLIBRA, TIMER_ID_1S_ASYLIBRA_DATA_RESP_WAIT, ASYLIBRA_TIMER_DURATION_DATA_RESP_WAIT, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
//	if (ret == FAILURE){
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		IhuErrorPrint("ASYLIBRA: Error ihu_timer_start start timer!");
//		return FAILURE;
//	}
//	
//	//设置状态机到目标状态
//	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_DATA_WF_CONF) == FAILURE){
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_ASYLIBRA_DATA_WF_CONF State!");
//		return FAILURE;
//	}	
//	//拉灯状态
//	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_DATA_WF_CONF);		

//	//通知L3，链路进入了非重入状态
//	//使用MSG_ID_ASYLIBRA_EMC_DLK_BLOCK消息发送给EMC
//	msg_struct_asylibra_emc_dlk_block_t snd1;
//	memset(&snd1, 0, sizeof(msg_struct_asylibra_emc_dlk_block_t));
//	snd1.length = sizeof(msg_struct_asylibra_emc_dlk_block_t);
//	ret = ihu_message_send(MSG_ID_ASYLIBRA_EMC_DLK_BLOCK, TASK_ID_EMC, TASK_ID_ASYLIBRA, &snd1, snd1.length);
//	if (ret == FAILURE){
//		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_VMDA_DATA_UL message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_EMC]);
//		IhuErrorPrint(strDebug);
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		return FAILURE;
//	}
//	
//	//返回
//	return SUCCESS;
//}

////收到L3的EMC的时钟同步请求后的处理过程
//OPSTAT fsm_asylibra_emc_time_sync_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
//{	
//	int ret=0;
//	char strDebug[BX_PRINT_SZ];

//	//入参检查
//	if ((param_ptr == NULL) || (dest_id != TASK_ID_ASYLIBRA)){
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		IhuErrorPrint("ASYLIBRA: Wrong fsm_asylibra_emc_time_sync_req input paramters!");
//		return FAILURE;
//	}

//	//收消息
//	msg_struct_emc_asylibra_time_sync_req_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_emc_asylibra_time_sync_req_t));
//	if (param_len > sizeof(msg_struct_emc_asylibra_time_sync_req_t)){
//		IhuErrorPrint("ASYLIBRA: Receive MSG_ID_EMC_ASYLIBRA_TIME_SYNC_REQ message error!");
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		return FAILURE;
//	}
//	memcpy(&rcv, param_ptr, param_len);
//	
//	//发送数据到底层BLE
//	//使用MSG_ID_ASYLIBRA_VMDA_DATA_UL消息发送给VMDA
//	msg_struct_vmda_asylibra_data_ul_t snd;
//	memset(&snd, 0, sizeof(msg_struct_vmda_asylibra_data_ul_t));
//	
//	if (func_asylibra_pack_time_sync_req(snd.dataBuf, snd.length, (INT8*)&rcv, rcv.length) == FAILURE){
//		IhuErrorPrint("ASYLIBRA: Pack TIME_SYNC/DATA_REQ message error!");
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		//恢复到发送DATA_REQ之前的状态，等待瑕疵数据报文的到来
//		FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_DATA_READY);
//		//拉灯状态
//		ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_DATA_READY);		
//		return FAILURE;
//	}
//	ret = ihu_message_send(MSG_ID_ASYLIBRA_VMDA_DATA_UL, TASK_ID_VMDASHELL, TASK_ID_ASYLIBRA, &snd, snd.length);
//	if (ret == FAILURE){
//		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_VMDA_DATA_UL message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_VMDASHELL]);
//		IhuErrorPrint(strDebug);
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		return FAILURE;
//	}
//	
//	//启动数据等待定时器
//	ret = ihu_timer_start(TASK_ID_ASYLIBRA, TIMER_ID_1S_ASYLIBRA_DATA_RESP_WAIT, ASYLIBRA_TIMER_DURATION_DATA_RESP_WAIT, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
//	if (ret == FAILURE){
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		IhuErrorPrint("ASYLIBRA: Error ihu_timer_start start timer!");
//		return FAILURE;
//	}
//	
//	//设置状态机到目标状态
//	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_DATA_WF_CONF) == FAILURE){
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_ASYLIBRA_DATA_WF_CONF State!");
//		return FAILURE;
//	}	
//	//拉灯状态
//	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_DATA_WF_CONF);		

//	//通知L3，链路进入了非重入状态
//	//使用MSG_ID_ASYLIBRA_EMC_DLK_BLOCK消息发送给EMC
//	msg_struct_asylibra_emc_dlk_block_t snd1;
//	memset(&snd1, 0, sizeof(msg_struct_asylibra_emc_dlk_block_t));
//	snd1.length = sizeof(msg_struct_asylibra_emc_dlk_block_t);
//	ret = ihu_message_send(MSG_ID_ASYLIBRA_EMC_DLK_BLOCK, TASK_ID_EMC, TASK_ID_ASYLIBRA, &snd1, snd1.length);
//	if (ret == FAILURE){
//		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_VMDA_DATA_UL message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_EMC]);
//		IhuErrorPrint(strDebug);
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		return FAILURE;
//	}
//	
//	//返回
//	return SUCCESS;
//}

////uint8_t EquipInfo[] = {0xFE,0x01,0x00,0x1C,0x27,0x12,0x12,0x37,0x0A,0x00,0x12,0x0E,0xF0,0x0C,0xD0,0x39,0x72,0xA5,0xEF,0x24,0x01,0x00,0x01,0x01,0x00,0x01, 0x18,0x00};
////收到L3的EMC的设备信息数据后的处理过程
//OPSTAT fsm_asylibra_emc_equ_info_sync(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
//{	
//	int ret=0;
//	char strDebug[BX_PRINT_SZ];

//	//入参检查
//	if ((param_ptr == NULL) || (dest_id != TASK_ID_ASYLIBRA)){
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		IhuErrorPrint("ASYLIBRA: Wrong fsm_asylibra_emc_equ_info_send input paramters!");
//		return FAILURE;
//	}

//	//收消息
//	msg_struct_emc_asylibra_equ_info_sync_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_emc_asylibra_equ_info_sync_t));
//	if (param_len > sizeof(msg_struct_emc_asylibra_equ_info_sync_t)){
//		IhuErrorPrint("ASYLIBRA: Receive MSG_ID_EMC_ASYLIBRA_DATA_REPORT message error!");
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		return FAILURE;
//	}
//	memcpy(&rcv, param_ptr, param_len);
//	
//	//发送数据到底层BLE
//	//使用MSG_ID_ASYLIBRA_VMDA_DATA_UL消息发送给VMDA
//	msg_struct_vmda_asylibra_data_ul_t snd;
//	memset(&snd, 0, sizeof(msg_struct_vmda_asylibra_data_ul_t));
//	
//	if (func_asylibra_pack_equ_info(snd.dataBuf, snd.length, (INT8*)&rcv, rcv.length) == FAILURE){
//		IhuErrorPrint("ASYLIBRA: Pack EQU_INFO/DATA_REQ message error!");
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		//恢复到发送DATA_REQ之前的状态，等待瑕疵数据报文的到来
//		FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_DATA_READY);
//		//拉灯状态
//		ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_DATA_READY);		
//		return FAILURE;
//	}
//	ret = ihu_message_send(MSG_ID_ASYLIBRA_VMDA_DATA_UL, TASK_ID_VMDASHELL, TASK_ID_ASYLIBRA, &snd, snd.length);
//	if (ret == FAILURE){
//		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_VMDA_DATA_UL message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_VMDASHELL]);
//		IhuErrorPrint(strDebug);
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		return FAILURE;
//	}
//	
//	//启动数据等待定时器
//	ret = ihu_timer_start(TASK_ID_ASYLIBRA, TIMER_ID_1S_ASYLIBRA_DATA_RESP_WAIT, ASYLIBRA_TIMER_DURATION_DATA_RESP_WAIT, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
//	if (ret == FAILURE){
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		IhuErrorPrint("ASYLIBRA: Error ihu_timer_start start timer!");
//		return FAILURE;
//	}
//	
//	//设置状态机到目标状态
//	if (FsmSetState(TASK_ID_ASYLIBRA, FSM_STATE_ASYLIBRA_DATA_WF_CONF) == FAILURE){
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		IhuErrorPrint("ASYLIBRA: Error Set FSM_STATE_ASYLIBRA_DATA_WF_CONF State!");
//		return FAILURE;
//	}	
//	//拉灯状态
//	ihu_led_blink_enable(ASYLIBRA_LED_IND_NO, ASYLIBRA_LED_STATE_DATA_WF_CONF);		

//	//通知L3，链路进入了非重入状态
//	//使用MSG_ID_ASYLIBRA_EMC_DLK_BLOCK消息发送给EMC
//	msg_struct_asylibra_emc_dlk_block_t snd1;
//	memset(&snd1, 0, sizeof(msg_struct_asylibra_emc_dlk_block_t));
//	snd1.length = sizeof(msg_struct_asylibra_emc_dlk_block_t);
//	ret = ihu_message_send(MSG_ID_ASYLIBRA_EMC_DLK_BLOCK, TASK_ID_EMC, TASK_ID_ASYLIBRA, &snd1, snd1.length);
//	if (ret == FAILURE){
//		sprintf(strDebug, "ASYLIBRA: Send MSG_ID_ASYLIBRA_VMDA_DATA_UL message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_ASYLIBRA], zIhuTaskNameList[TASK_ID_EMC]);
//		IhuErrorPrint(strDebug);
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		return FAILURE;
//	}
//	
//	//返回
//	return SUCCESS;
//}

//输入输出buf及len长度
//同时输出PACK是否成功
//该函数在缓冲区越界的保护由上一级来做，而不是在该函数内部来做
//先采用最原始的方式来pack函数，未来再完善
//发送Data_req, 直接二进制码流， 生成消息结构
//OPSTAT func_asylibra_pack_data_req(INT8 *buf, UINT8 len, INT8 *input, UINT8 inputLen)
//{	
//	//定时辐射强度(IHU -> Cloud-) FE (Magic Number) 01 (Ver) 00 1E (Length)  27 12 (cmdid/req) 1237 (seq) (变长消息体) 0A 00 (BaseReq) 12 10 (data) 20 (L3控制字) 0E (Len) 00 22 (EMC) YY MM DD HH MM SS (T) 01 02 03 04 05 06 (GPS) 18 00 (Manufacture Data)

//	msg_struct_emc_asylibra_data_req_t *p;
//	p = (msg_struct_emc_asylibra_data_req_t*)input;
//	
//#if 0
//	UINT8 AdcHigh, AdcLow, TmpHigh, TmpLow;
//	UINT16 tmp;
//	UINT8 DataReqEmc[]   = {0xFE,0x01,0x00,0x1C,0x27,0x12,0x12,0x37,0x0A,0x00,0x12,0x0C,0x20,0x0A,0x00,0x22,0x15,0x08,0x17,0x23,0x01,0x02,0x00,0x01,0x18,0x02,0x27,0x11};
//	struct tm ymd;
//		
//	//入参检查
//	if (buf == NULL){
//		IhuErrorPrint("ASYLIBRA: Pack message func_asylibra_pack_equ_info error!");
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		return FAILURE;
//	}
//	
//	//Magic
//	DataReqEmc[0] = IHU_ASY_HEADER_MAGICNUMBER;
//	DataReqEmc[1] = IHU_ASY_HEADER_BVERSION;
//	
//	//Length
//	if (p->EmDeviceDataType == EDDT_wxDeviceHtmlChatView)
//	{tmp = sizeof(DataReqEmc);}  //IE_Type=18022711, default
//	else
//	{tmp = sizeof(DataReqEmc)-2;} //IE_Type=1800
//	len = tmp;
//	TmpHigh = (tmp >>8) & 0xFF;
//	TmpLow = tmp & 0xFF;
//	DataReqEmc[2] = TmpHigh;
//	DataReqEmc[3] = TmpLow;
//	//MessageType
//	tmp = ECI_req_sendData;
//	TmpHigh = (tmp >>8) & 0xFF;
//	TmpLow = tmp & 0xFF;
//	DataReqEmc[4] = TmpHigh;
//	DataReqEmc[5] = TmpLow;
//	//SEQ设置为最新的发送值
//	TmpHigh = (zIhuAsyLinkCtx.seqId >>8) & 0xFF;
//	TmpLow = zIhuAsyLinkCtx.seqId & 0xFF;
//	DataReqEmc[6] = TmpHigh;
//	DataReqEmc[7] = TmpLow;	
//	//User Cmd Id
//	DataReqEmc[12] = p->usercmdid;
//	DataReqEmc[13] = 0x0A;
//	//ADC Value 采用网络大头的方式进行赋值
//	uint16_t adcvalue = p->emc.emcValue;
//	AdcLow = adcvalue & 0xFF;
//	AdcHigh = (adcvalue >> 8) & 0xFF;
//	DataReqEmc[14] = AdcHigh;
//	DataReqEmc[15] = AdcLow;
//	//年月日时分秒的赋值	
//	ymd = ihu_clock_unix_to_ymd(p->emc.timeStamp);
//	
//	DataReqEmc[16] = (ymd.tm_year+1900-2000) & 0xFF;
//	DataReqEmc[17] = ymd.tm_mon+1;
//	DataReqEmc[18] = ymd.tm_mday;
//	DataReqEmc[19] = ymd.tm_hour;
//	DataReqEmc[20] = ymd.tm_min;
//	DataReqEmc[21] = ymd.tm_sec;
//	
//	//GPS，抑制掉，没有多大意义
//	//tmp4 = param->gps_x;
//	//TmpH4 = (tmp4>>24) & 0xFF;
//	//TmpH3 = (tmp4>>16) & 0xFF;
//	//TmpHigh = (tmp4>>8) & 0xFF;
//	//TmpLow = tmp & 0xFF;
//	//DataReqEmc[22] = TmpH4;
//	//DataReqEmc[23] = TmpH3;
//	//DataReqEmc[24] = TmpHigh;
//	//DataReqEmc[25] = TmpLow;		
//	//tmp4 = param->gps_y;
//	//TmpH4 = (tmp4>>24) & 0xFF;
//	//TmpH3 = (tmp4>>16) & 0xFF;
//	//TmpHigh = (tmp4>>8) & 0xFF;
//	//TmpLow = tmp & 0xFF;
//	//DataReqEmc[26] = TmpH4;
//	//DataReqEmc[27] = TmpH3;
//	//DataReqEmc[28] = TmpHigh;
//	//DataReqEmc[29] = TmpLow;	
//	//ntimes
//	tmp = p->emc.nTimes;
//	TmpHigh = (tmp >>8) & 0xFF;
//	TmpLow = tmp & 0xFF;
//	DataReqEmc[22] = TmpHigh;
//	DataReqEmc[23] = TmpLow;
//	//EmDeviceDataType的处理
//	if (p->EmDeviceDataType == EDDT_wxDeviceHtmlChatView)
//	{  //IE_Type=18022711, default
//		DataReqEmc[25] = 0x02; //长度
//		DataReqEmc[26] = 0x27;
//		DataReqEmc[27] = 0x11;
//	}
//	else
//	{  //IE_Type=1800	
//		DataReqEmc[25] = 0x00; //长度
//	} 	
//	memcpy(buf, DataReqEmc, len);
//#endif
//	
//	//自定义数据格式
//	//0x20,0x0A,0x00,0x22,0x15,0x08,0x17,0x23,0x01,0x02,0x00,0x01,
//	//该编码方式，极爽！
//	
//	SendDataRequest *request = (SendDataRequest *)malloc(sizeof(SendDataRequest));
//	request->has_type = true;
//	request->type = p->EmDeviceDataType;
//	request->data.data = (UINT8*)input;
//	request->data.len = inputLen;
//	epb_pack_send_data_request(request, (UINT8*)buf, len);
//	free(request);
//	
//	return SUCCESS;
//}

////Pack time_req的DATA_REQ链路层消息
//OPSTAT func_asylibra_pack_time_sync_req(INT8 *buf, UINT8 len, INT8 *input, UINT8 inputLen)
//{
//#if 0
//	UINT8 TmpHigh, TmpLow;
//	UINT16 tmp;
//	UINT8 TimeSync[] = {0xFE,0x01,0x00,0x10,0x27,0x12,0x12,0x37,0x0A,0x00,0x12,0x02,0xF2,0x00,0x18,0x00};

//	//入参检查
//	if (buf == NULL){
//		IhuErrorPrint("ASYLIBRA: Pack message func_asylibra_pack_time_sync_req error!");
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		return FAILURE;
//	}
//	
//	//Magic
//	*buf = IHU_ASY_HEADER_MAGICNUMBER;
//	*(buf+1) = IHU_ASY_HEADER_BVERSION;
//	//Length
//	len = sizeof(TimeSync);
//	TmpHigh = (len>>8) & 0xFF;
//	TmpLow = len & 0xFF;
//	*(buf+2) = TmpHigh;
//	*(buf+3) = TmpLow;
//	//MessageType
//	tmp = ECI_req_sendData;
//	TmpHigh = (tmp>>8) & 0xFF;
//	TmpLow = tmp & 0xFF;	
//	*(buf+4) = TmpHigh;
//	*(buf+5) = TmpLow;	
//	//SEQ设置为最新的发送值
//	TmpHigh = (zIhuAsyLinkCtx.seqId >> 8) & 0xFF;
//	TmpLow = zIhuAsyLinkCtx.seqId & 0xFF;
//	*(buf+6) = TmpHigh;
//	*(buf+7) = TmpLow;
//	memcpy(buf, &TimeSync[8], sizeof(TimeSync)-8);
//#endif

//	//自定义数据格式
//	//F2 00
//	//该编码方式，极爽！
//	
//	SendDataRequest *request = (SendDataRequest *)malloc(sizeof(SendDataRequest));
//	request->data.data = (UINT8*)input;
//	request->data.len = inputLen;
//	epb_pack_send_data_request(request, (UINT8*)buf, len);
//	free(request);

//	return SUCCESS;
//}

////Pack equ_info的DATA_REQ链路层消息
//OPSTAT func_asylibra_pack_equ_info(INT8 *buf, UINT8 len, INT8 *input, UINT8 inputLen)
//{
//#if 0	
//	UINT8 TmpHigh, TmpLow;
//	UINT16 tmp;
//	UINT8 EquipInfo[] = {0xFE,0x01,0x00,0x1C,0x27,0x12,0x12,0x37,0x0A,0x00,0x12,0x0E,0xF0,0x0C,0xD0,0x39,0x72,0xA5,0xEF,0x24,0x01,0x00,0x01,0x01,0x00,0x01,0x18,0x00};  //  0xF0 0x0C (Len) D0 39 72 A5 EF 24 (MAC) 01 (HW_Type) 00 01 (PEM1) 01 (Release1) 00 01 (SW_Delivery) 

//	//入参检查
//	if (buf == NULL){
//		IhuErrorPrint("ASYLIBRA: Pack message func_asylibra_pack_equ_info error!");
//		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
//		return FAILURE;
//	}	

//	//Magic
//	*buf = IHU_ASY_HEADER_MAGICNUMBER;
//	*(buf+1) = IHU_ASY_HEADER_BVERSION;
//	//Length
//	len = sizeof(EquipInfo);
//	TmpHigh = (len>>8) & 0xFF;
//	TmpLow = len & 0xFF;
//	*(buf+2) = TmpHigh;
//	*(buf+3) = TmpLow;
//	//MessageType
//	tmp = ECI_req_sendData;
//	TmpHigh = (tmp>>8) & 0xFF;
//	TmpLow = tmp & 0xFF;	
//	*(buf+4) = TmpHigh;
//	*(buf+5) = TmpLow;	
//	//SEQ设置为最新的发送值
//	TmpHigh = (zIhuAsyLinkCtx.seqId >> 8) & 0xFF;
//	TmpLow = zIhuAsyLinkCtx.seqId & 0xFF;
//	*(buf+6) = TmpHigh;
//	*(buf+7) = TmpLow;
//	
//	//消息体赋值: MAC
//	msg_struct_emc_asylibra_equ_info_sync_t *p;
//	p = (msg_struct_emc_asylibra_equ_info_sync_t*)input;
//	memcpy(&EquipInfo[14], p->hw_uuid, 6);

//	//消息体赋值: HW
//	EquipInfo[20] = p->hw_type;
//	tmp = p->hw_version;
//	TmpHigh = (tmp >>8) & 0xFF;
//	TmpLow = tmp & 0xFF;
//	EquipInfo[21] = TmpHigh;
//	EquipInfo[22] = TmpLow;
//	//消息体赋值: SW
//	EquipInfo[23] = p->sw_release;
//	tmp = p->sw_delivery;
//	TmpHigh = (tmp >>8) & 0xFF;
//	TmpLow = tmp & 0xFF;
//	EquipInfo[24] = TmpHigh;
//	EquipInfo[25] = TmpLow;	
//	memcpy(buf+8, &EquipInfo[8], sizeof(EquipInfo)-8);
//#endif

//	//自定义数据格式
//	//0xF0,0x0C,0xD0,0x39,0x72,0xA5,0xEF,0x24,0x01,0x00,0x01,0x01,0x00,0x01
//	//该编码方式，极爽！
//	
//	SendDataRequest *request = (SendDataRequest *)malloc(sizeof(SendDataRequest));
//	request->data.data = (UINT8*)input;
//	request->data.len = inputLen;
//	epb_pack_send_data_request(request, (UINT8*)buf, len);
//	free(request);

//	return SUCCESS;
//}


