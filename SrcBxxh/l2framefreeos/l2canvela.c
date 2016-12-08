/**
 ****************************************************************************************
 *
 * @file l2canvela.c
 *
 * @brief L2 CANVELA
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
 #include "l2canvela.h"
 
/*
** FSM of the CANVELA
*/
FsmStateItem_t FsmCanvela[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_canvela_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_canvela_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_canvela_restart},

 //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_CANVELA_INITED,         					fsm_canvela_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_CANVELA_INITED,         					fsm_canvela_stop_rcv},

	//Task level actived status
  {MSG_ID_COM_RESTART,        						FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CANVELA_ACTIVED,         				  fsm_canvela_time_out},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CANVELA_ACTIVED,         				  fsm_canvela_time_out},
	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	{MSG_ID_L3BFSC_CAN_INIT_RESP,						FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_init_resp},	//初始化过程
	{MSG_ID_L3BFSC_CAN_NEW_WS_EVENT,				FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_new_ws_event},	//收到新的物料
	{MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP,				FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_roll_out_resp}, //出料证实
	{MSG_ID_L3BFSC_CAN_GIVE_UP_RESP,				FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_give_up_resp},	//退料证实
	{MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT,	FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_error_status_report},	//差错报告发送
	{MSG_ID_L3BFSC_CAN_CMD_RESP,						FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_cmd_resp}, 
	{MSG_ID_CAN_L2FRAME_RCV,								FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_bfsc_l2frame_rcv}, 
	
#endif
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_canvela_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_CANVELA, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Error Set FSM State at fsm_canvela_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//串行会送INIT_FB给VM，不然消息队列不够深度，此为节省内存机制
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM。由于消息队列不够深，所以不再回送FB证实。
//		msg_struct_com_init_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
//		snd.length = sizeof(msg_struct_com_init_fb_t);
//		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_CANVELA, &snd, snd.length);
//		if (ret == IHU_FAILURE){
//			IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CANVELA], zIhuTaskNameList[src_id]);
//			return IHU_FAILURE;
//		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_CANVELA, FSM_STATE_CANVELA_INITED) == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Error Set FSM State!\n");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_canvela_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("CANVELA: Error initialize interface!\n");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_CANVELA] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_CANVELA, FSM_STATE_CANVELA_ACTIVED) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//启动本地定时器，如果有必要
	//测试性启动周期性定时器
	ret = ihu_timer_start(TASK_ID_CANVELA, TIMER_ID_1S_CANVELA_PERIOD_SCAN, zIhuSysEngPar.timer.canvelaPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Error start timer!\n");
		return IHU_FAILURE;
	}	
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("CANVELA: Enter FSM_STATE_CANVELA_ACTIVE status, Keeping refresh here!\n");
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("CANVELA: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_canvela_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_CANVELA)){
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_CANVELA, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_canvela_hw_init(void)
{
	return IHU_SUCCESS;
}

//TIMER_OUT Processing
OPSTAT fsm_canvela_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuRunErrCnt[TASK_ID_CANVELA]是否超限
	if (zIhuRunErrCnt[TASK_ID_CANVELA] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuRunErrCnt[TASK_ID_CANVELA] = zIhuRunErrCnt[TASK_ID_CANVELA] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_CANVELA, TASK_ID_CANVELA, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_CANVELA]++;
			IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CANVELA], zIhuTaskNameList[TASK_ID_CANVELA]);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_CANVELA_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_CANVELA) != FSM_STATE_CANVELA_ACTIVED){
			ret = FsmSetState(TASK_ID_CANVELA, FSM_STATE_CANVELA_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_CANVELA]++;
				IhuErrorPrint("CANVELA: Error Set FSM State!\n");
				return IHU_FAILURE;
			}//FsmSetState
		}
		func_canvela_time_out_period_scan();
	}

	return IHU_SUCCESS;
}

void func_canvela_time_out_period_scan(void)
{
	IhuDebugPrint("CANVELA: Time Out Test!\n");
}


