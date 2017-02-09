/**
 ****************************************************************************************
 *
 * @file l3ccl.c
 *
 * @brief L3 CCL
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
 #include "l3ccl.h"
 
/*
** FSM of the CCL
*/
IhuFsmStateItem_t IhuFsmCcl[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            						fsm_ccl_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            							fsm_ccl_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            							fsm_ccl_restart},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_INITED,         					fsm_ccl_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_CCL_INITED,         					fsm_ccl_stop_rcv},

	//启动以后直接进入到SLEEP状态，不再需要ACTIVED状态
	//FSM_STATE_CCL_ACTIVED：激活状态，获得TRIGGER_EVENT则进入INQUERY查询状态。如果非常开门锁则进入FATAL_FAULT状态。如果长定时到达则进入EVENT_REPORT状态
	
	//FSM_STATE_CCL_EVENT_REPORT：发送完整的状态报告给后台
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_time_out},
	{MSG_ID_DIDO_CCL_SENSOR_STATUS_RESP,		FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_dido_sensor_status_resp},	 //传感器采样
	{MSG_ID_SPS_CCL_SENSOR_STATUS_RESP,			FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_sps_sensor_status_resp},	 //传感器采样
	{MSG_ID_I2C_CCL_SENSOR_STATUS_RESP,			FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_i2c_sensor_status_resp},	 //传感器采样
	{MSG_ID_DCMI_CCL_SENSOR_STATUS_RESP,		FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_dcmi_sensor_status_resp},  //传感器采样
	{MSG_ID_SPS_CCL_EVENT_REPORT_CFM,				FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_sps_event_report_cfm},     //发送周期报告的证实		
	
	//FSM_STATE_CCL_CLOUD_INQUERY：启动SESSION定时4.5分钟，等待后台回传指令，开门授权则进入TO_OPEN_DOOR状态，否则回到SLEEP状态
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_CLOUD_INQUERY,         		fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_CLOUD_INQUERY,         		fsm_ccl_time_out},
	{MSG_ID_SPS_CCL_CLOUD_FB,								FSM_STATE_CCL_CLOUD_INQUERY,         		fsm_ccl_sps_cloud_fb}, 							//后台反馈	
	
	//FSM_STATE_CCL_TO_OPEN_DOOR：启动30S开门定时，等待人工开门，超时则关门回归ACTIVE
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_TO_OPEN_DOOR,         		fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_TO_OPEN_DOOR,         		fsm_ccl_time_out},
	{MSG_ID_DIDO_CCL_DOOR_OPEN_EVENT,				FSM_STATE_CCL_TO_OPEN_DOOR,         		fsm_ccl_dido_door_open_event},      //开门事件
	{MSG_ID_DIDO_CCL_EVENT_STATUS_UPDATE,		FSM_STATE_CCL_TO_OPEN_DOOR,         		fsm_ccl_dido_event_status_update},  //事件更新，重复触发
	{MSG_ID_SPS_CCL_CLOSE_REPORT_CFM,				FSM_STATE_CCL_TO_OPEN_DOOR,         		fsm_ccl_sps_close_door_report_cfm},	//发送一次开关门报告的证实			
	
	//FSM_STATE_CCL_DOOR_OPEN：监控门限和门锁，大定时器超时进入FATAL FAULT。正常关门则发送报告给后台，然后进入ACTIVED状态
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_DOOR_OPEN,         				fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_DOOR_OPEN,         				fsm_ccl_time_out},
	{MSG_ID_DIDO_CCL_LOCK_C_DOOR_C_EVENT,		FSM_STATE_CCL_DOOR_OPEN,         				fsm_ccl_lock_and_door_close_event}, //关门事件
	{MSG_ID_DIDO_CCL_EVENT_STATUS_UPDATE,		FSM_STATE_CCL_DOOR_OPEN,         				fsm_ccl_dido_event_status_update},  //事件更新，重复触发
	{MSG_ID_SPS_CCL_CLOSE_REPORT_CFM,				FSM_STATE_CCL_DOOR_OPEN,         				fsm_ccl_sps_close_door_report_cfm},	//发送一次开关门报告的证实		
	
	//FSM_STATE_CCL_FATAL_FAULT：严重错误状态，发送报告给后台，等待人工干预或者关门信号后回归SLEEP
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_time_out},
	{MSG_ID_DIDO_CCL_LOCK_C_DOOR_C_EVENT,		FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_lock_and_door_close_event},	  //关门事件
	{MSG_ID_DIDO_CCL_EVENT_FAULT_TRIGGER,		FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_event_fault_trigger_to_stop},  //暴力开门等差错事件
	{MSG_ID_SPS_CCL_FAULT_REPORT_CFM,				FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_sps_fault_report_cfm},			//发送故障报告的证实
	{MSG_ID_SPS_CCL_CLOSE_REPORT_CFM,				FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_sps_close_door_report_cfm},	//发送一次开关门报告的证实	

	//FSM_STATE_CCL_SLEEP: 休眠状态，可以被允许触发，或进入工作模式，或进入差错模式
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_SLEEP,         						fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_SLEEP,         						fsm_ccl_time_out},
	{MSG_ID_DIDO_CCL_EVENT_LOCK_TRIGGER,		FSM_STATE_CCL_SLEEP,         						fsm_ccl_event_lock_trigger_to_work},  //锁具激活触发
	{MSG_ID_DIDO_CCL_EVENT_FAULT_TRIGGER,		FSM_STATE_CCL_SLEEP,         						fsm_ccl_event_fault_trigger_to_stop},	//暴力开门事件
	//下面这两个入口，完全是为了一种妥协，不然需要再增加一个用于后台通信证实的状态，没有必要，因为这会导致内存占用增加。如果内存允许，当然更好。
	{MSG_ID_SPS_CCL_FAULT_REPORT_CFM,				FSM_STATE_CCL_SLEEP,         						fsm_ccl_sps_fault_report_cfm},			//有可能发生定时器超时与SPS传输并发的问题
	{MSG_ID_SPS_CCL_CLOSE_REPORT_CFM,				FSM_STATE_CCL_SLEEP,         						fsm_ccl_sps_close_door_report_cfm},	//有可能发生定时器超时与SPS传输并发的问题
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             							NULL},  //Ending
};

