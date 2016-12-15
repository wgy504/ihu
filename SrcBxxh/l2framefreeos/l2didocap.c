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
	
	//启动永恒的外部触发扫描
	ret = ihu_timer_start(TASK_ID_DIDOCAP, TIMER_ID_1S_CCL_DIDO_TRIGGER_PERIOD_SCAN, zIhuSysEngPar.timer.cclDidoTriggerPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		IhuErrorPrint("DIDOCAP: Error start timer!\n");
		return IHU_FAILURE;
	}
	
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
	
	//永恒的外部触发扫描：只有在IHU_CCL_DIDO_WORKING_MODE_SLEEP模式下才会进行外部触发源的定时扫描
	else if ((rcv.timeId == TIMER_ID_1S_CCL_DIDO_TRIGGER_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if ((zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_SLEEP) || (zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_FAULT)){
			func_didocap_time_out_external_trigger_period_scan();
		}
	}
	
	//工作模式下所有外部传感器的扫描
	else if ((rcv.timeId == TIMER_ID_1S_CCL_DIDO_WORKING_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_ACTIVE) func_didocap_time_out_work_mode_period_scan();
	}
	
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

//定时扫描震动以及触发按键消息
void func_didocap_time_out_external_trigger_period_scan(void)
{
	int ret = 0;
	int temp = 0, i = 0;

	//这是纯粹测试概率，让系统具备1/100的概率自动被触发，未来需要一旦跟硬件连接后可以被去掉
	temp = rand()%100;
	
	//锁被触发，只有休眠模式下才被允许触发
	if ((zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_SLEEP) && (temp == 1)){
		//将周期扫描锁触发的结果发送给CCL
		msg_struct_dido_event_lock_trigger_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_dido_event_lock_trigger_t));
		snd0.lockid = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
		snd0.lockid = rand() % IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
		snd0.cmdid = IHU_CCL_DH_CMDID_EVENT_IND_LOCK_TRIGGER;
		snd0.length = sizeof(msg_struct_dido_event_lock_trigger_t);
		//存储做为上一次的状态
		zIhuCclDidocapCtrlTable.sensor.lockiTriggerState[snd0.lockid] = IHU_CCL_SENSOR_STATE_ACTIVE;
		ret = ihu_message_send(MSG_ID_DIDO_EVENT_LOCK_TRIGGER, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
			return ;
		}		
	}

	//震动被触发，只有休眠模式下才被允许触发
	else if ((zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_SLEEP) && (temp == 2)){
		//将周期扫描锁触发的结果发送给CCL
		msg_struct_dido_event_lock_trigger_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_dido_event_lock_trigger_t));
		snd1.lockid = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
		snd1.cmdid = IHU_CCL_DH_CMDID_EVENT_IND_SHAKE_TRIGGER;
		snd1.length = sizeof(msg_struct_dido_event_lock_trigger_t);
		//存储做为上一次的状态
		zIhuCclDidocapCtrlTable.sensor.shakeState = IHU_CCL_SENSOR_STATE_ACTIVE;
		ret = ihu_message_send(MSG_ID_DIDO_EVENT_LOCK_TRIGGER, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd1, snd1.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
			return ;
		}		
	}

	//出现差错，将导致状态机进入差错状态。如果在差错状态下，允许继续发送新的差错报告
	else if (temp == 3){
		//将周期扫描锁触发的结果发送给CCL
		msg_struct_dido_event_fault_trigger_t snd2;
		memset(&snd2, 0, sizeof(msg_struct_dido_event_fault_trigger_t));
		snd2.lockid = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
		snd2.cmdid = IHU_CCL_DH_CMDID_EVENT_IND_FAULT_MULTI; //多重错误
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
		
		//发送
		snd2.length = sizeof(msg_struct_dido_event_fault_trigger_t);
		ret = ihu_message_send(MSG_ID_DIDO_EVENT_FAULT_TRIGGER, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd2, snd2.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
			return ;
		}		
	}
	
	//完全恢复：只有FAULT模式下才谈得上恢复
	else if ((temp == 4) && (zIhuCclDidocapCtrlTable.cclDidoWorkingMode == IHU_CCL_DIDO_WORKING_MODE_FAULT)){
		//将周期扫描锁触发的结果发送给CCL
		msg_struct_dido_event_fault_trigger_t snd3;
		memset(&snd3, 0, sizeof(msg_struct_dido_event_fault_trigger_t));
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
		snd3.length = sizeof(msg_struct_dido_event_fault_trigger_t);
		ret = ihu_message_send(MSG_ID_DIDO_EVENT_FAULT_TRIGGER, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd3, snd3.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
			IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
			return ;
		}		
	}
	
	//返回
	return;
}

//工作模式下的定时器处理
void func_didocap_time_out_work_mode_period_scan(void)
{
	
	//返回
	return;
}


#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
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
	//int ret = 0;
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
	if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_ACTIVE) zIhuCclDidocapCtrlTable.cclDidoWorkingMode = IHU_CCL_DIDO_WORKING_MODE_ACTIVE;
	else if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_SLEEP) zIhuCclDidocapCtrlTable.cclDidoWorkingMode = IHU_CCL_DIDO_WORKING_MODE_SLEEP;
	else if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_FAULT) zIhuCclDidocapCtrlTable.cclDidoWorkingMode = IHU_CCL_DIDO_WORKING_MODE_FAULT;
	
	//有具体命令的操作
	else if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_CTRL){
		if (rcv.cmdid == IHU_CCL_DH_CMDID_CMD_ENABLE_LOCK){
			//给每一个不是本身的锁具发送ENABLE信号
			for (i=0; i<IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
				if (i != rcv.lockid) func_didocap_ccl_enable_lock(i);
			}
		}
		else if (rcv.cmdid == IHU_CCL_DH_CMDID_CMD_DOOR_OPEN){
			//给每一个锁具发送OPEN信号
			for (i=0; i<IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
				func_didocap_ccl_open_lock(i);
			}			
		}
		else if (rcv.cmdid == IHU_CCL_DH_CMDID_CMD_DOOR_CLOSE){
			//给每一个锁具发送CLOSE信号
			for (i=0; i<IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
				func_didocap_ccl_close_lock(i);
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

void func_didocap_ccl_open_lock(UINT8 lockid)
{
}

void func_didocap_ccl_close_lock(UINT8 lockid)
{
}

void func_didocap_ccl_enable_lock(UINT8 lockid)
{
}

#endif