#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
//收到MSG_ID_L3BFSC_CAN_INIT_RESP以后的处理过程
OPSTAT fsm_canvela_l3bfsc_init_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_l3bfsc_canvela_init_resp_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_canvela_init_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_canvela_init_resp_t))){
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//发送FRAME出去
	strIhuCanvelaCmdFrame_t snd;
	memset(&snd, 0, sizeof(strIhuCanvelaCmdFrame_t));
	ret = func_canvela_frame_encode(IHU_CANVELA_PREFIXH_node_resp, IHU_CANVELA_OPTID_node_set, 0, 0, &snd);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;		
	}
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	func_canvela_frame_send(&snd);
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_L3BFSC_CAN_NEW_WS_EVENT以后的处理过程
OPSTAT fsm_canvela_l3bfsc_new_ws_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_l3bfsc_canvela_new_ws_event_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t))){
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//发送FRAME出去
	strIhuCanvelaCmdFrame_t snd;
	memset(&snd, 0, sizeof(strIhuCanvelaCmdFrame_t));
	ret = func_canvela_frame_encode(IHU_CANVELA_PREFIXH_ws_resp, IHU_CANVELA_OPTID_wegith_read, 0, rcv.wsValue, &snd);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;		
	}
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	func_canvela_frame_send(&snd);	
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP以后的处理过程
OPSTAT fsm_canvela_l3bfsc_roll_out_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_l3bfsc_canvela_roll_out_resp_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_canvela_roll_out_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_canvela_roll_out_resp_t))){
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//发送FRAME出去
	strIhuCanvelaCmdFrame_t snd;
	memset(&snd, 0, sizeof(strIhuCanvelaCmdFrame_t));
	ret = func_canvela_frame_encode(IHU_CANVELA_PREFIXH_node_resp, IHU_CANVELA_OPTID_material_out_normal, 0, 0, &snd);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	func_canvela_frame_send(&snd);

	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_L3BFSC_CAN_GIVE_UP_RESP以后的处理过程
OPSTAT fsm_canvela_l3bfsc_give_up_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_l3bfsc_canvela_give_up_resp_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_canvela_give_up_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_canvela_give_up_resp_t))){
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//发送FRAME出去
	strIhuCanvelaCmdFrame_t snd;
	memset(&snd, 0, sizeof(strIhuCanvelaCmdFrame_t));
	ret = func_canvela_frame_encode(IHU_CANVELA_PREFIXH_node_resp, IHU_CANVELA_OPTID_material_give_up, 0, 0, &snd);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	func_canvela_frame_send(&snd);
	
	//返回
	return IHU_SUCCESS;
}

//MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT的处理
OPSTAT fsm_canvela_l3bfsc_error_status_report(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_l3bfsc_canvela_error_status_report_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_canvela_error_status_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_canvela_error_status_report_t))){
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//发送FRAME出去
	//这里应该通过新WS事件来体现，未来接口待完善
	strIhuCanvelaCmdFrame_t snd;
	memset(&snd, 0, sizeof(strIhuCanvelaCmdFrame_t));
	ret = func_canvela_frame_encode(IHU_CANVELA_PREFIXH_node_resp, IHU_CANVELA_OPTID_error_status, 0, rcv.errId, &snd);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	func_canvela_frame_send(&snd);
	
	//返回
	return IHU_SUCCESS;
}

//MSG_ID_L3BFSC_CAN_CMD_RESP
OPSTAT fsm_canvela_l3bfsc_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_l3bfsc_canvela_cmd_resp_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t))){
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if ((rcv.cmdid != IHU_BFSC_CAN_CMD_TYPE_RESP) || ((rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_ws_resp) && \
		(rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_motor_resp) && (rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_node_resp)))
	{
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}		
	
	//发送FRAME出去
	//这里应该通过新WS事件来体现，未来接口待完善
	strIhuCanvelaCmdFrame_t snd;
	memset(&snd, 0, sizeof(strIhuCanvelaCmdFrame_t));
	ret = func_canvela_frame_encode(rcv.cmd.prefixcmdid, rcv.cmd.optid, rcv.cmd.optpar, rcv.cmd.modbusVal, &snd);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	func_canvela_frame_send(&snd);
	
	//返回
	return IHU_SUCCESS;
}

