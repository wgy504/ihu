/**
 ****************************************************************************************
 *
 * @file l2didocap.c
 *
 * @brief L2 DIDOCAP
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
 #include "l2didocap.h"
 
/*
** FSM of the DIDOCAP
*/
FsmStateItem_t FsmDidocap[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_didocap_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_didocap_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_didocap_restart},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_DIDOCAP_INITED,         					fsm_didocap_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_DIDOCAP_INITED,         					fsm_didocap_stop_rcv},

	//Task level actived status
  {MSG_ID_COM_RESTART,        						FSM_STATE_DIDOCAP_ACTIVED,         					fsm_didocap_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_DIDOCAP_ACTIVED,         					fsm_didocap_stop_rcv},
  {MSG_ID_COM_TIME_OUT,										FSM_STATE_DIDOCAP_ACTIVED,         					fsm_didocap_time_out},
	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
  {MSG_ID_CCL_COM_SENSOR_STATUS_REQ,			FSM_STATE_DIDOCAP_ACTIVED,         					fsm_didocap_ccl_sensor_status_req},
  {MSG_ID_CCL_COM_CTRL_CMD,								FSM_STATE_DIDOCAP_ACTIVED,         					fsm_didocap_ccl_ctrl_cmd},
#endif

  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)

#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
strIhuCclDidoPar_t zIhuCclDidocapCtrlTable;
#else
#endif

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_didocap_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_DIDOCAP, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("DIDOCAP: Error Set FSM State at fsm_didocap_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_didocap_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//串行回送INIT_FB给VMFO
	ihu_usleep(dest_id * IHU_MODULE_START_DISTRIBUTION_DELAY_DURATION);
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_DIDOCAP, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[src_id]);
			return IHU_FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_DIDOCAP, FSM_STATE_DIDOCAP_INITED) == IHU_FAILURE){
		IhuErrorPrint("DIDOCAP: Error Set FSM State!\n");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_didocap_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("DIDOCAP: Error initialize interface!\n");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_DIDOCAP] = 0;
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	memset(&zIhuCclDidocapCtrlTable, 0, sizeof(strIhuCclDidoPar_t));
	zIhuCclDidocapCtrlTable.cclDidoWorkingMode = IHU_CCL_DIDO_WORKING_MODE_SLEEP;  //初始化就进入SLEEP，然后就看是否有触发
#else
#endif
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_DIDOCAP, FSM_STATE_DIDOCAP_ACTIVED) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		IhuErrorPrint("DIDOCAP: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//启动喂狗定时器
	ret = ihu_timer_start(TASK_ID_DIDOCAP, TIMER_ID_1S_DIDOCAP_PERIOD_SCAN, zIhuSysEngPar.timer.didocapPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		IhuErrorPrint("DIDOCAP: Error start timer!\n");
		return IHU_FAILURE;
	}	

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)	
	//启动永恒的外部触发扫描
	ret = ihu_timer_start(TASK_ID_DIDOCAP, TIMER_ID_1S_CCL_DIDO_TRIGGER_PERIOD_SCAN, zIhuSysEngPar.timer.cclDidoTriggerPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		IhuErrorPrint("DIDOCAP: Error start timer!\n");
		return IHU_FAILURE;
	}