//Global variables defination
strIhuCclCtrlPar_t zIhuCclSensorStatus;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_ccl_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("CCL: Error Set FSM State at fsm_ccl_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_ccl_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//串行回送INIT_FB给VMFO
	ihu_usleep(dest_id * IHU_MODULE_START_DISTRIBUTION_DELAY_DURATION);
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VMFO
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_CCL, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[src_id].taskName);
			return IHU_FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_INITED) == IHU_FAILURE){
		IhuErrorPrint("CCL: Error Set FSM State!");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_ccl_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("CCL: Error initialize interface!");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL] = 0;
	memset(&zIhuCclSensorStatus, 0, sizeof(strIhuCclCtrlPar_t));

//	//设置状态机到目标状态
//	if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_ACTIVED) == IHU_FAILURE){
//		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++;
//		IhuErrorPrint("CCL: Error Set FSM State!");
//		return IHU_FAILURE;
//	}
	
	//启动心跳定时器，确保喂狗的基本功能
	ret = ihu_timer_start(TASK_ID_CCL, TIMER_ID_1S_CCL_PERIOD_SCAN, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_CCL_PERIOD_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Error start timer!\n");
		return IHU_FAILURE;
	}

	//启动超长周期定时汇报定时器
	ret = ihu_timer_start(TASK_ID_CCL, TIMER_ID_1S_CCL_EVENT_REPORT_PEROID_SCAN, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_CCL_EVENT_REPORT_PEROID_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Error start timer!\n");
		return IHU_FAILURE;
	}
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("CCL: Enter FSM_STATE_CCL_ACTIVE status, Keeping refresh here!\n");
	}

	//进入休眠状态，等待被锁激活
	func_ccl_close_all_sensor();
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_SLEEP) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Error Set FSM State!");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_ccl_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("CCL: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_ccl_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_ccl_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_CCL)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Wrong input paramters!");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Error Set FSM State!");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_ccl_hw_init(void)
{
	return IHU_SUCCESS;
}

