/**
 ****************************************************************************************
 *
 * @file vmda_shell_task.c
 *
 * @brief VMDA SHELL task
 *
 * BXXH team
 * Created by ZJL, 20160127
 *
 ****************************************************************************************
 */
 
#include "vmda_shell_task.h"
//#include "buffer_man.h"
	
/*
 *  ==========================PART1========================================
 *
 *  采用DA14580 SPS工程体系的任务处理机制
 *  - 该任务模块，VMDA SHELL的主要功能，就是将底层BLE的消息分发给上层处理模块
 *  - 内部的上层处理模块处理后的消息，将通过该模块，发送给底层BLE过程
 *  - 本任务将具备传统的DA工程模块体系，同时也具备上层VM任务模块体系
 *  - 不做消息处理，只做消息转发
 *
 */

//按照SPS工程：消息路由表
const struct ke_msg_handler bxxh_fsm_vmda_default[] =
{
	{MSG_BLEAPP_VMDA_INIT, 								(ke_msg_func_t)hlBxxhVmdaBleInit},
	{MSG_ID_VMDA_TIMER_1S_PERIOD, 				(ke_msg_func_t)hlBxxhVmdaSysClock1sPeriod},
	{MSG_SPS_VMDA_BLE_DATA_DL_RCV,				(ke_msg_func_t)hlBxxhVmdaSpsBleDataDlRcv},
	{MSG_BLEAPP_VMDA_DISCONNECT, 					(ke_msg_func_t)hlBxxhVmdaBleDisc},
};

//按照SPS工程：定义全局变量
const struct ke_state_handler bxxh_handler_vmda_default = KE_STATE_HANDLER(bxxh_fsm_vmda_default); 
ke_state_t bxxh_vmda_state[BXXH_VMDA_IDX_MAX];
bxxh_vmda_env_tag_t bxxh_vmda_env;
static const struct ke_task_desc BXXH_TASK_DESC_VMDA = {NULL, &bxxh_handler_vmda_default, bxxh_vmda_state, BXXH_STATE_VMDA_MAX, BXXH_VMDA_IDX_MAX};

//按照SPS工程：初始化
void bxxh_task_vmda_init(void)
{
	// Reset the wxapp_receivererometer environment
	memset(&bxxh_vmda_env, 0, sizeof(bxxh_vmda_env_tag_t));	
	// Create VMDA task
	ke_task_create(TASK_VMDA, &BXXH_TASK_DESC_VMDA);
	// Go to IDLE state
	ke_state_set(TASK_VMDA, BXXH_STATE_VMDA_DISABLE);	
	
	/* MYC 2015/10/25 */
	//AmtMainLoopGuiMsgBufferInit(&gAmtMainLoopMsgBuffer);

	return;
}

//主函数钩子，可以通过这个机制生成定时器，暂时不这么用，所以空着啥都不干
void bxxh_task_vmda_mainloop(void)
{
	//未来可以考虑，将各个模块的RESTART机制放在这儿，即实现zIhuRunErrCnt[]的监控执行，从而实现各个模块的RESTART	
	return;
}

