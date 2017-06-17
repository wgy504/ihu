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

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
#include "l3bfsc.h" 
#include "l2adc_cs5532.h"
#endif

/*
** FSM of the ADCLIBRA
*/
IhuFsmStateItem_t IhuFsmAdclibra[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_adclibra_task_entry}, //Starting

	//System level initialization, only controlled by VM
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_adclibra_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_IDLE,            							    fsm_com_do_nothing},

  //Task level initialization
  {MSG_ID_COM_INIT,       								FSM_STATE_ADCLIBRA_INITED,            			fsm_adclibra_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_ADCLIBRA_INITED,            			fsm_com_do_nothing},

	//ANY state entry
  {MSG_ID_COM_INIT_FB,                    FSM_STATE_COMMON,                           fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,                 FSM_STATE_COMMON,                           fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,              FSM_STATE_COMMON,                           fsm_com_do_nothing},
	{MSG_ID_COM_STOP,                       FSM_STATE_COMMON,                           fsm_adclibra_stop_rcv},
  {MSG_ID_COM_RESTART,                    FSM_STATE_COMMON,                           fsm_adclibra_restart},
	{MSG_ID_COM_TIME_OUT,                   FSM_STATE_COMMON,                           fsm_adclibra_time_out},

	//Task level actived status
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
  {MSG_ID_L3BFSC_ADC_WS_CMD_CTRL,					FSM_STATE_ADCLIBRA_ACTIVED,         				fsm_adclibra_l3bfsc_ws_cmd_ctrl},
	{MSG_ID_CAN_ADC_WS_MAN_SET_ZERO,				FSM_STATE_ADCLIBRA_ACTIVED,         				fsm_adclibra_canvela_ws_man_set_zero},	
#endif

  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
strIhuBfscAdcWeightPar_t zIhuAdcBfscWs;
extern WeightSensorParamaters_t					zWeightSensorParam;

#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
strIhuCclAdcPar_t zIhuCclAdclibraCtrlTable;
#else
	#error Un-correct constant definition
#endif

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

	//串行回送INIT_FB给VMFO
	ihu_usleep(dest_id * IHU_SYSCFG_MODULE_START_DISTRIBUTION_DELAY_DURATION);
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_ADCLIBRA, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("ADCLIBRA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_ADCLIBRA].taskName, zIhuVmCtrTab.task[src_id].taskName);
			return IHU_FAILURE;
		}
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
	zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA] = 0;
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	memset(&zIhuAdcBfscWs, 0, sizeof(strIhuBfscAdcWeightPar_t));
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	memset(&zIhuCclAdclibraCtrlTable, 0, sizeof(strIhuCclAdcPar_t));
	zIhuCclAdclibraCtrlTable.cclAdcWorkingMode = IHU_CCL_ADC_WORKING_MODE_SLEEP;  //初始化就进入SLEEP，然后就看是否有触发
#else
	#error Un-correct constant definition
#endif

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ADCLIBRA, FSM_STATE_ADCLIBRA_ACTIVED) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
		IhuErrorPrint("ADCLIBRA: Error Set FSM State!\n");
		return IHU_FAILURE;
	}

	if (IHU_ADCLIBRA_PERIOD_TIMER_SET == IHU_ADCLIBRA_PERIOD_TIMER_ACTIVE){
		//启动周期性定时器，进行定时扫描
		ret = ihu_timer_start(TASK_ID_ADCLIBRA, TIMER_ID_1S_ADCLIBRA_PERIOD_SCAN, \
			zIhuSysEngPar.timer.array[TIMER_ID_1S_ADCLIBRA_PERIOD_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
			IhuErrorPrint("ADCLIBRA: Error start timer!\n");
			return IHU_FAILURE;
		}
	}
	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	ret = ihu_timer_start(TASK_ID_ADCLIBRA, TIMER_ID_10MS_BFSC_ADCLIBRA_SCAN_TIMER, \
		zIhuSysEngPar.timer.array[TIMER_ID_10MS_BFSC_ADCLIBRA_SCAN_TIMER].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_10MS);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
		IhuErrorPrint("ADCLIBRA: Error start timer!\n");
		return IHU_FAILURE;
	}	