#endif
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("DIDOCAP: Enter FSM_STATE_DIDOCAP_ACTIVE status, Keeping refresh here!\n");
	}
	
	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_didocap_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("DIDOCAP: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_didocap_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_didocap_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_DIDOCAP)){
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		IhuErrorPrint("DIDOCAP: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_DIDOCAP, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		IhuErrorPrint("DIDOCAP: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_didocap_hw_init(void)
{
	return IHU_SUCCESS;
}

//TIMER_OUT Processing
OPSTAT fsm_didocap_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("DIDOCAP: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuRunErrCnt[TASK_ID_DIDOCAP]是否超限
	if (zIhuRunErrCnt[TASK_ID_DIDOCAP] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuRunErrCnt[TASK_ID_DIDOCAP] = zIhuRunErrCnt[TASK_ID_DIDOCAP] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_DIDOCAP, TASK_ID_DIDOCAP, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_DIDOCAP]);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_DIDOCAP_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_DIDOCAP) != FSM_STATE_DIDOCAP_ACTIVED){
			ret = FsmSetState(TASK_ID_DIDOCAP, FSM_STATE_DIDOCAP_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
				IhuErrorPrint("DIDOCAP: Error Set FSM State!\n");
				return IHU_FAILURE;
			}//FsmSetState
		}
		func_didocap_time_out_period_scan();
	}	

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)	
	//永恒的外部触发扫描：只有在IHU_CCL_DIDO_WORKING_MODE_SLEEP模式下才会进行外部触发源的定时扫描
	//在FSM_STATE_CCL_EVENT_REPORT@CCL下也会遇到问题，所以需要多加一个逻辑判定
	else if ((rcv.timeId == TIMER_ID_1S_CCL_DIDO_TRIGGER_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if ((FsmGetState(TASK_ID_CCL) != FSM_STATE_CCL_EVENT_REPORT) && (FsmGetState(TASK_ID_SPSVIRGO) == FSM_STATE_SPSVIRGO_ACTIVED) &&\
			((zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_SLEEP) || (zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_FAULT))){
			func_didocap_time_out_external_trigger_period_scan();
		}
	}
	
	//工作模式下门和锁的扫描，只扫描门和锁！！！
	else if ((rcv.timeId == TIMER_ID_1S_CCL_DIDO_WORKING_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if ((zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_ACTIVE) || (zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_FAULT)) {
			func_didocap_time_out_work_mode_period_scan();
		}
	}
#endif
	
	else{
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		IhuErrorPrint("DIDOCAP: Received wrong Timer ID!\n");
		return IHU_FAILURE;		
	}	
	
	return IHU_SUCCESS;
}

//喂狗心跳
void func_didocap_time_out_period_scan(void)
{
	int ret = 0;

	//发送HeartBeat消息给VMFO模块，实现喂狗功能
	msg_struct_com_heart_beat_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_heart_beat_t));
	snd.length = sizeof(msg_struct_com_heart_beat_t);
	ret = ihu_message_send(MSG_ID_COM_HEART_BEAT, TASK_ID_VMFO, TASK_ID_DIDOCAP, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_VMFO]);
		return ;
	}
	
	return;
}

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)