//TIMER_OUT Processing
OPSTAT fsm_ccl_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]是否超限
	if (zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL] = zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_CCL, TASK_ID_CCL, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++;
			IhuErrorPrint("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
			return IHU_FAILURE;
		}
	}

	//喂狗心跳定时器
	if ((rcv.timeId == TIMER_ID_1S_CCL_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_ccl_time_out_period_event_report();
	}

	//周期汇报，超长周期定时器：一定要在SLEEP状态下才可以被激活，否则不应该不激活
	else if ((rcv.timeId == TIMER_ID_1S_CCL_EVENT_REPORT_PEROID_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (FsmGetState(TASK_ID_CCL) == FSM_STATE_CCL_SLEEP) func_ccl_time_out_event_report_period_scan();
	}
	
	//工作模式：锁的正常激活工作周期
	else if ((rcv.timeId == TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_ccl_time_out_lock_work_active();
	}

	//工作模式：等待门被打开
	else if ((rcv.timeId == TIMER_ID_1S_CCL_LOCK_WORK_WAIT_TO_OPEN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_ccl_time_out_lock_work_wait_door_for_open();
	}
	
	else{
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++;
			IhuErrorPrint("CCL: Wrong timer parameter received!\n");
			return IHU_FAILURE;		
	}

	return IHU_SUCCESS;
}

//L2传感器送来的状态报告
OPSTAT fsm_ccl_dido_sensor_status_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0, i=0;
	msg_struct_dido_ccl_sensor_status_rep_t rcv;
	msg_struct_ccl_com_sensor_status_req_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_ccl_sensor_status_rep_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_ccl_sensor_status_rep_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_RESP_STATUS_DIDO)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
		
	//获取报告数据
	for (i=0; i<IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
		zIhuCclSensorStatus.sensor.doorState[i] = rcv.sensor.doorState[i];
		zIhuCclSensorStatus.sensor.lockiTriggerState[i] = rcv.sensor.lockiTriggerState[i];
		zIhuCclSensorStatus.sensor.lockoEnableState[i] = rcv.sensor.lockoEnableState[i];
	}
	zIhuCclSensorStatus.sensor.batteryValue = rcv.sensor.batteryValue;
	zIhuCclSensorStatus.sensor.fallState = rcv.sensor.fallState;
	zIhuCclSensorStatus.sensor.tempValue = rcv.sensor.tempValue;
	zIhuCclSensorStatus.sensor.humidValue = rcv.sensor.humidValue;
	
	//按照顺序，继续扫描第二组SPS传感器的数据
	memset(&snd, 0, sizeof(msg_struct_ccl_com_sensor_status_req_t));
	snd.cmdid = IHU_CCL_DH_CMDID_REQ_STATUS_SPS;
	snd.length = sizeof(msg_struct_ccl_com_sensor_status_req_t);
	ret = ihu_message_send(MSG_ID_CCL_COM_SENSOR_STATUS_REQ, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd, snd.length);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
	
	//返回
	return IHU_SUCCESS;
}

//L2传感器送来的状态报告
OPSTAT fsm_ccl_sps_sensor_status_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_sps_ccl_sensor_status_rep_t rcv;
	msg_struct_ccl_com_sensor_status_req_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_sps_ccl_sensor_status_rep_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sps_ccl_sensor_status_rep_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_RESP_STATUS_SPS)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
		
	//获取报告数据
	zIhuCclSensorStatus.sensor.rssiValue = rcv.sensor.rssiValue;
	
	//按照顺序，继续扫描第三组I2C传感器的数据
	memset(&snd, 0, sizeof(msg_struct_ccl_com_sensor_status_req_t));
	snd.cmdid = IHU_CCL_DH_CMDID_REQ_STATUS_I2C;	
	snd.length = sizeof(msg_struct_ccl_com_sensor_status_req_t);
	ret = ihu_message_send(MSG_ID_CCL_COM_SENSOR_STATUS_REQ, TASK_ID_I2CARIES, TASK_ID_CCL, &snd, snd.length);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_I2CARIES].taskName);

	//返回
	return IHU_SUCCESS;
}

//L2传感器送来的状态报告
OPSTAT fsm_ccl_i2c_sensor_status_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_i2c_ccl_sensor_status_rep_t rcv;
	msg_struct_ccl_com_sensor_status_req_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_i2c_ccl_sensor_status_rep_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_i2c_ccl_sensor_status_rep_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_RESP_STATUS_I2C)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
		
	//获取报告数据
	zIhuCclSensorStatus.sensor.rsv1Value = rcv.sensor.rsv1Value;
	zIhuCclSensorStatus.sensor.rsv2Value = rcv.sensor.rsv2Value;
	
	//按照顺序，继续扫描第四组DCMI传感器的数据
	memset(&snd, 0, sizeof(msg_struct_ccl_com_sensor_status_req_t));
	snd.cmdid = IHU_CCL_DH_CMDID_REQ_STATUS_DCMI;	
	snd.length = sizeof(msg_struct_ccl_com_sensor_status_req_t);
	ret = ihu_message_send(MSG_ID_CCL_COM_SENSOR_STATUS_REQ, TASK_ID_DCMIARIS, TASK_ID_CCL, &snd, snd.length);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DCMIARIS].taskName);

	//返回
	return IHU_SUCCESS;
}

