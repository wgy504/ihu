/**
 ****************************************************************************************
 *
 * @file l2adclibra.c
 *
 * @brief L2 ADCLIBRA
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
#include "l2adclibra.h"
 
/*
** FSM of the ADCLIBRA
*/
FsmStateItem_t FsmAdclibra[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_adclibra_task_entry}, //Starting

	//System level initialization, only controlled by VM
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_adclibra_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_adclibra_restart},

 //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_ADCLIBRA_INITED,         					fsm_adclibra_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_ADCLIBRA_INITED,         					fsm_adclibra_stop_rcv},

	//Task level actived status
  {MSG_ID_COM_RESTART,        						FSM_STATE_ADCLIBRA_ACTIVED,         				fsm_adclibra_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_ADCLIBRA_ACTIVED,         				fsm_adclibra_stop_rcv},
  {MSG_ID_COM_TIME_OUT,										FSM_STATE_ADCLIBRA_ACTIVED,         				fsm_adclibra_time_out},

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
  {MSG_ID_L3BFSC_ADC_WS_CMD_CTRL,					FSM_STATE_ADCLIBRA_ACTIVED,         				fsm_adclibra_l3bfsc_ws_cmd_ctrl},
	{MSG_ID_CAN_ADC_WS_MAN_SET_ZERO,				FSM_STATE_ADCLIBRA_ACTIVED,         				fsm_adclibra_canvela_ws_man_set_zero},	
#endif

  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
strIhuBfscAdcWeightPar_t zIhuAdcBfscWs;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_adclibra_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_ADCLIBRA, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("ADCLIBRA: Error Set FSM State at fsm_adclibra_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_adclibra_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//串行会送INIT_FB给VM，不然消息队列不够深度，此为节省内存机制
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM。由于消息队列不够深，所以不再回送FB证实。
//		msg_struct_com_init_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
//		snd.length = sizeof(msg_struct_com_init_fb_t);
//		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_ADCLIBRA, &snd, snd.length);
//		if (ret == IHU_FAILURE){
//			IhuErrorPrint("ADCLIBRA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_ADCLIBRA], zIhuTaskNameList[src_id]);
//			return IHU_FAILURE;
//		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_ADCLIBRA, FSM_STATE_ADCLIBRA_INITED) == IHU_FAILURE){
		IhuErrorPrint("ADCLIBRA: Error Set FSM State!\n");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_adclibra_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("ADCLIBRA: Error initialize interface!\n");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_ADCLIBRA] = 0;
	memset(&zIhuAdcBfscWs, 0, sizeof(strIhuBfscAdcWeightPar_t));

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ADCLIBRA, FSM_STATE_ADCLIBRA_ACTIVED) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
		IhuErrorPrint("ADCLIBRA: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//启动周期性定时器，进行定时扫描
	ret = ihu_timer_start(TASK_ID_ADCLIBRA, TIMER_ID_1S_ADCLIBRA_PERIOD_SCAN, zIhuSysEngPar.timer.adclibraPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
		IhuErrorPrint("ADCLIBRA: Error start timer!\n");
		return IHU_FAILURE;
	}
	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	ret = ihu_timer_start(TASK_ID_ADCLIBRA, TIMER_ID_10MS_BFSC_ADCLIBRA_SCAN_TIMER, zIhuSysEngPar.timer.bfscAdclibraScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_10MS);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
		IhuErrorPrint("ADCLIBRA: Error start timer!\n");
		return IHU_FAILURE;
	}	