#endif
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
		IhuErrorPrint("ADCLIBRA: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ADCLIBRA, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
		IhuErrorPrint("ADCLIBRA: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs

//uint32_t counter_200ms_test_rcv = 0;
//uint32_t tick_to_record_200ms_rcv[100];

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
	static uint32_t number_of_200ms_ticks = 0;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("ADCLIBRA: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]是否超限
	if (zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA] = zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_ADCLIBRA, TASK_ID_ADCLIBRA, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
			IhuErrorPrint("ADCLIBRA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_ADCLIBRA].taskName, zIhuVmCtrTab.task[TASK_ID_ADCLIBRA].taskName);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_ADCLIBRA_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_ADCLIBRA) != FSM_STATE_ADCLIBRA_ACTIVED){
			ret = FsmSetState(TASK_ID_ADCLIBRA, FSM_STATE_ADCLIBRA_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
				IhuErrorPrint("ADCLIBRA: Error Set FSM State!\n");
				return IHU_FAILURE;
			}//FsmSetState
		}
		
		func_adclibra_time_out_period_scan();
	}

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	else if ((rcv.timeId == TIMER_ID_10MS_BFSC_ADCLIBRA_SCAN_TIMER) &&(rcv.timeRes == TIMER_RESOLUTION_10MS))
	{
		//if (zIhuAdcBfscWs.WeightWorkingMode == IHU_BFSC_ADC_WEIGHT_WORKING_MODE_NORMAL) 
//			if( (number_of_200ms_ticks % (WEIGHT_SENSOR_ADC_READ_TICK_MS/200) == 0))
			{
					func_adclibra_time_out_bfsc_read_weight_scan();
					number_of_200ms_ticks++;
			}
		
//		counter_200ms_test_rcv++;
//		tick_to_record_200ms_rcv[counter_200ms_test_rcv%5] = osKernelSysTick();
//		if(4 == counter_200ms_test_rcv%5)
//		{
//				IhuErrorPrint("TIMER: %d, 200ms R\r\n", tick_to_record_200ms_rcv[4]);
//		}
	}
#endif
	
	else{
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
		IhuErrorPrint("ADCLIBRA: Receive error timer ID!\n");
		return IHU_FAILURE;
	}
	
	return IHU_SUCCESS;
}

//纯测试目的
void func_adclibra_time_out_period_scan(void)
{
	int ret = 0;
	
	//发送HeartBeat消息给VMFO模块，实现喂狗功能
	msg_struct_com_heart_beat_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_heart_beat_t));
	snd.length = sizeof(msg_struct_com_heart_beat_t);
	ret = ihu_message_send(MSG_ID_COM_HEART_BEAT, TASK_ID_VMFO, TASK_ID_ADCLIBRA, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
		IhuErrorPrint("ADCLIBRA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_ADCLIBRA].taskName, zIhuVmCtrTab.task[TASK_ID_VMFO].taskName);
		return;
	}
	
	return;
}

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
//OPSTAT func_adclibra_time_out_bfsc_read_weight_scan(void)
//{
//	int ret = 0;
//	//扫描ADC的数据，这里应该有采样，以及滤波算法
//	//先生成假数据，用于极度疯狂的系统测试，暂时将采样周期定位250ms，后面甚至可以设置10ms级别，以验证系统的可靠性与稳定性
//	//正常情况下，100MS的采样，已经足以够得着人工处理速度了。如果这是一条产线，按照每分钟200包、一秒钟3包的速度，300MS的一半两次采样，100MS<150MS，
//	//也足以应付各种情况了，所以设置为100MS应该是理论上最好的效果了，足够了
//	UINT32 tempWeight = 0;
//	tempWeight = func_adclibra_bfsc_read_weight();
//	IhuDebugPrint("ADCLIBRA: func_adclibra_time_out_bfsc_read_weight_scan: T:%d, tempWeight = %d\n", osKernelSysTick(), tempWeight);
//	
//	//传感器一直是0重量
//	if ((tempWeight == 0) && (zIhuAdcBfscWs.WeightExistCnt == 0))
//	{
//		//Do nothing
//	}
//	
//	//传感器突然变成了0重量
//	else if ((tempWeight == 0) && (zIhuAdcBfscWs.WeightExistCnt > 0))
//	{
//		//IhuErrorPrint("ADCLIBRA: func_adclibra_time_out_bfsc_read_weight_scan: tempWeight = %d, WeightExistCnt = %d\n", tempWeight, zIhuAdcBfscWs.WeightExistCnt);
//		zIhuAdcBfscWs.WeightExistCnt = 0;
//		//发送MSG_ID_ADC_MATERIAL_DROP到L3BFSC
//		msg_struct_adc_material_drop_t snd1;
//		memset(&snd1, 0, sizeof(msg_struct_adc_material_drop_t));
//		snd1.length = sizeof(msg_struct_adc_material_drop_t);
//		ret = ihu_message_send(MSG_ID_ADC_MATERIAL_DROP, TASK_ID_BFSC, TASK_ID_ADCLIBRA, &snd1, snd1.length);
//		if (ret == IHU_FAILURE){
//			zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
//			IhuErrorPrint("ADCLIBRA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_ADCLIBRA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);
//			return IHU_FAILURE;
//		}
//	}
//	
//	//传感器有新重量：这里的误差，得有算法来控制结果，只要是重量不一样，这里的程序就得上报，不管是不是真的有变化，以确保系统反应的敏感性
//	else if (tempWeight > 0)
//	{
//		//IhuErrorPrint("ADCLIBRA: func_adclibra_time_out_bfsc_read_weight_scan: tempWeight = %d, WeightExistCnt = %d\n", tempWeight, zIhuAdcBfscWs.WeightExistCnt);
//		zIhuAdcBfscWs.WeightExistCnt++;
//		if (tempWeight != zIhuAdcBfscWs.Weightvalue)
//		{
//			//发送MSG_ID_ADC_NEW_MATERIAL_WS
//			zIhuAdcBfscWs.Weightvalue = tempWeight;
//			msg_struct_adc_new_material_ws_t snd2;
//			memset(&snd2, 0, sizeof(msg_struct_adc_new_material_ws_t));
//			snd2.length = sizeof(msg_struct_adc_new_material_ws_t);
//			ret = ihu_message_send(MSG_ID_ADC_NEW_MATERIAL_WS, TASK_ID_BFSC, TASK_ID_ADCLIBRA, &snd2, snd2.length);
//			if (ret == IHU_FAILURE){
//				zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
//				IhuErrorPrint("ADCLIBRA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_ADCLIBRA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);
//				return IHU_FAILURE;
//			}
//		}//if (tempWeight != zIhuAdcBfscSensorWeightValue)
//	}
//	
//	//垃圾CASE，应该不能到达
//	else
//	{
//		zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
//		IhuErrorPrint("ADCLIBRA: Wrong scan result!\n");
//		return IHU_FAILURE;	
//	}
//	
//	return IHU_SUCCESS;
//}

/*
**
** MA Yuchu, rewrite for Weight Sensor Scan process 
**
*/

void InitWeightAdcBfscLocalParam(WeightSensorParamaters_t *pwsp)
{
		if((NULL == pwsp))
		{
				IhuErrorPrint("InitWeightAdcBfscLocalParam, (NULL == pwsp), return\n");
				return;
		}
		
		zIhuAdcBfscWs.WeightSensorLoadDetectionTicks = pwsp->WeightSensorLoadDetectionTimeMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
		IhuErrorPrint("zIhuAdcBfscWs.WeightSensorLoadDetectionTicks=%d\n", zIhuAdcBfscWs.WeightSensorLoadDetectionTicks);
		
		zIhuAdcBfscWs.WeightSensorEmptyDetectionTicks = pwsp->WeightSensorEmptyDetectionTimeMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
		IhuErrorPrint("zIhuAdcBfscWs.WeightSensorEmptyDetectionTicks=%d\n", zIhuAdcBfscWs.WeightSensorEmptyDetectionTicks);
		
		zIhuAdcBfscWs.WeightSensorDynamicZeroHysteresisTicks = pwsp->WeightSensorDynamicZeroHysteresisMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
		IhuErrorPrint("zIhuAdcBfscWs.WeightSensorDynamicZeroHysteresisTicks=%d\n", zIhuAdcBfscWs.WeightSensorDynamicZeroHysteresisTicks);
		
}	
	


weight_sensor_filter_t g_weight_sensor_filter;

void WeightCalculateMaxMin(INT32* pWeightValueAdjusted, UINT32 ScanTicks, INT32 *pWeightMax, INT32 *pWeightMin)
{
		INT32 weight_max = 0;
		INT32 weight_min = 0;
		UINT32 index = 0;
	
		if(NULL == pWeightValueAdjusted)
			return;
		
		if(NULL == pWeightMax)
			return;
		
		if(NULL == pWeightMin)
			return;
		
		weight_max = pWeightValueAdjusted[0];
		weight_min = pWeightValueAdjusted[0];	
		for(index = 0; index < ScanTicks; index++)
		{
				if(pWeightValueAdjusted[index] >= weight_max)
						weight_max = pWeightValueAdjusted[index];
				
				if(pWeightValueAdjusted[index] <= weight_min)
						weight_min = pWeightValueAdjusted[index];
		}
		
		*pWeightMax = weight_max;
		*pWeightMin = weight_min;
		
		return;
		
}


INT32 WeightCalculateAverage(INT32* pWeightValueRaw, UINT32 MovingAverageTicks)
{
		UINT32 index = 0;
		INT32 average = 0;
		double average_double = 0;
	
		if(NULL == pWeightValueRaw)
			return 0;
		
		for(index = 0; index < MovingAverageTicks; index++)
				average_double = average_double + (double)(*(pWeightValueRaw + index));
		
		average = (INT32)((average_double)/((double)MovingAverageTicks));
		return average;
}

