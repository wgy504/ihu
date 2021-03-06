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

	//Common Handler
	{MSG_ID_CAN_L2FRAME_RCV,								FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l2frame_rcv_handler},
	
	//Task level actived status
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	  //老式消息处理过程
//	{MSG_ID_L3BFSC_CAN_INIT_RESP,						FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_init_resp},	//初始化过程
//	{MSG_ID_L3BFSC_CAN_NEW_WS_EVENT,				FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_new_ws_event},	//收到新的物料
//	{MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP,				FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_roll_out_resp}, //出料证实
//	{MSG_ID_L3BFSC_CAN_GIVE_UP_RESP,				FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_give_up_resp},	//退料证实
//	{MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT,	FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_error_status_report},	//差错报告发送
//	{MSG_ID_L3BFSC_CAN_CMD_RESP,						FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_l3bfsc_cmd_resp}, 

	//MYC update for state machine
	{MSG_ID_CAN_L2FRAME_RCV,								FSM_STATE_CANVELA_ACTIVED,         					fsm_canvela_bfsc_l2frame_rcv},
	{MSG_ID_L3BFSC_WMC_STARTUP_IND,					FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP,			FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_START_RESP,					FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_STOP_RESP,						FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_NEW_WS_EVENT,				FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT,			FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_COMBIN_RESP,					FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_FAULT_IND,						FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_COMMAND_RESP,				FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},
	{MSG_ID_L3BFSC_WMC_HEART_BEAT_REPORT,		FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_snd},

	//分别处理：每个消息独立处理，并包括大小端字序处理过程
//	{MSG_ID_L3BFSC_WMC_STARTUP_IND,					FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_startup_ind},
//	{MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP,			FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_set_config_resp},
//	{MSG_ID_L3BFSC_WMC_START_RESP,					FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_start_resp},
//	{MSG_ID_L3BFSC_WMC_STOP_RESP,						FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_stop_resp},
//	{MSG_ID_L3BFSC_WMC_NEW_WS_EVENT,				FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_new_ws_event},
//	{MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT,			FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_repeat_ws_event},
//	{MSG_ID_L3BFSC_WMC_COMBIN_RESP,					FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_combin_resp},
//	{MSG_ID_L3BFSC_WMC_FAULT_IND,						FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_fault_ind},
//	{MSG_ID_L3BFSC_WMC_COMMAND_RESP,				FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_command_resp},
//	{MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_RESP,		FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_err_inq_cmd_resp},
//	{MSG_ID_L3BFSC_WMC_HEART_BEAT_REPORT,		FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_bfsc_l2frame_heart_beat_report},	
#endif
	
	//Task level actived status	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)
	{MSG_ID_IAP_SW_INVENTORY_REPORT,	        FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_l3iap_inventory_report},
	{MSG_ID_IAP_SW_PACKAGE_REPORT,            FSM_STATE_CANVELA_ACTIVED,									fsm_canvela_l3iap_sw_package_report},
#endif
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)
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
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)
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
	//int ret = 0;
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
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_L3BFSC_CAN_NEW_WS_EVENT以后的处理过程
OPSTAT fsm_canvela_l3bfsc_new_ws_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3bfsc_canvela_new_ws_event_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t))){
		IhuErrorPrint("CANVELA: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//发送FRAME出去
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	//func_canvela_frame_send(&snd);	
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP以后的处理过程
OPSTAT fsm_canvela_l3bfsc_roll_out_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
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

	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	//func_canvela_frame_send(&snd);

	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_L3BFSC_CAN_GIVE_UP_RESP以后的处理过程
OPSTAT fsm_canvela_l3bfsc_give_up_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
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

	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	//func_canvela_frame_send(&snd);
	
	//返回
	return IHU_SUCCESS;
}

//MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT的处理
OPSTAT fsm_canvela_l3bfsc_error_status_report(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
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
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	//func_canvela_frame_send(&snd);
	
	//返回
	return IHU_SUCCESS;
}

//MSG_ID_L3BFSC_CAN_CMD_RESP
OPSTAT fsm_canvela_l3bfsc_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
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
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	//func_canvela_frame_send(&snd);
	
	//返回
	return IHU_SUCCESS;
}

uint16_t HuitpMsgIdMapToInternalMsgId(uint32_t huitp_msgid)
{
	switch (huitp_msgid)
	{
		case HUITP_MSGID_sui_bfsc_set_config_req:
		  return MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ;
		
		case HUITP_MSGID_sui_bfsc_start_req:
			return MSG_ID_L3BFSC_WMC_START_REQ;
		
		case HUITP_MSGID_sui_bfsc_stop_req:
			return MSG_ID_L3BFSC_WMC_STOP_REQ;
		
		case HUITP_MSGID_sui_bfsc_ws_comb_out_req:
			return MSG_ID_L3BFSC_WMC_COMBIN_REQ;
		
		case HUITP_MSGID_sui_bfsc_command_req:
			return MSG_ID_L3BFSC_WMC_COMMAND_REQ;

		case HUITP_MSGID_sui_bfsc_err_inq_cmd_req:
			return MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_REQ;
		
		case HUITP_MSGID_sui_bfsc_heart_beat_confirm:
			return MSG_ID_L3BFSC_WMC_HEART_BEAT_CONFIRM;

		default:
			return 0xFFFF;
	}
}


