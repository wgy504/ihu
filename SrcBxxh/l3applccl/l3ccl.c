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
FsmStateItem_t FsmCcl[] =
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

	//FSM_STATE_CCL_ACTIVED：激活状态，获得TRIGGER_EVENT则进入INQUERY查询状态。如果非常开门锁则进入FATAL_FAULT状态。如果长定时到达则进入EVENT_REPORT状态
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_ACTIVED,         					fsm_ccl_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_CCL_ACTIVED,         					fsm_ccl_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_ACTIVED,         				  fsm_ccl_time_out},
//	{MSG_ID_DIDO_LOCK_TRIGGER_EVENT,				FSM_STATE_CCL_ACTIVED,         					fsm_ccl_dido_lock_trigger_event},
//	{MSG_ID_DIDO_DOOR_ILG_OPEN_EVENT,				FSM_STATE_CCL_ACTIVED,         					fsm_ccl_dido_door_ilg_open_event},
//	{MSG_ID_DIDO_LOCK_ILG_OPEN_EVENT,				FSM_STATE_CCL_ACTIVED,         					fsm_ccl_dido_lock_ilg_open_event},
//	{MSG_ID_DIDO_SENSOR_WARNING_EVENT,			FSM_STATE_CCL_ACTIVED,         					fsm_ccl_dido_sensor_warning_event},
	{MSG_ID_SPS_TO_CCL_CLOUD_FB,						FSM_STATE_CCL_ACTIVED,         					fsm_ccl_sps_cloud_fb},	//证实的回复消息
	
	//FSM_STATE_CCL_EVENT_REPORT：发送完整的状态报告给后台
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_time_out},
	{MSG_ID_DIDO_CCL_SENSOR_STATUS_RESP,		FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_dido_sensor_status_resp},	
	{MSG_ID_SPS_CCL_SENSOR_STATUS_RESP,			FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_sps_sensor_status_resp},	
	{MSG_ID_I2C_CCL_SENSOR_STATUS_RESP,			FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_i2c_sensor_status_resp},	
	{MSG_ID_DCMI_CCL_SENSOR_STATUS_RESP,		FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_dcmi_sensor_status_resp},
	{MSG_ID_SPS_CCL_EVENT_REPORT_CFM,				FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_sps_event_report_cfm},	
	
	//FSM_STATE_CCL_CLOUD_INQUERY：等待后台回传指令，开门授权则进入TO_OPEN_DOOR状态，否则回到ACTIVED状态
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_CLOUD_INQUERY,         		fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_CLOUD_INQUERY,         		fsm_ccl_time_out},
	{MSG_ID_SPS_TO_CCL_CLOUD_FB,						FSM_STATE_CCL_CLOUD_INQUERY,         		fsm_ccl_sps_cloud_fb},
	
	//FSM_STATE_CCL_TO_OPEN_DOOR：启动定时1分钟，等待人工开门，超时则关门回归ACTIVE
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_TO_OPEN_DOOR,         		fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_TO_OPEN_DOOR,         		fsm_ccl_time_out},
	{MSG_ID_DIDO_DOOR_OPEN_EVENT,						FSM_STATE_CCL_TO_OPEN_DOOR,         		fsm_ccl_dido_door_open_event},

	//FSM_STATE_CCL_DOOR_OPEN：启动10分钟干活定时，监控门限和门锁，超时进入FATAL FAULT。正常关门则发送报告给后台，然后进入ACTIVED状态
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_DOOR_OPEN,         				fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_DOOR_OPEN,         				fsm_ccl_time_out},
	{MSG_ID_DIDO_LOCK_C_DOOR_C_EVENT,				FSM_STATE_CCL_DOOR_OPEN,         				fsm_ccl_lock_and_door_close_event},
	
	//FSM_STATE_CCL_FATAL_FAULT：严重错误状态，发送报告给后台，等待人工干预以后回归ACTIVE
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_time_out},
	{MSG_ID_SPS_TO_CCL_CLOUD_FB,						FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_sps_cloud_fb},
	{MSG_ID_DIDO_LOCK_C_DOOR_C_EVENT,				FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_lock_and_door_close_event},	

	//FSM_STATE_CCL_FATAL_FAULT：严重错误状态，发送报告给后台，等待人工干预以后回归ACTIVE
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_SLEEP,         					fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_SLEEP,         					fsm_ccl_time_out},
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             							NULL},  //Ending
};

