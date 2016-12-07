/**
 ****************************************************************************************
 *
 * @file l3bfsc.c
 *
 * @brief L3 BFSC
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
 #include "l3bfsc.h"
 
/*
** FSM of the BFSC
*/
FsmStateItem_t FsmBfsc[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_bfsc_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_bfsc_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_bfsc_restart},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_BFSC_INITED,         						fsm_bfsc_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_BFSC_INITED,         						fsm_bfsc_stop_rcv},

	//Task level actived status：等待初始化指令的到来，然后进行初始化，完成后进入SCAN工作状态
  {MSG_ID_COM_RESTART,        						FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_BFSC_ACTIVED,         				  fsm_bfsc_time_out},
	{MSG_ID_ADC_UL_CTRL_CMD_RESP,						FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_adc_ul_ctrl_cmd_resp},
	{MSG_ID_SPI_UL_DATA_PULL_BWD,						FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_spi_ul_data_pull_bwd},
	{MSG_ID_CAN_L3BFSC_INIT_REQ,						FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_canvela_init_req},
	

	//扫描模式工作状态：等待ADC上报合法的称重结果
  {MSG_ID_COM_RESTART,        						FSM_STATE_BFSC_SCAN,         							fsm_bfsc_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_BFSC_SCAN,         							fsm_bfsc_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_BFSC_SCAN,         				  		fsm_bfsc_time_out},
	{MSG_ID_ADC_NEW_MATERIAL_WS,						FSM_STATE_BFSC_SCAN,         				  		fsm_bfsc_adc_new_material_ws}, //新的称重结果
	{MSG_ID_ADC_MATERIAL_DROP,							FSM_STATE_BFSC_SCAN,         				  		fsm_bfsc_adc_material_drop},   //物料失重被拿走
	
	//称重上报工作状态：等待上层指令，收到后进入出料状态
  {MSG_ID_COM_RESTART,        						FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_time_out},
	{MSG_ID_CAN_L3BFSC_ROLL_OUT_REQ,				FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_canvela_roll_out_req},//正常出料
	{MSG_ID_CAN_L3BFSC_GIVE_UP_REQ,					FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_canvela_give_up_req}, //抛弃物料
	{MSG_ID_ADC_MATERIAL_DROP,							FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_adc_material_drop},   //物料失重被拿走
	
	//出料输出等待状态：完成后进入SCAN状态
  {MSG_ID_COM_RESTART,        						FSM_STATE_BFSC_ROLL_OUT,         					fsm_bfsc_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_BFSC_ROLL_OUT,         					fsm_bfsc_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_BFSC_ROLL_OUT,         					fsm_bfsc_time_out},
	{MSG_ID_ADC_MATERIAL_DROP,							FSM_STATE_BFSC_ROLL_OUT,         				  fsm_bfsc_adc_material_drop},   //出料完成

	//放弃物料输出等待状态：完成后进入SCAN状态
  {MSG_ID_COM_RESTART,        						FSM_STATE_BFSC_GIVE_UP,         					fsm_bfsc_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_BFSC_GIVE_UP,         					fsm_bfsc_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_BFSC_GIVE_UP,         					fsm_bfsc_time_out},
	{MSG_ID_ADC_MATERIAL_DROP,							FSM_STATE_BFSC_GIVE_UP,         				  fsm_bfsc_adc_material_drop},   //放弃物料完成

  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_bfsc_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("L3BFSC: Error Set FSM State at fsm_bfsc_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_bfsc_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret=0;

	//串行会送INIT_FB给VM，不然消息队列不够深度，此为节省内存机制
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM。由于消息队列不够深，所以不再回送FB证实。
//		msg_struct_com_init_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
//		snd.length = sizeof(msg_struct_com_init_fb_t);
//		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_BFSC, &snd, snd.length);
//		if (ret == IHU_FAILURE){
//			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_BFSC], zIhuTaskNameList[src_id]);
//			return IHU_FAILURE;
//		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_INITED) == IHU_FAILURE){
		IhuErrorPrint("L3BFSC: Error Set FSM State!");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_bfsc_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("L3BFSC: Error initialize interface!");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_BFSC] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_ACTIVED) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Error Set FSM State!");
		return IHU_FAILURE;
	}
	
	//启动本地定时器，如果有必要
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("L3BFSC: Enter FSM_STATE_BFSC_ACTIVE status, Keeping refresh here!\n");
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_bfsc_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("L3BFSC: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_bfsc_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_bfsc_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_BFSC)){
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Wrong input paramters!");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Error Set FSM State!");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_bfsc_hw_init(void)
{
	return IHU_SUCCESS;
}

