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
 #include "l2packet.h"
 
/*
** FSM of the CANVELA
*/
IhuFsmStateItem_t IhuFsmCanvela[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_canvela_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_canvela_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_IDLE,            							    fsm_com_do_nothing},

 //Task level initialization
  {MSG_ID_COM_INIT,       								FSM_STATE_CANVELA_INITED,            				fsm_canvela_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_CANVELA_INITED,            				fsm_com_do_nothing},

	//ANY state entry
  {MSG_ID_COM_INIT_FB,                    FSM_STATE_COMMON,                           fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,                 FSM_STATE_COMMON,                           fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,              FSM_STATE_COMMON,                           fsm_com_do_nothing},
	{MSG_ID_COM_STOP,                       FSM_STATE_COMMON,                           fsm_canvela_stop_rcv},
  {MSG_ID_COM_RESTART,                    FSM_STATE_COMMON,                           fsm_canvela_restart},
	{MSG_ID_COM_TIME_OUT,                   FSM_STATE_COMMON,                           fsm_canvela_time_out},

	//Task level actived status
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	{MSG_ID_L3BFSC_CAN_INIT_RESP,						FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_init_resp},	//初始化过程
	{MSG_ID_L3BFSC_CAN_NEW_WS_EVENT,				FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_new_ws_event},	//收到新的物料
	{MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP,				FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_roll_out_resp}, //出料证实
	{MSG_ID_L3BFSC_CAN_GIVE_UP_RESP,				FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_give_up_resp},	//退料证实
	{MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT,	FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_error_status_report},	//差错报告发送
	{MSG_ID_L3BFSC_CAN_CMD_RESP,						FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_cmd_resp}, 
	{MSG_ID_CAN_L2FRAME_RCV,								FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_bfsc_l2frame_rcv},

	//MYC update for state machine
	{MSG_ID_L3BFSC_WMC_STARTUP_IND,					FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP,			FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_GET_CONFIG_RESP,			FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_START_RESP,					FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_STOP_RESP,						FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_WEIGHT_IND,					FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_COMBIN_RESP,					FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_FAULT_IND,						FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_COMMAND_RESP,				FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	
#endif
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
#else
	#error Un-correct constant definition
#endif

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

	//串行回送INIT_FB给VMFO
	ihu_usleep(dest_id * IHU_SYSCFG_MODULE_START_DISTRIBUTION_DELAY_DURATION);
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VMFO
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_CANVELA, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[src_id].taskName);
			return IHU_FAILURE;
		}
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
	zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA] = 0;
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
#else
	#error Un-correct constant definition
#endif
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_CANVELA, FSM_STATE_CANVELA_ACTIVED) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	if (IHU_CANVELA_PERIOD_TIMER_SET == IHU_CANVELA_PERIOD_TIMER_ACTIVE){
		//测试性启动周期性定时器
		ret = ihu_timer_start(TASK_ID_CANVELA, TIMER_ID_1S_CANVELA_PERIOD_SCAN, \
			zIhuSysEngPar.timer.array[TIMER_ID_1S_CANVELA_PERIOD_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
			IhuErrorPrint("CANVELA: Error start timer!\n");
			return IHU_FAILURE;
		}	
	}	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("CANVELA: Enter FSM_STATE_CANVELA_ACTIVE status, Keeping refresh here!\n");
	}

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
//	//测试代码，纯粹为了启动整个业务流程
//	ihu_sleep(3);
//	msg_struct_canvela_l3bfsc_init_req_t snd1;
//	memset(&snd1, 0, sizeof(msg_struct_canvela_l3bfsc_init_req_t));
//	snd1.length = sizeof(msg_struct_canvela_l3bfsc_init_req_t);
//	ret = ihu_message_send(MSG_ID_CAN_L3BFSC_INIT_REQ, TASK_ID_BFSC, TASK_ID_CANVELA, &snd1, snd1.length);
//	if (ret == IHU_FAILURE){
//		IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);
//		return IHU_FAILURE;
//	}
#endif
	
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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_CANVELA, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
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
	//IhuErrorPrint("L3BFSC: fsm_canvela_time_out\n");
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]是否超限
	if (zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA] = zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_CANVELA, TASK_ID_CANVELA, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
			IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_CANVELA_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_CANVELA) != FSM_STATE_CANVELA_ACTIVED){
			ret = FsmSetState(TASK_ID_CANVELA, FSM_STATE_CANVELA_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
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
	int ret = 0;
	
	//发送HeartBeat消息给VMFO模块，实现喂狗功能
	msg_struct_com_heart_beat_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_heart_beat_t));
	snd.length = sizeof(msg_struct_com_heart_beat_t);
	ret = ihu_message_send(MSG_ID_COM_HEART_BEAT, TASK_ID_VMFO, TASK_ID_CANVELA, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_VMFO].taskName);
		return;
	}
	
	return;
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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//发送FRAME出去
	strIhuCanvelaCmdFrame_t snd;
	memset(&snd, 0, sizeof(strIhuCanvelaCmdFrame_t));
	ret = func_canvela_frame_encode(IHU_CANVELA_PREFIXH_node_resp, IHU_CANVELA_OPTID_node_set, 0, 0, &snd);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;		
	}
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	//func_canvela_frame_send(&snd);
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_L3BFSC_CAN_NEW_WS_EVENT以后的处理过程
OPSTAT fsm_canvela_l3bfsc_new_ws_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_l3bfsc_canvela_new_ws_event_t rcv;
	