//Global variables defination
com_sensor_status_t zIhuCclSensorStatus;

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
			IhuErrorPrint("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CCL], zIhuTaskNameList[src_id]);
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
	zIhuRunErrCnt[TASK_ID_CCL] = 0;
	memset(&zIhuCclSensorStatus, 0, sizeof(com_sensor_status_t));

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_ACTIVED) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Error Set FSM State!");
		return IHU_FAILURE;
	}
	
	//启动心跳定时器，确保喂狗的基本功能
	ret = ihu_timer_start(TASK_ID_CCL, TIMER_ID_1S_CCL_PERIOD_SCAN, zIhuSysEngPar.timer.cclPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Error start timer!\n");
		return IHU_FAILURE;
	}

	//启动超长周期定时汇报定时器
	ret = ihu_timer_start(TASK_ID_CCL, TIMER_ID_1S_CCL_EVENT_REPORT_PEROID_SCAN, zIhuSysEngPar.timer.cclEventReportPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_CCL]++;
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
		zIhuRunErrCnt[TASK_ID_CCL]++;
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
		zIhuRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Wrong input paramters!");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_CCL]++;
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
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuRunErrCnt[TASK_ID_CCL]是否超限
	if (zIhuRunErrCnt[TASK_ID_CCL] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuRunErrCnt[TASK_ID_CCL] = zIhuRunErrCnt[TASK_ID_CCL] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_CCL, TASK_ID_CCL, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_CCL]++;
			IhuErrorPrint("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CCL], zIhuTaskNameList[TASK_ID_CCL]);
			return IHU_FAILURE;
		}
	}

	//喂狗心跳定时器
	if ((rcv.timeId == TIMER_ID_1S_CCL_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_ccl_time_out_period_event_report();
	}

	//超长周期定时器：一定要在SLEEP状态下才可以被激活，否则不应该不激活
	else if ((rcv.timeId == TIMER_ID_1S_CCL_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (FsmGetState(TASK_ID_CCL) == FSM_STATE_CCL_SLEEP) func_ccl_time_out_event_report_period_scan();
	}
	
	//锁的正常激活工作周期
	else if ((rcv.timeId == TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_ccl_time_out_lock_work_active();
	}	
	
	else{
			zIhuRunErrCnt[TASK_ID_CCL]++;
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
	msg_struct_ccl_sps_sensor_status_req_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_ccl_sensor_status_rep_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_ccl_sensor_status_rep_t))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_RESP_STATUS_DIDO){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;		
	}
		
	//获取报告数据
	for (i=0; i<IHU_CCL_SENSOR_NUMBER_MAX; i++){
		zIhuCclSensorStatus.doorState[i] = rcv.sensor.doorState[i];
		zIhuCclSensorStatus.lockiTriggerState[i] = rcv.sensor.lockiTriggerState[i];
		zIhuCclSensorStatus.lockoEnableState[i] = rcv.sensor.lockoEnableState[i];
	}
	zIhuCclSensorStatus.batteryValue = rcv.sensor.batteryValue;
	zIhuCclSensorStatus.fallState = rcv.sensor.fallState;
	zIhuCclSensorStatus.tempValue = rcv.sensor.tempValue;
	zIhuCclSensorStatus.humidValue = rcv.sensor.humidValue;
	
	//按照顺序，继续扫描第二组SPS传感器的数据
	memset(&snd, 0, sizeof(msg_struct_ccl_sps_sensor_status_req_t));
	snd.length = sizeof(msg_struct_ccl_sps_sensor_status_req_t);
	ret = ihu_message_send(MSG_ID_CCL_SPS_SENSOR_STATUS_REQ, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CCL], zIhuTaskNameList[TASK_ID_SPSVIRGO]);
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//L2传感器送来的状态报告
OPSTAT fsm_ccl_sps_sensor_status_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_sps_ccl_sensor_status_rep_t rcv;
	msg_struct_ccl_i2c_sensor_status_req_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_sps_ccl_sensor_status_rep_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sps_ccl_sensor_status_rep_t))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_RESP_STATUS_SPS){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;		
	}
		
	//获取报告数据
	zIhuCclSensorStatus.rssiValue = rcv.sensor.rssiValue;
	
	//按照顺序，继续扫描第三组I2C传感器的数据
	memset(&snd, 0, sizeof(msg_struct_ccl_i2c_sensor_status_req_t));
	snd.length = sizeof(msg_struct_ccl_i2c_sensor_status_req_t);
	ret = ihu_message_send(MSG_ID_CCL_I2C_SENSOR_STATUS_REQ, TASK_ID_I2CARIES, TASK_ID_CCL, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CCL], zIhuTaskNameList[TASK_ID_I2CARIES]);
		return IHU_FAILURE;
	}

	//返回
	return IHU_SUCCESS;
}