OPSTAT fsm_bfsc_adc_ul_ctrl_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_BFSC)){
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Wrong input paramters!\n");
		return IHU_FAILURE;
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_bfsc_spi_ul_data_pull_bwd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_BFSC)){
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Wrong input paramters!\n");
		return IHU_FAILURE;
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_bfsc_spi_ul_ctrl_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_BFSC)){
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Wrong input paramters!\n");
		return IHU_FAILURE;
	}

	//返回
	return IHU_SUCCESS;
}

//TIMER_OUT Processing
OPSTAT fsm_bfsc_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("L3BFSC: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuRunErrCnt[TASK_ID_BFSC]是否超限
	if (zIhuRunErrCnt[TASK_ID_BFSC] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuRunErrCnt[TASK_ID_BFSC] = zIhuRunErrCnt[TASK_ID_BFSC] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_BFSC, TASK_ID_BFSC, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_BFSC]++;
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_BFSC], zIhuTaskNameList[TASK_ID_BFSC]);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_BFSC_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_BFSC) != FSM_STATE_BFSC_ACTIVED){
			ret = FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_BFSC]++;
				IhuErrorPrint("L3BFSC: Error Set FSM State!\n");
				return IHU_FAILURE;
			}//FsmSetState
		}
		func_bfsc_time_out_period_scan();
	}

	return IHU_SUCCESS;
}

void func_bfsc_time_out_period_scan(void)
{
	IhuDebugPrint("L3BFSC: Time Out Test!\n");
}


//收到MSG_ID_CAN_L3BFSC_INIT_REQ以后的处理过程
OPSTAT fsm_bfsc_canvela_init_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_canvela_l3bfsc_init_req_t rcv;
	msg_struct_l3bfsc_canvela_init_resp_t snd;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_canvela_l3bfsc_init_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_l3bfsc_init_req_t))){
		IhuErrorPrint("L3BFSC: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//处理消息

	//停止定时器
	
	//发送消息出去
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_init_resp_t));
	snd.length = sizeof(msg_struct_l3bfsc_canvela_init_resp_t);
	ret = ihu_message_send(MSG_ID_L3BFSC_CAN_INIT_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_BFSC], zIhuTaskNameList[TASK_ID_CANVELA]);
		return IHU_FAILURE;
	}
	
	//状态转移
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN) == IHU_FAILURE){
		IhuErrorPrint("L3BFSC: Error Set FSM State!");	
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}


//收到MSG_ID_ADC_NEW_MATERIAL_WS以后的处理过程
OPSTAT fsm_bfsc_adc_new_material_ws(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_adc_new_material_ws_t rcv;
	msg_struct_l3bfsc_canvela_new_ws_event_t snd;

	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_adc_new_material_ws_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_adc_new_material_ws_t))){
		IhuErrorPrint("L3BFSC: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//处理消息

	//停止定时器
	
	//发送消息出去
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t));
	snd.length = sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t);
	ret = ihu_message_send(MSG_ID_L3BFSC_CAN_INIT_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_BFSC], zIhuTaskNameList[TASK_ID_CANVELA]);
		return IHU_FAILURE;
	}
	
	//状态转移
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_WEIGHT_REPORT) == IHU_FAILURE){
		IhuErrorPrint("L3BFSC: Error Set FSM State!");	
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}


