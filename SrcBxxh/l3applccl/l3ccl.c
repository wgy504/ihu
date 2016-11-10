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

	//FSM_STATE_CCL_ACTIVED：激活状态，完成后台工作命令查询，发送开门命令给锁具。状态报告给后套。
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_ACTIVED,         					fsm_ccl_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_CCL_ACTIVED,         					fsm_ccl_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_ACTIVED,         				  fsm_ccl_time_out},
//	{MSG_ID_ADC_UL_DATA_PULL_BWD,						FSM_STATE_CCL_ACTIVED,         					fsm_ccl_adc_ul_data_pull_bwd},
//	{MSG_ID_ADC_UL_CTRL_CMD_RESP,						FSM_STATE_CCL_ACTIVED,         					fsm_ccl_adc_ul_ctrl_cmd_resp},
//	{MSG_ID_SPI_UL_DATA_PULL_BWD,						FSM_STATE_CCL_ACTIVED,         					fsm_ccl_spi_ul_data_pull_bwd},
//	{MSG_ID_SPI_UL_CTRL_CMD_RESP,						FSM_STATE_CCL_ACTIVED,         					fsm_ccl_spi_ul_ctrl_cmd_resp},
	{MSG_ID_DIDO_PERIPH_SENSOR_STATUS_REP,	FSM_STATE_CCL_ACTIVED,         					fsm_ccl_dido_perip_sensor_status_rep},
	
	//FSM_STATE_CCL_CLOUD_INQUERY：等待后台回传指令（可能是正常开门触发的，也可能是关门超时，ACTIVE状态下EVENT定时查询报告，或者FATAL_FAULT触发）
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_CLOUD_INQUERY,         		fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_CLOUD_INQUERY,         		fsm_ccl_time_out},
	{MSG_ID_SPS_TO_CCL_CLOUD_FB,						FSM_STATE_CCL_CLOUD_INQUERY,         		fsm_ccl_sps_cloud_fb},	
	
	//FSM_STATE_CCL_TO_OPEN_DOOR：等待人工开门，超时则关门回归ACTIVE
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_TO_OPEN_DOOR,         		fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_TO_OPEN_DOOR,         		fsm_ccl_time_out},
	{MSG_ID_DIDO_PERIPH_SENSOR_STATUS_REP,	FSM_STATE_CCL_TO_OPEN_DOOR,         		fsm_ccl_dido_perip_sensor_status_rep},	

	//FSM_STATE_CCL_DOOR_OPEN：启动定时，监控门限和门锁
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_DOOR_OPEN,         				fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_DOOR_OPEN,         				fsm_ccl_time_out},
	{MSG_ID_DIDO_PERIPH_SENSOR_STATUS_REP,	FSM_STATE_CCL_DOOR_OPEN,         				fsm_ccl_dido_perip_sensor_status_rep},		
	
	//FSM_STATE_CCL_FATAL_FAULT：严重错误状态，发送报告给后台，等待人工干预以后回归ACTIVE
  {MSG_ID_COM_RESTART,        						FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_time_out},
	{MSG_ID_DIDO_PERIPH_SENSOR_STATUS_REP,	FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_dido_perip_sensor_status_rep},
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             							NULL},  //Ending
};

//Global variables defination

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

	//串行会送INIT_FB给VM，不然消息队列不够深度，此为节省内存机制
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM。由于消息队列不够深，所以不再回送FB证实。
//		msg_struct_com_init_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
//		snd.length = sizeof(msg_struct_com_init_fb_t);
//		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_CCL, &snd, snd.length);
//		if (ret == IHU_FAILURE){
//			IhuErrorPrint("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_CCL], zIhuTaskNameList[src_id]);
//			return IHU_FAILURE;
//		}
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

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_ACTIVED) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Error Set FSM State!");
		return IHU_FAILURE;
	}
	
	//启动本地定时器，如果有必要
	ret = ihu_timer_start(TASK_ID_CCL, TIMER_ID_1S_CCL_PERIOD_SCAN, zIhuSysEngPar.timer.cclPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Error start timer!\n");
		return IHU_FAILURE;
	}
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("CCL: Enter FSM_STATE_CCL_ACTIVE status, Keeping refresh here!\n");
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