//	uint8_t *p = (uint8_t *)param_ptr;
//	IhuErrorPrint("CANVELA: fsm_canvela_l3bfsc_new_ws_event: param_len=%d, [0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X]\n", \
//		param_len, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t))){
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//发送FRAME出去
	strIhuCanvelaCmdFrame_t snd;
	memset(&snd, 0, sizeof(strIhuCanvelaCmdFrame_t));
	ret = func_canvela_frame_encode(IHU_CANVELA_PREFIXH_ws_resp, IHU_CANVELA_OPTID_wegith_read, 0, rcv.wsValue, &snd);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;		
	}
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	//func_canvela_frame_send(&snd);	
	
	//返回
//	p = (uint8_t *)&rcv;
//	IhuErrorPrint("CANVELA: fsm_canvela_l3bfsc_new_ws_event: param_len=%d, [0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X]\n", \
//		param_len, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//发送FRAME出去
	strIhuCanvelaCmdFrame_t snd;
	memset(&snd, 0, sizeof(strIhuCanvelaCmdFrame_t));
	ret = func_canvela_frame_encode(IHU_CANVELA_PREFIXH_node_resp, IHU_CANVELA_OPTID_material_out_normal, 0, 0, &snd);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	//func_canvela_frame_send(&snd);

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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//发送FRAME出去
	strIhuCanvelaCmdFrame_t snd;
	memset(&snd, 0, sizeof(strIhuCanvelaCmdFrame_t));
	ret = func_canvela_frame_encode(IHU_CANVELA_PREFIXH_node_resp, IHU_CANVELA_OPTID_material_give_up, 0, 0, &snd);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	//func_canvela_frame_send(&snd);
	
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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	//func_canvela_frame_send(&snd);
	
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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if ((rcv.cmdid != IHU_SYSMSG_BFSC_CAN_CMD_TYPE_RESP) || ((rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_ws_resp) && \
		(rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_motor_resp) && (rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_node_resp)))
	{
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}		
	
	//发送FRAME出去
	//这里应该通过新WS事件来体现，未来接口待完善
	strIhuCanvelaCmdFrame_t snd;
	memset(&snd, 0, sizeof(strIhuCanvelaCmdFrame_t));
	ret = func_canvela_frame_encode(rcv.cmd.prefixcmdid, rcv.cmd.optid, rcv.cmd.optpar, rcv.cmd.modbusVal, &snd);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	//func_canvela_frame_send(&snd);
	
	//返回
	return IHU_SUCCESS;
}