//MSG_ID_CAN_L2FRAME_RCV
OPSTAT fsm_canvela_bfsc_l2frame_rcv(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
{
	int ret = 0, i = 0;
	UINT32 msg_id = 0;
	msg_struct_l3bfsc_wmc_msg_header_t *pMsgInnerHeader;
	UINT16 msg_len = 0;

	//pMsgOutHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)param_ptr;
	pMsgInnerHeader = (msg_struct_l3bfsc_wmc_msg_header_t *)((UINT8 *)param_ptr + 4);
	msg_len = pMsgInnerHeader->length + 4; ////!!!TO Align to HCU HUITP msg lenth
  IhuDebugPrint("CANVELA: Header (huitp_msg_id = 0x%08X, msg_len = %d bytes)\r\n", pMsgInnerHeader->msgid, msg_len);
	pMsgInnerHeader->msgid = HuitpMsgIdMapToInternalMsgId(pMsgInnerHeader->msgid);
	msg_id = pMsgInnerHeader->msgid;

	IhuDebugPrint("CANVELA: Header (mwc_msg_id = 0x%08X, msg_len = %d bytes)\r\n", msg_id, msg_len);
	for(i = 0; i < msg_len; i++)
	{
			if(0 == (i % 32))  printf("\r\n");
			printf("%02X ", *(((UINT8 *)pMsgInnerHeader)+i));
			//OSTimeDlyHMSM(0, 0, 0, 1);  //schedule other task, so that not block
	}
	printf("\r\n");
  
	/* Check message length */
	if( msg_len != (param_len - MAX_WMC_CONTROL_MSG_HEADER_LEN))
	{
			IhuErrorPrint("CANVELA: fsm_canvela_bfsc_l2frame_rcv: msg_len(%d) != param_len (%d)\n", msg_len, (param_len - MAX_WMC_CONTROL_MSG_HEADER_LEN));
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
			return IHU_FAILURE;
	}
	
	/* Check message ID and message length mapping */
	if(IHU_FAILURE == WmcAwsMsgCheck(pMsgInnerHeader, msg_len)) ////!!!TO Align to HCU HUITP msg lenth
	{
			IhuErrorPrint("CANVELA: fsm_canvela_bfsc_l2frame_rcv, msg check failure, return!\n");
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
			return IHU_FAILURE;
	}

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
	//IhuDebugPrint("CANVELA: WmcAwsMsgCheck: received msgid = [0x%08X]%s, msg_len = [%d]\n", msgid, s3, msg_len);
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
		
		case MSG_ID_L3BFSC_WMC_NEW_WS_EVENT:
			if(MSG_SIZE_L3BFSC_WMC_NEW_WS_EVENT == msg_len) return IHU_SUCCESS;
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

    case MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT:
      if(MSG_SIZE_L3BFSC_WMC_REPEAT_WS_EVENT == msg_len) return IHU_SUCCESS;
      break;
		
		case MSG_ID_L3BFSC_WMC_HEART_BEAT_REPORT:
			if(MSG_SIZE_L3BFSC_WMC_HEART_BEAT_REP == msg_len) return IHU_SUCCESS;
			break;

    case MSG_ID_L3BFSC_WMC_HEART_BEAT_CONFIRM:
      if(MSG_SIZE_L3BFSC_WMC_HEART_BEAT_CONF == msg_len) return IHU_SUCCESS;
      break;
      
		default:
			IhuErrorPrint("CANVELA: WmcAwsMsgCheck, Invalid msgid = [0x%08X], return\n", msgid);
			return IHU_FAILURE;
	}
	
	IhuErrorPrint("CANVELA: WmcAwsMsgCheck, should not enter here, msgid=%d msglen=%d return\n", msgid, msg_len);
	return IHU_FAILURE;
}



uint16_t InternalMsgIdMapToHuitpMsgId(uint32_t internal_msgid)
{
	switch (internal_msgid)
	{
		case MSG_ID_L3BFSC_WMC_STARTUP_IND:
			return HUITP_MSGID_sui_bfsc_startup_ind;
		case MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP:
		  return HUITP_MSGID_sui_bfsc_set_config_resp;
		case MSG_ID_L3BFSC_WMC_START_RESP:
			return HUITP_MSGID_sui_bfsc_start_resp;
		case MSG_ID_L3BFSC_WMC_STOP_RESP:
			return HUITP_MSGID_sui_bfsc_stop_resp;
		case MSG_ID_L3BFSC_WMC_NEW_WS_EVENT:
			return HUITP_MSGID_sui_bfsc_new_ws_event;
    case MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT:
      return HUITP_MSGID_sui_bfsc_repeat_ws_event;
		case MSG_ID_L3BFSC_WMC_COMBIN_RESP:
			return HUITP_MSGID_sui_bfsc_ws_comb_out_resp;
		case MSG_ID_L3BFSC_WMC_COMMAND_RESP:
			return HUITP_MSGID_sui_bfsc_command_resp;
		case MSG_ID_L3BFSC_WMC_FAULT_IND:
			return HUITP_MSGID_sui_bfsc_fault_ind;
		case MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_RESP:
			return HUITP_MSGID_sui_bfsc_err_inq_cmd_resp;
		case MSG_ID_L3BFSC_WMC_HEART_BEAT_REPORT:
			return HUITP_MSGID_sui_bfsc_heart_beat_report;

		default:
			return 0xFFFF;
	}
}

//MYC TODO: to see whether we have other global variables to use
//MYC TODO: to think about how we use when we need to do SW update
UINT8	ctrlMsgBuf[MAX_WMC_CONTROL_MSG_LEN];

OPSTAT fsm_canvela_bfsc_l2frame_snd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
  msg_struct_l3bfsc_wmc_msg_header_t *pBFHeader = (msg_struct_l3bfsc_wmc_msg_header_t *)param_ptr;
	
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

  /* change the internal msg id to external msg id */
  pBFHeader->msgid = InternalMsgIdMapToHuitpMsgId(pBFHeader->msgid);
  pBFHeader->length = param_len - 4; ///!!!! TO ALIGN WITH HCU !!!!
	/* Clear buffer all to 0*/
	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
	
	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
	pFrameHeader->len = param_len + MAX_WMC_CONTROL_MSG_HEADER_LEN;
	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], param_ptr, param_len);
	//IhuDebugPrint("CANVELA: fsm_canvela_bfsc_l2frame_snd: start = [0x%02X], chksum = [0x%02X], len = [%d]\n", pFrameHeader->start, pFrameHeader->chksum, pFrameHeader->len);
	
	//然后执行L2FRAME发送原语命令，通过中断函数将L2FRAME发送出去
	func_canvela_frame_send(pFrameHeader);
	
	//返回
	return IHU_SUCCESS;
}
//MYC END