//按照SPS工程：初始化处理函数API
OPSTAT hlBxxhVmdaBleInit(ke_msg_id_t const msgid, msg_struct_bleapp_vmda_init_t const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{	
	int ret = 0;
	char strDebug[BX_PRINT_SZ];		
	
	//入参检查
	if ((param == NULL) || (dest_id != TASK_VMDA)){
		IhuErrorPrint("VMDA: Wrong hlBxxhVmdaBleInit incoming parameters!");
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		return FAILURE;
	}
	
	//启动周期性定时器
	vmda_timer_set(MSG_ID_VMDA_TIMER_1S_PERIOD, TASK_VMDA, VMDA_TIMER_1SECOND_CLOCK);
	
	//发送MSG_ID_VMDA_ASY_BLE_CON给ASYLIBRA模块
	msg_struct_vmda_asylibra_ble_con_t snd;
	memset(&snd, 0, sizeof(msg_struct_vmda_asylibra_ble_con_t));
	snd.length = sizeof(msg_struct_vmda_asylibra_ble_con_t);
	ret = ihu_message_send(MSG_ID_VMDA_ASYLIBRA_BLE_CON, TASK_ID_ASYLIBRA, TASK_ID_VMDA, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "VMDA: Send MSG_ID_VMDA_ASYLIBRA_BLE_CON message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_VMDA], zIhuTaskNameList[TASK_ID_ASYLIBRA]);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
		
	//状态转移到连接态
	ke_state_set(TASK_VMDA, BXXH_STATE_VMDA_CONNECT);	
	
	//返回
	return (KE_MSG_CONSUMED);
}

//VM及上层系统时钟的定海神针！
OPSTAT hlBxxhVmdaSysClock1sPeriod(ke_msg_id_t const msgid, msg_struct_vmda_1s_period_timtout_t const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//入参检查
	if ((param == NULL) || (dest_id != TASK_VMDA)){
		IhuErrorPrint("VMDA: Wrong hlBxxhVmdaSysClock1sPeriod incoming parameters!");
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		return FAILURE;
	}

	//简易时钟，更新时钟逻辑
	zIhuSystemTimeUnix++;
	zIhuSystemTimeYmd = *(localtime(&zIhuSystemTimeUnix)); //节省系统处理能力，需要的时候再做该转化
	
	//调用TIMER_1S中的处理函数，该函数相当于抛锚点，定时激活TIMER_1S的扫描过程，从而实现定时任务的发生
	ihu_timer_routine_handler_1s();
	
	//再次启动周期性定时器
	vmda_timer_set(MSG_ID_VMDA_TIMER_1S_PERIOD, TASK_VMDA, VMDA_TIMER_1SECOND_CLOCK);
		
	//返回
	return (KE_MSG_CONSUMED);
}

//按照SPS工程：接收到数据的总处理函数
OPSTAT hlBxxhVmdaSpsBleDataDlRcv(ke_msg_id_t const msgid, msg_struct_sps_vmda_ble_data_dl_rcv_t const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	int ret = 0;
	char strDebug[BX_PRINT_SZ];	
	
	//入参检查
	if ((param == NULL) || (dest_id != TASK_VMDA)){
		IhuErrorPrint("VMDA: Wrong hlBxxhVmdaSpsBleDataDlRcv incoming parameters!");
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		return FAILURE;
	}	
	if (param->length > MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL){
		sprintf(strDebug, "VMDA: Too long buffer received from BLE, length=%d > limitation=%d.", param->length, MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL);
		IhuErrorPrint(strDebug);
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		return FAILURE;
	}

	//发送MSG_ID_VMDA_ASYLIBRA_DATA_DL给ASYLIBRA模块
	msg_struct_vmda_asylibra_data_dl_t snd;
	memset(&snd, 0, sizeof(msg_struct_vmda_asylibra_data_dl_t));
	snd.length = param->length;
	memcpy(snd.dataBuf, param->value, snd.length);	
	ret = ihu_message_send(MSG_ID_VMDA_ASYLIBRA_DATA_DL, TASK_ID_ASYLIBRA, TASK_ID_VMDA, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "VMDA: Send MSG_ID_VMDA_ASYLIBRA_DATA_DL message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_VMDA], zIhuTaskNameList[TASK_ID_ASYLIBRA]);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	
	//状态转移到连接态
	ke_state_set(TASK_VMDA, BXXH_STATE_VMDA_CONNECT);
	
	return (KE_MSG_CONSUMED);
}


//按照SPS工程：处理拆链过程
OPSTAT hlBxxhVmdaBleDisc(ke_msg_id_t const msgid, msg_struct_bleapp_vmda_disconnect_t const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	int ret = 0;
	char strDebug[BX_PRINT_SZ];		
	
	//入参检查
	if ((param == NULL) || (dest_id != TASK_VMDA)){
		IhuErrorPrint("VMDA: Wrong hlBxxhVmdaBleDisc incoming parameters!");
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		return FAILURE;
	}
		
	//发送MSG_ID_VMDA_ASY_BLE_DISC给ASYLIBRA模块
	msg_struct_vmda_asylibra_ble_disc_t snd;
	memset(&snd, 0, sizeof(msg_struct_vmda_asylibra_ble_disc_t));
	snd.length = sizeof(msg_struct_vmda_asylibra_ble_disc_t);
	ret = ihu_message_send(MSG_ID_VMDA_ASYLIBRA_BLE_DISC, TASK_ID_ASYLIBRA, TASK_ID_VMDA, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "VMDA: Send MSG_ID_VMDA_ASYLIBRA_BLE_DISC message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_VMDA], zIhuTaskNameList[TASK_ID_ASYLIBRA]);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}

	//状态转移到连接态
	ke_state_set(TASK_VMDA, BXXH_STATE_VMDA_DISABLE);
	
	//返回
	return (KE_MSG_CONSUMED);
}


/*
 *  
 *  ==========================PART2========================================
 *
 *  采用VM任务模块机制的状态转移机
 *  本模块内部的消息队列，将被VM进行持续扫描 
 *
 */
/*
** FSM of the VMDA
*/
FsmStateItem_t FsmVmda[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_vmda_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_vmda_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_vmda_restart},
  {MSG_ID_COM_INIT_FB,										FSM_STATE_IDLE,            									fsm_com_do_nothing},

  //Task level initialization
	//允许在这种状态下，接收到INIT消息，并重新初始化整个系统，比如TIMER不正常后，可以发送INIT消息到这儿，从而重新初始化上层系统
  {MSG_ID_COM_RESTART,        						FSM_STATE_VMDA_ACTIVE,         							fsm_vmda_restart},
  {MSG_ID_COM_INIT,       								FSM_STATE_VMDA_ACTIVE,            					fsm_vmda_init},	
  {MSG_ID_COM_INIT_FB,										FSM_STATE_VMDA_ACTIVE,         							fsm_com_do_nothing},
  {MSG_ID_COM_STOP,												FSM_STATE_VMDA_ACTIVE,         							fsm_vmda_stop_rcv},  //消息逻辑的来源待定
  {MSG_ID_COM_TIME_OUT,       						FSM_STATE_VMDA_ACTIVE,            					fsm_vmda_init_time_out},		
  {MSG_ID_ASYLIBRA_VMDA_DATA_UL,       		FSM_STATE_VMDA_ACTIVE,            					fsm_vmda_data_ul_send},
	
  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_VMDA_WF_INIT_FB,         					fsm_vmda_restart},
  {MSG_ID_COM_INIT_FB,										FSM_STATE_VMDA_WF_INIT_FB,         					fsm_vmda_init_fb_rcv},	
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables def
bool zVmdaInitFbRcvCtrl[MAX_TASK_NUM_IN_ONE_IHU];

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_vmda_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_VMDA, FSM_STATE_IDLE) == FAILURE){
		IhuErrorPrint("VMDA: Error Set FSM_STATE_IDLE State at fsm_vmda_task_entry.");
	}
	return SUCCESS;
}