//MSG_ID_CAN_L2FRAME_RCV
OPSTAT fsm_canvela_bfsc_l2frame_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_canvela_l2frame_rcv_t rcv;
	msg_struct_canvela_l3bfsc_cmd_ctrl_t snd;
	msg_struct_canvela_adc_ws_man_set_zero_t snd1;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_canvela_l2frame_rcv_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_l2frame_rcv_t))){
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//再使用strIhuCanvelaCmdFrame_t数据结果对其进行格式化
	strIhuCanvelaCmdFrame_t *pd;
	pd = (strIhuCanvelaCmdFrame_t *)rcv.data;
	
	//入参检查
	if ((pd->bfscCmdPrefixL != IHU_CANVELA_CMD_PREFIXL) || (pd->bfscCmdId != IHU_CANVELA_CMD_BFSC_ID))
	{
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	
	//以下是具体的处理
	UINT8 prefixcmdid = 0;
	UINT8 optid = 0;
	UINT8 optpar = 0;
	UINT32 modbusval = 0;
	ret = func_canvela_frame_decode(pd, prefixcmdid, optid, optpar, modbusval);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;		
	}	

	//心跳函数
	if (prefixcmdid == IHU_CANVELA_PREFIXH_heart_beat)
	{
		ret = func_canvela_frame_encode(IHU_CANVELA_PREFIXH_heart_beat, 0, 0, 0, pd);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
			zIhuRunErrCnt[TASK_ID_CANVELA]++;
			return IHU_FAILURE;		
		}
		func_canvela_frame_send(pd);
	}
	
	//整个节点的控制消息
	else if (prefixcmdid == IHU_CANVELA_PREFIXH_node_ctrl)
	{
		memset(&snd, 0, sizeof(msg_struct_canvela_l3bfsc_cmd_ctrl_t));
		snd.cmd.prefixcmdid = IHU_CANVELA_PREFIXH_node_ctrl;
		snd.cmd.optid = optid;
		snd.cmd.optpar = optpar;
		snd.cmd.modbusVal = modbusval;
		snd.length = sizeof(msg_struct_canvela_l3bfsc_cmd_ctrl_t);
		ret = ihu_message_send(MSG_ID_CAN_L3BFSC_CMD_CTRL, TASK_ID_BFSC, TASK_ID_CANVELA, &snd, snd.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_CANVELA]++;
			IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CANVELA], zIhuTaskNameList[TASK_ID_BFSC]);
			return IHU_FAILURE;
		}	
	}

	//WS控制中的SET ZERO手动清零命令
	else if ((prefixcmdid == IHU_CANVELA_PREFIXH_ws_ctrl) && (optid == IHU_CANVELA_OPTID_manual_set_zero))
	{
		//只有在BFSC主控模块不在ACTIVE状态下，就使用直接穿透方式去设置ADC-WS的手动清零指令
		if (FsmGetState(TASK_ID_BFSC) != FSM_STATE_CANVELA_ACTIVED)
		{
			memset(&snd1, 0, sizeof(msg_struct_canvela_adc_ws_man_set_zero_t));
			snd1.length = sizeof(msg_struct_canvela_adc_ws_man_set_zero_t);
			ret = ihu_message_send(MSG_ID_CAN_ADC_WS_MAN_SET_ZERO, TASK_ID_ADCLIBRA, TASK_ID_CANVELA, &snd1, snd1.length);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_CANVELA]++;
				IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CANVELA], zIhuTaskNameList[TASK_ID_ADCLIBRA]);
				return IHU_FAILURE;
			}			
		}
	}
	
	//WS控制
	else if (prefixcmdid == IHU_CANVELA_PREFIXH_ws_ctrl)
	{
		memset(&snd, 0, sizeof(msg_struct_canvela_l3bfsc_cmd_ctrl_t));
		snd.cmd.prefixcmdid = IHU_CANVELA_PREFIXH_ws_ctrl;
		snd.cmd.optid = optid;
		snd.cmd.optpar = optpar;
		snd.cmd.modbusVal = modbusval;
		snd.length = sizeof(msg_struct_canvela_l3bfsc_cmd_ctrl_t);
		ret = ihu_message_send(MSG_ID_CAN_L3BFSC_CMD_CTRL, TASK_ID_BFSC, TASK_ID_CANVELA, &snd, snd.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_CANVELA]++;
			IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CANVELA], zIhuTaskNameList[TASK_ID_BFSC]);
			return IHU_FAILURE;
		}	
	}
	
	//MOTO控制
	else if (prefixcmdid == IHU_CANVELA_PREFIXH_motor_ctrl)
	{
		memset(&snd, 0, sizeof(msg_struct_canvela_l3bfsc_cmd_ctrl_t));
		snd.cmd.prefixcmdid = IHU_CANVELA_PREFIXH_motor_ctrl;
		snd.cmd.optid = optid;
		snd.cmd.optpar = optpar;
		snd.cmd.modbusVal = modbusval;
		snd.length = sizeof(msg_struct_canvela_l3bfsc_cmd_ctrl_t);
		ret = ihu_message_send(MSG_ID_CAN_L3BFSC_CMD_CTRL, TASK_ID_BFSC, TASK_ID_CANVELA, &snd, snd.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_CANVELA]++;
			IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CANVELA], zIhuTaskNameList[TASK_ID_BFSC]);
			return IHU_FAILURE;
		}	
	}
	
	//不正常的参数
	else
	{
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Input parameters error!\n");
		return IHU_FAILURE;	
	}
	
	//返回
	return IHU_SUCCESS;
}

