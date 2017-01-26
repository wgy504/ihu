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
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            							fsm_bfsc_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            								fsm_bfsc_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            								fsm_bfsc_restart},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_BFSC_INITED,         						fsm_bfsc_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_BFSC_INITED,         						fsm_bfsc_stop_rcv},

	//Task level actived status：等待初始化指令的到来，然后进行初始化，完成后进入SCAN工作状态
  {MSG_ID_COM_RESTART,        						FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_BFSC_ACTIVED,         				  fsm_bfsc_time_out},
	{MSG_ID_CAN_L3BFSC_CMD_CTRL,						FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_canvela_cmd_ctrl},  //参数配置设置
	{MSG_ID_ADC_L3BFSC_MEAS_CMD_RESP,				FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_adc_meas_cmd_resp}, //参数配置反馈
	{MSG_ID_I2C_L3BFSC_MOTO_CMD_RESP,				FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_i2c_moto_cmd_resp}, //参数配置反馈
	{MSG_ID_CAN_L3BFSC_INIT_REQ,						FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_canvela_init_req},  //初始化完成
	
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
	{MSG_ID_ADC_NEW_MATERIAL_WS,						FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_adc_new_material_ws}, //新的称重结果，此时也是允许上报的
	{MSG_ID_CAN_L3BFSC_ROLL_OUT_REQ,				FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_canvela_roll_out_req},//正常出料
	{MSG_ID_CAN_L3BFSC_GIVE_UP_REQ,					FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_canvela_give_up_req}, //抛弃物料
	{MSG_ID_ADC_MATERIAL_DROP,							FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_adc_material_drop},   //物料失重被拿走
	
	//出料输出等待状态：完成后进入SCAN状态。如果连续N次都未能成功，停止马达和称重传感器，并进入ERROR_TRAP状态。
  {MSG_ID_COM_RESTART,        						FSM_STATE_BFSC_ROLL_OUT,         					fsm_bfsc_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_BFSC_ROLL_OUT,         					fsm_bfsc_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_BFSC_ROLL_OUT,         					fsm_bfsc_time_out},
	{MSG_ID_ADC_MATERIAL_DROP,							FSM_STATE_BFSC_ROLL_OUT,         				  fsm_bfsc_adc_material_drop},   //出料完成

	//放弃物料输出等待状态：完成后进入SCAN状态。如果连续N次都未能成功，停止马达和称重传感器，并进入ERROR_TRAP状态。
  {MSG_ID_COM_RESTART,        						FSM_STATE_BFSC_GIVE_UP,         					fsm_bfsc_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_BFSC_GIVE_UP,         					fsm_bfsc_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_BFSC_GIVE_UP,         					fsm_bfsc_time_out},
	{MSG_ID_ADC_MATERIAL_DROP,							FSM_STATE_BFSC_GIVE_UP,         				  fsm_bfsc_adc_material_drop},   //放弃物料完成

	//硬件出错，该传感器进入错误陷阱状态，不再工作，等待人工干预
  {MSG_ID_COM_RESTART,        						FSM_STATE_BFSC_ERROR_TRAP,         				fsm_bfsc_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_BFSC_ERROR_TRAP,         				fsm_bfsc_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_BFSC_ERROR_TRAP,         				fsm_bfsc_time_out},

  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             								NULL},  //Ending
};

//Global variables defination
INT32 zIhuL3bfscLatestMeasureWeightValue = 0;
UINT32 zIhuL3bfscMotoRecoverTimes = 0;