OPSTAT fsm_canvela_bfsc_l2frame_startup_ind(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3bfsc_wmc_startup_ind_t rcv;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;

	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_wmc_startup_ind_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_wmc_startup_ind_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_startup_ind_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_startup_ind_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_startup_ind);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);

	pMsgProc.wmc_inventory.hw_inventory_id = HUITP_ENDIAN_EXG32(rcv.wmc_inventory.hw_inventory_id);
	pMsgProc.wmc_inventory.sw_inventory_id = HUITP_ENDIAN_EXG32(rcv.wmc_inventory.sw_inventory_id);
	pMsgProc.wmc_inventory.stm32_cpu_id = HUITP_ENDIAN_EXG32(rcv.wmc_inventory.stm32_cpu_id);
	pMsgProc.wmc_inventory.weight_sensor_type = HUITP_ENDIAN_EXG32(rcv.wmc_inventory.weight_sensor_type);
	pMsgProc.wmc_inventory.motor_type = HUITP_ENDIAN_EXG32(rcv.wmc_inventory.motor_type);
	pMsgProc.wmc_inventory.hw_inventory_id = HUITP_ENDIAN_EXG32(rcv.wmc_inventory.hw_inventory_id);
	pMsgProc.wmc_inventory.hw_inventory_id = HUITP_ENDIAN_EXG32(rcv.wmc_inventory.hw_inventory_id);
	pMsgProc.wmc_inventory.hw_inventory_id = HUITP_ENDIAN_EXG32(rcv.wmc_inventory.hw_inventory_id);
	pMsgProc.wmc_inventory.wmc_id.wmc_id = rcv.wmc_inventory.wmc_id.wmc_id;
	
	//发送出去
	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
	pFrameHeader->len = msgProcLen + MAX_WMC_CONTROL_MSG_HEADER_LEN;
	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], &pMsgProc, msgProcLen);
	func_canvela_frame_send(pFrameHeader);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_bfsc_l2frame_set_config_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3bfsc_set_config_resp_t rcv;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_set_config_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_set_config_resp_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_set_config_resp_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_set_config_resp_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_set_config_resp);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	
	pMsgProc.validFlag = rcv.validFlag;
	pMsgProc.errCode = HUITP_ENDIAN_EXG16(rcv.errCode);
	
	//发送出去
	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
	pFrameHeader->len = msgProcLen + MAX_WMC_CONTROL_MSG_HEADER_LEN;
	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], &pMsgProc, msgProcLen);
	func_canvela_frame_send(pFrameHeader);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_bfsc_l2frame_start_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3bfsc_start_resp_t rcv;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_start_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_start_resp_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_start_resp_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_start_resp_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_start_resp);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	
	pMsgProc.validFlag = rcv.validFlag;
	pMsgProc.errCode = HUITP_ENDIAN_EXG16(rcv.errCode);
	
	//发送出去
	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
	pFrameHeader->len = msgProcLen + MAX_WMC_CONTROL_MSG_HEADER_LEN;
	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], &pMsgProc, msgProcLen);
	func_canvela_frame_send(pFrameHeader);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_bfsc_l2frame_stop_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3bfsc_stop_resp_t rcv;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_stop_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_stop_resp_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_stop_resp_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_stop_resp_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_stop_resp);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	
	pMsgProc.validFlag = rcv.validFlag;
	pMsgProc.errCode = HUITP_ENDIAN_EXG16(rcv.errCode);
	
	//发送出去
	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
	pFrameHeader->len = msgProcLen + MAX_WMC_CONTROL_MSG_HEADER_LEN;
	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], &pMsgProc, msgProcLen);
	func_canvela_frame_send(pFrameHeader);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_bfsc_l2frame_new_ws_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3bfsc_new_ws_event_t rcv;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_new_ws_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_new_ws_event_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_new_ws_event_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_new_ws_event_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_new_ws_event);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	
	pMsgProc.wmc_id.wmc_id = rcv.wmc_id.wmc_id;
	pMsgProc.weight_ind.weight_event = HUITP_ENDIAN_EXG32(rcv.weight_ind.weight_event);
	pMsgProc.weight_ind.average_weight = HUITP_ENDIAN_EXG32(rcv.weight_ind.average_weight);
	pMsgProc.weight_ind.repeat_times = HUITP_ENDIAN_EXG32(rcv.weight_ind.repeat_times);
	pMsgProc.weight_combin_type.WeightCombineType = HUITP_ENDIAN_EXG32(rcv.weight_combin_type.WeightCombineType);
	pMsgProc.weight_combin_type.ActionDelayMs = HUITP_ENDIAN_EXG32(rcv.weight_combin_type.ActionDelayMs);

	//发送出去
	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
	pFrameHeader->len = msgProcLen + MAX_WMC_CONTROL_MSG_HEADER_LEN;
	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], &pMsgProc, msgProcLen);
	func_canvela_frame_send(pFrameHeader);	
	
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_bfsc_l2frame_repeat_ws_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3bfsc_repeat_ws_event_t rcv;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_repeat_ws_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_repeat_ws_event_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_repeat_ws_event_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_repeat_ws_event_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_repeat_ws_event);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	
	pMsgProc.wmc_id.wmc_id = rcv.wmc_id.wmc_id;
	pMsgProc.weight_ind.weight_event = HUITP_ENDIAN_EXG32(rcv.weight_ind.weight_event);
	pMsgProc.weight_ind.average_weight = HUITP_ENDIAN_EXG32(rcv.weight_ind.average_weight);
	pMsgProc.weight_ind.repeat_times = HUITP_ENDIAN_EXG32(rcv.weight_ind.repeat_times);
	pMsgProc.weight_combin_type.WeightCombineType = HUITP_ENDIAN_EXG32(rcv.weight_combin_type.WeightCombineType);
	pMsgProc.weight_combin_type.ActionDelayMs = HUITP_ENDIAN_EXG32(rcv.weight_combin_type.ActionDelayMs);

	//发送出去
	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
	pFrameHeader->len = msgProcLen + MAX_WMC_CONTROL_MSG_HEADER_LEN;
	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], &pMsgProc, msgProcLen);
	func_canvela_frame_send(pFrameHeader);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_bfsc_l2frame_combin_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3bfsc_wmc_combin_out_resp_t rcv;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_wmc_combin_out_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_wmc_combin_out_resp_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_resp_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_resp_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_ws_comb_out_resp);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	
	pMsgProc.weight_combin_type.WeightCombineType = HUITP_ENDIAN_EXG32(rcv.weight_combin_type.WeightCombineType);
	pMsgProc.weight_combin_type.ActionDelayMs = HUITP_ENDIAN_EXG32(rcv.weight_combin_type.ActionDelayMs);

	//发送出去
	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
	pFrameHeader->len = msgProcLen + MAX_WMC_CONTROL_MSG_HEADER_LEN;
	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], &pMsgProc, msgProcLen);
	func_canvela_frame_send(pFrameHeader);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_bfsc_l2frame_fault_ind(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3bfsc_wmc_fault_ind_t rcv;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_wmc_fault_ind_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_wmc_fault_ind_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_fault_ind_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_fault_ind_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_fault_ind);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	
	pMsgProc.wmc_id.wmc_id = rcv.wmc_id.wmc_id;
	pMsgProc.error_code = HUITP_ENDIAN_EXG16(rcv.error_code);

	//发送出去
	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
	pFrameHeader->len = msgProcLen + MAX_WMC_CONTROL_MSG_HEADER_LEN;
	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], &pMsgProc, msgProcLen);
	func_canvela_frame_send(pFrameHeader);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_bfsc_l2frame_command_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3bfsc_wmc_command_resp_t rcv;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_wmc_command_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_wmc_command_resp_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_command_resp_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_command_resp_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_command_resp);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	
	pMsgProc.wmc_id.wmc_id = rcv.wmc_id.wmc_id;
	pMsgProc.result.error_code = HUITP_ENDIAN_EXG16(rcv.result.error_code);
	pMsgProc.cmdvalue1 = HUITP_ENDIAN_EXG32(rcv.cmdvalue1);;
	pMsgProc.cmdvalue1 = HUITP_ENDIAN_EXG32(rcv.cmdvalue1);;
	pMsgProc.validFlag = rcv.validFlag;

	//发送出去
	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
	pFrameHeader->len = msgProcLen + MAX_WMC_CONTROL_MSG_HEADER_LEN;
	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], &pMsgProc, msgProcLen);
	func_canvela_frame_send(pFrameHeader);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_bfsc_l2frame_err_inq_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3bfsc_wmc_err_inq_resp_t rcv;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_wmc_err_inq_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_wmc_err_inq_resp_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_resp_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_resp_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_err_inq_cmd_resp);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	
	pMsgProc.wmc_id.wmc_id = rcv.wmc_id.wmc_id;
	pMsgProc.error_code = HUITP_ENDIAN_EXG16(rcv.error_code);
	pMsgProc.average_weight = HUITP_ENDIAN_EXG32(rcv.average_weight);
	pMsgProc.validFlag = rcv.validFlag;

	//发送出去
	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
	pFrameHeader->len = msgProcLen + MAX_WMC_CONTROL_MSG_HEADER_LEN;
	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], &pMsgProc, msgProcLen);
	func_canvela_frame_send(pFrameHeader);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_bfsc_l2frame_heart_beat_report(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3bfsc_wmc_heart_beat_report_t rcv;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_wmc_heart_beat_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_wmc_heart_beat_report_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_report_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_report_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_heart_beat_report);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	
	pMsgProc.wmc_id.wmc_id = rcv.wmc_id.wmc_id;

	//发送出去
	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
	pFrameHeader->len = msgProcLen + MAX_WMC_CONTROL_MSG_HEADER_LEN;
	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], &pMsgProc, msgProcLen);
	func_canvela_frame_send(pFrameHeader);
	
	return IHU_SUCCESS;
}