//生成FRAME函数
OPSTAT func_canvela_frame_encode(UINT8 prefixcmdid, UINT8 optid, UINT8 optpar, UINT32 modbusval, strIhuCanvelaCmdFrame_t *pframe)
{
	//先检查输入参数
	if ((optid <= IHU_CANVELA_OPTID_min) || (optid >= IHU_CANVELA_OPTID_max) || (pframe == NULL)){
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Input parameters error!\n");
		return IHU_FAILURE;
	}

	//按字节，生成消息结构帧
	pframe->bfscCmdPrefixH = prefixcmdid;
	pframe->bfscCmdPrefixL = IHU_CANVELA_CMD_PREFIXL;
	pframe->bfscCmdId = IHU_CANVELA_CMD_BFSC_ID;
	pframe->bfscOptId = optid;

	//分别针对不同的OPTID进行帧的分类处理
	switch(optid){
	case IHU_CANVELA_OPTID_wegith_read:
		//do nothing
		break;

	case IHU_CANVELA_OPTID_auto_zero_track:
		//Modbus值起作用
		pframe->bfscOptPar = modbusval & 0xFF;
		pframe->bfscPar1 = ((modbusval & 0xFF00) >> 8) & 0xFF;
		pframe->bfscPar2 = ((modbusval & 0xFF0000) >> 16) & 0xFF;
		pframe->bfscPar3 = ((modbusval & 0xFF000000) >> 24) & 0xFF;
		break;

	case IHU_CANVELA_OPTID_min_sensitivity:
		//Modbus值起作用
		pframe->bfscOptPar = modbusval & 0xFF;
		pframe->bfscPar1 = ((modbusval & 0xFF00) >> 8) & 0xFF;
		pframe->bfscPar2 = ((modbusval & 0xFF0000) >> 16) & 0xFF;
		pframe->bfscPar3 = ((modbusval & 0xFF000000) >> 24) & 0xFF;
		break;

	case IHU_CANVELA_OPTID_manual_set_zero:
		//Modbus值起作用
		pframe->bfscOptPar = modbusval & 0xFF;
		pframe->bfscPar1 = ((modbusval & 0xFF00) >> 8) & 0xFF;
		pframe->bfscPar2 = ((modbusval & 0xFF0000) >> 16) & 0xFF;
		pframe->bfscPar3 = ((modbusval & 0xFF000000) >> 24) & 0xFF;
		break;

	case IHU_CANVELA_OPTID_static_detect_range:
		//Modbus值起作用
		pframe->bfscOptPar = modbusval & 0xFF;
		pframe->bfscPar1 = ((modbusval & 0xFF00) >> 8) & 0xFF;
		pframe->bfscPar2 = ((modbusval & 0xFF0000) >> 16) & 0xFF;
		pframe->bfscPar3 = ((modbusval & 0xFF000000) >> 24) & 0xFF;
		break;

	case IHU_CANVELA_OPTID_static_detect_duration:
		//Modbus值起作用
		pframe->bfscOptPar = modbusval & 0xFF;
		pframe->bfscPar1 = ((modbusval & 0xFF00) >> 8) & 0xFF;
		pframe->bfscPar2 = ((modbusval & 0xFF0000) >> 16) & 0xFF;
		pframe->bfscPar3 = ((modbusval & 0xFF000000) >> 24) & 0xFF;
		break;

	case IHU_CANVELA_OPTID_weight_scale_calibration:
		pframe->bfscOptPar = optpar;
		break;

	case IHU_CANVELA_OPTID_motor_turn_around:
		pframe->bfscOptPar = optpar;
		break;

	case IHU_CANVELA_OPTID_motor_speed:
		//do nothing
		break;

	case IHU_CANVELA_OPTID_scale_range:
		//do nothing
		break;

	case IHU_CANVELA_OPTID_node_set:
	//do nothing
	break;
	
	default:
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Input parameters error!\n");
		return IHU_FAILURE;
		//break;
	}

	//返回
	return IHU_SUCCESS;
}