//MYC Global Varible for WMC
WmcInventory_t										zWmcInvenory;
CombinationAlgorithmParamaters_t 	zCombAlgoParam;
WeightSensorParamaters_t					zWeightSensorParam;
MotorControlParamaters_t 					zMotorControlParam;

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
	int ret=0;

	//串行回送INIT_FB给VMFO
	ihu_usleep(dest_id * IHU_MODULE_START_DISTRIBUTION_DELAY_DURATION);
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VMFO
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_BFSC, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[src_id].taskName);
			return IHU_FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_INITED) == IHU_FAILURE){
		IhuErrorPrint("L3BFSC: Error Set FSM State!");	
		return IHU_FAILURE;
	}
	
	//MYC Initialize globale variables:
	memset(&zWmcInvenory, 0, sizeof(WmcInventory_t));
	IhuDebugPrint("L3BFSC: fsm_bfsc_init: zWmcInvenory set 0, size = %d bytes\n", sizeof(WmcInventory_t));	
	memset(&zCombAlgoParam, 0, sizeof(CombinationAlgorithmParamaters_t));
	IhuDebugPrint("L3BFSC: fsm_bfsc_init: zCombAlgoParam set 0, size = %d bytes\n", sizeof(CombinationAlgorithmParamaters_t));	
	memset(&zWeightSensorParam, 0, sizeof(WeightSensorParamaters_t));
	IhuDebugPrint("L3BFSC: fsm_bfsc_init: zWeightSensorParam set 0, size = %d bytes\n", sizeof(WeightSensorParamaters_t));	
	memset(&zMotorControlParam, 0, sizeof(MotorControlParamaters_t));
	IhuDebugPrint("L3BFSC: fsm_bfsc_init: zMotorControlParam set 0, size = %d bytes\n", sizeof(MotorControlParamaters_t));

	//初始化硬件接口
	if (func_bfsc_hw_init(&zWmcInvenory) == IHU_FAILURE){	
		IhuErrorPrint("L3BFSC: Error initialize interface!");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_BFSC] = 0;
	zIhuL3bfscLatestMeasureWeightValue = 0;
	zIhuL3bfscMotoRecoverTimes = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_ACTIVED) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Error Set FSM State!");
		return IHU_FAILURE;
	}
	
	//启动本地定时器，如果有必要	
	ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_PERIOD_SCAN, zIhuSysEngPar.timer.bfscPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Error start timer!\n");
		return IHU_FAILURE;
	}
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("L3BFSC: Enter FSM_STATE_BFSC_ACTIVE status, Keeping refresh here!\n");
	}

	//MYC add the first message to AWS
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VMFO
		msg_struct_l3bfsc_wmc_startup_ind_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfsc_wmc_startup_ind_t));
		snd.length = sizeof(msg_struct_l3bfsc_wmc_startup_ind_t);
		snd.msgid = MSG_ID_L3BFSC_WMC_STARTUP_IND;
		snd.wmc_state = FSM_STATE_BFSC_ACTIVED;
		memcpy(&snd.wmc_inventory, &zWmcInvenory, sizeof(WmcInventory_t));
		
		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_STARTUP_IND, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC], zIhuTaskInfo[TASK_ID_CANVELA]);
			return IHU_FAILURE;
		}
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
//MYC
UINT32 GetWmcId()
{
	/* PA5,PA6,PA7,PA8 => SW0,1,2,3 */
	GPIO_PinState ps;
	UINT32 wmc_id = 0;
	
	/* SW0 <= PA5 */
	ps = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);
	if (GPIO_PIN_RESET == ps)
	{
		wmc_id = wmc_id & 0xFFFFFFFE;  //1110
	}
	else
	{
		wmc_id = wmc_id | 0x00000001;  //0001
	}
	
	/* SW1 <= PA6 */
	ps = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);
	if (GPIO_PIN_RESET == ps)
	{
		wmc_id = wmc_id & 0xFFFFFFFD;  //1101
	}
	else
	{
		wmc_id = wmc_id | 0x00000002;  //0010
	}
	
	/* SW2 <= PA7 */
	ps = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);
	if (GPIO_PIN_RESET == ps)
	{
		wmc_id = wmc_id & 0xFFFFFFFB;  //1011
	}
	else
	{
		wmc_id = wmc_id | 0x00000004;  //0100
	}
	
	/* SW3 <= PA8 */
	ps = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);
	if (GPIO_PIN_RESET == ps)
	{
		wmc_id = wmc_id & 0xFFFFFFF7;  //0111
	}
	else
	{
		wmc_id = wmc_id | 0x00000008;  //1000
	}
	
	/* make sure only the last 4 digit values */
	wmc_id = wmc_id & 0xF;
	return wmc_id;
}
//MYC
OPSTAT func_bfsc_hw_init(WmcInventory_t *pwi)
{
	if(NULL == pwi)
	{
		IhuErrorPrint("L3BFSC: func_bfsc_hw_init: input parameter pwi == BULL");
		return IHU_FAILURE;
	}
	
	//MYC TODO
	pwi->hw_inventory_id = 0x0;
	pwi->sw_inventory_id = 0x1;
	
	//MYC Get CPU ID for STM32
	ihu_bsp_stm32_get_cpuid_f2board(&pwi->stm32_cpu_id);
	IhuDebugPrint("L3BFSC: func_bfsc_hw_init: stm32_cpu_id = [0x%08X]\n", pwi->stm32_cpu_id);
	
	//MYC TODO
	pwi->weight_sensor_type = 0x2;
	pwi->motor_type = 0x03;
	pwi->wmc_id = GetWmcId();               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	IhuDebugPrint("L3BFSC: func_bfsc_hw_init: wmc_id = [0x%01X]\n", pwi->wmc_id);
	
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
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_BFSC].taskName);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_BFSC_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_bfsc_time_out_period_scan();
	}

	else if ((rcv.timeId == TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_bfsc_time_out_wait_weight_command_process();
	}
	
	else if ((rcv.timeId == TIMER_ID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_bfsc_time_out_roll_out_process();
	}
	
	else if ((rcv.timeId == TIMER_ID_1S_BFSC_L3BFSC_GIVE_UP_TIMER) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_bfsc_time_out_give_up_process();
	}
			
	else{
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Receive error timer ID!\n");
		return IHU_FAILURE;
	}
	
	return IHU_SUCCESS;
}

//暂时无用，而只是为了回归状态的定时器
OPSTAT func_bfsc_time_out_period_scan(void)
{
	int ret = 0;
	
	//发送HeartBeat消息给VMFO模块，实现喂狗功能
	msg_struct_com_heart_beat_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_heart_beat_t));
	snd.length = sizeof(msg_struct_com_heart_beat_t);
	ret = ihu_message_send(MSG_ID_COM_HEART_BEAT, TASK_ID_VMFO, TASK_ID_BFSC, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_VMFO].taskName);
		return IHU_FAILURE;
	}

	return IHU_SUCCESS;	
}