void WeightSaveHistory(strIhuBfscAdcWeightPar_t *pBfscAdcWeightPar, INT32 WeightValueCurrentMovingAverage)
{
		UINT32	index = 0;
	
		if(NULL == pBfscAdcWeightPar)
			return;
		
		//MAKE SURE WeightValueEvaluated[0] is the latest one, WeightValueEvaluated[WEIGHT_SENSOR_MAX_TICKS_SAVED-1] is the oldest one
		for(index = WEIGHT_SENSOR_MAX_TICKS_SAVED - 1; index > 0; index--)
		{
				pBfscAdcWeightPar->WeightValueEvaluated[index] = pBfscAdcWeightPar->WeightValueEvaluated[index-1];
		}
		
		//SAVE the latest one
		pBfscAdcWeightPar->WeightValueEvaluated[0] = WeightValueCurrentMovingAverage;
		//IhuDebugPrint("S4:WeightValueEvaluated[0]=%d\n", pBfscAdcWeightPar->WeightValueEvaluated[0]);
		
		return;		
}

void WeightAdjustHistory(strIhuBfscAdcWeightPar_t *pbawp, WeightSensorParamaters_t *pwsp)
{
		
		UINT32	WeightSensorLoadDetectionTimeMs;
		UINT32	WeightSensorLoadThread;
		UINT32	WeightSensorEmptyThread;
		UINT32	WeightSensorEmptyDetectionTimeMs;
		UINT32	MaxAllowedWeight;										

		UINT32	WeightSensorStaticZeroValue;				
		UINT32	WeightSensorTailorValue;						
		UINT32	WeightSensorDynamicZeroThreadValue;	
		UINT32	WeightSensorDynamicZeroHysteresisMs;
	
		UINT32  Index;
		UINT32	WeightSensorLoadDetectionTicks; // = WeightSensorLoadDetectionTimeMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
		UINT32 	WeightSensorEmptyDetectionTicks; // = WeightSensorEmptyDetectionTimeMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
		UINT32	WeightSensorDynamicZeroHysteresisTicks; // = WeightSensorDynamicZeroHysteresisMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
	
		if((NULL == pbawp)||(NULL == pwsp))
		{
				return;
		}
		
		/* Save parameters */
		WeightSensorLoadDetectionTimeMs = pwsp->WeightSensorLoadDetectionTimeMs;
		WeightSensorLoadThread = pwsp->WeightSensorLoadThread;
		WeightSensorEmptyThread = pwsp->WeightSensorEmptyThread;
		WeightSensorEmptyDetectionTimeMs = pwsp->WeightSensorEmptyDetectionTimeMs;
		MaxAllowedWeight = pwsp->MaxAllowedWeight;										

		WeightSensorStaticZeroValue = pwsp->WeightSensorStaticZeroValue;				
		WeightSensorTailorValue = pwsp->WeightSensorTailorValue;						
		WeightSensorDynamicZeroThreadValue = pwsp->WeightSensorDynamicZeroThreadValue;	
		WeightSensorDynamicZeroHysteresisMs = pwsp->WeightSensorDynamicZeroHysteresisMs;
		//IhuDebugPrint("pwsp->WeightSensorDynamicZeroHysteresisMs=%d\n", pwsp->WeightSensorDynamicZeroHysteresisMs);

//		WeightSensorLoadDetectionTicks = WeightSensorLoadDetectionTimeMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
//		WeightSensorEmptyDetectionTicks = WeightSensorEmptyDetectionTimeMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
//		WeightSensorDynamicZeroHysteresisTicks = WeightSensorDynamicZeroHysteresisMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
		WeightSensorDynamicZeroHysteresisTicks = pbawp->WeightSensorDynamicZeroHysteresisTicks;
		
		/* Static zero and tailor */
		
		/*
		** Paramter checking !!!!! make sure these parameter relationship: 
		** (WEIGHT_SENSOR_DYNAMIC_ZERO_HYSTERESIS_MS / WEIGHT_SENSOR_ADC_READ_TICK_MS) < WEIGHT_SENSOR_MAX_TICKS_SAVED
		** (WEIGHT_SENSOR_LOAD_DETECTION_TIME_MS / WEIGHT_SENSOR_ADC_READ_TICK_MS) < WEIGHT_SENSOR_MAX_TICKS_SAVED
		** (WEIGHT_SENSOR_EMPTY_DETECTION_TIME_MS / WEIGHT_SENSOR_ADC_READ_TICK_MS) < WEIGHT_SENSOR_MAX_TICKS_SAVED
		** !!!! If these parameter are not checked, memory will be corrupt !!!!
		*/
		/* 
		** PROTECTION MEMORY START, as WeightValueAdjusted[Index], Index needs to be small than WEIGHT_SENSOR_MAX_TICKS_SAVED 
		*/
		if(WeightSensorDynamicZeroHysteresisTicks > WEIGHT_SENSOR_MAX_TICKS_SAVED)
		{
				WeightSensorDynamicZeroHysteresisTicks = WEIGHT_SENSOR_MAX_TICKS_SAVED;
		}
		/* PROTECTION MEMORY END */
		
		for (Index = 0; Index < WeightSensorDynamicZeroHysteresisTicks; Index++)
		{
				pbawp->WeightValueAdjusted[Index] = pbawp->WeightValueEvaluated[Index] - \
			                                      pwsp->WeightSensorStaticZeroValue - \
																						pwsp->WeightSensorTailorValue - \
																						pbawp->WeightDynamicZeroValue;
		}
		
		/* Auto Zero */
		for (Index = 0; Index < WeightSensorDynamicZeroHysteresisTicks; Index++)
		{
				if(pbawp->WeightValueEvaluated[Index] > WeightSensorDynamicZeroThreadValue)
						break;
		}
		
		/* WHICH MEANS ALL [0, WeightSensorDynamicZeroHysteresisTicks-1] ARE ALL LESS THAN WeightSensorDynamicZeroThreadValue */
		if( (Index == WeightSensorDynamicZeroHysteresisTicks) && (0 != Index) ) //(0 != Index) to avoid TOO SHORT WeightSensorDynamicZeroHysteresisTicks
				pbawp->WeightDynamicZeroValue = pbawp->WeightValueEvaluated[0];
		else
				pbawp->WeightDynamicZeroValue = 0;
		
		//IhuDebugPrint("S5:WeightValueAdjusted[0]=%d, WeightDynamicZeroValue=%d, Index=%d, HysteresisTicks=%d, HysteresisMs=%d\n", pbawp->WeightValueAdjusted[0], pbawp->WeightDynamicZeroValue, Index, WeightSensorDynamicZeroHysteresisTicks, WeightSensorDynamicZeroHysteresisMs);

}

