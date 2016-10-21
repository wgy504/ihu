/**
 ****************************************************************************************
 *
 * @file l3emc68x.c
 *
 * @brief L3 EMC68X
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
 #include "l3emc68x.h"
 
/*
** FSM of the EMC68X
*/
FsmStateItem_t FsmEmc68x[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_emc68x_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_emc68x_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_emc68x_restart},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_EMC68X_AVTIVE,         					fsm_emc68x_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_EMC68X_AVTIVE,         					fsm_emc68x_stop_rcv},

  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_emc68x_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_EMC68X, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("EMC68X: Error Set FSM State at fsm_emc68x_task_entry.");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_emc68x_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret=0;

	//串行会送INIT_FB给VM，不然消息队列不够深度，此为节省内存机制
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM。由于消息队列不够深，所以不再回送FB证实。
//		msg_struct_com_init_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
//		snd.length = sizeof(msg_struct_com_init_fb_t);
//		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_EMC68X, &snd, snd.length);
//		if (ret == IHU_FAILURE){
//			IhuErrorPrint("EMC68X: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_EMC68X], zIhuTaskNameList[src_id]);
//			return IHU_FAILURE;
//		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_EMC68X, FSM_STATE_EMC68X_INITED) == IHU_FAILURE){
		IhuErrorPrint("EMC68X: Error Set FSM State!");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_emc68x_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("EMC68X: Error initialize interface!");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_EMC68X] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_EMC68X, FSM_STATE_EMC68X_AVTIVE) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_EMC68X]++;
		IhuErrorPrint("EMC68X: Error Set FSM State!");
		return IHU_FAILURE;
	}
	
	//启动本地定时器，如果有必要
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("EMC68X: Enter FSM_STATE_EMC68X_ACTIVE status, Keeping refresh here!");
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_emc68x_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("EMC68X: Internal error counter reach DEAD level, SW-RESTART soon!");
	fsm_emc68x_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_emc68x_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_EMC68X)){
		zIhuRunErrCnt[TASK_ID_EMC68X]++;
		IhuErrorPrint("EMC68X: Wrong input paramters!");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_EMC68X, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_EMC68X]++;
		IhuErrorPrint("EMC68X: Error Set FSM State!");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_emc68x_hw_init(void)
{
	return IHU_SUCCESS;
}

