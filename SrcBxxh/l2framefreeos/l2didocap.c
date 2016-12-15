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
  {MSG_ID_CCL_DIDO_SENSOR_STATUS_REQ,			FSM_STATE_DIDOCAP_ACTIVED,         					fsm_didocap_ccl_sensor_status_req},
  {MSG_ID_CCL_DIDO_CTRL_CMD,							FSM_STATE_DIDOCAP_ACTIVED,         					fsm_didocap_ccl_ctrl_cmd},
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
	//定时将扫描结果发给上层
	//每一个发送消息，都需要判定L3CCL的状态，不然发送不成功。为了发送TRACE消息的完美，最好判定状态一下。再说，L3CCL不在合适的状态，本来就不应该处理
	//不合适的消息和EVENT
	//MSG_ID_DIDO_LOCK_TRIGGER_EVENT,
	//MSG_ID_DIDO_DOOR_ILG_OPEN_EVENT,
	//MSG_ID_DIDO_LOCK_DOOR_OPEN_EVENT,
	//MSG_ID_DIDO_LOCK_O_DOOR_C_EVENT,
	//MSG_ID_DIDO_LOCK_C_DOOR_C_EVENT,
	//MSG_ID_DIDO_LOCK_C_DOOR_O_EVENT,
	//MSG_ID_DIDO_SENSOR_WARNING_EVENT,

	//只会产生触发CCL醒来的触发命令
	//扫描函数

	//这是纯粹测试概率，让系统具备1/100的概率自动被触发，未来需要一旦跟硬件连接后可以被去掉
	if (rand()%100 !=1) return;	
	
	//将扫描结果发送给CCL
	msg_struct_dido_event_lock_trigger_t snd;
	memset(&snd, 0, sizeof(msg_struct_dido_event_lock_trigger_t));
	snd.lockid = rand() % IHU_CCL_SENSOR_NUMBER_MAX;
	snd.cmdid = ((rand()%2 == 1)? IHU_CCL_DH_CMDID_EVENT_IND_LOCK_TRIGGER:IHU_CCL_DH_CMDID_EVENT_IND_SHAKE_TRIGGER);
	snd.length = sizeof(msg_struct_dido_event_lock_trigger_t);
	ret = ihu_message_send(MSG_ID_DIDO_EVENT_LOCK_TRIGGER, TASK_ID_CCL, TASK_ID_DIDOCAP, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		IhuErrorPrint("DIDOCAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_DIDOCAP], zIhuTaskNameList[TASK_ID_CCL]);
		return ;
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
	msg_struct_ccl_dido_sensor_status_req_t rcv;
	msg_struct_dido_ccl_sensor_status_rep_t snd;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_dido_sensor_status_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_dido_sensor_status_req_t))){
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
	for (i=0; i<IHU_CCL_SENSOR_NUMBER_MAX; i++){
		snd.sensor.doorState[i] = ((rand()%2 == 0)?IHU_CCL_SENSOR_STATE_CLOSE:IHU_CCL_SENSOR_STATE_OPEN);
		snd.sensor.lockiTriggerState[i] = ((rand()%2 == 0)?IHU_CCL_SENSOR_STATE_DEACTIVE:IHU_CCL_SENSOR_STATE_ACTIVE);
		snd.sensor.lockoEnableState[i] = ((rand()%2 == 0)?IHU_CCL_SENSOR_STATE_DEACTIVE:IHU_CCL_SENSOR_STATE_ACTIVE);;
	}
	snd.sensor.batteryValue = rand()%100;
	snd.sensor.fallState = IHU_CCL_SENSOR_STATE_DEACTIVE;
	snd.sensor.tempValue = rand()%100;
	snd.sensor.humidValue = rand()%100;
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

OPSTAT fsm_didocap_ccl_ctrl_cmd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret;
	msg_struct_ccl_dido_ctrl_cmd_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_dido_ctrl_cmd_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_dido_ctrl_cmd_t))){
		IhuErrorPrint("DIDOCAP: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//操作门锁

	
	return IHU_SUCCESS;
}
#endif