OPSTAT func_bfsc_time_out_wait_weight_command_process(void)
{
	int ret = 0;
	msg_struct_l3bfsc_canvela_new_ws_event_t snd;
	
	//重发数据
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t));
	snd.wsValue = zIhuL3bfscLatestMeasureWeightValue;
	snd.length = sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t);
	ret = ihu_message_send(MSG_ID_L3BFSC_CAN_NEW_WS_EVENT, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_CANVELA].taskName);
		return IHU_FAILURE;
	}
	
	//启动定时器
	ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER, zIhuSysEngPar.timer.bfscL3bfscWaitWeightTimer, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Error start timer!\n");
		return IHU_FAILURE;
	}
	
	return IHU_SUCCESS;	
}

//[组合]出料TIME OUT处理
OPSTAT func_bfsc_time_out_roll_out_process(void)
{
	int ret = 0;
	msg_struct_l3bfsc_canvela_error_status_report_t snd;
	msg_struct_l3bfsc_adc_ws_cmd_ctrl_t snd1;
	msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t snd2;	
	
	//发送错误报告给上位机
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_error_status_report_t));
	snd.length = sizeof(msg_struct_l3bfsc_canvela_error_status_report_t);
	ret = ihu_message_send(MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_CANVELA].taskName);
		return IHU_FAILURE;
	}

	//出错次数统计
	zIhuL3bfscMotoRecoverTimes++;
	
	if (zIhuL3bfscMotoRecoverTimes < IHU_L3BFSC_MOTO_HW_ERROR_RECOVER_TIMES_MAX)
	{
		//继续启动定时器，并打开MOTO命令
		
		//重启定时器
		ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER, zIhuSysEngPar.timer.bfscL3bfscRolloutTimer, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Error start timer!\n");
			return IHU_FAILURE;
		}
	}
	else
	{
		//状态强行转移到ERROR_TRAP模式，等待人工恢复
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_ERROR_TRAP) == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Error Set FSM State!");	
			return IHU_FAILURE;
		}	
		//发送命令给ADC，停止测量工作
		memset(&snd1, 0, sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t));
		snd1.cmdid = IHU_BFSC_ADC_WS_CMD_TYPE_STOP;
		snd1.length = sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t);
		ret = ihu_message_send(MSG_ID_L3BFSC_ADC_WS_CMD_CTRL, TASK_ID_ADCLIBRA, TASK_ID_BFSC, &snd1, snd1.length);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_ADCLIBRA].taskName);
			return IHU_FAILURE;
		}
		//发送命令给I2C-MOTO，停止测量工作
		memset(&snd2, 0, sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t));
		snd2.cmdid = IHU_BFSC_I2C_MOTO_CMD_TYPE_STOP;
		snd2.length = sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t);
		ret = ihu_message_send(MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL, TASK_ID_I2CARIES, TASK_ID_BFSC, &snd2, snd2.length);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_I2CARIES].taskName);
			return IHU_FAILURE;
		}
	}
	
	//返回
	return IHU_SUCCESS;	
}