//MSG_ID_CAN_L2FRAME_RCV
OPSTAT fsm_canvela_bfsc_l2frame_rcv(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
{
	int ret = 0, i = 0;
	UINT32 msg_id = 0;
	msg_struct_l3bfsc_wmc_msg_header_t *pMsgInnerHeader;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pMsgOutHeader;
	UINT16 msg_len = 0;
	
	pMsgOutHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)param_ptr;
	pMsgInnerHeader = (msg_struct_l3bfsc_wmc_msg_header_t *)((UINT8 *)param_ptr + 4);
	msg_len = pMsgInnerHeader->length;
	msg_id = pMsgInnerHeader->msgid;
	
	/* Check message length */
	if( msg_len != (param_len - MAX_WMC_CONTROL_MSG_HEADER_LEN))
	{
			IhuErrorPrint("CANVELA: fsm_canvela_bfsc_l2frame_rcv: msg_len(%d) != param_len (%d)\n", msg_len, (param_len - MAX_WMC_CONTROL_MSG_HEADER_LEN));
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
			return IHU_FAILURE;
	}
	
	/* Check message ID and message length mapping */
	if(IHU_FAILURE == WmcAwsMsgCheck(pMsgInnerHeader, msg_len))
	{
			IhuErrorPrint("CANVELA: fsm_canvela_bfsc_l2frame_rcv, msg check failure, return!\n");
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
			return IHU_FAILURE;
	}

	IhuDebugPrint("CANVELA: Header (msg_id = 0x%08X, msg_len = %d bytes)\r\n", msg_id, msg_len);
	IhuDebugPrint("CANVELA: Header + Payload(%d bytes):", msg_len);
	for(i = 0; i < msg_len; i++)
	{
			if(0 == (i % 32))  printf("\r\n");
			printf("%02X ", *(((UINT8 *)pMsgInnerHeader)+i));
			//OSTimeDlyHMSM(0, 0, 0, 1);  //schedule other task, so that not block
	}
	printf("\r\n");
	
//	ret = ihu_message_send(MSG_ID_CAN_L3BFSC_CMD_CTRL, TASK_ID_BFSC, TASK_ID_CANVELA, &snd, snd.length);
//	if (ret == IHU_FAILURE){
//		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
//		IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);
//		return IHU_FAILURE;
//	}	
	/* ==================== TODO ==================*/
	/* START TO PROCESS THE RECIEVED VALID MESSAGE */
	/* STEP 1: Check Parameters, if PARAMETER IS NOK, RETURN HERE OR RETURN IN BFSC TASK */
	/* STEP 2: FORWARD TO FBSC TASK */
	/* ============================================*/
	
	/* REMOVE THE CAN HEADER */
	ret = ihu_message_send(msg_id, TASK_ID_BFSC, TASK_ID_CANVELA, (void *)pMsgInnerHeader, msg_len);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);
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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Input parameters error!\n");
		return IHU_FAILURE;
	}

	//检查最重要的参数
	if ((pframe->bfscCmdPrefixH <= IHU_CANVELA_PREFIXH_min) || (pframe->bfscCmdPrefixH >= IHU_CANVELA_PREFIXH_max) || (pframe->bfscCmdPrefixL != IHU_CANVELA_CMD_PREFIXL) || (pframe->bfscCmdId != IHU_CANVELA_CMD_BFSC_ID)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Frame header error!\n");
		return IHU_FAILURE;
	}
	prefixcmdid = pframe->bfscCmdId;
	optid = pframe->bfscOptId;
	if ((optid <=IHU_CANVELA_OPTID_min) || (optid >=IHU_CANVELA_OPTID_max)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		IhuErrorPrint("CANVELA: Input parameters error!\n");
		return IHU_FAILURE;
		//break;
	}

	//返回
	return IHU_SUCCESS;
}