//定时扫描震动以及触发按键消息
//待改进这个扫描的事件，以便形成有价值的结果事件
void func_didocap_time_out_external_trigger_period_scan(void)
{
	int ret = 0, i = 0;
	
	//锁被触发，只有休眠模式下才被允许触发
	if ((zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_SLEEP) && (func_didocap_ccl_sleep_mode_ul_scan_lock_trigger() == TRUE)){
		//将周期扫描锁触发的结果发送给CCL
		msg_struct_dido_ccl_event_lock_trigger_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_dido_ccl_event_lock_trigger_t));
		snd0.lockid = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
		snd0.lockid = rand() % IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
		snd0.cmdid = IHU_CCL_DH_CMDID_EVENT_IND_LOCK_TRIGGER;
		snd0.length = sizeof(msg_struct_dido_ccl_event_lock_trigger_t);
		//存储做为上一次的状态
		zIhuCclDidocapCtrlTable.sensor.lockiTriggerState[snd0.lockid] = IHU_CCL_SENSOR_STATE_ACTIVE;
		ret = ihu_message_send(MSG_ID_DIDO_CCL_EVENT_LOCK_TRIGGER, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
			return ;
		}
	}

	//震动被触发，只有休眠模式下才被允许触发
	else if ((zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_SLEEP) && (func_didocap_ccl_sleep_mode_ul_scan_shake_trigger() == TRUE)){
		//将周期扫描锁触发的结果发送给CCL
		msg_struct_dido_ccl_event_lock_trigger_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_dido_ccl_event_lock_trigger_t));
		snd1.lockid = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
		snd1.cmdid = IHU_CCL_DH_CMDID_EVENT_IND_SHAKE_TRIGGER;
		snd1.length = sizeof(msg_struct_dido_ccl_event_lock_trigger_t);
		//存储做为上一次的状态
		zIhuCclDidocapCtrlTable.sensor.shakeState = IHU_CCL_SENSOR_STATE_ACTIVE;
		ret = ihu_message_send(MSG_ID_DIDO_CCL_EVENT_LOCK_TRIGGER, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd1, snd1.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
			return ;
		}		
	}

	//出现差错，将导致状态机进入差错状态。如果在差错状态下，允许继续发送新的差错报告
	//这种报告，可以在SLEEP下，也可以在FAULT状态下发送
	//至于是不是向后方送太频繁，可以由CCL状态机控制，这里不控
	else if (func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_status() == TRUE){
		//将周期扫描锁触发的结果发送给CCL
		msg_struct_dido_ccl_event_fault_trigger_t snd2;
		memset(&snd2, 0, sizeof(msg_struct_dido_ccl_event_fault_trigger_t));
		snd2.cmdid = IHU_CCL_DH_CMDID_EVENT_IND_FAULT_MULTI; //多重错误
		memcpy(&snd2.sensor, &zIhuCclDidocapCtrlTable.sensor, sizeof(com_sensor_status_t));
		//发送差错报告
		snd2.length = sizeof(msg_struct_dido_ccl_event_fault_trigger_t);
		ret = ihu_message_send(MSG_ID_DIDO_CCL_EVENT_FAULT_TRIGGER, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd2, snd2.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
			return ;
		}
	}
	
	//完全恢复：只有FAULT模式下才谈得上恢复
	else if ((zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_FAULT) && (func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_recover() == TRUE)){
		//将周期扫描锁触发的结果发送给CCL
		msg_struct_dido_ccl_event_fault_trigger_t snd3;
		memset(&snd3, 0, sizeof(msg_struct_dido_ccl_event_fault_trigger_t));
		snd3.cmdid = IHU_CCL_DH_CMDID_EVENT_IND_FAULT_RECOVER; //错误恢复
		for (i=0; i<IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
			zIhuCclDidocapCtrlTable.sensor.doorState[i] = IHU_CCL_SENSOR_STATE_CLOSE;
			zIhuCclDidocapCtrlTable.sensor.lockTongueState[i] = IHU_CCL_SENSOR_STATE_CLOSE;
		}
		zIhuCclDidocapCtrlTable.sensor.smokeState = IHU_CCL_SENSOR_STATE_DEACTIVE;
		zIhuCclDidocapCtrlTable.sensor.waterState = IHU_CCL_SENSOR_STATE_DEACTIVE;
		zIhuCclDidocapCtrlTable.sensor.fallState = IHU_CCL_SENSOR_STATE_DEACTIVE;
		zIhuCclDidocapCtrlTable.sensor.batteryState = IHU_CCL_SENSOR_STATE_DEACTIVE;	
		
		//发送
		snd3.length = sizeof(msg_struct_dido_ccl_event_fault_trigger_t);
		ret = ihu_message_send(MSG_ID_DIDO_CCL_EVENT_FAULT_TRIGGER, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd3, snd3.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
			return ;
		}
	}
	
	//返回
	return;
}