//[放弃]出料TIME OUT处理
OPSTAT func_bfsc_time_out_give_up_process(void)
{
	int ret = 0;
	msg_struct_l3bfsc_canvela_error_status_report_t snd;
	msg_struct_l3bfsc_adc_ws_cmd_ctrl_t snd1;
	msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t snd2;		
	
	//发送错误报告给上位机
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_error_status_report_t));
	snd.length = sizeof(msg_struct_l3bfsc_canvela_error_status_report_t);
	ret = ihu_message_send(MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_CANVELA].taskName);
		return IHU_FAILURE;
	}

	//出错次数统计
	zIhuL3bfscMotoRecoverTimes++;
	
	if (zIhuL3bfscMotoRecoverTimes < IHU_L3BFSC_MOTO_HW_ERROR_RECOVER_TIMES_MAX)
	{
		//继续启动定时器，并打开MOTO命令
		
		//重启定时器
		ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_GIVE_UP_TIMER, zIhuSysEngPar.timer.bfscL3bfscGiveupTimer, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Error start timer!\n");
			return IHU_FAILURE;
		}
	}
	else
	{
		//状态强行转移到ERROR_TRAP模式，等待人工恢复
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_ERROR_TRAP) == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Error Set FSM State!");	
			return IHU_FAILURE;
		}	
		//发送命令给ADC，停止测量工作
		memset(&snd1, 0, sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t));
		snd1.length = sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t);
		snd1.cmdid = IHU_BFSC_ADC_WS_CMD_TYPE_STOP;
		ret = ihu_message_send(MSG_ID_L3BFSC_ADC_WS_CMD_CTRL, TASK_ID_ADCLIBRA, TASK_ID_BFSC, &snd1, snd1.length);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_ADCLIBRA].taskName);
			return IHU_FAILURE;
		}
		//发送命令给I2C-MOTO，停止测量工作
		memset(&snd2, 0, sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t));
		snd2.cmdid = IHU_BFSC_I2C_MOTO_CMD_TYPE_STOP;
		snd2.length = sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t);
		ret = ihu_message_send(MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL, TASK_ID_I2CARIES, TASK_ID_BFSC, &snd2, snd2.length);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_I2CARIES].taskName);
			return IHU_FAILURE;
		}
	}
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_CAN_L3BFSC_CMD_CTRL以后的处理过程
OPSTAT fsm_bfsc_canvela_cmd_ctrl(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_canvela_l3bfsc_cmd_ctrl_t rcv;
	msg_struct_l3bfsc_canvela_cmd_resp_t snd;
	msg_struct_l3bfsc_adc_ws_cmd_ctrl_t snd1;
	msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t snd2;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_canvela_l3bfsc_cmd_ctrl_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_l3bfsc_cmd_ctrl_t))){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Receive message error!\n");
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if ((rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_ws_ctrl) && (rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_motor_ctrl))
	{
		IHU_ERROR_PRINT_BFSC("L3BFSC: Receive message error!\n");
		return IHU_FAILURE;	
	}

	//如果是单独的Node Control
	if (rcv.cmd.prefixcmdid == IHU_CANVELA_PREFIXH_node_ctrl)
	{
		//先干活，干什么待定
		
		//然后反馈给CAN模块
		memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t));
		snd.cmdid = IHU_BFSC_ADC_WS_CMD_TYPE_CTRL;
		snd.cmd.optid = rcv.cmd.optid;
		snd.cmd.optpar = rcv.cmd.optpar;
		snd.cmd.modbusVal = rcv.cmd.modbusVal;
		snd.length = sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t);
		ret = ihu_message_send(MSG_ID_L3BFSC_CAN_CMD_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_CANVELA].taskName);
			return IHU_FAILURE;
		}	
	}
	
	//发送命令给ADC
	if (rcv.cmd.prefixcmdid == IHU_CANVELA_PREFIXH_ws_ctrl)
	{		
		memset(&snd1, 0, sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t));
		snd1.cmdid = IHU_BFSC_ADC_WS_CMD_TYPE_CTRL;
		snd1.cmd.optid = rcv.cmd.optid;
		snd1.cmd.optpar = rcv.cmd.optpar;
		snd1.cmd.modbusVal = rcv.cmd.modbusVal;
		snd1.length = sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t);
		ret = ihu_message_send(MSG_ID_L3BFSC_ADC_WS_CMD_CTRL, TASK_ID_ADCLIBRA, TASK_ID_BFSC, &snd1, snd1.length);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_ADCLIBRA].taskName);
			return IHU_FAILURE;
		}
	}
	
	//发送命令给I2C-MOTO
	if (rcv.cmd.prefixcmdid == IHU_CANVELA_PREFIXH_motor_ctrl)
	{		
		memset(&snd2, 0, sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t));		
		snd2.cmdid = IHU_BFSC_I2C_MOTO_CMD_TYPE_CTRL;
		snd2.cmd.optid = rcv.cmd.optid;
		snd2.cmd.optpar = rcv.cmd.optpar;
		snd2.cmd.modbusVal = rcv.cmd.modbusVal;
		snd2.length = sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t);
		ret = ihu_message_send(MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL, TASK_ID_I2CARIES, TASK_ID_BFSC, &snd2, snd2.length);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_I2CARIES].taskName);
			return IHU_FAILURE;
		}
	}
	
	//返回
	return IHU_SUCCESS;
}

