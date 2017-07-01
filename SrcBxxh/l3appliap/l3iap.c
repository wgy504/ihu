/**
 ****************************************************************************************
 *
 * @file l3iap.c
 *
 * @brief L3 IAP
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
#include "l3iap.h"
 
/*
** FSM of the L3IAP
*/
IhuFsmStateItem_t IhuFsmL3iap[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            						fsm_l3iap_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            							fsm_l3iap_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_IDLE,            							fsm_com_do_nothing},

  //Task level initialization
  {MSG_ID_COM_INIT,       								FSM_STATE_L3IAP_INITED,            				fsm_l3iap_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_L3IAP_INITED,            				fsm_com_do_nothing},

	//ANY state entry
  {MSG_ID_COM_INIT_FB,                    FSM_STATE_COMMON,                       fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,                 FSM_STATE_COMMON,                       fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,              FSM_STATE_COMMON,                       fsm_com_do_nothing},
	{MSG_ID_COM_STOP,                       FSM_STATE_COMMON,                       fsm_l3iap_stop_rcv},
  {MSG_ID_COM_RESTART,                    FSM_STATE_COMMON,                       fsm_l3iap_restart},
	{MSG_ID_COM_TIME_OUT,                   FSM_STATE_COMMON,                       fsm_l3iap_time_out},
	
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             							NULL},  //Ending
};

//Global variables defination
strIhuL3iapTaskContext_t zIhuL3iapTaskContext;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_l3iap_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_L3IAP, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("L3IAP: Error Set FSM State at fsm_l3iap_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_l3iap_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//串行回送INIT_FB给VMFO
	ihu_usleep(dest_id * IHU_SYSCFG_MODULE_START_DISTRIBUTION_DELAY_DURATION);
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VMFO
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_L3IAP, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3IAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_L3IAP].taskName, zIhuVmCtrTab.task[src_id].taskName);
			return IHU_FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_L3IAP, FSM_STATE_L3IAP_INITED) == IHU_FAILURE){
		IhuErrorPrint("L3IAP: Error Set FSM State!");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_l3iap_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("L3IAP: Error initialize interface!");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP] = 0;
	memset(&zIhuL3iapTaskContext, 0, sizeof(strIhuL3iapTaskContext_t));
	
	//启动心跳定时器，确保喂狗的基本功能
	ret = ihu_timer_start(TASK_ID_L3IAP, TIMER_ID_1S_L3IAP_PERIOD_SCAN, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_L3IAP_PERIOD_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
		IhuErrorPrint("L3IAP: Error start timer!\n");
		return IHU_FAILURE;
	}
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("L3IAP: Enter FSM_STATE_L3IAP_ACTIVE status, Keeping refresh here!\n");
	}
	
	//拉灯拉BEEP
	ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_GLOBAL_WORK_STATE, 20);
	ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_L3IAP_BEEP_PATTERN_SYS_START, 6);
	
	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_l3iap_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("L3IAP: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_l3iap_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_l3iap_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_L3IAP)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
		IhuErrorPrint("L3IAP: Wrong input paramters!");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3IAP, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
		IhuErrorPrint("L3IAP: Error Set FSM State!");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_l3iap_hw_init(void)
{
	return IHU_SUCCESS;
}

//TIMER_OUT Processing
OPSTAT fsm_l3iap_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("L3IAP: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]是否超限
	if (zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP] = zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_L3IAP, TASK_ID_L3IAP, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
			IhuErrorPrint("L3IAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_L3IAP].taskName, zIhuVmCtrTab.task[TASK_ID_L3IAP].taskName);
			return IHU_FAILURE;
		}
	}

	//喂狗心跳定时器
	if ((rcv.timeId == TIMER_ID_1S_L3IAP_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//func_l3iap_time_out_period_event_report();
	}
	
	else{
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
			IhuErrorPrint("L3IAP: Wrong timer parameter received!\n");
			return IHU_FAILURE;		
	}

	return IHU_SUCCESS;
}



//由于错误，直接关机，等待再次被激活
void func_l3iap_stm_main_recovery_from_fault(void)
{
	//msg_struct_l3iap_com_ctrl_cmd_t snd;

	//关闭所有外部器件的电源
	//延时并关断CPU系统
	ihu_sleep(1);
	
	//初始化模块的任务资源
	//初始化定时器：暂时决定不做，除非该模块重新RESTART
	//初始化模块级全局变量：暂时决定不做，除非该模块重新RESTART
	
	return;
}