//L2传感器送来的状态报告
OPSTAT fsm_ccl_dcmi_sensor_status_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	int i=0;
	msg_struct_dcmi_ccl_sensor_status_rep_t rcv;
	msg_struct_ccl_sps_event_report_send_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dcmi_ccl_sensor_status_rep_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dcmi_ccl_sensor_status_rep_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_RESP_STATUS_DCMI)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
		
	//获取报告数据
	for (i=0; i<IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
		zIhuCclSensorStatus.sensor.cameraState[i] = rcv.sensor.cameraState[i];
	}
	
	//发送SPS数据给串口，形成发送的报告
	memset(&snd, 0, sizeof(msg_struct_ccl_sps_event_report_send_t));
	snd.cmdid = IHU_CCL_DH_CMDID_EVENT_REPORT;
	memcpy(&(snd.sensor), &zIhuCclSensorStatus.sensor, sizeof(com_sensor_status_t));
	snd.length = sizeof(msg_struct_ccl_sps_event_report_send_t);
	ret = ihu_message_send(MSG_ID_CCL_SPS_EVENT_REPORT_SEND, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd, snd.length);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
	
	//返回
	return IHU_SUCCESS;
}

//L2传感器送来的状态报告
OPSTAT fsm_ccl_sps_event_report_cfm(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_sps_ccl_event_report_cfm_t rcv;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_sps_ccl_event_report_cfm_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sps_ccl_event_report_cfm_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
		
	//关闭所有接口
	func_ccl_close_all_sensor();
	
	//进入SLEEP状态
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_SLEEP) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Error Set FSM State!");
		return IHU_FAILURE;
	}
		
	//返回
	return IHU_SUCCESS;
}

//喂狗和心跳定时器工作
void func_ccl_time_out_period_event_report(void)
{
	int ret = 0;
	
	//发送HeartBeat消息给VMFO模块，实现喂狗功能
	msg_struct_com_heart_beat_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_heart_beat_t));
	snd.length = sizeof(msg_struct_com_heart_beat_t);
	ret = ihu_message_send(MSG_ID_COM_HEART_BEAT, TASK_ID_VMFO, TASK_ID_CCL, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_VMFO].taskName);
		return ;
	}
	
	//返回
	return;			
}

//超长定时报告工作周期
void func_ccl_time_out_event_report_period_scan(void)
{
	int ret = 0;
	msg_struct_ccl_com_sensor_status_req_t snd;

	//打开所有接口
	func_ccl_open_all_sensor();
	
	//先进入EVENT_REPORT状态
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_EVENT_REPORT) == IHU_FAILURE){
		IHU_ERROR_PRINT_CCL_WO_RETURN("CCL: Error Set FSM State!");
		return;
	}

	//控制所有传感器，进入工作模式
	//作为周期采样模式，这里是不必要的，只有在工作模式下才需要这个过程
	
	//准备接收数据的缓冲区
	memset(&zIhuCclSensorStatus.sensor, 0, sizeof(com_sensor_status_t));
	
	//由于消息队列的长度问题，这里采用串行发送接收模式，避免了多个接口的消息同时到达
	//发送第一组DIDO采样命令
	memset(&snd, 0, sizeof(msg_struct_ccl_com_sensor_status_req_t));
	snd.cmdid = IHU_CCL_DH_CMDID_REQ_STATUS_DIDO;
	snd.length = sizeof(msg_struct_ccl_com_sensor_status_req_t);
	ret = ihu_message_send(MSG_ID_CCL_COM_SENSOR_STATUS_REQ, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_CCL_WO_RETURN("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DIDOCAP].taskName);
		return;
	}
		
	//返回
	return;
}