//工作模式下的定时器处理：只扫描锁和门
void func_didocap_time_out_work_mode_period_scan(void)
{
	int ret = 0;
	//首先扫描是否有门开的信号：只有在WORK_MODE下被允许
	if ((zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_ACTIVE) && (FsmGetState(TASK_ID_CCL) == FSM_STATE_CCL_TO_OPEN_DOOR)){
		if (func_didocap_ccl_work_mode_ul_scan_any_door_open() == TRUE)
		{
			msg_struct_dido_ccl_door_open_event_t snd1;
			memset(&snd1, 0, sizeof(msg_struct_dido_ccl_door_open_event_t));
			snd1.cmdid = IHU_CCL_DH_CMDID_EVENT_IND_DOOR_C_TO_O;
			snd1.length = sizeof(msg_struct_dido_ccl_door_open_event_t);
			ret = ihu_message_send(MSG_ID_DIDO_CCL_DOOR_OPEN_EVENT, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd1, snd1.length);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
				IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
				return ;
			}
		}		
	}

	//再扫描是否所有锁和门都关闭：这就跟定时器允许FAULT下运行对上了
	//本来这个函数的入口就是(zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_ACTIVE || IHU_CCL_DIDO_WORKING_MODE_FAULT
	//所以这里不用再做额外的判定了
	if ((FsmGetState(TASK_ID_CCL) == FSM_STATE_CCL_DOOR_OPEN) || (FsmGetState(TASK_ID_CCL) == FSM_STATE_CCL_FATAL_FAULT)){
		if (func_didocap_ccl_work_mode_ul_scan_any_door_open() == TRUE)
		{
			msg_struct_dido_ccl_lock_c_door_c_event_t snd2;
			memset(&snd2, 0, sizeof(msg_struct_dido_ccl_lock_c_door_c_event_t));
			snd2.cmdid = IHU_CCL_DH_CMDID_EVENT_IND_DOOR_LOCK_ALL_C;
			snd2.length = sizeof(msg_struct_dido_ccl_lock_c_door_c_event_t);
			ret = ihu_message_send(MSG_ID_DIDO_CCL_LOCK_C_DOOR_C_EVENT, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd2, snd2.length);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
				IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
				return ;
			}
		}
	}

	//扫描是否有重复触发信号：这个信号的发送，只有在工作模式下。当FAULT模式下，不允许重复激活触发
	//这个重复触发的目的是延长WORKING_ACTIVE定时器的长度，这是客户特别要的FEATURE
	if ((zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_ACTIVE) && (func_didocap_ccl_work_mode_ul_scan_enable_lock_trigger() == TRUE))
	{
		msg_struct_dido_ccl_status_update_t snd3;
		memset(&snd3, 0, sizeof(msg_struct_dido_ccl_status_update_t));
		snd3.length = sizeof(msg_struct_dido_ccl_status_update_t);
		snd3.cmdid = IHU_CCL_DH_CMDID_EVENT_IND_LOCK_TRIGGER;
		ret = ihu_message_send(MSG_ID_DIDO_CCL_EVENT_STATUS_UPDATE, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd3, snd3.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
			return ;
		}		
	}

	//WORK_MODE/FAULT_MODE下的状态变化，则发送UPDATE消息给CCL，暂时没有什么用途
	//这里的状态，需要再完善，CCL并不是特别关心，但有知晓权利，是否有必要在未来继续扩大，留个口子吧
	//由于差错状态下本来就有FAULT_REPORT模式，这里不要再添乱了
	if ((zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_ACTIVE) && (func_didocap_ccl_work_mode_ul_scan_door_and_lock_status_change() == TRUE))
	{
		msg_struct_dido_ccl_status_update_t snd4;
		memset(&snd4, 0, sizeof(msg_struct_dido_ccl_status_update_t));
		snd4.length = sizeof(msg_struct_dido_ccl_status_update_t);
		snd4.cmdid = IHU_CCL_DH_CMDID_EVENT_IND_LOCK_O_TO_C;
		ret = ihu_message_send(MSG_ID_DIDO_CCL_EVENT_STATUS_UPDATE, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd4, snd4.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
			return ;
		}
	}
	
	//其它的情形，不考虑
	
	//返回
	return;
}