//将L2FRAME发送出去
void func_canvela_frame_send(IHU_HUITP_L2FRAME_STD_frame_header_t *pframe)
{
	//直接调用BSP_STM32的函数发送出去
	ihu_l1hd_can_bfsc_send_data((UINT8 *)pframe, pframe->len);
}

#endif //#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	


#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)	
OPSTAT fsm_canvela_l3iap_inventory_report(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3iap_sw_upgrade_report_t rcv;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
	pFrameHeader = pFrameHeader;
//	typedef struct msg_struct_l3iap_sw_inventory_report
//	{
//		UINT16  msgid;
//		UINT16  length;
//		UINT16  hwType;
//		UINT16  hwId;
//		UINT16  swRel;
//		UINT16  swVer;
//		UINT8   upgradeFlag;
//		//UINT32  timeStamp;
//	}msg_struct_l3iap_sw_upgrade_report_t;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_l3iap_sw_upgrade_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3iap_sw_upgrade_report_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);	

	//准备组装发送消息
	//	typedef struct StrMsg_HUITP_MSGID_sui_inventory_report
	//	{
	//		UINT16 msgid;
	//		UINT16 length;
	//		UINT16 hwType;
	//		UINT16 hwId;
	//		UINT16 swRel;
	//		UINT16 swVer;
	//		UINT8  upgradeFlag;
	//	}StrMsg_HUITP_MSGID_sui_inventory_report_t;

	StrMsg_HUITP_MSGID_sui_inventory_report_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_inventory_report_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_inventory_report);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);

	pMsgProc.hwType      = HUITP_ENDIAN_EXG16(rcv.hwType);
	pMsgProc.hwId        = HUITP_ENDIAN_EXG16(rcv.hwType);
	pMsgProc.swRel       = HUITP_ENDIAN_EXG16(rcv.hwType);
	pMsgProc.swVer       = HUITP_ENDIAN_EXG16(rcv.hwType);
	pMsgProc.upgradeFlag = rcv.hwType;
	