//锁的工作周期之内：超时，则发送出错到各个下位机，因为此时门肯定没有正常被关闭，导致正常的工作流程没有完成
OPSTAT func_ccl_time_out_lock_work_active(void)
{
	int ret = 0;
	msg_struct_ccl_com_ctrl_cmd_t snd;
	msg_struct_ccl_sps_close_report_send_t snd1;
	msg_struct_ccl_sps_fault_report_send_t snd2;
	
	//只需要去激活所有下位机
	if (FsmGetState(TASK_ID_CCL) == FSM_STATE_CCL_CLOUD_INQUERY){
		//去激活所有下位机，此时不需要发送后台命令，因为后台都连不上
		memset(&snd, 0, sizeof(msg_struct_ccl_com_ctrl_cmd_t));
		snd.length = sizeof(msg_struct_ccl_com_ctrl_cmd_t);
		snd.workmode = IHU_CCL_DH_CMDID_WORK_MODE_SLEEP;
		ret = ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DIDOCAP].taskName);
		ret = ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd, snd.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
		ret = ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_I2CARIES, TASK_ID_CCL, &snd, snd.length);		
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_I2CARIES].taskName);
		ret = ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DCMIARIS, TASK_ID_CCL, &snd, snd.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DCMIARIS].taskName);
		
		//关闭所有接口
		func_ccl_close_all_sensor();
		
		//状态转移：直接去SLEEP状态了
		if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_SLEEP) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error Set FSM State!");
	}
	
	//待开门，意味着门还未开：这里面也有个逻辑，就是必须也得等待CLOSE_REPORT_CFM的到来，不然也会出现问题
	else if (FsmGetState(TASK_ID_CCL) == FSM_STATE_CCL_TO_OPEN_DOOR){
		//停止开门定时器：这是因为卡边，大定时器到点但开门定时器还未到点
		ret = ihu_timer_stop(TASK_ID_CCL, TIMER_ID_1S_CCL_LOCK_WORK_WAIT_TO_OPEN, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error start timer!\n");
		
		//发送关门报告给后台
		memset(&snd1, 0, sizeof(msg_struct_ccl_sps_close_report_send_t));
		snd1.cause = IHU_CCL_CLOSE_DOOR_NOT_YET_OPEN;
		snd1.length = sizeof(msg_struct_ccl_sps_close_report_send_t);
		ret = ihu_message_send(MSG_ID_CCL_SPS_CLOSE_REPORT_SEND, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd1, snd1.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
	}
	
	//门开着，为收到关门信号，这个就要进入差错了
	else if (FsmGetState(TASK_ID_CCL) == FSM_STATE_CCL_DOOR_OPEN){
		//改变下位机状态
		memset(&snd, 0, sizeof(msg_struct_ccl_com_ctrl_cmd_t));
		snd.length = sizeof(msg_struct_ccl_com_ctrl_cmd_t);
		snd.workmode = IHU_CCL_DH_CMDID_WORK_MODE_FAULT;
		if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DIDOCAP].taskName);
		if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
		if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_I2CARIES, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_I2CARIES].taskName);
		if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DCMIARIS, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DCMIARIS].taskName);
		
		//发送差错状态报告给后台
		memset(&snd2, 0, sizeof(msg_struct_ccl_sps_fault_report_send_t));
		snd2.cause = IHU_CCL_FAULT_CAUSE_CLOSE_DOOR_TIME_OUT;
		snd2.length = sizeof(msg_struct_ccl_sps_fault_report_send_t);
		if (ihu_message_send(MSG_ID_CCL_SPS_FAULT_REPORT_SEND, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd2, snd2.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);

		//状态转移：直接去FATAL_FAULT状态了
		if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_FATAL_FAULT) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error Set FSM State!");
	}
	
	//不应该还有其它可能性
	else{
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error state during TIME OUT processing!");
	}
	
	//返回
	return IHU_SUCCESS;
}

//L2 SPS_GPRS工作查询的结果
OPSTAT fsm_ccl_sps_cloud_fb(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	int ret = 0;
	msg_struct_spsvirgo_ccl_cloud_fb_t rcv;
	msg_struct_ccl_com_ctrl_cmd_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_spsvirgo_ccl_cloud_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_spsvirgo_ccl_cloud_fb_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);	
	
	//对收到的后台指令反馈结果进行处理
	if (rcv.authResult == IHU_CCL_LOCK_AUTH_RESULT_NOK){
		//去激活所有下位机
		memset(&snd, 0, sizeof(msg_struct_ccl_com_ctrl_cmd_t));
		snd.length = sizeof(msg_struct_ccl_com_ctrl_cmd_t);
		snd.workmode = IHU_CCL_DH_CMDID_WORK_MODE_SLEEP;
		if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DIDOCAP].taskName);
		if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
		if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_I2CARIES, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_I2CARIES].taskName);
		if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DCMIARIS, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DCMIARIS].taskName);
		
		//关闭所有接口
		func_ccl_close_all_sensor();
		
		//停止定时器
		if (ihu_timer_stop(TASK_ID_CCL, TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE, TIMER_RESOLUTION_1S) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error start timer!\n");

		//状态转移：直接去SLEEP状态了
		if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_SLEEP) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error Set FSM State!");
	}
		
	//如果是得到开门授权指令，则发送命令到DIDO模块
	else if (rcv.authResult == IHU_CCL_LOCK_AUTH_RESULT_OK){
		memset(&snd, 0, sizeof(msg_struct_ccl_com_ctrl_cmd_t));
		snd.length = sizeof(msg_struct_ccl_com_ctrl_cmd_t);
		snd.workmode = IHU_CCL_DH_CMDID_WORK_MODE_CTRL;
		snd.cmdid = IHU_CCL_DH_CMDID_CMD_DOOR_OPEN;
		snd.lockid = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
		if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DIDOCAP].taskName);
		
		//启动定时器
		ret = ihu_timer_start(TASK_ID_CCL, TIMER_ID_1S_CCL_LOCK_WORK_WAIT_TO_OPEN, \
			zIhuSysEngPar.timer.array[TIMER_ID_1S_CCL_LOCK_WORK_WAIT_TO_OPEN].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error start timer!\n");
		
		//状态转移
		if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_TO_OPEN_DOOR) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error Set FSM State!");
	}
	
	else{
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error received message parameter!");
	}

	//返回
	return IHU_SUCCESS;
}