OPSTAT fsm_didocap_ccl_sensor_status_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0, i=0;
	msg_struct_ccl_com_sensor_status_req_t rcv;
	msg_struct_dido_ccl_sensor_status_rep_t snd;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_com_sensor_status_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_com_sensor_status_req_t))){
		IhuErrorPrint("DIDOCAP: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_REQ_STATUS_DIDO){
		IhuErrorPrint("DIDOCAP: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		return IHU_FAILURE;
	}
	
	//具体扫描处理
	
	//扫描后将结果发给上层
	memset(&snd, 0, sizeof(msg_struct_dido_ccl_sensor_status_rep_t));
	snd.cmdid = IHU_CCL_DH_CMDID_RESP_STATUS_DIDO;
	for (i=0; i<IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
		snd.sensor.doorState[i] = ((rand()%2 == 0)?IHU_CCL_SENSOR_STATE_CLOSE:IHU_CCL_SENSOR_STATE_OPEN);
		snd.sensor.lockiTriggerState[i] = ((rand()%2 == 0)?IHU_CCL_SENSOR_STATE_DEACTIVE:IHU_CCL_SENSOR_STATE_ACTIVE);
		snd.sensor.lockoEnableState[i] = ((rand()%2 == 0)?IHU_CCL_SENSOR_STATE_DEACTIVE:IHU_CCL_SENSOR_STATE_ACTIVE);;
	}
	snd.sensor.smokeState = ((rand()%2 == 0)?IHU_CCL_SENSOR_STATE_DEACTIVE:IHU_CCL_SENSOR_STATE_ACTIVE);
	snd.sensor.waterState = ((rand()%2 == 0)?IHU_CCL_SENSOR_STATE_DEACTIVE:IHU_CCL_SENSOR_STATE_ACTIVE);
	snd.sensor.fallState = ((rand()%2 == 0)?IHU_CCL_SENSOR_STATE_DEACTIVE:IHU_CCL_SENSOR_STATE_ACTIVE);
	snd.sensor.batteryState = ((rand()%2 == 0)?IHU_CCL_SENSOR_STATE_DEACTIVE:IHU_CCL_SENSOR_STATE_ACTIVE);	
	snd.sensor.tempValue = rand()%100;
	snd.sensor.humidValue = rand()%100;
	snd.sensor.batteryValue = rand()%100;
	snd.length = sizeof(msg_struct_dido_ccl_sensor_status_rep_t);
	ret = ihu_message_send(MSG_ID_DIDO_CCL_SENSOR_STATUS_RESP, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
		return IHU_FAILURE;
	}
			
	//返回
	return IHU_SUCCESS;
}

//控制状态
OPSTAT fsm_didocap_ccl_ctrl_cmd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	int i = 0;
	msg_struct_ccl_com_ctrl_cmd_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_com_ctrl_cmd_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_com_ctrl_cmd_t))){
		IhuErrorPrint("DIDOCAP: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//操作状态
	if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_ACTIVE){		
		zIhuCclDidocapCtrlTable.cclDidoWorkingMode = IHU_CCL_DIDO_WORKING_MODE_ACTIVE;
		//启动工作扫描定时器
		ret = ihu_timer_start(TASK_ID_DIDOCAP, TIMER_ID_1S_CCL_DIDO_WORKING_PERIOD_SCAN, zIhuSysEngPar.timer.cclDidoWorkingPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Error start timer!\n");
			return IHU_FAILURE;
		}
	}
	
	//休眠操作，终于可以清静了，就留下慢扫描定时器了
	else if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_SLEEP){
		zIhuCclDidocapCtrlTable.cclDidoWorkingMode = IHU_CCL_DIDO_WORKING_MODE_SLEEP;
		//停止工作定时器
		ret = ihu_timer_stop(TASK_ID_DIDOCAP, TIMER_ID_1S_CCL_DIDO_WORKING_PERIOD_SCAN, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Error start timer!\n");
			return IHU_FAILURE;
		}
	}
	
	//在此模式下，依然需要随时扫描，等待关闭信息上报给CCL
	//在FAULT下，唯一等待就是DOOR_AND_LOCK_C事件，如果扫描到了，才能进SLEEP模式，所以一旦进入FAULT模式后，工作扫描定时器还要保留的
	else if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_FAULT){
		zIhuCclDidocapCtrlTable.cclDidoWorkingMode = IHU_CCL_DIDO_WORKING_MODE_FAULT;
		//启动工作扫描定时器
		ret = ihu_timer_start(TASK_ID_DIDOCAP, TIMER_ID_1S_CCL_DIDO_WORKING_PERIOD_SCAN, zIhuSysEngPar.timer.cclDidoWorkingPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Error start timer!\n");
			return IHU_FAILURE;
		}	
	}
	
	//有具体命令的操作
	else if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_CTRL){
		if (rcv.cmdid == IHU_CCL_DH_CMDID_CMD_ENABLE_LOCK){
			//给每一个不是本身的锁具发送ENABLE信号
			for (i=0; i<IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
				if (i != rcv.lockid) func_didocap_ccl_work_mode_dl_cmd_enable_lock(i);
			}
		}
		else if (rcv.cmdid == IHU_CCL_DH_CMDID_CMD_DOOR_OPEN){
			//给每一个锁具发送OPEN信号
			for (i=0; i<IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
				func_didocap_ccl_work_mode_dl_cmd_open_lock(i);
			}
		}
		else if (rcv.cmdid == IHU_CCL_DH_CMDID_CMD_DOOR_CLOSE){
			//给每一个锁具发送CLOSE信号
			for (i=0; i<IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
				func_didocap_ccl_work_mode_dl_cmd_close_lock(i);
			}
		}
		else{
		IhuErrorPrint("DIDOCAP: Receive message error on cmdid!\n");
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		return IHU_FAILURE;				
		}
	}
	else{
		IhuErrorPrint("DIDOCAP: Receive message error on workmode!\n");
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		return IHU_FAILURE;		
	}
	
	//返回
	return IHU_SUCCESS;
}