//发送出去
//	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
//	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
//	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
//	pFrameHeader->len = msgProcLen + MAX_WMC_CONTROL_MSG_HEADER_LEN;
//	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
//	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], &pMsgProc, msgProcLen);
//	func_canvela_frame_send(pFrameHeader);
	
	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_canvela_l3iap_sw_package_report(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3iap_sw_package_report_t rcv;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pFrameHeader = NULL;
	pFrameHeader = pFrameHeader;
	
	//typedef struct msg_struct_l3iap_sw_package_report
	//{
	//	UINT16  msgid;
	//	UINT16  length;
	//	UINT16  swRelId;
	//	UINT16  swVerId;
	//	UINT8   upgradeFlag;
	//	UINT16  segIndex;
	//	UINT16  segTotal;
	//	UINT16  segSplitLen;
	//}msg_struct_l3iap_sw_package_report_t;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_l3iap_sw_package_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3iap_sw_package_report_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);	

	//准备组装发送消息
	//typedef struct StrMsg_HUITP_MSGID_sui_sw_package_report
	//{
	//	UINT16 msgid;
	//	UINT16 length;
	//	UINT16 swRelId;
	//	UINT16 swVerId;
	//	UINT8  upgradeFlag;
	//	UINT16 segIndex;
	//	UINT16 segTotal;
	//	UINT16 segSplitLen;
	//}StrMsg_HUITP_MSGID_sui_sw_package_report_t;
	
	StrMsg_HUITP_MSGID_sui_sw_package_report_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_sw_package_report_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_sw_package_report);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);

	pMsgProc.swRelId      = HUITP_ENDIAN_EXG16(rcv.swRelId);
	pMsgProc.swVerId      = HUITP_ENDIAN_EXG16(rcv.swVerId);
	pMsgProc.upgradeFlag  = rcv.upgradeFlag;   // 1 byte 
	pMsgProc.segIndex     = HUITP_ENDIAN_EXG16(rcv.segIndex);
	pMsgProc.segTotal     = HUITP_ENDIAN_EXG16(rcv.segTotal);
	pMsgProc.segSplitLen  = HUITP_ENDIAN_EXG16(rcv.segSplitLen);
	
	//发送出去
//	memset(ctrlMsgBuf, 0, MAX_WMC_CONTROL_MSG_LEN);
//	pFrameHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)ctrlMsgBuf;
//	pFrameHeader->start = IHU_L2PACKET_START_CHAR;
//	pFrameHeader->len = msgProcLen + MAX_WMC_CONTROL_MSG_HEADER_LEN;
//	pFrameHeader->chksum = l2packet_gen_chksum(pFrameHeader);
//	memcpy(&ctrlMsgBuf[MAX_WMC_CONTROL_MSG_HEADER_LEN], &pMsgProc, msgProcLen);
//	func_canvela_frame_send(pFrameHeader);
	
	//返回
	return IHU_SUCCESS;
}

#endif //#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)	