//门被正常打开
OPSTAT fsm_ccl_dido_door_open_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//int ret = 0;
	msg_struct_dido_ccl_door_open_event_t rcv;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_ccl_door_open_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_ccl_door_open_event_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//停止定时器
	if (ihu_timer_stop(TASK_ID_CCL, TIMER_ID_1S_CCL_LOCK_WORK_WAIT_TO_OPEN, TIMER_RESOLUTION_1S) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error start timer!\n");
	
	//状态转移
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_DOOR_OPEN) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error Set FSM State!");

	//返回
	return IHU_SUCCESS;
}

//锁打开了，但门没有打开，超时的处理过程
OPSTAT func_ccl_time_out_lock_work_wait_door_for_open(void)
{
	//int ret = 0;
	msg_struct_ccl_com_ctrl_cmd_t snd;
	msg_struct_ccl_sps_close_report_send_t snd1;
	
	//先发送关锁命令
	memset(&snd, 0, sizeof(msg_struct_ccl_com_ctrl_cmd_t));
	snd.length = sizeof(msg_struct_ccl_com_ctrl_cmd_t);
	snd.workmode = IHU_CCL_DH_CMDID_WORK_MODE_CTRL;
	snd.cmdid = IHU_CCL_DH_CMDID_CMD_DOOR_CLOSE;
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DIDOCAP].taskName);

	//发送关门报告给后台
	memset(&snd1, 0, sizeof(msg_struct_ccl_sps_close_report_send_t));
	snd1.cause = IHU_CCL_CLOSE_DOOR_NOT_YET_OPEN;
	snd1.length = sizeof(msg_struct_ccl_sps_close_report_send_t);
	if (ihu_message_send(MSG_ID_CCL_SPS_CLOSE_REPORT_SEND, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd1, snd1.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
	
	//返回
	return IHU_SUCCESS;
}

//门和锁均被正常关闭
OPSTAT fsm_ccl_lock_and_door_close_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//int ret = 0;
	msg_struct_dido_ccl_lock_c_door_c_event_t rcv;
	msg_struct_ccl_sps_close_report_send_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_ccl_lock_c_door_c_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_ccl_lock_c_door_c_event_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
		
	//发送关门报告给后台
	memset(&snd, 0, sizeof(msg_struct_ccl_sps_close_report_send_t));
	snd.cause = IHU_CCL_CLOSE_DOOR_NORMAL;
	snd.length = sizeof(msg_struct_ccl_sps_close_report_send_t);
	if (ihu_message_send(MSG_ID_CCL_SPS_CLOSE_REPORT_SEND, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);

	//返回
	return IHU_SUCCESS;
}

//门和锁均被正常关闭之后，发送报告的结果
OPSTAT fsm_ccl_sps_close_door_report_cfm(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	int ret = 0;
	msg_struct_sps_ccl_close_report_cfm_t rcv;
	msg_struct_ccl_com_ctrl_cmd_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_sps_ccl_close_report_cfm_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sps_ccl_close_report_cfm_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
		
	//去激活所有下位机
	memset(&snd, 0, sizeof(msg_struct_ccl_com_ctrl_cmd_t));
	snd.length = sizeof(msg_struct_ccl_com_ctrl_cmd_t);
	snd.workmode = IHU_CCL_DH_CMDID_WORK_MODE_SLEEP;
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DIDOCAP].taskName);
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_I2CARIES, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_I2CARIES].taskName);
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DCMIARIS, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DCMIARIS].taskName);
	
	//关闭所有接口
	func_ccl_close_all_sensor();
	
	//停止定时器
	//实际上只有在FSM_STATE_CCL_DOOR_OPEN状态下才有意义，在FSM_STATE_CCL_FATAL_FAULT状态下这个定时器本来就没有被激活
	//考虑到即便定时器没有启动，停止一次也没关系，就不做判定了。这种强制停止，也可以避免其它差错的潜在可能性
	ret = ihu_timer_stop(TASK_ID_CCL, TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error start timer!\n");
	
	//状态转移
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_SLEEP) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error Set FSM State!");

	//返回
	return IHU_SUCCESS;
}