#endif
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("ADCLIBRA: Enter FSM_STATE_ADCLIBRA_ACTIVE status, Keeping refresh here!\n");
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_adclibra_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("ADCLIBRA: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_adclibra_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_adclibra_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_ADCLIBRA)){
		zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
		IhuErrorPrint("ADCLIBRA: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ADCLIBRA, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
		IhuErrorPrint("ADCLIBRA: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_adclibra_hw_init(void)
{
	return IHU_SUCCESS;
}

//TIMER_OUT Processing
OPSTAT fsm_adclibra_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("ADCLIBRA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuRunErrCnt[TASK_ID_ADCLIBRA]是否超限
	if (zIhuRunErrCnt[TASK_ID_ADCLIBRA] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuRunErrCnt[TASK_ID_ADCLIBRA] = zIhuRunErrCnt[TASK_ID_ADCLIBRA] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_ADCLIBRA, TASK_ID_ADCLIBRA, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
			IhuErrorPrint("ADCLIBRA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_ADCLIBRA], zIhuTaskNameList[TASK_ID_ADCLIBRA]);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_ADCLIBRA_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_ADCLIBRA) != FSM_STATE_ADCLIBRA_ACTIVED){
			ret = FsmSetState(TASK_ID_ADCLIBRA, FSM_STATE_ADCLIBRA_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
				IhuErrorPrint("ADCLIBRA: Error Set FSM State!\n");
				return IHU_FAILURE;
			}//FsmSetState
		}
		func_adclibra_time_out_period_scan();
	}

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	else if ((rcv.timeId == TIMER_ID_10MS_BFSC_ADCLIBRA_SCAN_TIMER) &&(rcv.timeRes == TIMER_RESOLUTION_10MS)){
		if (zIhuAdcBfscWs.WeightWorkingMode == IHU_BFSC_ADC_WEIGHT_WORKING_MODE_NORMAL) func_adclibra_time_out_bfsc_read_weight_scan();
	}
#endif
	
	else{
		zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
		IhuErrorPrint("ADCLIBRA: Receive error timer ID!\n");
		return IHU_FAILURE;
	}
	
	return IHU_SUCCESS;
}

//纯测试目的
void func_adclibra_time_out_period_scan(void)
{
	IhuDebugPrint("ADCLIBRA: Period Time Out Test, do nothing!\n");
}

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
OPSTAT func_adclibra_time_out_bfsc_read_weight_scan(void)
{
	int ret = 0;
	//扫描ADC的数据，这里应该有采样，以及滤波算法
	//先生成假数据，用于极度疯狂的系统测试，暂时将采样周期定位250ms，后面甚至可以设置10ms级别，以验证系统的可靠性与稳定性
	//正常情况下，100MS的采样，已经足以够得着人工处理速度了。如果这是一条产线，按照每分钟200包、一秒钟3包的速度，300MS的一半两次采样，100MS<150MS，
	//也足以应付各种情况了，所以设置为100MS应该是理论上最好的效果了，足够了
	UINT32 tempWeight = 0;
	tempWeight = func_adclibra_bfsc_read_weight();
	
	//传感器一直是0重量
	if ((tempWeight == 0) && (zIhuAdcBfscWs.WeightExistCnt == 0))
	{
		//Do nothing
	}
	
	//传感器突然变成了0重量
	else if ((tempWeight == 0) && (zIhuAdcBfscWs.WeightExistCnt > 0))
	{
		zIhuAdcBfscWs.WeightExistCnt = 0;
		//发送MSG_ID_ADC_MATERIAL_DROP到L3BFSC
		msg_struct_adc_material_drop_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_adc_material_drop_t));
		snd1.length = sizeof(msg_struct_adc_material_drop_t);
		ret = ihu_message_send(MSG_ID_ADC_MATERIAL_DROP, TASK_ID_BFSC, TASK_ID_ADCLIBRA, &snd1, snd1.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
			IhuErrorPrint("ADCLIBRA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_ADCLIBRA], zIhuTaskNameList[TASK_ID_BFSC]);
			return IHU_FAILURE;
		}
	}
	
	//传感器有新重量：这里的误差，得有算法来控制结果，只要是重量不一样，这里的程序就得上报，不管是不是真的有变化，以确保系统反应的敏感性
	else if (tempWeight > 0)
	{
		zIhuAdcBfscWs.WeightExistCnt++;
		if (tempWeight != zIhuAdcBfscWs.Weightvalue)
		{
			//发送MSG_ID_ADC_NEW_MATERIAL_WS
			zIhuAdcBfscWs.Weightvalue = tempWeight;
			msg_struct_adc_new_material_ws_t snd2;
			memset(&snd2, 0, sizeof(msg_struct_adc_new_material_ws_t));
			snd2.length = sizeof(msg_struct_adc_new_material_ws_t);
			ret = ihu_message_send(MSG_ID_ADC_NEW_MATERIAL_WS, TASK_ID_BFSC, TASK_ID_ADCLIBRA, &snd2, snd2.length);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
				IhuErrorPrint("ADCLIBRA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_ADCLIBRA], zIhuTaskNameList[TASK_ID_BFSC]);
				return IHU_FAILURE;
			}
		}//if (tempWeight != zIhuAdcBfscSensorWeightValue)
	}
	
	//垃圾CASE，应该不能到达
	else
	{
		zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
		IhuErrorPrint("ADCLIBRA: Wrong scan result!\n");
		return IHU_FAILURE;	
	}
	
	return IHU_SUCCESS;
}

//MSG_ID_L3BFSC_ADC_WS_CMD_CTRL
OPSTAT fsm_adclibra_l3bfsc_ws_cmd_ctrl(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_l3bfsc_adc_ws_cmd_ctrl_t rcv;
	msg_struct_adclibra_l3bfsc_meas_cmd_resp_t snd;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t))){
		IhuErrorPrint("ADCLIBRA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//简单控制
	if (rcv.cmdid == IHU_BFSC_ADC_WS_CMD_TYPE_START){
		zIhuAdcBfscWs.WeightWorkingMode = IHU_BFSC_ADC_WEIGHT_WORKING_MODE_NORMAL;
	}
	//简单控制	
	else if (rcv.cmdid == IHU_BFSC_ADC_WS_CMD_TYPE_STOP){
		zIhuAdcBfscWs.WeightWorkingMode = IHU_BFSC_ADC_WEIGHT_WORKING_MODE_STOP;
	}
	//复杂控制	
	else if (rcv.cmdid == IHU_BFSC_ADC_WS_CMD_TYPE_CTRL)
	{
		//入参检查
		if (rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_ws_ctrl)
		{
			IhuErrorPrint("ADCLIBRA: Receive message error!\n");
			zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
			return IHU_FAILURE;
		}
		
		//依赖不同的控制命令，分门别类的处理
		//分别针对不同的OPTID进行帧的分类处理
		memset(&snd, 0, sizeof(msg_struct_adclibra_l3bfsc_meas_cmd_resp_t));
		switch(rcv.cmd.optid)
			{
				//重量读取
				case IHU_CANVELA_OPTID_wegith_read:					
					snd.cmd.modbusVal = func_adclibra_bfsc_read_weight();
					break;

				//（值设定及含义同modbus协议） 自动0点跟踪
				case IHU_CANVELA_OPTID_auto_zero_track: 
					//这个值的设定，将会使得每一次称重，自动清零，判定的标准是单体重量小于设定目标重量的10%
					zIhuAdcBfscWs.WeightZeroTrackMode = IHU_BFSC_ADC_WEIGHT_ZERO_TRACK_MODE_ACTIVE; 
					break;

				//（值设定及含义同modbus协议） 最小灵敏度
				case IHU_CANVELA_OPTID_min_sensitivity: 
					zIhuAdcBfscWs.WeightMinSens = rcv.cmd.modbusVal;
					break;

				//（值设定及含义同modbus协议）  手动清零
				case IHU_CANVELA_OPTID_manual_set_zero:  				
					zIhuAdcBfscWs.WeightManSetZero = func_adclibra_bfsc_read_origin();
					break;

				//（值设定及含义同modbus协议） 静止检测范围
				case IHU_CANVELA_OPTID_static_detect_range: 					
					zIhuAdcBfscWs.WeightStaticRange = rcv.cmd.modbusVal;
					break;

				//（值设定及含义同modbus协议） 静止检测时间
				case IHU_CANVELA_OPTID_static_detect_duration:
					zIhuAdcBfscWs.WeightStaticDur = rcv.cmd.modbusVal;		
					break;

				//称量校准
				case IHU_CANVELA_OPTID_weight_scale_calibration:
					if (rcv.cmd.optpar == IHU_CANVELA_OPTPAR_weight_scale_calibration_0)
					{
						zIhuAdcBfscWs.WeightWorkingMode = IHU_BFSC_ADC_WEIGHT_WORKING_MODE_CAL;
						zIhuAdcBfscWs.WeightCal0Kg = func_adclibra_bfsc_read_origin();
					}
					else if (rcv.cmd.optpar == IHU_CANVELA_OPTPAR_weight_scale_calibration_1kg)
					{
						zIhuAdcBfscWs.WeightWorkingMode = IHU_BFSC_ADC_WEIGHT_WORKING_MODE_CAL;						
						zIhuAdcBfscWs.WeightCal1Kg = func_adclibra_bfsc_read_origin();
					}
					else if (rcv.cmd.optpar == IHU_CANVELA_OPTPAR_weight_scale_calibration_exit)
					{
						zIhuAdcBfscWs.WeightWorkingMode = IHU_BFSC_ADC_WEIGHT_WORKING_MODE_NORMAL;
					}
					else
					{
						IhuErrorPrint("ADCLIBRA: Receive message error!\n");
						zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
						return IHU_FAILURE;
					}					
					break;

				//量程设置
				case IHU_CANVELA_OPTID_scale_range:					
					zIhuAdcBfscWs.WeightMaxScale = rcv.cmd.modbusVal;

					break;

				default:
					zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
					IhuErrorPrint("ADCLIBRA: Input parameters error!\n");
					return IHU_FAILURE;
					//break;
			} //switch(rcv.cmd.optid)
		

		//发送回去消息	
		snd.cmdid = IHU_ADC_BFSC_WS_CMD_TYPE_RESP;
		snd.cmd.optid = rcv.cmd.optid;
		snd.cmd.optpar = rcv.cmd.optpar;
		snd.cmd.prefixcmdid = IHU_CANVELA_PREFIXH_ws_resp;
		snd.length = sizeof(msg_struct_adclibra_l3bfsc_meas_cmd_resp_t);
		ret = ihu_message_send(MSG_ID_ADC_L3BFSC_MEAS_CMD_RESP, TASK_ID_BFSC, TASK_ID_ADCLIBRA, &snd, snd.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
			IhuErrorPrint("ADCLIBRA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_ADCLIBRA], zIhuTaskNameList[TASK_ID_BFSC]);
			return IHU_FAILURE;
		}
	}
	
	else{
		IhuErrorPrint("ADCLIBRA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
		return IHU_FAILURE;
	}
	
	return IHU_SUCCESS;
}

//MSG_ID_CAN_ADC_WS_MAN_SET_ZERO
OPSTAT fsm_adclibra_canvela_ws_man_set_zero(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_canvela_adc_ws_man_set_zero_t rcv;

	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_canvela_adc_ws_man_set_zero_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_adc_ws_man_set_zero_t))){
		IhuErrorPrint("ADCLIBRA: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_ADCLIBRA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//将ADC读数存入到控制表中
	zIhuAdcBfscWs.WeightManSetZero = func_adclibra_bfsc_read_origin() - zIhuAdcBfscWs.WeightBasket;
	return IHU_SUCCESS;
}


//根据读数，计算出应该得到的数据
INT32 func_adclibra_bfsc_read_weight(void)
{
	INT32 tempVal = 0;
	
	//读取假数据
	//需要直接访问ADC的HAL函数
	//tempVal = rand() % 1000;
	tempVal = ihu_l1hd_adc_bfsc_get_value();

	//算法处理
	//按道理，应该先对数据进行定标，再滤波，然后对照计算好的校准表，利用梯形线性差值，或者二次方B样条法，计算得到目标值
	//目标值，还需要按照系统配置的参数，进行变换

	//这里只考虑最为简单的减皮重和0值
	tempVal = tempVal - zIhuAdcBfscWs.WeightBasket - zIhuAdcBfscWs.WeightManSetZero; 
	
	return tempVal;
}

//读取原始数值
INT32 func_adclibra_bfsc_read_origin(void)
{
	INT32 tempVal = 0;
	
	//读取假数据
	//需要直接访问ADC的HAL函数
	//tempVal = rand() % 1000;
	tempVal = ihu_l1hd_adc_bfsc_get_value();
	
	//这里只做定标，然后返回
	
	return tempVal;
}


#endif