void WeightLoadEmptyDetection(strIhuBfscAdcWeightPar_t *pbawp, WeightSensorParamaters_t *pwsp)
{

		UINT32	WeightSensorLoadDetectionTimeMs;
		UINT32	WeightSensorLoadThread;
		UINT32	WeightSensorEmptyThread;
		UINT32	WeightSensorEmptyDetectionTimeMs;
		UINT32	MaxAllowedWeight;										

		UINT32	WeightSensorStaticZeroValue;				
		UINT32	WeightSensorTailorValue;						
		UINT32	WeightSensorDynamicZeroThreadValue;	
		UINT32	WeightSensorDynamicZeroHysteresisMs;
	
		UINT32	RemainDetectionTimeSec;
	
		UINT32  Index;
		UINT32	WeightSensorLoadDetectionTicks; // = WeightSensorLoadDetectionTimeMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
		UINT32 	WeightSensorEmptyDetectionTicks; // = WeightSensorEmptyDetectionTimeMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
		UINT32	WeightSensorDynamicZeroHysteresisTicks; // = WeightSensorDynamicZeroHysteresisMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
	
		INT32	WeightMaxLoad, WeightMinLoad, WeightMaxEmpty, WeightMinEmpty;
	
		if((NULL == pbawp)||(NULL == pwsp))
		{
				return;
		}
		
		/* Save parameters */
		WeightSensorLoadDetectionTimeMs = pwsp->WeightSensorLoadDetectionTimeMs;
		WeightSensorLoadThread = pwsp->WeightSensorLoadThread;
		WeightSensorEmptyThread = pwsp->WeightSensorEmptyThread;
		WeightSensorEmptyDetectionTimeMs = pwsp->WeightSensorEmptyDetectionTimeMs;
		MaxAllowedWeight = pwsp->MaxAllowedWeight;
		
		RemainDetectionTimeSec = pwsp->RemainDetectionTimeSec;

		WeightSensorStaticZeroValue = pwsp->WeightSensorStaticZeroValue;				
		WeightSensorTailorValue = pwsp->WeightSensorTailorValue;						
		WeightSensorDynamicZeroThreadValue = pwsp->WeightSensorDynamicZeroThreadValue;	
		WeightSensorDynamicZeroHysteresisMs = pwsp->WeightSensorDynamicZeroHysteresisMs;
		
		WeightSensorLoadDetectionTicks = pbawp->WeightSensorLoadDetectionTicks;
		WeightSensorEmptyDetectionTicks = pbawp->WeightSensorEmptyDetectionTicks;
		WeightSensorDynamicZeroHysteresisTicks = pbawp->WeightSensorDynamicZeroHysteresisTicks;
		
		/* Get Max and Min value within WeightSensorLoadDetectionTicks &WeightSensorEmptyDetectionTicks */
		WeightCalculateMaxMin(pbawp->WeightValueAdjusted, WeightSensorLoadDetectionTicks, &WeightMaxLoad, &WeightMinLoad);
		WeightCalculateMaxMin(pbawp->WeightValueAdjusted, WeightSensorEmptyDetectionTicks, &WeightMaxEmpty, &WeightMinEmpty);

		/* Event Detection */
		/* EMPTY EVENT */
		if( (abs(WeightMaxEmpty) <= WeightSensorEmptyThread) && (abs(WeightMinEmpty) <= WeightSensorEmptyThread))
		{
				pbawp->WeightValueLastLoadTicks = pbawp->WeightValueCurrLoadTicks;
				pbawp->WeightValueLastLoadValue = pbawp->WeightValueCurrLoadValue;
				pbawp->WeightValueCurrLoadTicks = pbawp->WeightCurrentTicks;
				pbawp->WeightValueCurrLoadValue = pbawp->WeightValueAdjusted[0];
			
				pbawp->WeightLastEventType = pbawp->WeightCurrEventType;
				pbawp->WeightLastEventTicks = pbawp->WeightCurrEventTicks;
				pbawp->WeightLastValue = pbawp->WeightCurrValue;
				pbawp->WeightCurrEventType = WEIGHT_EVENT_ID_EMPTY;
				pbawp->WeightCurrEventTicks = pbawp->WeightCurrentTicks;
				pbawp->WeightCurrValue = pbawp->WeightValueAdjusted[0];
			
				IhuDebugPrint("S6:%d:%d:[L(%d):Max=%d,Min=%d,Max-Min=%d,T=%d][E(%d):Max=%d,Min=%d,T=%d]:EmptyEvent\n", zIhuAdcBfscWs.SysTicksMs, zIhuAdcBfscWs.WeightCurrentTicks,\
										WeightSensorLoadDetectionTicks, WeightMaxLoad, WeightMinLoad, (WeightMaxLoad-WeightMinLoad), WeightSensorLoadThread, \
										WeightSensorEmptyDetectionTicks, WeightMaxEmpty, WeightMinEmpty, WeightSensorEmptyThread);
		}
		else if( ((WeightMaxLoad - WeightMinLoad) <= WeightSensorLoadThread) && /* LOAD EVENT */
						 (WeightMinLoad >= (WeightSensorEmptyThread*2)) && 
						 (WeightMinLoad > 0) )
		{
				pbawp->WeightValueLastLoadTicks = pbawp->WeightValueCurrLoadTicks;
				pbawp->WeightValueLastLoadValue = pbawp->WeightValueCurrLoadValue;
				pbawp->WeightValueCurrLoadTicks = pbawp->WeightCurrentTicks;
				pbawp->WeightValueCurrLoadValue = pbawp->WeightValueAdjusted[0];
			
				pbawp->WeightLastEventType = pbawp->WeightCurrEventType;
				pbawp->WeightLastEventTicks = pbawp->WeightCurrEventTicks;
				pbawp->WeightLastValue = pbawp->WeightCurrValue;
				pbawp->WeightCurrEventType = WEIGHT_EVENT_ID_LOAD;
				pbawp->WeightCurrEventTicks = pbawp->WeightCurrentTicks;
				pbawp->WeightCurrValue = pbawp->WeightValueAdjusted[0];
			
				IhuDebugPrint("S6:%d:%d:[L(%d):Max=%d,Min=%d,Max-Min=%d,T=%d][E(%d):Max=%d,Min=%d,T=%d]:LoadEvent\n", zIhuAdcBfscWs.SysTicksMs, zIhuAdcBfscWs.WeightCurrentTicks,\
										WeightSensorLoadDetectionTicks, WeightMaxLoad, WeightMinLoad, (WeightMaxLoad-WeightMinLoad), WeightSensorLoadThread, \
										WeightSensorEmptyDetectionTicks, WeightMaxEmpty, WeightMinEmpty, WeightSensorEmptyThread);			
		}
		else
		{			
				pbawp->WeightLastEventType = pbawp->WeightCurrEventType;
				pbawp->WeightLastEventTicks = pbawp->WeightCurrEventTicks;
				pbawp->WeightLastValue = pbawp->WeightCurrValue;
				pbawp->WeightCurrEventType = WEIGHT_EVENT_ID_PICKUP;
				pbawp->WeightCurrEventTicks = pbawp->WeightCurrentTicks;
				pbawp->WeightCurrValue = pbawp->WeightValueAdjusted[0];
			
				IhuDebugPrint("S6:%d:%d:[L(%d):Max=%d,Min=%d,Max-Min=%d,T=%d][E(%d):Max=%d,Min=%d,T=%d]:Pickuping\n", zIhuAdcBfscWs.SysTicksMs, zIhuAdcBfscWs.WeightCurrentTicks,\
										WeightSensorLoadDetectionTicks, WeightMaxLoad, WeightMinLoad, (WeightMaxLoad-WeightMinLoad), WeightSensorLoadThread, \
										WeightSensorEmptyDetectionTicks, WeightMaxEmpty, WeightMinEmpty, WeightSensorEmptyThread);
		}

}