//收到MSG_ID_ADC_MATERIAL_DROP以后的处理过程
OPSTAT fsm_bfsc_adc_material_drop(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_adc_material_drop_t rcv;
	msg_struct_l3bfsc_canvela_new_ws_event_t snd;
	msg_struct_l3bfsc_canvela_roll_out_resp_t snd1;
	msg_struct_l3bfsc_canvela_give_up_resp_t snd2;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_adc_material_drop_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_adc_material_drop_t))){
		IhuErrorPrint("L3BFSC: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//处理消息
	//在称重条件下，直接返回SCAN状态，没有任何处理过程
	if (FsmGetState(TASK_ID_BFSC) == FSM_STATE_BFSC_WEIGHT_REPORT){
		//发送状态命令给上位机，表示这个空闲了	
		memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t));
		snd.length = sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t);
		ret = ihu_message_send(MSG_ID_L3BFSC_CAN_INIT_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_BFSC]++;
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_BFSC], zIhuTaskNameList[TASK_ID_CANVELA]);
			return IHU_FAILURE;
		}		
		//状态转移
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN) == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_BFSC]++;
			IhuErrorPrint("L3BFSC: Error Set FSM State!");	
			return IHU_FAILURE;
		}

	}
	
	//在出料条件下
	else if (FsmGetState(TASK_ID_BFSC) == FSM_STATE_BFSC_ROLL_OUT){
		//停止定时器
		
		//发送消息出去
		memset(&snd1, 0, sizeof(msg_struct_l3bfsc_canvela_roll_out_resp_t));
		snd1.length = sizeof(msg_struct_l3bfsc_canvela_roll_out_resp_t);
		ret = ihu_message_send(MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd1, snd1.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_BFSC]++;
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_BFSC], zIhuTaskNameList[TASK_ID_CANVELA]);
			return IHU_FAILURE;
		}	
		
		//状态转移
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN) == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_BFSC]++;
			IhuErrorPrint("L3BFSC: Error Set FSM State!");	
			return IHU_FAILURE;
		}
	}

	//在出料条件下
	else if (FsmGetState(TASK_ID_BFSC) == FSM_STATE_BFSC_GIVE_UP){
		//停止定时器
		
		//发送消息出去
		memset(&snd2, 0, sizeof(msg_struct_l3bfsc_canvela_give_up_resp_t));
		snd2.length = sizeof(msg_struct_l3bfsc_canvela_give_up_resp_t);
		ret = ihu_message_send(MSG_ID_L3BFSC_CAN_GIVE_UP_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd2, snd2.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_BFSC]++;
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_BFSC], zIhuTaskNameList[TASK_ID_CANVELA]);
			return IHU_FAILURE;
		}	
		
		//状态转移
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN) == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_BFSC]++;
			IhuErrorPrint("L3BFSC: Error Set FSM State!");	
			return IHU_FAILURE;
		}
	}
	
	else{
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Receive command at wrong status!");	
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_CAN_L3BFSC_ROLL_OUT_REQ以后的处理过程
OPSTAT fsm_bfsc_canvela_roll_out_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret;
	msg_struct_canvela_l3bfsc_roll_out_req_t rcv;

	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_canvela_l3bfsc_roll_out_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_l3bfsc_roll_out_req_t))){
		IhuErrorPrint("L3BFSC: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//处理消息：发送质量给马达，进行出料处理
	//启动定时器
	//等待出料完成
	//如果差错，需要送出差错消息给上位机

	//停止定时器
	
	//发送消息出去
	
	//状态转移
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_ROLL_OUT) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Error Set FSM State!");	
		return IHU_FAILURE;
	}	
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_CAN_L3BFSC_GIVE_UP_REQ以后的处理过程
OPSTAT fsm_bfsc_canvela_give_up_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret;
	msg_struct_canvela_l3bfsc_give_up_req_t rcv;

	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_canvela_l3bfsc_give_up_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_l3bfsc_give_up_req_t))){
		IhuErrorPrint("L3BFSC: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//处理消息：发送质量给马达，进行出料处理
	//启动定时器
	//等待出料完成
	//如果差错，需要送出差错消息给上位机

	//停止定时器
	
	//发送消息出去
	
	//状态转移
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_GIVE_UP) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Error Set FSM State!");	
		return IHU_FAILURE;
	}	
	//返回
	return IHU_SUCCESS;
}