//L2传感器送来的状态报告
OPSTAT fsm_ccl_i2c_sensor_status_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_i2c_ccl_sensor_status_rep_t rcv;
	msg_struct_ccl_dcmi_sensor_status_req_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_i2c_ccl_sensor_status_rep_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_i2c_ccl_sensor_status_rep_t))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_RESP_STATUS_I2C){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;		
	}
		
	//获取报告数据
	zIhuCclSensorStatus.rsv1Value = rcv.sensor.rsv1Value;
	zIhuCclSensorStatus.rsv2Value = rcv.sensor.rsv2Value;
	
	//按照顺序，继续扫描第四组DCMI传感器的数据
	memset(&snd, 0, sizeof(msg_struct_ccl_dcmi_sensor_status_req_t));
	snd.length = sizeof(msg_struct_ccl_dcmi_sensor_status_req_t);
	ret = ihu_message_send(MSG_ID_CCL_DCMI_SENSOR_STATUS_REQ, TASK_ID_DCMIARIS, TASK_ID_CCL, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CCL], zIhuTaskNameList[TASK_ID_DCMIARIS]);
		return IHU_FAILURE;
	}

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
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dcmi_ccl_sensor_status_rep_t))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_RESP_STATUS_DCMI){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;		
	}
		
	//获取报告数据
	for (i=0; i<IHU_CCL_SENSOR_NUMBER_MAX; i++){
		zIhuCclSensorStatus.cameraState[i] = rcv.sensor.cameraState[i];
	}
	
	//发送SPS数据给串口，形成发送的报告
	memset(&snd, 0, sizeof(msg_struct_ccl_sps_event_report_send_t));
	snd.cmdid = IHU_CCL_DH_CMDID_EVENT_REPORT;
	memcpy(&(snd.sensor), &zIhuCclSensorStatus, sizeof(com_sensor_status_t));
	snd.length = sizeof(msg_struct_ccl_sps_event_report_send_t);
	ret = ihu_message_send(MSG_ID_CCL_SPS_EVENT_REPORT_SEND, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CCL], zIhuTaskNameList[TASK_ID_SPSVIRGO]);
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//L2传感器送来的状态报告
OPSTAT fsm_ccl_sps_event_report_cfm(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_sps_ccl_event_report_cfm rcv;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_sps_ccl_event_report_cfm));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sps_ccl_event_report_cfm))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_EVENT_REPORT){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;		
	}
		
	//进入SLEEP状态
	func_ccl_close_all_sensor();
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_SLEEP) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_CCL]++;
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
		zIhuRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CCL], zIhuTaskNameList[TASK_ID_VMFO]);
		return ;
	}
	
	//返回
	return;			
}