//公共过程
//MSG_ID_CAN_L2FRAME_RCV
OPSTAT fsm_canvela_l2frame_rcv_handler(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
{
	int ret = 0;
	UINT16 msgId = 0, msgLen = 0;
	msg_struct_canvela_l2frame_rcv_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_canvela_l2frame_rcv_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_l2frame_rcv_t)))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Receive message error!\n");
	//头上4B的CHECKSUM
	memcpy(&rcv, param_ptr, param_len);	
	
	//先解码CHECKSUM的正确性
	
	//再解码MSGID/MSGLEN
	StrMsg_HUITP_MSGID_sui_bfsc_wmc_msg_header_t *pBfscMsg = (StrMsg_HUITP_MSGID_sui_bfsc_wmc_msg_header_t *)(&rcv.data[4]);
	msgId = HUITP_ENDIAN_EXG16(pBfscMsg->msgid);
	msgLen = HUITP_ENDIAN_EXG16(pBfscMsg->length);
	if (msgLen != (param_len-8))
		IHU_ERROR_PRINT_CANVELA("CANVELA: Decode message error on length!\n");	
	
	//按照消息类型进行分类处理
	switch(msgId){

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)		
	case HUITP_MSGID_sui_bfsc_set_config_req:
	{
		IHU_DEBUG_PRINT_INF("CANVELA: Receive L3 MSG = HUITP_MSGID_sui_bfsc_set_config_req \n");
		StrMsg_HUITP_MSGID_sui_bfsc_set_config_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_set_config_req_t) - 4))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_set_config_req_t*)(&rcv.data[4]);
		ret = func_canitfleo_l2frame_msg_bfsc_set_config_req_received_handle(snd);
	}
	break;
	
	case HUITP_MSGID_sui_bfsc_start_req:
	{
		IHU_DEBUG_PRINT_INF("CANVELA: Receive L3 MSG = HUITP_MSGID_sui_bfsc_start_req \n");
		StrMsg_HUITP_MSGID_sui_bfsc_start_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_start_req_t) - 4))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_start_req_t*)(&rcv.data[4]);
		ret = func_canitfleo_l2frame_msg_bfsc_start_req_received_handle(snd);
	}
	break;
	
	case HUITP_MSGID_sui_bfsc_stop_req:
	{
		IHU_DEBUG_PRINT_INF("CANVELA: Receive L3 MSG = HUITP_MSGID_sui_bfsc_stop_req \n");
		StrMsg_HUITP_MSGID_sui_bfsc_stop_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_stop_req_t) - 4))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_stop_req_t*)(&rcv.data[4]);
		ret = func_canitfleo_l2frame_msg_bfsc_stop_req_received_handle(snd);
	}
	break;

	case HUITP_MSGID_sui_bfsc_ws_comb_out_req:
	{
		IHU_DEBUG_PRINT_INF("CANVELA: Receive L3 MSG = HUITP_MSGID_sui_bfsc_ws_comb_out_req \n");
		StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_req_t) - 4))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_req_t*)(&rcv.data[4]);
		ret = func_canitfleo_l2frame_msg_bfsc_ws_comb_out_req_received_handle(snd);
	}
	break;

	case HUITP_MSGID_sui_bfsc_command_req:
	{
		IHU_DEBUG_PRINT_INF("CANVELA: Receive L3 MSG = HUITP_MSGID_sui_bfsc_command_req \n");
		StrMsg_HUITP_MSGID_sui_bfsc_command_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_command_req_t) - 4))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_command_req_t*)(&rcv.data[4]);
		ret = func_canitfleo_l2frame_msg_bfsc_command_req_received_handle(snd);
	}
	break;
	
	case HUITP_MSGID_sui_bfsc_err_inq_cmd_req:
	{
		IHU_DEBUG_PRINT_INF("CANVELA: Receive L3 MSG = HUITP_MSGID_sui_bfsc_err_inq_cmd_req \n");
		StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_req_t) - 4))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_req_t*)(&rcv.data[4]);
		ret = func_canitfleo_l2frame_msg_bfsc_err_inq_cmd_req_received_handle(snd);
	}
	break;

	case HUITP_MSGID_sui_bfsc_heart_beat_confirm:
	{
		IHU_DEBUG_PRINT_INF("CANVELA: Receive L3 MSG = HUITP_MSGID_sui_bfsc_heart_beat_confirm \n");
		StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_confirm_t) - 4))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_confirm_t*)(&rcv.data[4]);
		ret = func_canitfleo_l2frame_msg_bfsc_heart_beat_confirm_received_handle(snd);
	}
	break;

#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)
	case HUITP_MSGID_sui_inventory_confirm:
	{
		IHU_DEBUG_PRINT_INF("CANVELA: Receive L3 MSG = HUITP_MSGID_sui_inventory_confirm \n");
		StrMsg_HUITP_MSGID_sui_inventory_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_inventory_confirm_t) - 4))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_inventory_confirm_t*)(&rcv.data[4]);
		ret = func_canitfleo_l2frame_msg_inventory_confirm_received_handle(snd);
	}
	break;
	
	case HUITP_MSGID_sui_sw_package_confirm:
	{
		IHU_DEBUG_PRINT_INF("CANVELA: Receive L3 MSG = HUITP_MSGID_sui_sw_package_confirm \n");
		StrMsg_HUITP_MSGID_sui_sw_package_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_sw_package_confirm_t) - 4))
			IHU_ERROR_PRINT_CANVELA("CANVELA: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_sw_package_confirm_t*)(&rcv.data[4]);
		ret = func_canitfleo_l2frame_msg_sw_package_confirm_received_handle(snd);
	}
	break;	