OPSTAT fsm_vmda_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	int ret = 0;
	
	//入参检查
	if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
		IhuErrorPrint("VMDA: Receive fsm_vmda_init wrong message!");
		return FAILURE;
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_VMDA, FSM_STATE_VMDA_INITED) == FAILURE){
		IhuErrorPrint("VMDA: Error Set FSM_STATE_VMDA_INITED State!");	
		return FAILURE;
	}

	//初始化硬件接口
	if (func_vmda_hw_init() == FAILURE){	
		IhuErrorPrint("VMDA: Error func_vmda_hw_init initialize interface!");
		return FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_VMDA] = 0;
	memset(zVmdaInitFbRcvCtrl, FALSE, sizeof(bool)*MAX_TASK_NUM_IN_ONE_IHU);

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_VMDA, FSM_STATE_VMDA_ACTIVE) == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("VMDA: Error Set FSM_STATE_VMDA_ACTIVE State!");
		return FAILURE;
	}

	//打印报告进入常规状态	
	if ((zIhuSysEngPar.debugMode & TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("VMDA: Enter FSM_STATE_VMDA_ACTIVE status, Keeping refresh here!");
	}

	//发送INIT消息给第一个任务
	if (func_vmda_send_out_init(TASK_ID_TIMER) == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("VMDA: Error send MSG_ID_COM_INIT message to task TIMER!");
	}
		
	//启动本地定时器
	ret = ihu_timer_start(TASK_ID_VMDA, TIMER_ID_1S_VMDA_INIT_FB, VMDA_TIMER_DURATION_INIT_FB, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("VMDA: Error ihu_timer_start start timer!");
		return FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_VMDA, FSM_STATE_VMDA_WF_INIT_FB) == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("VMDA: Error Set FSM_STATE_VMDA_WF_INIT_FB State!");
		return FAILURE;
	}
	
	//返回
	return SUCCESS;
}

