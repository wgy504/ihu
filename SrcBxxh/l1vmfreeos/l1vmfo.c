/**
 ****************************************************************************************
 *
 * @file l1vmfo.c
 *
 * @brief L1 VMFO
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
 #include "l1vmfo.h"
 
/*
** FSM of the VMFO
*/
FsmStateItem_t IhuFsmVmfo[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_vmfo_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_vmfo_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_vmfo_restart},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_VMFO_INITED,         							fsm_vmfo_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_VMFO_INITED,         							fsm_vmfo_stop_rcv},

	//Task level actived status
  {MSG_ID_COM_RESTART,        						FSM_STATE_VMFO_ACTIVED,         						fsm_vmfo_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_VMFO_ACTIVED,         						fsm_vmfo_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_VMFO_ACTIVED,         				  	fsm_vmfo_time_out},
	{MSG_ID_COM_INIT_FB,										FSM_STATE_VMFO_ACTIVED,         				  	fsm_vmfo_init_fb},
	{MSG_ID_COM_HEART_BEAT,									FSM_STATE_VMFO_ACTIVED,         				  	fsm_vmfo_heart_beat},	
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
IhuVmfoTaskInitCtrlInfo_t zIhuVmfoTaskInitCtrlInfo[MAX_TASK_NUM_IN_ONE_IHU]; //存储所有初始化任务的状态信息

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_vmfo_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_VMFO, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("VMFO: Error Set FSM State at fsm_vmfo_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_vmfo_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0, i=0;

	//串行会送INIT_FB给VM，不然消息队列不够深度，此为节省内存机制
	//因为是自己发给自己，所以没有必要再次处理
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM。由于消息队列不够深，所以不再回送FB证实。
//		msg_struct_com_init_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
//		snd.length = sizeof(msg_struct_com_init_fb_t);
//		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_VMFO, &snd, snd.length);
//		if (ret == IHU_FAILURE){
//			IhuErrorPrint("VMFO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_VMFO].taskName, zIhuTaskInfo[src_id].taskName);
//			return IHU_FAILURE;
//		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_VMFO, FSM_STATE_VMFO_INITED) == IHU_FAILURE){
		IhuErrorPrint("VMFO: Error Set FSM State!\n");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_vmfo_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("VMFO: Error initialize interface!\n");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_VMFO] = 0;
	memset(zIhuVmfoTaskInitCtrlInfo, 0, (sizeof(IhuVmfoTaskInitCtrlInfo_t) * MAX_TASK_NUM_IN_ONE_IHU));

	//固定任务
	zIhuVmfoTaskInitCtrlInfo[TASK_ID_VMFO].active = IHU_VMFO_TASK_ACTIVE;
	zIhuVmfoTaskInitCtrlInfo[TASK_ID_VMFO].state = IHU_VMFO_TASK_INIT_FEEDBACK;
	zIhuVmfoTaskInitCtrlInfo[TASK_ID_TIMER].active = IHU_VMFO_TASK_ACTIVE;
	zIhuVmfoTaskInitCtrlInfo[TASK_ID_TIMER].state = IHU_VMFO_TASK_INIT_WAIT_FOR_BACK;
	//变动任务
	for (i=TASK_ID_TIMER+1; i < TASK_ID_MAX; i++){
		if (zIhuTaskInfo[i].pnpState == IHU_TASK_PNP_ON){			
			zIhuVmfoTaskInitCtrlInfo[i].active = IHU_VMFO_TASK_ACTIVE;
			zIhuVmfoTaskInitCtrlInfo[i].state = IHU_VMFO_TASK_INIT_WAIT_FOR_BACK;
		}
	}
	
	//简化模式，对于初始化的后续追踪，不再考虑设置超时定时器等方式
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_VMFO, FSM_STATE_VMFO_ACTIVED) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//启动本地定时器，如果有必要
	//测试性启动周期性定时器
	ret = ihu_timer_start(TASK_ID_VMFO, TIMER_ID_1S_VMFO_PERIOD_SCAN, zIhuSysEngPar.timer.vmfoPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error start timer!\n");
		return IHU_FAILURE;
	}	
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("VMFO: Enter FSM_STATE_VMFO_ACTIVE status, Keeping refresh here!\n");
	}

	//启动IWDG的看门狗：STM32CubeMX初始化已经自动启动了，这里就不需要了
		
	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_vmfo_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("VMFO: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_vmfo_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_vmfo_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_VMFO)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_VMFO, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_vmfo_hw_init(void)
{
	return IHU_SUCCESS;
}

//TIMER_OUT Processing
OPSTAT fsm_vmfo_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("VMFO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuRunErrCnt[TASK_ID_VMFO]是否超限
	if (zIhuRunErrCnt[TASK_ID_VMFO] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuRunErrCnt[TASK_ID_VMFO] = zIhuRunErrCnt[TASK_ID_VMFO] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_VMFO, TASK_ID_VMFO, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_VMFO]++;
			IhuErrorPrint("VMFO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_VMFO].taskName, zIhuTaskInfo[TASK_ID_VMFO].taskName);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_VMFO_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_VMFO) != FSM_STATE_VMFO_ACTIVED){
			ret = FsmSetState(TASK_ID_VMFO, FSM_STATE_VMFO_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: Error Set FSM State!\n");
				return IHU_FAILURE;
			}//FsmSetState
		}
		func_vmfo_time_out_period_scan();
	}

	return IHU_SUCCESS;
}