void SendWeightIndicationToBfsc(UINT32 adc_filtered, UINT32 average_weight, UINT32 weight_event, UINT32 repeat_times)
{
		msg_struct_l3bfsc_weight_ind_t weight_ind;
		OPSTAT ret;
	
		weight_ind.adc_filtered = adc_filtered;
		weight_ind.average_weight = average_weight;
		weight_ind.weight_event = weight_event;
		weight_ind.repeat_times = repeat_times;
	
		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_WEIGHT_IND, TASK_ID_BFSC, TASK_ID_BFSC, \
										       &weight_ind, sizeof(msg_struct_l3bfsc_weight_ind_t));

		if (ret == IHU_FAILURE){
			IhuErrorPrint("WS: SendWeightIndicationToBfsc error!\n");
		}
		
}


void WeightLoadEmptyEventReport(strIhuBfscAdcWeightPar_t *pbawp, WeightSensorParamaters_t *pwsp)
{
		
		if((NULL == pbawp)||(NULL == pwsp))
		{
				return;
		}
		
		/* Four cases considerred */
		/* Case 1: EMPTY -> EMPTY */
		if( (WEIGHT_EVENT_ID_EMPTY == pbawp->WeightCurrEventType) && 
			  (WEIGHT_EVENT_ID_EMPTY == pbawp->WeightLastEventType) )
		{
				if((pbawp->ConsecutiveTimes) >= ( (pwsp->RemainDetectionTimeSec * 1000)/WEIGHT_SENSOR_ADC_READ_TICK_MS) )
				{					
					
						IhuDebugPrint("S7:%d:%d:%d:%d: Empty->Empty, WeightCurrValue=%d, ConsecutiveTimes=%d, RepeatTimes=%d\n", pbawp->SysTicksMs, pbawp->WeightCurrentTicks, \
					                 pbawp->WeightCurrEventTicks, pbawp->WeightLastEventTicks, pbawp->WeightCurrValue, pbawp->ConsecutiveTimes, pbawp->RepeatTimes);

						SendWeightIndicationToBfsc(pwsp->WeightSensorAdcValue, pbawp->WeightCurrValue, \
					                              WEIGHT_EVENT_ID_EMPTY, pbawp->RepeatTimes);
					
						pbawp->RepeatTimes++;
						pbawp->ConsecutiveTimes = 0;
				}
				
				pbawp->ConsecutiveTimes++;
				
		}
		/* Case 2: LOAD -> EMPTY */
		else if( (WEIGHT_EVENT_ID_EMPTY == pbawp->WeightCurrEventType) && 
			       (WEIGHT_EVENT_ID_EMPTY != pbawp->WeightLastEventType) )
		{
							
				pbawp->RepeatTimes = 0;
				pbawp->ConsecutiveTimes = 0;
			
				IhuDebugPrint("S7:%d:%d:%d:%d: !Empty->Empty, WeightCurrValue=%d, ConsecutiveTimes=%d, RepeatTimes=%d\n", pbawp->SysTicksMs, pbawp->WeightCurrentTicks, \
											 pbawp->WeightCurrEventTicks, pbawp->WeightLastEventTicks, pbawp->WeightCurrValue, pbawp->ConsecutiveTimes, pbawp->RepeatTimes);

				SendWeightIndicationToBfsc(pwsp->WeightSensorAdcValue, pbawp->WeightCurrValue, \
														WEIGHT_EVENT_ID_EMPTY, pbawp->RepeatTimes);
			
		}
		/* Case 3: EMPTY -> LOAD */
		else if( (WEIGHT_EVENT_ID_LOAD == pbawp->WeightCurrEventType) && 
			       (WEIGHT_EVENT_ID_LOAD != pbawp->WeightLastEventType) )
		{
				pbawp->RepeatTimes = 0;
				pbawp->ConsecutiveTimes = 0;
			
				IhuDebugPrint("S7:%d:%d:%d:%d: !Load->Load, WeightCurrValue=%d, ConsecutiveTimes=%d, RepeatTimes=%d\n", pbawp->SysTicksMs, pbawp->WeightCurrentTicks, \
											 pbawp->WeightCurrEventTicks, pbawp->WeightLastEventTicks, pbawp->WeightCurrValue, pbawp->ConsecutiveTimes, pbawp->RepeatTimes);			
			
				SendWeightIndicationToBfsc(pwsp->WeightSensorAdcValue, pbawp->WeightCurrValue, \
														WEIGHT_EVENT_ID_LOAD, pbawp->RepeatTimes);
		}
		/* Case 4: LOAD -> LOAD */
		else if( (WEIGHT_EVENT_ID_LOAD == pbawp->WeightCurrEventType) && 
			       (WEIGHT_EVENT_ID_LOAD == pbawp->WeightLastEventType) )
		{
				if( (pbawp->ConsecutiveTimes) >= ( (pwsp->RemainDetectionTimeSec *1000)/WEIGHT_SENSOR_ADC_READ_TICK_MS ) )
				{
					
						IhuDebugPrint("S7:%d:%d:%d:%d: Load->Load, WeightCurrValue=%d, ConsecutiveTimes=%d, RepeatTimes=%d\n", pbawp->SysTicksMs, pbawp->WeightCurrentTicks, \
					                 pbawp->WeightCurrEventTicks, pbawp->WeightLastEventTicks, pbawp->WeightCurrValue, pbawp->ConsecutiveTimes, pbawp->RepeatTimes);

						SendWeightIndicationToBfsc(pwsp->WeightSensorAdcValue, pbawp->WeightCurrValue, \
					                              WEIGHT_EVENT_ID_LOAD, pbawp->RepeatTimes);
					
						pbawp->RepeatTimes++;
						pbawp->ConsecutiveTimes = 0;
				}			
				pbawp->ConsecutiveTimes++;
		}
		/* Case 5: ELSE */
		else
		{
			
		}
}