//MYC START
//处理所有从WMC到AWS的消息。
OPSTAT WmcAwsMsgCheck(void * param_ptr, UINT16 msg_len)
{
	UINT32	msgid = 0xFFFFFFFF;
	char s3[IHU_SYSDIM_MSGID_NAME_LEN_MAX] = "";
	msg_struct_l3bfsc_wmc_start_req_t *pMsg = (msg_struct_l3bfsc_wmc_start_req_t *) param_ptr;
	
	if(NULL == param_ptr)
	{
		IhuErrorPrint("CANVELA: WmcAwsMsgCheck, param_ptr = NULL, return!\n");
		return IHU_FAILURE;
	}
	
	if(msg_len > MAX_WMC_CONTROL_MSG_BODY_LEN)
	{
		IhuErrorPrint("CANVELA: WmcAwsMsgCheck, msg_len(%d) > MAX_WMC_CONTROL_MSG_BODY_LEN(%d), return!\n", msg_len, MAX_WMC_CONTROL_MSG_BODY_LEN);
		return IHU_FAILURE;		
	}
	
	/* Get MsgId */
	msgid = pMsg->msgid;
	ihu_msgid_to_string(msgid, s3);
	IhuDebugPrint("CANVELA: WmcAwsMsgCheck: received msgid = [0x%08X]%s, msg_len = [%d]\n", msgid, s3, msg_len);
	switch (msgid)
	{
		case MSG_ID_L3BFSC_WMC_STARTUP_IND:
			if(MSG_SIZE_L3BFSC_WMC_STARTUP_IND == msg_len) return IHU_SUCCESS;
			break;
		
		case MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ:
			if(MSG_SIZE_L3BFSC_WMC_SET_CONFIG_REQ == msg_len) return IHU_SUCCESS;
			break;		
		
		case MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP:
			if(MSG_SIZE_L3BFSC_WMC_SET_CONFIG_RESP == msg_len) return IHU_SUCCESS;
			break;
		
//		case MSG_ID_L3BFSC_WMC_GET_CONFIG_REQ:
//			if(MSG_SIZE_L3BFSC_WMC_GET_CONFIG_REQ == msg_len) return IHU_SUCCESS;
//			break;
//		
//		case MSG_ID_L3BFSC_WMC_GET_CONFIG_RESP:
//			if(MSG_SIZE_L3BFSC_WMC_GET_CONFIG_RESP == msg_len) return IHU_SUCCESS;
//			break;
		
		case MSG_ID_L3BFSC_WMC_START_REQ:
			if(MSG_SIZE_L3BFSC_WMC_START_REQ == msg_len) return IHU_SUCCESS;
			break;
		
		case MSG_ID_L3BFSC_WMC_START_RESP:
			if(MSG_SIZE_L3BFSC_WMC_START_RESP == msg_len) return IHU_SUCCESS;
			break;
		
		case MSG_ID_L3BFSC_WMC_STOP_REQ:
			if(MSG_SIZE_L3BFSC_WMC_STOP_REQ == msg_len) return IHU_SUCCESS;
			break;
		
		case MSG_ID_L3BFSC_WMC_STOP_RESP:
			if(MSG_SIZE_L3BFSC_WMC_STOP_RESP == msg_len) return IHU_SUCCESS;
			break;
		
		case MSG_ID_L3BFSC_WMC_WEIGHT_IND:
			if(MSG_SIZE_L3BFSC_WMC_WEIGHT_IND == msg_len) return IHU_SUCCESS;
			break;
		
		case MSG_ID_L3BFSC_WMC_COMBIN_REQ:
			if(MSG_SIZE_L3BFSC_WMC_COMBIN_REQ == msg_len) return IHU_SUCCESS;
			break;
		
		case MSG_ID_L3BFSC_WMC_COMBIN_RESP:
			if(MSG_SIZE_L3BFSC_WMC_COMBIN_RESP == msg_len) return IHU_SUCCESS;
			break;
		
		case MSG_ID_L3BFSC_WMC_FAULT_IND:
			if(MSG_SIZE_L3BFSC_WMC_FAULT_IND == msg_len) return IHU_SUCCESS;
			break;
		
		case MSG_ID_L3BFSC_WMC_COMMAND_REQ:
			if(MSG_SIZE_L3BFSC_WMC_COMMAND_REQ == msg_len) return IHU_SUCCESS;
			break;
		
		case MSG_ID_L3BFSC_WMC_COMMAND_RESP:
			if(MSG_SIZE_L3BFSC_WMC_COMMAND_RESP == msg_len) return IHU_SUCCESS;
			break;
		
		default:
			IhuErrorPrint("CANVELA: WmcAwsMsgCheck, Invalid msgid = [0x%08X], return\n", msgid);
			return IHU_FAILURE;
	}
	
	IhuErrorPrint("CANVELA: WmcAwsMsgCheck, should not enter here, return\n");
	return IHU_FAILURE;
}

//MYC TODO: to see whether we have other global variables to use
//MYC TODO: to think about how we use when we need to do SW update
UINT8	ctrlMsgBuf[MAX_WMC_CONTROL_MSG_LEN];

OPSTAT fsm_canvela_bfsc_l2frame_snd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
	
	if(NULL == param_ptr)
	{
		IhuErrorPrint("CANVELA: fsm_canvela_bfsc_l2frame_snd, param_ptr = NULL, return!\n");
    return IHU_FAILURE;
	}
	
	if(IHU_FAILURE == WmcAwsMsgCheck(param_ptr, param_len))
	{
		IhuErrorPrint("CANVELA: fsm_canvela_bfsc_l2frame_snd, msg check failure, return!\n");
    return IHU_FAILURE;
	}
	
	/* Clear buffer all to 0*/
	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
	
	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
	pFrameHeader->len = param_len + MAX_WMC_CONTROL_MSG_HEADER_LEN;
	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], param_ptr, param_len);
	IhuDebugPrint("CANVELA: fsm_canvela_bfsc_l2frame_snd: start = [0x%02X], chksum = [0x%02X], len = [%d]\n", pFrameHeader->start, pFrameHeader->chksum, pFrameHeader->len);
	
	//发送FRAME出去
//	strIhuCanvelaCmdFrame_t snd;
//	memset(&snd, 0, sizeof(strIhuCanvelaCmdFrame_t));
//	ret = func_canvela_frame_encode(IHU_CANVELA_PREFIXH_node_resp, IHU_CANVELA_OPTID_node_set, 0, 0, &snd);
//	if (ret == IHU_FAILURE){
//		IhuErrorPrint("CANVELA: Encode CAN L2FRAME error!\n");
//		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
//		return IHU_FAILURE;		
//	}
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	func_canvela_frame_send(pFrameHeader);
	
	//返回
	return IHU_SUCCESS;
}
//MYC END

//将L2FRAME发送出去
void func_canvela_frame_send(IHU_HUITP_L2FRAME_STD_frame_header_t *pframe)
{
	//直接调用BSP_STM32的函数发送出去
	ihu_l1hd_can_bfsc_send_data((UINT8 *)pframe, pframe->len);
}

#endif //#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	