OPSTAT fsm_ccl_adc_ul_data_pull_bwd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_CCL)){
		zIhuRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Wrong input paramters!\n");
		return IHU_FAILURE;
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_ccl_adc_ul_ctrl_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_CCL)){
		zIhuRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Wrong input paramters!\n");
		return IHU_FAILURE;
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_ccl_spi_ul_data_pull_bwd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_CCL)){
		zIhuRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Wrong input paramters!\n");
		return IHU_FAILURE;
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_ccl_spi_ul_ctrl_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_CCL)){
		zIhuRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Wrong input paramters!\n");
		return IHU_FAILURE;
	}

	//返回
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

	//超长定时超时Period time out received
	if ((rcv.timeId == TIMER_ID_1S_CCL_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//启动硬件扫描定时器
		ret = ihu_timer_start(TASK_ID_CCL, TIMER_ID_1S_CCL_WORKING_STATUS_SCAN, zIhuSysEngPar.timer.cclWorkingStatusScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_CCL]++;
			IhuErrorPrint("CCL: Error start timer!\n");
			return IHU_FAILURE;
		}
		func_ccl_time_out_period_event_report();
	}
	
	//周期性工作状态
	if ((rcv.timeId == TIMER_ID_1S_CCL_WORKING_STATUS_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_CCL) != FSM_STATE_CCL_ACTIVED){
			ret = FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_CCL]++;
				IhuErrorPrint("CCL: Error Set FSM State!\n");
				return IHU_FAILURE;
			}//FsmSetState
		}
		func_ccl_time_out_working_status_scan();
	}	
	
	

	return IHU_SUCCESS;
}

//L2传感器送来的状态报告
OPSTAT fsm_ccl_dido_perip_sensor_status_rep(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//int ret = 0;
	int state = 0;
	msg_struct_didocap_periph_sensor_status_rep_t rcv;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_didocap_periph_sensor_status_rep_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_didocap_periph_sensor_status_rep_t))){
		IhuErrorPrint("CCL: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_CCL]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//获取状态
	state = FsmGetState(TASK_ID_CCL);
	switch(state)
	{
		//回送后台周期报告
		case 	FSM_STATE_CCL_ACTIVED:
			break;
				
		//等待超时，或者门限开
		case 	FSM_STATE_CCL_TO_OPEN_DOOR:
			//门限开，进入DOOR_OPEN状态
			//超时，则发送关门命令，进入ACTIVE状态
			break;
		
		//等待超时，或者门限关
		case 	FSM_STATE_CCL_DOOR_OPEN:
			//门限关，锁关，则进入ACTIVE状态
			//超时，则进入FATAL_FAULT状态		
			break;
		
		//出现问题，意味着不正常状态
		case 	FSM_STATE_CCL_FATAL_FAULT:
			//声光告警，并送告警给后台
			//监控门限和门锁，一旦恢复，则送清除告警给后台，并进入ACTIVE状态
			break;
		
		//不用理睬，报错处理，强行回到ACTIVE状态
		default:
			break;
		
		//注意，需要使用门开最长时间的超时定时器，以及后台反馈定时器，这两个定时器来控制状态
		//这两个定时器的启动和停止，需要想好逻辑
		//目前的技术定时器，TIMER_ID_1S_CCL_WORKING_STATUS_SCAN，需要改造		
	}
	//返回
	return IHU_SUCCESS;
}

//超长时间，触发周期性汇报
void func_ccl_time_out_period_event_report(void)
{
	//发送状态查询请求给DIDO模块
	//MSG_ID_CCL_TO_DH_SENSOR_SCAN

	//状态机不转移
}

//工作状态下的短定时扫描，可以当做计数器来使用
void func_ccl_time_out_working_status_scan(void)
{

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
	
	//发送开门指令到DIDO模块
	//MSG_ID_CCL_TO_DIDO_CTRL_CMD
	
	//状态转移

	//返回
	return IHU_SUCCESS;
}