//MSG_ID_ADC_L3BFSC_MEAS_CMD_RESP
OPSTAT fsm_bfsc_adc_meas_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_adclibra_l3bfsc_meas_cmd_resp_t rcv;
	msg_struct_l3bfsc_canvela_cmd_resp_t snd;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_adclibra_l3bfsc_meas_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_adclibra_l3bfsc_meas_cmd_resp_t))){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Receive message error!\n");
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查：暂时不处理IHU_ADC_BFSC_WS_CMD_TYPE_RESP之外其它的类型，未来待完善
	if ((rcv.cmdid != IHU_ADC_BFSC_WS_CMD_TYPE_RESP) || (rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_ws_resp))
	{
		IHU_ERROR_PRINT_BFSC("L3BFSC: Receive message error!\n");
		return IHU_FAILURE;
	}	
	
	//发送消息给CANVELA
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t));
	snd.cmdid = IHU_BFSC_CAN_CMD_TYPE_RESP;
	snd.cmd.prefixcmdid = IHU_CANVELA_PREFIXH_ws_resp;
	snd.cmd.optid = rcv.cmd.optid;
	snd.cmd.optpar = rcv.cmd.optpar;
	snd.cmd.modbusVal = rcv.cmd.modbusVal;
	snd.length = sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t);
	ret = ihu_message_send(MSG_ID_L3BFSC_CAN_CMD_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_CANVELA].taskName);
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//MSG_ID_I2C_L3BFSC_MOTO_CMD_RESP
OPSTAT fsm_bfsc_i2c_moto_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_i2caries_l3bfsc_cmd_resp_t rcv;
	msg_struct_l3bfsc_canvela_cmd_resp_t snd;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_i2caries_l3bfsc_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_i2caries_l3bfsc_cmd_resp_t))){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Receive message error!\n");
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查：暂时不处理IHU_I2C_BFSC_WS_CMD_TYPE_RESP之外其它的类型，未来待完善
	if ((rcv.cmdid != IHU_I2C_BFSC_WS_CMD_TYPE_RESP) || (rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_motor_resp))
	{
		IHU_ERROR_PRINT_BFSC("L3BFSC: Receive message error!\n");
		return IHU_FAILURE;
	}	
	
	//发送消息给CANVELA
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t));
	snd.cmdid = IHU_BFSC_CAN_CMD_TYPE_RESP;
	snd.cmd.prefixcmdid = IHU_CANVELA_PREFIXH_motor_resp;
	snd.cmd.optid = rcv.cmd.optid;
	snd.cmd.optpar = rcv.cmd.optpar;
	snd.cmd.modbusVal = rcv.cmd.modbusVal;
	snd.length = sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t);
	ret = ihu_message_send(MSG_ID_L3BFSC_CAN_CMD_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_CANVELA].taskName);
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_CAN_L3BFSC_INIT_REQ以后的处理过程
OPSTAT fsm_bfsc_canvela_init_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_canvela_l3bfsc_init_req_t rcv;
	msg_struct_l3bfsc_canvela_init_resp_t snd;
	msg_struct_l3bfsc_adc_ws_cmd_ctrl_t snd1;
	msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t snd2;			
	
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_canvela_l3bfsc_init_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_l3bfsc_init_req_t))){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Receive message error!\n");
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//处理消息：命令并指令WS、MOTO开始干活
	//发送命令给ADC，开始测量工作
	memset(&snd1, 0, sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t));
	snd1.length = sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t);
	snd1.cmdid = IHU_BFSC_ADC_WS_CMD_TYPE_START;
	ret = ihu_message_send(MSG_ID_L3BFSC_ADC_WS_CMD_CTRL, TASK_ID_ADCLIBRA, TASK_ID_BFSC, &snd1, snd1.length);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_ADCLIBRA].taskName);
		return IHU_FAILURE;
	}
	//发送命令给I2C-MOTO，开始测量工作
	memset(&snd2, 0, sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t));
	snd2.cmdid = IHU_BFSC_I2C_MOTO_CMD_TYPE_START;
	snd2.length = sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t);
	ret = ihu_message_send(MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL, TASK_ID_I2CARIES, TASK_ID_BFSC, &snd2, snd2.length);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_I2CARIES].taskName);
		return IHU_FAILURE;
	}
	
	//发送反馈消息出去
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_init_resp_t));
	snd.length = sizeof(msg_struct_l3bfsc_canvela_init_resp_t);
	ret = ihu_message_send(MSG_ID_L3BFSC_CAN_INIT_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_CANVELA].taskName);
		return IHU_FAILURE;
	}
	
	//状态转移
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN) == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Error Set FSM State!");	
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
	//if ((param_ptr == NULL || param_len > sizeof(msg_struct_adc_new_material_ws_t)) || (rcv.wsValue == 0)){
	if (param_ptr == NULL || param_len > sizeof(msg_struct_adc_new_material_ws_t)){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Receive message error!\n");
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//处理消息，存储最后收到的新测量值，以便定时超时后再复送
	zIhuL3bfscLatestMeasureWeightValue = rcv.wsValue;
	
	//停止定时器：没有定时器可以停止
	
	//发送消息出去
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t));
	snd.wsValue = rcv.wsValue;
	snd.length = sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t);
	ret = ihu_message_send(MSG_ID_L3BFSC_CAN_NEW_WS_EVENT, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_CANVELA].taskName);
		return IHU_FAILURE;
	}
	
	//启动定时器
	ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER, zIhuSysEngPar.timer.bfscL3bfscWaitWeightTimer, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Error start timer!\n");
		return IHU_FAILURE;
	}	
	
	//状态转移
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_WEIGHT_REPORT) == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Error Set FSM State!");	
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
		IHU_ERROR_PRINT_BFSC("L3BFSC: Receive message error!\n");
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//处理消息
	//在称重条件下，直接返回SCAN状态，没有任何处理过程
	if (FsmGetState(TASK_ID_BFSC) == FSM_STATE_BFSC_WEIGHT_REPORT){
		//发送状态命令给上位机，表示这个空闲了	
		memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t));
		snd.length = sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t);
		ret = ihu_message_send(MSG_ID_L3BFSC_CAN_NEW_WS_EVENT, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_CANVELA].taskName);
			return IHU_FAILURE;
		}		
		//状态转移
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN) == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Error Set FSM State!");	
			return IHU_FAILURE;
		}
	}
	
	//在[组合]出料条件下
	else if (FsmGetState(TASK_ID_BFSC) == FSM_STATE_BFSC_ROLL_OUT){
		//停止定时器
		ret = ihu_timer_stop(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Error stop timer!\n");
			return IHU_FAILURE;
		}
		
		//发送消息出去
		memset(&snd1, 0, sizeof(msg_struct_l3bfsc_canvela_roll_out_resp_t));
		snd1.length = sizeof(msg_struct_l3bfsc_canvela_roll_out_resp_t);
		ret = ihu_message_send(MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd1, snd1.length);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_CANVELA].taskName);
			return IHU_FAILURE;
		}	
		
		//状态转移
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN) == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Error Set FSM State!");	
			return IHU_FAILURE;
		}
	}

	//在[放弃]出料条件下
	else if (FsmGetState(TASK_ID_BFSC) == FSM_STATE_BFSC_GIVE_UP){
		//停止定时器
		ret = ihu_timer_stop(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_GIVE_UP_TIMER, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Error stop timer!\n");
			return IHU_FAILURE;
		}
		
		//发送消息出去
		memset(&snd2, 0, sizeof(msg_struct_l3bfsc_canvela_give_up_resp_t));
		snd2.length = sizeof(msg_struct_l3bfsc_canvela_give_up_resp_t);
		ret = ihu_message_send(MSG_ID_L3BFSC_CAN_GIVE_UP_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd2, snd2.length);
		if (ret == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC].taskName, zIhuTaskInfo[TASK_ID_CANVELA].taskName);
			return IHU_FAILURE;
		}	
		
		//状态转移
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN) == IHU_FAILURE){
			IHU_ERROR_PRINT_BFSC("L3BFSC: Error Set FSM State!");	
			return IHU_FAILURE;
		}
	}
	
	else{
		IHU_ERROR_PRINT_BFSC("L3BFSC: Receive command at wrong status!");	
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_CAN_L3BFSC_ROLL_OUT_REQ以后的处理过程
OPSTAT fsm_bfsc_canvela_roll_out_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_canvela_l3bfsc_roll_out_req_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_canvela_l3bfsc_roll_out_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_l3bfsc_roll_out_req_t))){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Receive message error!\n");
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//停止定时器
	ret = ihu_timer_stop(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Error stop timer!\n");
		return IHU_FAILURE;
	}
	
	//发送马达运转的指令
	//等待出料完成
	//如果差错，需要送出差错消息给上位机
	//处理消息：发送质量给马达，进行出料处理
	
	//启动定时器
	ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER, zIhuSysEngPar.timer.bfscL3bfscRolloutTimer, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Error start timer!\n");
		return IHU_FAILURE;
	}	
	
	//状态转移
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_ROLL_OUT) == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Error Set FSM State!");	
		return IHU_FAILURE;
	}	
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_CAN_L3BFSC_GIVE_UP_REQ以后的处理过程
OPSTAT fsm_bfsc_canvela_give_up_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_canvela_l3bfsc_give_up_req_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_canvela_l3bfsc_give_up_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_l3bfsc_give_up_req_t))){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Receive message error!\n");
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//停止定时器
	ret = ihu_timer_stop(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Error stop timer!\n");
		return IHU_FAILURE;
	}
	
	//发送马达运转的指令
	//等待出料完成
	//如果差错，需要送出差错消息给上位机
	//处理消息：发送质量给马达，进行出料处理
	
	//启动定时器
	ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_GIVE_UP_TIMER, zIhuSysEngPar.timer.bfscL3bfscGiveupTimer, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Error start timer!\n");
		return IHU_FAILURE;
	}
	
	//状态转移
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_GIVE_UP) == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC("L3BFSC: Error Set FSM State!");	
		return IHU_FAILURE;
	}	
	//返回
	return IHU_SUCCESS;
}