//超长定时报告工作周期
void func_ccl_time_out_event_report_period_scan(void)
{
	int ret = 0;
	msg_struct_ccl_dido_sensor_status_req_t snd;

	//先进入EVENT_REPORT状态
	func_ccl_open_all_sensor();
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_EVENT_REPORT) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Error Set FSM State!");
		return;
	}

	//控制所有传感器，进入工作模式
	//作为周期采样模式，这里是不必要的，只有在工作模式下才需要这个过程
	
	//准备接收数据的缓冲区
	memset(&zIhuCclSensorStatus, 0, sizeof(com_sensor_status_t));
	
	//发送第一组DIDO采样命令
	memset(&snd, 0, sizeof(msg_struct_ccl_dido_sensor_status_req_t));
	snd.length = sizeof(msg_struct_ccl_dido_sensor_status_req_t);
	ret = ihu_message_send(MSG_ID_CCL_DIDO_SENSOR_STATUS_REQ, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CCL], zIhuTaskNameList[TASK_ID_DIDOCAP]);
		return;
	}
		
	//返回
	return;
}


//锁的工作周期之内
void func_ccl_time_out_lock_work_active(void)
{
	//发送状态查询请求给DIDO模块
	//MSG_ID_CCL_TO_DH_SENSOR_STATUS_REQ

	//状态机转移	

}

//L2 SPS_GPRS工作查询的结果
OPSTAT fsm_ccl_sps_cloud_fb(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//int ret = 0;
	msg_struct_spsvirgo_to_ccl_cloud_fb_t rcv;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_spsvirgo_to_ccl_cloud_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_spsvirgo_to_ccl_cloud_fb_t))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);	
	
	//对收到的后台指令反馈结果进行处理
	
	//如果是证实发送的报告结果，则结束

	//如果是得到开门授权指令，则发送命令到DIDO模块
	//MSG_ID_CCL_TO_DIDO_CTRL_CMD

	//如果是没有得到开门授权，则结束
	
	//状态转移

	//返回
	return IHU_SUCCESS;
}

//收到门锁被触发
OPSTAT fsm_ccl_dido_lock_trigger_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//int ret = 0;
	msg_struct_dido_lock_trigger_event_t rcv;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_lock_trigger_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_lock_trigger_event_t))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);	
	
	//对收到的后台指令反馈结果进行处理
	
	//发送查询后台命令给SPS模块
	//MSG_ID_CCL_TO_SPS_OPEN_AUTH_INQ
		
	//状态转移

	//返回
	return IHU_SUCCESS;
}

//门被非法打开
OPSTAT fsm_ccl_dido_door_ilg_open_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//int ret = 0;
	msg_struct_dido_door_ilg_open_event_t rcv;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_door_ilg_open_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_door_ilg_open_event_t))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);	
	
	//对收到的后台指令反馈结果进行处理
		
	//状态转移

	//返回
	return IHU_SUCCESS;
}

//锁被非法打开
OPSTAT fsm_ccl_dido_lock_ilg_open_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//int ret = 0;
	msg_struct_dido_lock_ilg_open_event_t rcv;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_lock_ilg_open_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_lock_ilg_open_event_t))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);	
	
	//对收到的后台指令反馈结果进行处理
		
	//状态转移

	//返回
	return IHU_SUCCESS;
}

//门被正常打开
OPSTAT fsm_ccl_dido_door_open_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//int ret = 0;
	msg_struct_dido_door_open_event_t rcv;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_door_open_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_door_open_event_t))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);	
	
	//对收到的后台指令反馈结果进行处理
		
	//状态转移

	//返回
	return IHU_SUCCESS;
}

//门和锁均被正常关闭
OPSTAT fsm_ccl_lock_and_door_close_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//int ret = 0;
	msg_struct_dido_lock_c_door_c_event_t rcv;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_lock_c_door_c_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_lock_c_door_c_event_t))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);	
	
	//对收到的后台指令反馈结果进行处理
		
	//状态转移

	//返回
	return IHU_SUCCESS;
}

//门和锁均被正常关闭
OPSTAT fsm_ccl_dido_sensor_warning_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//int ret = 0;
	msg_struct_dido_sensor_warning_event_t rcv;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_sensor_warning_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_sensor_warning_event_t))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);	
	
	//对收到的后台指令反馈结果进行处理
		
	//状态转移

	//返回
	return IHU_SUCCESS;
}

//关掉所有的外设
void func_ccl_close_all_sensor(void)
{
	

}

//打开所有的外设
void func_ccl_open_all_sensor(void)
{
	

}