//解码FRAME的函数
OPSTAT func_canvela_frame_decode(strIhuCanvelaCmdFrame_t *pframe, UINT8 prefixcmdid, UINT8 optid, UINT8 optpar, UINT32 modbusval)
{
	//入参检查
	if (pframe == NULL){
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Input parameters error!\n");
		return IHU_FAILURE;
	}

	//检查最重要的参数
	if ((pframe->bfscCmdPrefixH <= IHU_CANVELA_PREFIXH_min) || (pframe->bfscCmdPrefixH >= IHU_CANVELA_PREFIXH_max) || (pframe->bfscCmdPrefixL != IHU_CANVELA_CMD_PREFIXL) || (pframe->bfscCmdId != IHU_CANVELA_CMD_BFSC_ID)){
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Frame header error!\n");
		return IHU_FAILURE;
	}
	prefixcmdid = pframe->bfscCmdId;
	optid = pframe->bfscOptId;
	if ((optid <=IHU_CANVELA_OPTID_min) || (optid >=IHU_CANVELA_OPTID_max)){
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Encoding Canitf frame OptId error!\n");
		return IHU_FAILURE;
	}

	//分类处理不同的情况
	switch(optid){
	case IHU_CANVELA_OPTID_wegith_read:
		//do nothing
		break;

	case IHU_CANVELA_OPTID_auto_zero_track:
		//Modbus值起作用
		modbusval = pframe->bfscOptPar + (pframe->bfscPar1 << 8) + (pframe->bfscPar2 << 16) + (pframe->bfscPar3 << 24);
		break;

	case IHU_CANVELA_OPTID_min_sensitivity:
		//Modbus值起作用
		modbusval = pframe->bfscOptPar + (pframe->bfscPar1 << 8) + (pframe->bfscPar2 << 16) + (pframe->bfscPar3 << 24);
		break;

	case IHU_CANVELA_OPTID_manual_set_zero:
		//Modbus值起作用
		modbusval = pframe->bfscOptPar + (pframe->bfscPar1 << 8) + (pframe->bfscPar2 << 16) + (pframe->bfscPar3 << 24);
		break;

	case IHU_CANVELA_OPTID_static_detect_range:
		//Modbus值起作用
		modbusval = pframe->bfscOptPar + (pframe->bfscPar1 << 8) + (pframe->bfscPar2 << 16) + (pframe->bfscPar3 << 24);
		break;

	case IHU_CANVELA_OPTID_static_detect_duration:
		//Modbus值起作用
		modbusval = pframe->bfscOptPar + (pframe->bfscPar1 << 8) + (pframe->bfscPar2 << 16) + (pframe->bfscPar3 << 24);
		break;

	case IHU_CANVELA_OPTID_weight_scale_calibration:
		optpar = pframe->bfscOptPar;
		break;

	case IHU_CANVELA_OPTID_motor_turn_around:
		optpar = pframe->bfscOptPar;
		break;

	case IHU_CANVELA_OPTID_motor_speed:
		//do nothing
		break;

	case IHU_CANVELA_OPTID_scale_range:
		//do nothing
		break;
	
	case IHU_CANVELA_OPTID_node_set:
		//do nothing
		break;
	default:
		zIhuRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Input parameters error!\n");
		return IHU_FAILURE;
		//break;
	}

	//返回
	return IHU_SUCCESS;
}

//将L2FRAME发送出去
void func_canvela_frame_send(strIhuCanvelaCmdFrame_t *pframe)
{
	//直接调用BSP_STM32的函数发送出去
	ihu_l1hd_can_iau_send_data((UINT8 *)pframe, sizeof(strIhuCanvelaCmdFrame_t));
}

#endif //#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	