OPSTAT fsm_vmda_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	IhuErrorPrint("VMDA: Internal error counter reach DEAD level, SW-RESTART soon!!!");
	fsm_vmda_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT fsm_vmda_init_fb_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	int ret = 0;
	char strDebug[BX_PRINT_SZ];	
	//入参检查
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "VMDA: Error fsm_vmda_init_fb_rcv on task_id, dest_id=%d!!!", dest_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "VMDA: Error fsm_vmda_init_fb_rcv on task_id, src_id=%d!!!", src_id);		
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	if (param_len>MAX_IHU_MSG_BODY_LENGTH){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "VMDA: Too large message length than IHU set capability, param_len=%d!!!", param_len);			
		IhuErrorPrint(strDebug);
		return FAILURE;
	}

	if (zVmdaInitFbRcvCtrl[src_id] == FALSE){
		zVmdaInitFbRcvCtrl[src_id] = TRUE;
	}else{
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "VMDA: Error receive MSG_ID_COM_INIT_FB from src_id=%d!!!", src_id);		
		IhuErrorPrint(strDebug);
		return FAILURE;
	}

	//检查INIT_FB时候收齐全
	if (func_vmda_check_init_fb_complete() == FAILURE){
		if (zVmdaInitFbRcvCtrl[TASK_ID_ASYLIBRA] == FALSE)
		{
			if (func_vmda_send_out_init(TASK_ID_ASYLIBRA) == FAILURE){
				zIhuRunErrCnt[TASK_ID_VMDA]++;	
				IhuErrorPrint("VMDA: Send MSG_ID_COM_INIT message to TASK_ID_ASYLIBRA error!");
				return FAILURE;
			}
			//保持FSM_STATE_VMDA_WF_INIT_FB状态
			return SUCCESS;
		}
		if (zVmdaInitFbRcvCtrl[TASK_ID_ADCARIES] == FALSE)
		{
			if (func_vmda_send_out_init(TASK_ID_ADCARIES) == FAILURE){
				zIhuRunErrCnt[TASK_ID_VMDA]++;	
				IhuErrorPrint("VMDA: Send MSG_ID_COM_INIT message to TASK_ID_ADCARIES error!");
				return FAILURE;
			}
			//保持FSM_STATE_VMDA_WF_INIT_FB状态
			return SUCCESS;
		}
		if (zVmdaInitFbRcvCtrl[TASK_ID_EMC] == FALSE)
		{
			if (func_vmda_send_out_init(TASK_ID_EMC) == FAILURE){
				zIhuRunErrCnt[TASK_ID_VMDA]++;	
				IhuErrorPrint("VMDA: Send MSG_ID_COM_INIT message to TASK_ID_EMC error!");
				return FAILURE;
			}
			//保持FSM_STATE_VMDA_WF_INIT_FB状态
			return SUCCESS;
		}
	}
	
	//停止INIT_FB时钟
	ret = ihu_timer_stop(TASK_ID_VMDA, TIMER_ID_1S_VMDA_INIT_FB, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("VMDA: Error ihu_timer_stop stop timer!\n");
		return FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_VMDA, FSM_STATE_VMDA_ACTIVE) == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("VMDA: Error Set FSM_STATE_VMDA_ACTIVE State!");
		return FAILURE;
	}
	
	//打印报告进入常规状态	
	//if ((zIhuSysEngPar.debugMode & TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("VMDA: VMDA Shell and all L2/L3 layers init complete, enter into permant working status!");
	//}
	return SUCCESS;
}


OPSTAT fsm_vmda_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	//int ret = 0;
	//char strDebug[BX_PRINT_SZ];
	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_VMDA)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("VMDA: Wrong fsm_vmda_stop_rcv input paramters!");
		return FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_VMDA, FSM_STATE_IDLE) == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("VMDA: Error Set FSM_STATE_IDLE State!");
		return FAILURE;
	}
	
	/*
	//驱动ASYLIBRA任务模块进入OFFLINE状态，然后再通知EMC等L3模块
	msg_struct_com_stop_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_stop_t));
	snd.length = sizeof(msg_struct_com_stop_t);
	ret = ihu_message_send(MSG_ID_COM_STOP, TASK_ID_ASYLIBRA, TASK_ID_VMDA, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "VMDA: Send MSG_ID_COM_STOP message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_VMDA], zIhuTaskNameList[TASK_ID_ASYLIBRA]);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}	
	*/
	//返回
	return SUCCESS;
}