#endif //#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)
	
	default:
		IHU_ERROR_PRINT_CANVELA("CANVELA: Receive unsupported message!\n");
	}
	
	//返回
	return ret;
}

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
OPSTAT func_canitfleo_l2frame_msg_bfsc_set_config_req_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_set_config_req_t *rcv)
{
	//准备组装发送消息
	msg_struct_l3bfsc_wmc_set_config_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_wmc_set_config_req_t));
	
	snd.weight_sensor_param.WeightSensorLoadDetectionTimeMs = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorLoadDetectionTimeMs);
	snd.weight_sensor_param.WeightSensorLoadThread = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorLoadThread);
	snd.weight_sensor_param.WeightSensorEmptyThread = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorEmptyThread);
	snd.weight_sensor_param.WeightSensorEmptyDetectionTimeMs = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorEmptyDetectionTimeMs);
	snd.weight_sensor_param.WeightSensorPickupThread = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorEmptyDetectionTimeMs);
	snd.weight_sensor_param.WeightSensorPickupDetectionTimeMs = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorPickupDetectionTimeMs);
	snd.weight_sensor_param.StardardReadyTimeMs = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.StardardReadyTimeMs);
	snd.weight_sensor_param.MaxAllowedWeight = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.MaxAllowedWeight);
	snd.weight_sensor_param.RemainDetectionTimeSec = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.RemainDetectionTimeSec);
	snd.weight_sensor_param.WeightSensorInitOrNot = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorInitOrNot);
	snd.weight_sensor_param.WeightSensorAdcSampleFreq = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorAdcSampleFreq);
	snd.weight_sensor_param.WeightSensorAdcGain = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorAdcGain);
	snd.weight_sensor_param.WeightSensorAdcBitwidth = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorAdcBitwidth);
	snd.weight_sensor_param.WeightSensorAdcValue = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorAdcValue);
	snd.weight_sensor_param.WeightSensorCalibrationZeroAdcValue = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorCalibrationZeroAdcValue);
	snd.weight_sensor_param.WeightSensorCalibrationFullAdcValue = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorCalibrationFullAdcValue);
	snd.weight_sensor_param.WeightSensorCalibrationFullWeight = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorCalibrationFullWeight);
	snd.weight_sensor_param.WeightSensorStaticZeroValue = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorStaticZeroValue);
	snd.weight_sensor_param.WeightSensorTailorValue = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorTailorValue);
	snd.weight_sensor_param.WeightSensorDynamicZeroThreadValue = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorDynamicZeroThreadValue);
	snd.weight_sensor_param.WeightSensorDynamicZeroHysteresisMs = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorDynamicZeroHysteresisMs);
	snd.weight_sensor_param.WeightSensorFilterCoeff[0] = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorFilterCoeff[0]);
	snd.weight_sensor_param.WeightSensorFilterCoeff[1] = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorFilterCoeff[1]);
	snd.weight_sensor_param.WeightSensorFilterCoeff[2] = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorFilterCoeff[2]);
	snd.weight_sensor_param.WeightSensorFilterCoeff[3] = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorFilterCoeff[3]);
	snd.weight_sensor_param.WeightSensorOutputValue[0] = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorOutputValue[0]);
	snd.weight_sensor_param.WeightSensorOutputValue[1] = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorOutputValue[1]);
	snd.weight_sensor_param.WeightSensorOutputValue[2] = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorOutputValue[2]);
	snd.weight_sensor_param.WeightSensorOutputValue[3] = HUITP_ENDIAN_EXG32(rcv->weight_sensor_param.WeightSensorOutputValue[3]);	
	snd.motor_control_param.MotorSpeed = HUITP_ENDIAN_EXG32(rcv->motor_control_param.MotorSpeed);
	snd.motor_control_param.MotorDirection = HUITP_ENDIAN_EXG32(rcv->motor_control_param.MotorDirection);
	snd.motor_control_param.MotorRollingStartMs = HUITP_ENDIAN_EXG32(rcv->motor_control_param.MotorRollingStartMs);
	snd.motor_control_param.MotorRollingStopMs = HUITP_ENDIAN_EXG32(rcv->motor_control_param.MotorRollingStopMs);
	snd.motor_control_param.MotorRollingInveralMs = HUITP_ENDIAN_EXG32(rcv->motor_control_param.MotorRollingInveralMs);
	snd.motor_control_param.MotorFailureDetectionVaration = HUITP_ENDIAN_EXG32(rcv->motor_control_param.MotorFailureDetectionVaration);
	snd.motor_control_param.MotorFailureDetectionTimeMs = HUITP_ENDIAN_EXG32(rcv->motor_control_param.MotorFailureDetectionTimeMs);

	snd.length = sizeof(msg_struct_l3bfsc_wmc_set_config_req_t);
	if (ihu_message_send(MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ, TASK_ID_BFSC, TASK_ID_CANVELA, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CANVELA("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);
	
	//返回
	return IHU_SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_bfsc_start_req_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_start_req_t *rcv)
{
	//准备组装发送消息
	msg_struct_l3bfsc_wmc_start_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_wmc_start_req_t));
	
	snd.length = sizeof(msg_struct_l3bfsc_wmc_start_req_t);
	if (ihu_message_send(MSG_ID_L3BFSC_WMC_START_REQ, TASK_ID_BFSC, TASK_ID_CANVELA, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CANVELA("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);

	//返回
	return IHU_SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_bfsc_stop_req_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_stop_req_t *rcv)
{
	//准备组装发送消息
	msg_struct_l3bfsc_wmc_stop_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_wmc_stop_req_t));
	
	snd.length = sizeof(msg_struct_l3bfsc_wmc_stop_req_t);
	if (ihu_message_send(MSG_ID_L3BFSC_WMC_STOP_REQ, TASK_ID_BFSC, TASK_ID_CANVELA, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CANVELA("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);

	//返回
	return IHU_SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_bfsc_ws_comb_out_req_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_req_t *rcv)
{
	//准备组装发送消息
	msg_struct_l3bfsc_wmc_combin_out_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_wmc_combin_out_req_t));
	
	snd.weight_combin_type.WeightCombineType = HUITP_ENDIAN_EXG32(rcv->weight_combin_type.WeightCombineType);
	snd.weight_combin_type.ActionDelayMs = HUITP_ENDIAN_EXG32(rcv->weight_combin_type.ActionDelayMs);
	
	snd.length = sizeof(msg_struct_l3bfsc_wmc_combin_out_req_t);
	if (ihu_message_send(MSG_ID_L3BFSC_WMC_COMBIN_REQ, TASK_ID_BFSC, TASK_ID_CANVELA, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CANVELA("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);

	//返回
	return IHU_SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_bfsc_command_req_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_command_req_t *rcv)
{
	//准备组装发送消息
	msg_struct_l3bfsc_wmc_command_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_wmc_command_req_t));
	
	snd.cmdid = HUITP_ENDIAN_EXG32(rcv->cmdid);
	snd.cmdvalue = HUITP_ENDIAN_EXG32(rcv->cmdvalue);
	snd.cmdvalue = HUITP_ENDIAN_EXG32(rcv->cmdvalue);
	
	snd.length = sizeof(msg_struct_l3bfsc_wmc_command_req_t);
	if (ihu_message_send(MSG_ID_L3BFSC_WMC_COMMAND_REQ, TASK_ID_BFSC, TASK_ID_CANVELA, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CANVELA("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);

	//返回
	return IHU_SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_bfsc_err_inq_cmd_req_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_req_t *rcv)
{
	//准备组装发送消息
	msg_struct_l3bfsc_wmc_err_inq_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_wmc_err_inq_req_t));
	
	snd.error_code = HUITP_ENDIAN_EXG16(rcv->error_code);
	snd.wmc_id.wmc_id = rcv->wmc_id.wmc_id;
	
	snd.length = sizeof(msg_struct_l3bfsc_wmc_err_inq_req_t);
	if (ihu_message_send(MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_REQ, TASK_ID_BFSC, TASK_ID_CANVELA, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CANVELA("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);

	//返回
	return IHU_SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_bfsc_heart_beat_confirm_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_confirm_t *rcv)
{
	//准备组装发送消息
	msg_struct_l3bfsc_wmc_heart_beat_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_wmc_heart_beat_confirm_t));
	
	snd.wmcState = rcv->wmcState;
	snd.wmc_id.wmc_id = rcv->wmc_id.wmc_id;
		
	snd.length = sizeof(msg_struct_l3bfsc_wmc_heart_beat_confirm_t);
	if (ihu_message_send(MSG_ID_L3BFSC_WMC_HEART_BEAT_CONFIRM, TASK_ID_BFSC, TASK_ID_CANVELA, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CANVELA("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);

	//返回
	return IHU_SUCCESS;
}

#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)
OPSTAT func_canitfleo_l2frame_msg_inventory_confirm_received_handle(StrMsg_HUITP_MSGID_sui_inventory_confirm_t *rcv)
{
	//准备组装发送消息
//	typedef struct StrMsg_HUITP_MSGID_sui_inventory_confirm
//	{
//		UINT16 msgid;
//		UINT16 length;
//		UINT16 swRel;
//		UINT16 swVer;
//		UINT8  upgradeFlag;
//		UINT16 	swCheckSum;
//		UINT32  swTotalLengthInBytes;
//	}StrMsg_HUITP_MSGID_sui_inventory_confirm_t;
	
//		typedef struct msg_struct_l3iap_sw_inventory_confirm
//		{
//				UINT16  msgid;
//				UINT16  length;
//				UINT16  swRel;
//				UINT16  swVer;
//				UINT8   upgradeFlag;
//				UINT16 	swCheckSum;
//				UINT32  swTotalLengthInBytes;
//		}msg_struct_l3iap_sw_inventory_confirm_t;

	msg_struct_l3iap_sw_inventory_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3iap_sw_inventory_confirm_t));
	
	snd.msgid = MSG_ID_IAP_SW_INVENTORY_CONFIRM;
	snd.length = sizeof(msg_struct_canvela_l3iap_inventory_confirm_t);
	
	snd.swRel = rcv->swRel;
	snd.swRel = rcv->swVer;
	snd.upgradeFlag = rcv->upgradeFlag;					       //APP or FACTORY, or ... same as msg_struct_l3iap_iap_sw_upgrade_req
	snd.swCheckSum = rcv->swCheckSum;
	snd.swTotalLengthInBytes = rcv->swTotalLengthInBytes;
			
	if (ihu_message_send(MSG_ID_IAP_SW_INVENTORY_CONFIRM, TASK_ID_L3IAP, TASK_ID_CANVELA, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CANVELA("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_L3IAP].taskName);

	//返回
	return IHU_SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_sw_package_confirm_received_handle(StrMsg_HUITP_MSGID_sui_sw_package_confirm_t *rcv)
{
	//准备组装发送消息
//	typedef struct StrMsg_HUITP_MSGID_sui_sw_package_confirm
//	{
//		UINT16 msgid;
//		UINT16 length;
//		UINT16 swRelId;
//		UINT16 swVerId;
//		UINT8  upgradeFlag;
//		UINT16 segIndex;
//		UINT16 segTotal;
//		UINT16 segSplitLen;
//		UINT16 segValidLen;
//		UINT16 segCheckSum;
//		UINT8  swPkgBody[HUITP_IEID_SUI_SW_PACKAGE_BODY_MAX_LEN];
//	}StrMsg_HUITP_MSGID_sui_sw_package_confirm_t;
	
//	typedef struct msg_struct_l3iap_sw_package_confirm
//	{
//			UINT16 msgid;
//			UINT16 length;
//			UINT16 swRelId;
//			UINT16 swVerId;
//			UINT8  upgradeFlag;
//			UINT16 segIndex;
//			UINT16 segTotal;
//			UINT16 segSplitLen;
//			UINT16 segValidLen;
//			UINT16 segCheckSum;
//			UINT8  swPkgBody[MAX_LEN_PER_LOAD_SEGMENT];
//	}msg_struct_l3iap_sw_package_confirm_t;

	msg_struct_l3iap_sw_package_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3iap_sw_package_confirm_t));
	
	snd.msgid = MSG_ID_IAP_SW_PACKAGE_CONFIRM;
	snd.length = sizeof(msg_struct_l3iap_sw_package_confirm_t);  //TO CHECK LENTH
	
	snd.swRelId       = rcv->swRelId;
	snd.swVerId       = rcv->swVerId;
	snd.upgradeFlag   = rcv->upgradeFlag;
	snd.segIndex      = rcv->segIndex;
	snd.segTotal      = rcv->segTotal;
	snd.segSplitLen   = rcv->segSplitLen;
	snd.segValidLen   = rcv->segValidLen;
	snd.segCheckSum   = rcv->segCheckSum;
	
	if(rcv->segSplitLen <= MAX_LEN_PER_LOAD_SEGMENT)
	{
			memcpy(snd.swPkgBody, rcv->swPkgBody, MAX_LEN_PER_LOAD_SEGMENT);  //rcv.segSplitLen ????????
			snd.length = sizeof(msg_struct_l3iap_sw_package_confirm_t);
			if (ihu_message_send(MSG_ID_IAP_SW_PACKAGE_CONFIRM, TASK_ID_L3IAP, TASK_ID_CANVELA, &snd, snd.length) == IHU_FAILURE)
					IHU_ERROR_PRINT_CANVELA("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_L3IAP].taskName);
	}
	
	//返回
	return IHU_SUCCESS;
}
#endif //#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)	