//工作状态下的一些门锁状态变化
OPSTAT fsm_ccl_dido_event_status_update(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	int ret = 0;
	msg_struct_dido_ccl_status_update_t rcv;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_ccl_status_update_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_ccl_status_update_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);	
	
	//如果重复收到锁被激活，则持续延长工作定时器长度
	if (rcv.cmdid == IHU_CCL_DH_CMDID_EVENT_IND_LOCK_TRIGGER){
		ret = ihu_timer_start(TASK_ID_CCL, TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE, \
			zIhuSysEngPar.timer.array[TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error start timer!\n");
	}	
	
	//其它事件，暂时不干啥
	else if (rcv.cmdid == IHU_CCL_DH_CMDID_EVENT_IND_LOCK_O_TO_C){
		//do nothing
	}

	//返回
	return IHU_SUCCESS;
}


//触发激活系统到工作
OPSTAT fsm_ccl_event_lock_trigger_to_work(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	int ret = 0;
	msg_struct_dido_ccl_event_lock_trigger_t rcv;
	msg_struct_ccl_com_ctrl_cmd_t snd;
	msg_struct_ccl_sps_open_auth_inq_t snd1;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_ccl_event_lock_trigger_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_ccl_event_lock_trigger_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);	
	
	//打开所有接口
	func_ccl_open_all_sensor();
	
	//发送控制给所有下位机
	memset(&snd, 0, sizeof(msg_struct_ccl_com_ctrl_cmd_t));
	snd.length = sizeof(msg_struct_ccl_com_ctrl_cmd_t);
	snd.workmode = IHU_CCL_DH_CMDID_WORK_MODE_ACTIVE;
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DIDOCAP].taskName);
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_I2CARIES, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_I2CARIES].taskName);
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DCMIARIS, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DCMIARIS].taskName);
	
	//启动定时器：如果是在工作模式下，允许被重复触发
	ret = ihu_timer_start(TASK_ID_CCL, TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error start timer!\n");
	
	//控制其它锁具的触发
	memset(&snd, 0, sizeof(msg_struct_ccl_com_ctrl_cmd_t));
	snd.length = sizeof(msg_struct_ccl_com_ctrl_cmd_t);
	snd.workmode = IHU_CCL_DH_CMDID_WORK_MODE_CTRL;
	snd.cmdid = IHU_CCL_DH_CMDID_CMD_ENABLE_LOCK;
	snd.lockid = rcv.lockid;
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DIDOCAP].taskName);
		
	//发送后台查询命令
	memset(&snd1, 0, sizeof(msg_struct_ccl_sps_open_auth_inq_t));
	snd1.length = sizeof(msg_struct_ccl_sps_open_auth_inq_t);
	if (ihu_message_send(MSG_ID_CCL_SPS_OPEN_AUTH_INQ, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd1, snd1.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
		
	//状态转移
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_CLOUD_INQUERY) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error Set FSM State!");
	
	//返回
	return IHU_SUCCESS;
}