OPSTAT fsm_vmda_init_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	int ret=0;
	char strDebug[BX_PRINT_SZ];
	
	//收消息
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if (param_len != sizeof(msg_struct_com_time_out_t)){
		IhuErrorPrint("VMDA: Receive MSG_ID_COM_TIME_OUT message error!");
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	if ((rcv.timeId == TIMER_ID_1S_VMDA_INIT_FB) && (rcv.timeRes == TIMER_RESOLUTION_1S)){
		IhuErrorPrint("VMDA: Error! MSG_ID_COM_TIME_OUT before receve all tasks feedback, fatal error!");
		//不用考虑STOP TIMER，底层TIMER任务在发出TIME_OUT时已经自动停止了该定时器，除非定时器错误，比如类型设置为PERIOD才会不断重报
		ret = FsmSetState(TASK_ID_VMDA, FSM_STATE_IDLE);
		if (ret == FAILURE){
			zIhuRunErrCnt[TASK_ID_VMDA]++;
			IhuErrorPrint("VMDA: Error set FSM_STATE_IDLE state!\n");
			return FAILURE;
		}

		//先使用无限重复INIT所有任务的方式吧，给自己再发送一个INIT消息
		msg_struct_com_init_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_t));
		snd.length = sizeof(msg_struct_com_init_t);
		ret = ihu_message_send(MSG_ID_COM_INIT, TASK_ID_VMDA, TASK_ID_VMDA, &snd, snd.length);
		if (ret == FAILURE){
			zIhuRunErrCnt[TASK_ID_VMDA]++;
			sprintf(strDebug, "VMDA: Send MSG_ID_COM_INIT message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_VMDA], zIhuTaskNameList[TASK_ID_VMDA]); 
			IhuErrorPrint(strDebug);
			return FAILURE;
		}
	}else{
		IhuErrorPrint("VMDA: Error! TIME-OUT message received with wrong timerId or timeResolution!");
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		return FAILURE;
	}
	
	//返回
	return SUCCESS;
}

//MSG_ID_ASYLIBRA_VMDA_DATA_UL消息的处理，以便发送到BLE底层
OPSTAT fsm_vmda_data_ul_send(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	int ret=0;
	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_VMDA)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("VMDA: Wrong fsm_vmda_data_ul_send input paramters!");
		return FAILURE;
	}	
	
	//收消息
	msg_struct_vmda_asylibra_data_ul_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_vmda_asylibra_data_ul_t));
	if (param_len > sizeof(msg_struct_vmda_asylibra_data_ul_t)){
		IhuErrorPrint("VMDA: Receive MSG_ID_ASYLIBRA_VMDA_DATA_UL message error!");
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//发送内容体到BLE底层
	__disable_irq();
	bxxh_data_send_to_ble((uint8_t *)&rcv.dataBuf, rcv.length);
	__enable_irq();
	
	//继续保持在FSM_STATE_VMDA_ACTIVE状态
	ret = FsmSetState(TASK_ID_VMDA, FSM_STATE_VMDA_ACTIVE);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("VMDA: Error set FSM_STATE_VMDA_ACTIVE state!\n");
		return FAILURE;
	}

	//返回
	return SUCCESS;
}

//Local functions
OPSTAT func_vmda_hw_init(void)
{
	return SUCCESS;
}

//一次性发送INIT消息给目标任务
OPSTAT func_vmda_send_out_init(UINT8 task_id)
{
	int ret = 0;
	char strDebug[BX_PRINT_SZ];	
	
	//入参检查task_id
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "VMDA: Error func_vmda_send_out_init on task_id, dest_id=%d!!!", task_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
		
	//检查task_id对应的PNP状态
	if (zIhuTaskInfo[task_id].pnpState != IHU_TASK_PNP_ON){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("VMDA: Task PNP state not meet request, not able to execute send command!");
		return FAILURE;
	}
		
	msg_struct_com_init_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_init_t));
	snd.length = sizeof(msg_struct_com_init_t);
	ret = ihu_message_send(MSG_ID_COM_INIT, task_id, TASK_ID_VMDA, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "VMDA: Send MSG_ID_COM_INIT message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_VMDA], zIhuTaskNameList[task_id]);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	return SUCCESS;
}

//检查所有INIT_FB时候收齐
OPSTAT func_vmda_check_init_fb_complete(void)
{
	int i = 0;
	
	for (i = 0; i<MAX_TASK_NUM_IN_ONE_IHU; i++){
		if ((zIhuTaskInfo[i].pnpState == IHU_TASK_PNP_ON) && (i != TASK_ID_VMDA) && (zVmdaInitFbRcvCtrl[i] == FALSE))
		{
			return FAILURE;
		}
	}
	return SUCCESS;
}