/*
** WeightSensorFilterCoeff[4];				// NOT for GUI
**
** WeightSensorFilterCoeff[0] => 0, Moving average
** WeightSensorFilterCoeff[1] => 0 
**
** Input:
**  g_weight_sensor_filter.beta_num[0] = WS_BETA_NUM1;    => WeightSensorFilterCoeff[0]
**  g_weight_sensor_filter.beta_num[1] = WS_BETA_NUM2;    => WeightSensorFilterCoeff[1]
**  g_weight_sensor_filter.stable_thresh = 12;    // ~2g  => WeightSensorFilterCoeff[2]
**  g_weight_sensor_filter.change_thresh = 30;    // ~5g  => WeightSensorFilterCoeff[3]
**
** Output:
**
**
*/
#define NOTEST
OPSTAT func_adclibra_time_out_bfsc_read_weight_scan(void)
{
	
//	INT32	WeightValueRaw[WEIGHT_SENSOR_MOVING_AVERAGE_TICKS];
//	INT32	WeightValueMovingAverage[WEIGHT_SENSOR_MOVING_AVERAGE_TICKS];
//	UIN32	WeightValueCurrentIndexMovingAverage;
//	INT32 WeightValueEvaluated[WEIGHT_SENSOR_MAX_TICKS_SAVED];
//	UIN32	WeightValueCurrentIndexEvaluated;
	
		weight_sensor_cmd_t command;
		#ifdef NOTEST
		int is_started = 0;
		#else
		int is_started = 1;
		#endif
		uint32_t last_adc_filtered = 0xFFFF, adc_filtered;
		uint32_t last_adc_tick = 0xFFFF, current_tick, repeat_times = 0;
		int weight = 0;
		msg_struct_l3bfsc_weight_ind_t weight_ind;
		OPSTAT ret;
	
		/* wait for a new command */
		// process command
		if(weight_sensor_recv_cmd(&command))
		{
			if(command.type == WIGHT_SENSOR_CMD_TYPE_STOP)
				is_started = 0;
			else if(command.type == WIGHT_SENSOR_CMD_TYPE_START)
				is_started = 1;
		}
		
		/* IF NOT STARTED, DIRECT RETURN */
		if( 0 == is_started )
			return SUCCESS;
	
		/* IF if( 1 == is_started ), continue for two option algorithm */
		
		/* MA ALGORITHM */
		if( (0 == zWeightSensorParam.WeightSensorFilterCoeff[0]) && (0 == zWeightSensorParam.WeightSensorFilterCoeff[1]) )
		{
				INT32	WeightValueCurrentMovingAverage = 0;
				
				// STEP 0, Record Time/Ticks
				//zIhuAdcBfscWs.WeightCurrentTicks++;
				zIhuAdcBfscWs.SysTicksMs = osKernelSysTick();
				
				// STEP 1, READ ADC VALUE, MAPPED TO WEIGHT VALUE
				zIhuAdcBfscWs.WeightValueRaw[zIhuAdcBfscWs.WeightValueCurrentIndexMovingAverage] = WeightSensorReadCurrent(&zWeightSensorParam);
				//IhuDebugPrint("S1:%d:%d:%d:[RAW:%d]\n", zIhuAdcBfscWs.SysTicksMs, zIhuAdcBfscWs.WeightCurrentTicks,zIhuAdcBfscWs.WeightValueCurrentIndexMovingAverage, zIhuAdcBfscWs.WeightValueRaw[zIhuAdcBfscWs.WeightValueCurrentIndexMovingAverage]);
				
				// STEP 2, MOVE THE POINTER TO NEXT INDEX OF MOVING AVERAE
				zIhuAdcBfscWs.WeightValueCurrentIndexMovingAverage++;
				if( WEIGHT_SENSOR_MOVING_AVERAGE_TICKS == zIhuAdcBfscWs.WeightValueCurrentIndexMovingAverage) 
						zIhuAdcBfscWs.WeightValueCurrentIndexMovingAverage = 0;
				
				// STEP 3, CALCULATE MOVING AVEARAGE
				WeightValueCurrentMovingAverage = WeightCalculateAverage(zIhuAdcBfscWs.WeightValueRaw, WEIGHT_SENSOR_MOVING_AVERAGE_TICKS);
				//IhuDebugPrint("S3:%d:%d:[MA:%d]\n", zIhuAdcBfscWs.SysTicksMs, zIhuAdcBfscWs.WeightCurrentTicks, WeightValueCurrentMovingAverage);

				// STEP 4, SAVE IN THE INTERNAL BUFFER
				WeightSaveHistory(&zIhuAdcBfscWs, WeightValueCurrentMovingAverage);
				
				// STEP 5, Adjust the weight according to tailoring and also Auto Zero
				WeightAdjustHistory(&zIhuAdcBfscWs, &zWeightSensorParam);
				
				// STEP 6, LOAD/EMPTY DETECTION DETECTION
				WeightLoadEmptyDetection(&zIhuAdcBfscWs, &zWeightSensorParam);
				
				// STEP 7, Decide how to report !!!
				WeightLoadEmptyEventReport(&zIhuAdcBfscWs, &zWeightSensorParam);

				// STEP 8, Record Time/Ticks
				zIhuAdcBfscWs.WeightCurrentTicks++;
		}
		else /* FILTER ALGORITHM */
		{
				// read sensor for a stable value
				if(weight_sensor_read_and_filtering(&g_weight_sensor_filter))
				{
						adc_filtered = (g_weight_sensor_filter.adc_filtered[0] + g_weight_sensor_filter.adc_filtered[1]) >> 1;

						if(abs(adc_filtered - last_adc_filtered) > g_weight_sensor_filter.change_thresh)
						{
								last_adc_filtered = adc_filtered;
								last_adc_tick = osKernelSysTick();
								repeat_times = 0;
								
								weight_ind.adc_filtered = adc_filtered;
								weight = weight_sensor_map_adc_to_weight(adc_filtered);
								
								if (weight >= 0)
										weight_ind.average_weight = weight;
								else
										weight_ind.average_weight = 0;
								
								zWeightSensorParam.WeightSensorOutputValue[0] = weight_ind.average_weight;
								
								weight_ind.repeat_times = repeat_times;
								
								IhuDebugPrint("tick%d: WS: new weight ind: adc_filtered=%d weight=%d\n", last_adc_tick, adc_filtered, weight_ind.average_weight);
								//IhuDebugPrint("tick%d: WS: new weight ind: adc_filtered=%d\n", last_adc_tick, adc_filtered);
								
								#ifdef NOTEST
								// weight changed, send weight indication to L3BFSC
								ret = ihu_message_send(MSG_ID_L3BFSC_WMC_WEIGHT_IND, TASK_ID_BFSC, TASK_ID_BFSC, \
																	&weight_ind, sizeof(msg_struct_l3bfsc_weight_ind_t));

								if (ret == IHU_FAILURE){
									IhuErrorPrint("WS: Send new weight ind message error!\n");
								}
								#endif
						}
						else
						{
								current_tick = osKernelSysTick();
								if(current_tick - last_adc_tick > 2000)
								{
									last_adc_tick = current_tick;
									repeat_times ++;
									
									weight_ind.adc_filtered = adc_filtered;
									weight_ind.repeat_times = repeat_times;
									
									//IhuDebugPrint("tick%d: WS: repeat weight ind: adc_filtered=%d weight=%d repeat_times=%d\n", last_adc_tick, adc_filtered, weight_sensor_map_adc_to_weight(adc_filtered), repeat_times);
									IhuDebugPrint("tick%d: WS: repeat weight ind: adc_filtered=%d repeat_times=%d\n", last_adc_tick, adc_filtered, repeat_times);

									#ifdef NOTEST
									// weight changed, send weight indication to L3BFSC
									ret = ihu_message_send(MSG_ID_L3BFSC_WMC_WEIGHT_IND, TASK_ID_BFSC, TASK_ID_BFSC, \
																		&weight_ind, sizeof(msg_struct_l3bfsc_weight_ind_t));

									if (ret == IHU_FAILURE){
										IhuErrorPrint("WS: Send repeat weight ind message error!\n");
									}
									#endif
								}
						}
				}
		}

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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//简单控制
	if (rcv.cmdid == IHU_SYSMSG_BFSC_ADC_WS_CMD_TYPE_START){
		zIhuAdcBfscWs.WeightWorkingMode = IHU_BFSC_ADC_WEIGHT_WORKING_MODE_NORMAL;
	}
	//简单控制	
	else if (rcv.cmdid == IHU_SYSMSG_BFSC_ADC_WS_CMD_TYPE_STOP){
		zIhuAdcBfscWs.WeightWorkingMode = IHU_BFSC_ADC_WEIGHT_WORKING_MODE_STOP;
	}
	//复杂控制	
	else if (rcv.cmdid == IHU_SYSMSG_BFSC_ADC_WS_CMD_TYPE_CTRL)
	{
		//入参检查
		if (rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_ws_ctrl)
		{
			IhuErrorPrint("ADCLIBRA: Receive message error!\n");
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
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
						zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
						return IHU_FAILURE;
					}					
					break;

				//量程设置
				case IHU_CANVELA_OPTID_scale_range:					
					zIhuAdcBfscWs.WeightMaxScale = rcv.cmd.modbusVal;

					break;

				default:
					zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
					IhuErrorPrint("ADCLIBRA: Input parameters error!\n");
					return IHU_FAILURE;
					//break;
			} //switch(rcv.cmd.optid)
		

		//发送回去消息	
		snd.cmdid = IHU_SYSMSG_ADC_BFSC_WS_CMD_TYPE_RESP;
		snd.cmd.optid = rcv.cmd.optid;
		snd.cmd.optpar = rcv.cmd.optpar;
		snd.cmd.prefixcmdid = IHU_CANVELA_PREFIXH_ws_resp;
		snd.length = sizeof(msg_struct_adclibra_l3bfsc_meas_cmd_resp_t);
		ret = ihu_message_send(MSG_ID_ADC_L3BFSC_MEAS_CMD_RESP, TASK_ID_BFSC, TASK_ID_ADCLIBRA, &snd, snd.length);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
			IhuErrorPrint("ADCLIBRA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_ADCLIBRA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);
			return IHU_FAILURE;
		}
	}
	
	else{
		IhuErrorPrint("ADCLIBRA: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_ADCLIBRA]++;
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
	//tempVal = rand() % 1000;  // MYC comment 2017/04/30 
	tempVal = ihu_l1hd_adc_bfsc_get_value(); // MYC added 2017/04/30
	IhuDebugPrint("ADCLIBRA: func_adclibra_bfsc_read_weight: tempVal = 0x%X!\n", tempVal);

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


//CCL项目下通过扫描电池判定是否有电量过高或者过低的告警
bool ihu_adclibra_ccl_scan_battery_warning_level(void)
{
	//先启动一会儿
	ihu_l1hd_adc1_start();
	ihu_usleep(200);
	
	//打印信息，方便调测
	IHU_DEBUG_PRINT_INF("ADCLIBRA: Battery Result = %d%%\n", ihu_l1hd_adc1_ccl_get_battery_value());
	
	if ((ihu_l1hd_adc1_ccl_get_battery_value() > IHU_CCL_ADC_BATTERY_WARNING_MAX_VALUE) || (ihu_l1hd_adc1_ccl_get_battery_value() < IHU_CCL_ADC_BATTERY_WARNING_MIN_VALUE))
		return TRUE;
	else
		return FALSE;
}

//=======================================================
//START: Local API from Xiong Puhui, for ADC Weight Filter
//=======================================================
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
extern WeightSensorParamaters_t					zWeightSensorParam;
extern WmcInventory_t										zWmcInvenory;

weight_sensor_cmd_t g_weight_sensor_cmd;

int weight_sensor_map_adc_to_weight(uint32_t adc_value)
{
  int den, weight;
	uint32_t temp = 0;
	int32_t temp2 = 0;
	int32_t temp3 = 0;
	
	static int test_sample_count_1 = 0;
	static int32_t test_sample_value_1 = 0;

  
  den = (zWeightSensorParam.WeightSensorCalibrationFullAdcValue - zWeightSensorParam.WeightSensorCalibrationZeroAdcValue);
  if(den <= 0)
    den = 6000;  // set a default value
  
  weight = (adc_value-zWeightSensorParam.WeightSensorCalibrationZeroAdcValue);
  if(weight < 0)
    weight = 0;

  weight = weight*zWeightSensorParam.WeightSensorCalibrationFullWeight/den;
	
extern WeightSensorCalirationKB_t wsckb;
	temp = adc_value;
	if(wsckb.k == 0)
		return 0xFFFFFFFF;
		
	temp2 = temp - wsckb.b;
	temp3 = temp2 / wsckb.k;

	weight = temp3;
	
	////// !!!!!!!!!!! FOR TEST !!!!!!!!! //////
	if(0 == zWeightSensorParam.WeightSensorOutputValue[1])
	{
			return temp3;
	}
	/* Test Code Skip All ADC, but Generate ramdom ADC/Weight */
	else if(1 == zWeightSensorParam.WeightSensorOutputValue[1])
	{
extern BfscWmcState_t										zBfscWmcState;			
			if( HUITP_IEID_SUI_BFSC_COMINETYPE_NULL == zBfscWmcState.last_combin_type.WeightCombineType )
			{
					if(0 == test_sample_count_1)
					{
							test_sample_value_1 = 10000 + (rand() % 30000);  /* 100g ~ 400g */
							test_sample_count_1 = 1;
					}
					return test_sample_value_1 + ((rand() % 300) - 150);
					
			}
			else /* HUITP_IEID_SUI_BFSC_COMINETYPE_ROOLOUT */
			{
					test_sample_value_1 = 10000 + (rand() % 30000);  /* Next random data, 100g ~ 400g */
					return ((rand() % 200) - 100);  // SO THAT the COMBIN_OUT MESSAGE WILL BE SENT OUT //
			}		
	}

  return weight;
}

// send a comman to weight sensor
int weight_sensor_send_cmd(uint32_t type)
{
  weight_sensor_cmd_t command;

  command.valid = 1;
  command.type = type;
  command.reserved = 0;
  
  taskENTER_CRITICAL();
  g_weight_sensor_cmd = command;
  taskEXIT_CRITICAL();
  
  return g_weight_sensor_cmd.valid;
}

// weight sensor task receive a command
int weight_sensor_recv_cmd(weight_sensor_cmd_t *command)
{
  taskENTER_CRITICAL();
  *command = g_weight_sensor_cmd;
  g_weight_sensor_cmd.valid = 1;
  taskEXIT_CRITICAL();
  
  return command->valid;
}




uint32_t weight_sensor_read_and_filtering(weight_sensor_filter_t *wsf)
{
  int i=0, adc_raw=0;
  int temp;

  do
  {
    if(ReadWheChanOk())
    {
      temp = ReadSeriesADValue();
      // skip the abnormal adc value
      if((temp > zWeightSensorParam.WeightSensorCalibrationZeroAdcValue/2) && (temp < zWeightSensorParam.WeightSensorCalibrationFullAdcValue*15))
      {
        adc_raw += temp;
        i++;
      }
      //IhuDebugPrint("adc_raw=%d\n", temp);
    }

    osDelay(1);
  }while(i<4);

  adc_raw = adc_raw >> 2;
	//IhuDebugPrint("adc_raw=%d\n", temp);
  
  temp = ((wsf->adc_filtered[0] - adc_raw) * wsf->beta_num[0]);
  wsf->adc_filtered[0] = (temp >> WS_BETA_DEN) + adc_raw;
  temp = ((wsf->adc_filtered[1] - adc_raw) * wsf->beta_num[1]);
  wsf->adc_filtered[1] = (temp >> WS_BETA_DEN) + adc_raw;
  //wsf->adc_filtered[0] = (((wsf->adc_filtered[0] - adc_raw) * wsf->beta_num[0]) >> WS_BETA_DEN) + adc_raw;
  //wsf->adc_filtered[1] = (((wsf->adc_filtered[1] - adc_raw) * wsf->beta_num[1]) >> WS_BETA_DEN) + adc_raw;

  IhuDebugPrint("adc_raw=%d adc_filtered=(%d-%d=%d), stable_thresh = %d\n", adc_raw, wsf->adc_filtered[0], wsf->adc_filtered[1], wsf->adc_filtered[0] - wsf->adc_filtered[1], wsf->stable_thresh);
	//  wsf->adc_filtered[0] = adc_raw;
	//	wsf->adc_filtered[1] = adc_raw;
	
  if(abs(wsf->adc_filtered[0] - wsf->adc_filtered[1]) < wsf->stable_thresh)
  {
    return 1;
  }

  return 0;  /// Only for Test 
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// weight sensor task
// 1) process the L3BF message
// 2) filter the ADC and report the weight value
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void weight_sensor_task(void const *param)
{
	weight_sensor_cmd_t command;
  #ifdef NOTEST
	int is_started = 0;
  #else
  int is_started = 1;
  #endif
	uint32_t last_adc_filtered = 0xFFFF, adc_filtered;
  uint32_t last_adc_tick = 0xFFFF, current_tick, repeat_times = 0;
	int weight = 0;
  msg_struct_l3bfsc_weight_ind_t weight_ind;
  OPSTAT ret;
  WeightSensorParamaters_t *wsparm = (WeightSensorParamaters_t *)param;

  // give control to other tasks
  osDelay(20);
  
  //åˆå§‹åŒ–Weight Sensor ADC
	WeightSensorInit(&zWeightSensorParam);
	IhuDebugPrint("L3BFSC: fsm_bfsc_init: WeightSensorInit()\r\n");

  zWeightSensorParam.WeightSensorCalibrationFullWeight = 100000;//0.01g????
	zWeightSensorParam.WeightSensorCalibrationZeroAdcValue = 435734;
	zWeightSensorParam.WeightSensorCalibrationFullAdcValue = 508076;
		
  g_weight_sensor_filter.adc_filtered[0] = 0;
  g_weight_sensor_filter.adc_filtered[1] = 0;
  g_weight_sensor_filter.beta_num[0] = WS_BETA_NUM1;
  g_weight_sensor_filter.beta_num[1] = WS_BETA_NUM2;
  g_weight_sensor_filter.stable_thresh = 12;    // ~2g
  g_weight_sensor_filter.change_thresh = 30;    // ~5g

	srand(zWmcInvenory.wmc_id.wmc_id);
	
	while(1)
	{		
		/* wait for a new command */
    // process command
		if(weight_sensor_recv_cmd(&command))
    {
      if(command.type == WIGHT_SENSOR_CMD_TYPE_STOP)
        is_started = 0;
      else if(command.type == WIGHT_SENSOR_CMD_TYPE_START)
        is_started = 1;
    }

    if(is_started)
    {
      // read sensor for a stable value
      if(weight_sensor_read_and_filtering(&g_weight_sensor_filter))
			//if(1)
      {
        adc_filtered = (g_weight_sensor_filter.adc_filtered[0] + g_weight_sensor_filter.adc_filtered[1]) >> 1;

        //if(abs(adc_filtered - last_adc_filtered) > g_weight_sensor_filter.change_thresh)
        if(1)
				{
          last_adc_filtered = adc_filtered;
          last_adc_tick = osKernelSysTick();
          repeat_times = 0;
          
          weight_ind.adc_filtered = adc_filtered;
          //weight_ind.adc_filtered = 10000 + (rand() % 10000);  /// TO BE REMOVED, OR ADD TO TEST MODE
					weight = weight_sensor_map_adc_to_weight(adc_filtered);
					
					if (weight >= 0)
						weight_ind.average_weight = weight;
					else
						weight_ind.average_weight = 0;
					
					zWeightSensorParam.WeightSensorOutputValue[0] = weight_ind.average_weight;
					
					weight_ind.repeat_times = repeat_times;
          
					IhuDebugPrint("tick%d: WS: new weight ind: adc_filtered=%d weight=%d\n", last_adc_tick, adc_filtered, weight_ind.average_weight);
					//IhuDebugPrint("tick%d: WS: new weight ind: adc_filtered=%d\n", last_adc_tick, adc_filtered);
					
					#ifdef NOTEST
          // weight changed, send weight indication to L3BFSC
          ret = ihu_message_send(MSG_ID_L3BFSC_WMC_WEIGHT_IND, TASK_ID_BFSC, TASK_ID_BFSC, \
														&weight_ind, sizeof(msg_struct_l3bfsc_weight_ind_t));

          if (ret == IHU_FAILURE){
      			IhuErrorPrint("WS: Send new weight ind message error!\n");
      		}
          #endif
        }
        else
        {
          current_tick = osKernelSysTick();
          if(current_tick - last_adc_tick > 2000)
          {
            last_adc_tick = current_tick;
            repeat_times ++;
            
            weight_ind.adc_filtered = adc_filtered;
            weight_ind.repeat_times = repeat_times;
            
						//IhuDebugPrint("tick%d: WS: repeat weight ind: adc_filtered=%d weight=%d repeat_times=%d\n", last_adc_tick, adc_filtered, weight_sensor_map_adc_to_weight(adc_filtered), repeat_times);
						IhuDebugPrint("tick%d: WS: repeat weight ind: adc_filtered=%d repeat_times=%d\n", last_adc_tick, adc_filtered, repeat_times);

            #ifdef NOTEST
            // weight changed, send weight indication to L3BFSC
            ret = ihu_message_send(MSG_ID_L3BFSC_WMC_WEIGHT_IND, TASK_ID_BFSC, TASK_ID_BFSC, \
  														&weight_ind, sizeof(msg_struct_l3bfsc_weight_ind_t));

            if (ret == IHU_FAILURE){
        			IhuErrorPrint("WS: Send repeat weight ind message error!\n");
        		}
            #endif
          }
        }
      }
    }
    else
    {  
			osDelay(5000);
		}
		osDelay(1000); // For Test ONLY
	}
}
#endif
//=======================================================
//END: Local API from Xiong Puhui, for ADC Weight Filter
//=======================================================
