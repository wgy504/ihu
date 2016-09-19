/**
 ****************************************************************************************
 *
 * @file l2ledpisces.c
 *
 * @brief L2 LEDPISCES
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
 #include "l2ledpisces.h"
 
/*
** FSM of the LEDPISCES
*/
FsmStateItem_t FsmLedpisces[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_ledpisces_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_ledpisces_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_ledpisces_restart},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_LEDPISCES_AVTIVE,         					fsm_ledpisces_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_LEDPISCES_AVTIVE,         					fsm_ledpisces_stop_rcv},

  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_ledpisces_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_LEDPISCES, FSM_STATE_IDLE) == FAILURE){
		IhuErrorPrint("LEDPISCES: Error Set FSM State at fsm_ledpisces_task_entry.");
	}
	return SUCCESS;
}

OPSTAT fsm_ledpisces_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret=0;

	//串行会送INIT_FB给VM，不然消息队列不够深度，此为节省内存机制
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM。由于消息队列不够深，所以不再回送FB证实。
//		msg_struct_com_init_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
//		snd.length = sizeof(msg_struct_com_init_fb_t);
//		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_LEDPISCES, &snd, snd.length);
//		if (ret == FAILURE){
//			IhuErrorPrint("LEDPISCES: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_LEDPISCES], zIhuTaskNameList[src_id]);
//			return FAILURE;
//		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_LEDPISCES, FSM_STATE_LEDPISCES_INITED) == FAILURE){
		IhuErrorPrint("LEDPISCES: Error Set FSM State!");	
		return FAILURE;
	}

	//初始化硬件接口
	if (func_ledpisces_hw_init() == FAILURE){	
		IhuErrorPrint("LEDPISCES: Error initialize interface!");
		return FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_LEDPISCES] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_LEDPISCES, FSM_STATE_LEDPISCES_AVTIVE) == FAILURE){
		zIhuRunErrCnt[TASK_ID_LEDPISCES]++;
		IhuErrorPrint("LEDPISCES: Error Set FSM State!");
		return FAILURE;
	}
	
	//启动本地定时器，如果有必要
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("LEDPISCES: Enter FSM_STATE_LEDPISCES_ACTIVE status, Keeping refresh here!");
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_ledpisces_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("LEDPISCES: Internal error counter reach DEAD level, SW-RESTART soon!");
	fsm_ledpisces_init(0, 0, NULL, 0);
	
	return SUCCESS;
}

OPSTAT fsm_ledpisces_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_LEDPISCES)){
		zIhuRunErrCnt[TASK_ID_LEDPISCES]++;
		IhuErrorPrint("LEDPISCES: Wrong input paramters!");
		return FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_LEDPISCES, FSM_STATE_IDLE) == FAILURE){
		zIhuRunErrCnt[TASK_ID_LEDPISCES]++;
		IhuErrorPrint("LEDPISCES: Error Set FSM State!");
		return FAILURE;
	}
	
	//返回
	return SUCCESS;
}

//Local APIs
OPSTAT func_ledpisces_hw_init(void)
{
	return SUCCESS;
}