//WORK_MODE下三种对外指令
void func_didocap_ccl_work_mode_dl_cmd_open_lock(UINT8 lockid)
{
}

void func_didocap_ccl_work_mode_dl_cmd_close_lock(UINT8 lockid)
{
}

void func_didocap_ccl_work_mode_dl_cmd_enable_lock(UINT8 lockid)
{
}


//WORK_MODE下的四种EVENT扫描情况，目前暂时只需要这四种，不需要更多的
bool func_didocap_ccl_work_mode_ul_scan_any_door_open(void)
{
	if (rand()%10 == 1)
		return TRUE;
	else
		return FALSE;
}

//WORK_MODE下的四种EVENT扫描情况，目前暂时只需要这四种，不需要更多的
bool func_didocap_ccl_work_mode_ul_scan_all_door_and_lock_close(void)
{
	if (rand()%10 == 1)
		return TRUE;
	else
		return FALSE;
}

//WORK_MODE下的四种EVENT扫描情况，目前暂时只需要这四种，不需要更多的
bool func_didocap_ccl_work_mode_ul_scan_enable_lock_trigger(void)
{
	if (rand()%10 == 1)
		return TRUE;
	else
		return FALSE;
}

//WORK_MODE下的四种EVENT扫描情况，目前暂时只需要这四种，不需要更多的
bool func_didocap_ccl_work_mode_ul_scan_door_and_lock_status_change(void)
{
	if (rand()%10 == 1)
		return TRUE;
	else
		return FALSE;
}

//SLEEP模式下扫描
bool func_didocap_ccl_sleep_mode_ul_scan_lock_trigger(void)
{
	if (rand()%100 == 1)
		return TRUE;
	else
		return FALSE;
}

//SLEEP模式下扫描
bool func_didocap_ccl_sleep_mode_ul_scan_shake_trigger(void)
{
	if (rand()%100 == 1)
		return TRUE;
	else
		return FALSE;
}