//INIT_FB的处理过程
OPSTAT fsm_vmfo_init_fb(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_com_init_fb_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_init_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_init_fb_t))){
		IhuErrorPrint("VMFO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//IhuDebugPrint("VMFO: Init received, srcId = %d, %s\n", src_id, zIhuTaskInfo[src_id].taskName);

	//存入到反馈表中
	zIhuVmfoTaskInitCtrlInfo[src_id].state = IHU_VMFO_TASK_INIT_FEEDBACK;	
	
	//检查所有的反馈是否都收到，不然维持状态不变
	if (func_vmfo_init_caculate_all_fb() == TRUE){
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
			IhuDebugPrint("VMFO: All task init feedback received, init of all tasks are successful!\n");
		}
	}

	return IHU_SUCCESS;
}

//喂狗处理过程
//由于iwdg造成系统不断重启，暂时去掉，未来再加入
void func_vmfo_time_out_period_scan(void)
{	
	if (func_vmfo_heart_caculate_all_received() == TRUE){
	//Min-max timeout value @32KHz (LSI): ~125us / ~32.7s，所以各个任务的HEART-BEAT周期需要改进到10S以内，并让本周期取20s，以确保至少又一次落在	
	//ihu_l1hd_watch_dog_refresh();	
	}
	return;
}

//HEART_BEAT的处理过程
OPSTAT fsm_vmfo_heart_beat(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_com_heart_beat_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_heart_beat_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_heart_beat_t))){
		IhuErrorPrint("VMFO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//存入到反馈表中
	zIhuVmfoTaskInitCtrlInfo[src_id].heart = IHU_VMFO_TASK_HEART_RECEIVED;	
	
	//返回
	return IHU_SUCCESS;
}

//计算所有的INIT_FB的反馈情况
BOOL func_vmfo_init_caculate_all_fb(void)
{
	int i=0;
	for(i=0;i<MAX_TASK_NUM_IN_ONE_IHU;i++){
		if ((zIhuTaskInfo[i].pnpState == IHU_TASK_PNP_ON) && (zIhuVmfoTaskInitCtrlInfo[i].active == IHU_VMFO_TASK_ACTIVE) && (zIhuVmfoTaskInitCtrlInfo[i].state == IHU_VMFO_TASK_INIT_WAIT_FOR_BACK)){
			return FALSE;
		}
	}

	return TRUE;
}

//计算所有的HEART_BEAT的反馈情况
//有两种选择：一种是选择所有的任务进行反馈，这种方式将会导致MessageQue过载。一种方式是只留下L3任务反馈，目前选择了这种机制
BOOL func_vmfo_heart_caculate_all_received(void)
{
	int i=0;
	if (IHU_VMFO_TASK_HEART_BEAT_CHOICE == IHU_VMFO_TASK_HEART_BEAT_ALL){
		for(i=0;i<MAX_TASK_NUM_IN_ONE_IHU;i++){
			if ((i != TASK_ID_VMFO) && (i != TASK_ID_TIMER)){
				if ((zIhuTaskInfo[i].pnpState == IHU_TASK_PNP_ON) && (zIhuVmfoTaskInitCtrlInfo[i].active == IHU_VMFO_TASK_ACTIVE) && (zIhuVmfoTaskInitCtrlInfo[i].heart != IHU_VMFO_TASK_HEART_RECEIVED)){
					return FALSE;
				}
			}		
		}
		return TRUE;
	}
	
	else if (IHU_VMFO_TASK_HEART_BEAT_CHOICE == IHU_VMFO_TASK_HEART_BEAT_L3_ONLY){
		#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
			if (zIhuVmfoTaskInitCtrlInfo[TASK_ID_CCL].heart != IHU_VMFO_TASK_HEART_RECEIVED) return FALSE;
			else return TRUE;
		#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
			if (zIhuVmfoTaskInitCtrlInfo[TASK_ID_BFSC].heart != IHU_VMFO_TASK_HEART_RECEIVED) return FALSE;
			else return TRUE;
		#else
			#error Un-correct constant definition
		#endif
	}
	
	//如果依然找不到，则总返回TRUE
	//return TRUE;
}



