/**
 ****************************************************************************************
 *
 * @file l2pwmtaurus.c
 *
 * @brief L2 PWMTAURUS
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
 #include "l2pwmtaurus.h"
 
/*
** FSM of the PWMTAURUS
*/
IhuFsmStateItem_t IhuFsmPwmtaurus[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_pwmtaurus_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_pwmtaurus_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_IDLE,            							    fsm_com_do_nothing},

  //Task level initialization
  {MSG_ID_COM_INIT,       								FSM_STATE_PWMTAURUS_INITED,            			fsm_pwmtaurus_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_PWMTAURUS_INITED,            			fsm_com_do_nothing},

	//ANY state entry
  {MSG_ID_COM_INIT_FB,                    FSM_STATE_COMMON,                           fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,                 FSM_STATE_COMMON,                           fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,              FSM_STATE_COMMON,                           fsm_com_do_nothing},
	{MSG_ID_COM_STOP,                       FSM_STATE_COMMON,                           fsm_pwmtaurus_stop_rcv},
  {MSG_ID_COM_RESTART,                    FSM_STATE_COMMON,                           fsm_pwmtaurus_restart},
	{MSG_ID_COM_TIME_OUT,                   FSM_STATE_COMMON,                           fsm_pwmtaurus_time_out},

	//Task level actived status
  {MSG_ID_COM_RESTART,        						FSM_STATE_PWMTAURUS_ACTIVED,         					fsm_pwmtaurus_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_PWMTAURUS_ACTIVED,         					fsm_pwmtaurus_stop_rcv},
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_pwmtaurus_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_PWMTAURUS, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("PWMTAURUS: Error Set FSM State at fsm_pwmtaurus_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_pwmtaurus_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//串行回送INIT_FB给VMFO
	ihu_usleep(dest_id * IHU_SYSCFG_MODULE_START_DISTRIBUTION_DELAY_DURATION);
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VMFO
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_PWMTAURUS, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("PWMTAURUS: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_PWMTAURUS].taskName, zIhuVmCtrTab.task[src_id].taskName);
			return IHU_FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_PWMTAURUS, FSM_STATE_PWMTAURUS_INITED) == IHU_FAILURE){
		IhuErrorPrint("PWMTAURUS: Error Set FSM State!\n");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_pwmtaurus_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("PWMTAURUS: Error initialize interface!\n");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuSysStaPm.taskRunErrCnt[TASK_ID_PWMTAURUS] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_PWMTAURUS, FSM_STATE_PWMTAURUS_ACTIVED) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_PWMTAURUS]++;
		IhuErrorPrint("PWMTAURUS: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//启动本地定时器，如果有必要
	//测试性启动周期性定时器
	ret = ihu_timer_start(TASK_ID_PWMTAURUS, TIMER_ID_1S_PWMTAURUS_PERIOD_SCAN, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_PWMTAURUS_PERIOD_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_PWMTAURUS]++;
		IhuErrorPrint("PWMTAURUS: Error start timer!\n");
		return IHU_FAILURE;
	}	
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("PWMTAURUS: Enter FSM_STATE_PWMTAURUS_ACTIVE status, Keeping refresh here!\n");
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_pwmtaurus_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("PWMTAURUS: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_pwmtaurus_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_pwmtaurus_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_PWMTAURUS)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_PWMTAURUS]++;
		IhuErrorPrint("PWMTAURUS: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_PWMTAURUS, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_PWMTAURUS]++;
		IhuErrorPrint("PWMTAURUS: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_pwmtaurus_hw_init(void)
{
	return IHU_SUCCESS;
}

//TIMER_OUT Processing
OPSTAT fsm_pwmtaurus_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("PWMTAURUS: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_PWMTAURUS]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuSysStaPm.taskRunErrCnt[TASK_ID_PWMTAURUS]是否超限
	if (zIhuSysStaPm.taskRunErrCnt[TASK_ID_PWMTAURUS] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_PWMTAURUS] = zIhuSysStaPm.taskRunErrCnt[TASK_ID_PWMTAURUS] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_PWMTAURUS, TASK_ID_PWMTAURUS, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_PWMTAURUS]++;
			IhuErrorPrint("PWMTAURUS: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_PWMTAURUS].taskName, zIhuVmCtrTab.task[TASK_ID_PWMTAURUS].taskName);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_PWMTAURUS_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_PWMTAURUS) != FSM_STATE_PWMTAURUS_ACTIVED){
			ret = FsmSetState(TASK_ID_PWMTAURUS, FSM_STATE_PWMTAURUS_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuSysStaPm.taskRunErrCnt[TASK_ID_PWMTAURUS]++;
				IhuErrorPrint("PWMTAURUS: Error Set FSM State!\n");
				return IHU_FAILURE;
			}//FsmSetState
		}
		func_pwmtaurus_time_out_period_scan();
	}

	return IHU_SUCCESS;
}

void func_pwmtaurus_time_out_period_scan(void)
{
	int ret = 0;
	
	//发送HeartBeat消息给VMFO模块，实现喂狗功能
	msg_struct_com_heart_beat_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_heart_beat_t));
	snd.length = sizeof(msg_struct_com_heart_beat_t);
	ret = ihu_message_send(MSG_ID_COM_HEART_BEAT, TASK_ID_VMFO, TASK_ID_PWMTAURUS, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_PWMTAURUS]++;
		IhuErrorPrint("PWMTAURUS: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_PWMTAURUS].taskName, zIhuVmCtrTab.task[TASK_ID_VMFO].taskName);
		return;
	}
	
	return;
}