//SLEEP&FAULT模式下扫描：或者综合结果
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_status(void)
{
	int i = 0;
	UINT8 tmp = 0;
	UINT8 LockDoorCnt = 0;
	
	for (i=0; i<IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
		zIhuCclDidocapCtrlTable.sensor.doorState[i] = ((func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_door_open(i) == TRUE)?IHU_CCL_SENSOR_STATE_OPEN:IHU_CCL_SENSOR_STATE_CLOSE);
		if (zIhuCclDidocapCtrlTable.sensor.doorState[i] == IHU_CCL_SENSOR_STATE_OPEN) {tmp++; LockDoorCnt++;}
		zIhuCclDidocapCtrlTable.sensor.lockTongueState[i] = ((func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_lock_open(i) == TRUE)?IHU_CCL_SENSOR_STATE_OPEN:IHU_CCL_SENSOR_STATE_CLOSE);
		if (zIhuCclDidocapCtrlTable.sensor.lockTongueState[i] == IHU_CCL_SENSOR_STATE_OPEN) {tmp++; LockDoorCnt++;}
	}
	zIhuCclDidocapCtrlTable.sensor.waterState = ((func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_water() == TRUE)?IHU_CCL_SENSOR_STATE_ACTIVE:IHU_CCL_SENSOR_STATE_DEACTIVE);
	if (zIhuCclDidocapCtrlTable.sensor.waterState == IHU_CCL_SENSOR_STATE_ACTIVE) tmp++;
	zIhuCclDidocapCtrlTable.sensor.smokeState = ((func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_smoke() == TRUE)?IHU_CCL_SENSOR_STATE_ACTIVE:IHU_CCL_SENSOR_STATE_DEACTIVE);
	if (zIhuCclDidocapCtrlTable.sensor.smokeState == IHU_CCL_SENSOR_STATE_ACTIVE) tmp++;
	zIhuCclDidocapCtrlTable.sensor.fallState = ((func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_fall() == TRUE)?IHU_CCL_SENSOR_STATE_ACTIVE:IHU_CCL_SENSOR_STATE_DEACTIVE);
	if (zIhuCclDidocapCtrlTable.sensor.fallState == IHU_CCL_SENSOR_STATE_ACTIVE) tmp++;
	zIhuCclDidocapCtrlTable.sensor.batteryState = ((func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_battery() == TRUE)?IHU_CCL_SENSOR_STATE_ACTIVE:IHU_CCL_SENSOR_STATE_DEACTIVE);
	if (zIhuCclDidocapCtrlTable.sensor.batteryState == IHU_CCL_SENSOR_STATE_ACTIVE) tmp++;
	
	zIhuCclDidocapCtrlTable.flagSensorLastTimeScanFault = zIhuCclDidocapCtrlTable.flagSensorThisTimeScanFault;
	zIhuCclDidocapCtrlTable.flagDoorLockLastTimeScanFault = zIhuCclDidocapCtrlTable.flagDoorLockThisTimeScanFault;
	if (LockDoorCnt == 0) zIhuCclDidocapCtrlTable.flagDoorLockThisTimeScanFault = FALSE;
	else zIhuCclDidocapCtrlTable.flagDoorLockThisTimeScanFault = TRUE;
	
	if (tmp == 0){
		zIhuCclDidocapCtrlTable.flagSensorThisTimeScanFault = FALSE;
		return FALSE;
	}
	else{
		zIhuCclDidocapCtrlTable.flagSensorThisTimeScanFault = TRUE;
		return TRUE;
	}
}

//SLEEP&FAULT模式下扫描：是否恢复
//恢复不能看门锁，不然跟关门信号冲突。关门信号留待差错模式下的关门信号扫描去完成
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_recover(void)
{
	bool flag;
	flag = ((zIhuCclDidocapCtrlTable.flagDoorLockLastTimeScanFault == TRUE) && (zIhuCclDidocapCtrlTable.flagDoorLockThisTimeScanFault == FALSE));

	if ((zIhuCclDidocapCtrlTable.flagSensorLastTimeScanFault == TRUE) && (zIhuCclDidocapCtrlTable.flagSensorThisTimeScanFault == FALSE) &&\
		(flag == FALSE)) return TRUE;
	else return FALSE;
}

//SLEEP&FAULT模式下扫描：扫描出哪个门，如果是IHU_CCL_SENSOR_LOCK_NUMBER_MAX则意味着没有
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_door_open(UINT8 doorid)
{
	if (rand()%1000 == 1)
		return TRUE;
	else
		return FALSE;
}

//SLEEP&FAULT模式下扫描：扫描出哪个锁，如果是IHU_CCL_SENSOR_LOCK_NUMBER_MAX则意味着没有
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_lock_open(UINT8 lockid)
{
	if (rand()%1000 == 1)
		return TRUE;
	else
		return FALSE;
}