//暴力开门等差错状态
OPSTAT fsm_ccl_event_fault_trigger_to_stop(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//int ret = 0;
	msg_struct_dido_ccl_event_fault_trigger_t rcv;
	msg_struct_ccl_com_ctrl_cmd_t snd;
	msg_struct_ccl_sps_fault_report_send_t snd1;
	msg_struct_ccl_sps_close_report_send_t snd2;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_ccl_event_fault_trigger_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_ccl_event_fault_trigger_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);	
			
	//打开所有接口
	func_ccl_open_all_sensor();

	//控制所有下位机到到差错状态
	memset(&snd, 0, sizeof(msg_struct_ccl_com_ctrl_cmd_t));
	snd.length = sizeof(msg_struct_ccl_com_ctrl_cmd_t);
	snd.workmode = IHU_CCL_DH_CMDID_WORK_MODE_FAULT;
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DIDOCAP].taskName);
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_I2CARIES, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_I2CARIES].taskName);
	if (ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DCMIARIS, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DCMIARIS].taskName);
	
	//分为状态恢复
	if (rcv.cmdid == IHU_CCL_DH_CMDID_EVENT_IND_FAULT_RECOVER){
		//先确定正常的情形
		if (FsmGetState(TASK_ID_CCL) != FSM_STATE_CCL_FATAL_FAULT)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Wrong state receive message!");
		//没有定时器
		//直接发送恢复关门报告给后台，等待证实以后，执行所有关闭操作
		memset(&snd2, 0, sizeof(msg_struct_ccl_sps_close_report_send_t));
		snd2.cause = IHU_CCL_CLOSE_DOOR_BY_FAULT;
		snd2.length = sizeof(msg_struct_ccl_sps_close_report_send_t);
		if (ihu_message_send(MSG_ID_CCL_SPS_CLOSE_REPORT_SEND, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd2, snd2.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
	}
	
	//继续差错故障状态
	else if (rcv.cmdid == IHU_CCL_DH_CMDID_EVENT_IND_FAULT_MULTI){
		//首次控制，发送差错报告出去
		if (zIhuCclSensorStatus.faultReportCnt == 0){
			//发送差错状态报告给后台：真正的原因，待填入
			memset(&snd1, 0, sizeof(msg_struct_ccl_sps_fault_report_send_t));
			snd1.cause = IHU_CCL_FAULT_CAUSE_SENSOR_WARNING;
			//待填入具体传感器告警内容
			snd1.length = sizeof(msg_struct_ccl_sps_fault_report_send_t);
			if (ihu_message_send(MSG_ID_CCL_SPS_FAULT_REPORT_SEND, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd1, snd1.length) == IHU_FAILURE)
				IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
		}
		zIhuCclSensorStatus.faultReportCnt = (zIhuCclSensorStatus.faultReportCnt+1)%IHU_CCL_FALULT_REPORT_TIMES_MAX;

		//状态转移
		if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_FATAL_FAULT) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error Set FSM State!");
	}
	
	else{
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Wrong parameter received!");
	}

	//返回
	return IHU_SUCCESS;
}


//差错报告的反馈证实
OPSTAT fsm_ccl_sps_fault_report_cfm(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//int ret = 0;
	msg_struct_sps_ccl_fault_report_cfm_t rcv;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_sps_ccl_fault_report_cfm_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sps_ccl_fault_report_cfm_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//返回
	return IHU_SUCCESS;
}

//打开所有的外设
void func_ccl_open_all_sensor(void)
{
	ihu_l1hd_dido_f2board_gprsmod_power_ctrl_on();
	ihu_l1hd_dido_f2board_ble_power_ctrl_on();
	ihu_l1hd_dido_f2board_rfid_power_ctrl_on();
	ihu_l1hd_dido_f2board_sensor_power_ctrl_on();
	//未来需要确定，温度传感器等耗电的传感器是否在用GPIO控制开关在控
	//打开温湿度传感器
	ihu_l1hd_dido_f2board_dht11_init();
}

//关掉所有的外设
void func_ccl_close_all_sensor(void)
{
	ihu_l1hd_dido_f2board_gprsmod_power_ctrl_off();
	ihu_l1hd_dido_f2board_ble_power_ctrl_off();	
	ihu_l1hd_dido_f2board_rfid_power_ctrl_off();
	ihu_l1hd_dido_f2board_sensor_power_ctrl_off();
}

//由于错误，直接从差错中转入休眠状态
void func_ccl_stm_main_recovery_from_fault(void)
{
	msg_struct_ccl_com_ctrl_cmd_t snd;

	//关闭所有外部器件的电源
	func_ccl_close_all_sensor();

	//设置三个模块进入SLEEP工作模式，发送控制给所有下位机
	memset(&snd, 0, sizeof(msg_struct_ccl_com_ctrl_cmd_t));
	snd.length = sizeof(msg_struct_ccl_com_ctrl_cmd_t);
	snd.workmode = IHU_CCL_DH_CMDID_WORK_MODE_SLEEP;
	ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length);
	ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd, snd.length);
	ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_I2CARIES, TASK_ID_CCL, &snd, snd.length);		
	ihu_message_send(MSG_ID_CCL_COM_CTRL_CMD, TASK_ID_DCMIARIS, TASK_ID_CCL, &snd, snd.length);
	
	//状态转移到SLEEP状态
	FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_SLEEP);
	
	//初始化模块的任务资源
	//初始化定时器：暂时决定不做，除非该模块重新RESTART
	//初始化模块级全局变量：暂时决定不做，除非该模块重新RESTART
	
	return;
}