//由于错误，直接从差错中转入扫描状态，确保业务一直处理工作状态
void func_bfsc_stm_main_recovery_from_fault(void)
{	
	msg_struct_l3bfsc_adc_ws_cmd_ctrl_t snd1;
	msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t snd2;

	//将传感器情形归化到工作状态	
	//发送命令给ADC，开始测量工作
	memset(&snd1, 0, sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t));
	snd1.length = sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t);
	snd1.cmdid = IHU_BFSC_ADC_WS_CMD_TYPE_START;
	ihu_message_send(MSG_ID_L3BFSC_ADC_WS_CMD_CTRL, TASK_ID_ADCLIBRA, TASK_ID_BFSC, &snd1, snd1.length);
	//发送命令给I2C-MOTO，开始测量工作
	memset(&snd2, 0, sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t));
	snd2.cmdid = IHU_BFSC_I2C_MOTO_CMD_TYPE_START;
	snd2.length = sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t);
	ihu_message_send(MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL, TASK_ID_I2CARIES, TASK_ID_BFSC, &snd2, snd2.length);
	
	//状态转移到SCAN状态
	FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN);
	
	//初始化模块的任务资源
	//初始化定时器：暂时决定不做，除非该模块重新RESTART
	//初始化模块级全局变量：暂时决定不做，除非该模块重新RESTART
	
	return;
}