//SLEEP&FAULT模式下扫描：扫描水
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_water(void)
{
	if (rand()%1000 == 1)
		return TRUE;
	else
		return FALSE;
}

//SLEEP&FAULT模式下扫描：扫描烟
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_smoke(void)
{
	if (rand()%1000 == 1)
		return TRUE;
	else
		return FALSE;
}

//SLEEP&FAULT模式下扫描：扫描倾
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_fall(void)
{
	if (rand()%1000 == 1)
		return TRUE;
	else
		return FALSE;
}

//SLEEP&FAULT模式下扫描：扫描电池
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_battery(void)
{
	if (rand()%1000 == 1)
		return TRUE;
	else
		return FALSE;
}

#endif

/*
//锁
snd2.lockid = rand() % IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
snd2.sensor.lockTongueState[snd2.lockid] = ((rand()%2==1)?IHU_CCL_SENSOR_STATE_OPEN:IHU_CCL_SENSOR_STATE_CLOSE);
snd2.faultBitmap[IHU_CCL_DIDO_SENSOR_INDEX_LOCK] = (snd2.sensor.lockTongueState[snd2.lockid] == IHU_CCL_SENSOR_STATE_OPEN)?IHU_CCL_SENSOR_STATE_ACTIVE:IHU_CCL_SENSOR_STATE_DEACTIVE;
zIhuCclDidocapCtrlTable.sensor.lockTongueState[snd2.lockid] = snd2.sensor.lockTongueState[snd2.lockid];	
//门
snd2.sensor.doorState[snd2.lockid] = ((rand()%2==1)?IHU_CCL_SENSOR_STATE_OPEN:IHU_CCL_SENSOR_STATE_CLOSE);
snd2.faultBitmap[IHU_CCL_DIDO_SENSOR_INDEX_DOOR] = (snd2.sensor.doorState[snd2.lockid] == IHU_CCL_SENSOR_STATE_OPEN)?IHU_CCL_SENSOR_STATE_ACTIVE:IHU_CCL_SENSOR_STATE_DEACTIVE;
zIhuCclDidocapCtrlTable.sensor.doorState[snd2.lockid] = snd2.sensor.doorState[snd2.lockid];
//烟
snd2.sensor.smokeState = ((rand()%2==1)?IHU_CCL_SENSOR_STATE_ACTIVE:IHU_CCL_SENSOR_STATE_DEACTIVE);
snd2.faultBitmap[IHU_CCL_DIDO_SENSOR_INDEX_SMOKE] = snd2.sensor.smokeState;
zIhuCclDidocapCtrlTable.sensor.waterState = snd2.sensor.smokeState;
//水
snd2.sensor.waterState = ((rand()%2==1)?IHU_CCL_SENSOR_STATE_ACTIVE:IHU_CCL_SENSOR_STATE_DEACTIVE);
snd2.faultBitmap[IHU_CCL_DIDO_SENSOR_INDEX_WATER] = snd2.sensor.waterState;
zIhuCclDidocapCtrlTable.sensor.waterState = snd2.sensor.waterState;
//倒
snd2.sensor.fallState = ((rand()%2==1)?IHU_CCL_SENSOR_STATE_ACTIVE:IHU_CCL_SENSOR_STATE_DEACTIVE);
snd2.faultBitmap[IHU_CCL_DIDO_SENSOR_INDEX_FALL] = snd2.sensor.fallState;
zIhuCclDidocapCtrlTable.sensor.fallState = snd2.sensor.fallState;
//电
snd2.sensor.batteryState = ((rand()%2==1)?IHU_CCL_SENSOR_STATE_ACTIVE:IHU_CCL_SENSOR_STATE_DEACTIVE);
snd2.faultBitmap[IHU_CCL_DIDO_SENSOR_INDEX_FALL] = snd2.sensor.batteryState;
zIhuCclDidocapCtrlTable.sensor.batteryState = snd2.sensor.batteryState;
*/




