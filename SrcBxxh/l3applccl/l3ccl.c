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
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_IDLE,            							fsm_com_do_nothing},

  //Task level initialization
  {MSG_ID_COM_INIT,       								FSM_STATE_CCL_INITED,            				fsm_ccl_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_CCL_INITED,            				fsm_com_do_nothing},

	//ANY state entry
  {MSG_ID_COM_INIT_FB,                    FSM_STATE_COMMON,                       fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,                 FSM_STATE_COMMON,                       fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,              FSM_STATE_COMMON,                       fsm_com_do_nothing},
	{MSG_ID_COM_STOP,                       FSM_STATE_COMMON,                       fsm_ccl_stop_rcv},
  {MSG_ID_COM_RESTART,                    FSM_STATE_COMMON,                       fsm_ccl_restart},
	{MSG_ID_COM_TIME_OUT,                   FSM_STATE_COMMON,                       fsm_ccl_time_out},
	
	//启动以后，有可能是正常的SLEEP或者FAULT状态，所以先检测门限，如果门限开，则直接进入FAULT状态，如果门限关闭，则继续干活
	//FSM_STATE_CCL_ACTIVED：激活状态，获得TRIGGER_EVENT则进入INQUERY查询状态。如果非常开门锁则进入FATAL_FAULT状态。如果长定时到达则进入EVENT_REPORT状态
	
	//FSM_STATE_CCL_EVENT_REPORT：发送完整的状态报告给后台
	{MSG_ID_CCL_PERIOD_REPORT_TRIGGER,			FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_period_report_trigger},	 		//周期性报告来临
	{MSG_ID_DIDO_CCL_SENSOR_STATUS_RESP,		FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_dido_sensor_status_resp},	 	//传感器采样
	{MSG_ID_SPS_CCL_SENSOR_STATUS_RESP,			FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_sps_sensor_status_resp},	 	//传感器采样
	{MSG_ID_I2C_CCL_SENSOR_STATUS_RESP,			FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_i2c_sensor_status_resp},	 	//传感器采样
	{MSG_ID_DCMI_CCL_SENSOR_STATUS_RESP,		FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_dcmi_sensor_status_resp},  	//传感器采样
	{MSG_ID_SPS_CCL_EVENT_REPORT_CFM,				FSM_STATE_CCL_EVENT_REPORT,         		fsm_ccl_sps_event_report_cfm},     	//发送周期报告的证实

	//启动以后直接干活
	{MSG_ID_CCL_HAND_ACTIVE_TRIGGER,				FSM_STATE_CCL_ACTIVE,         					fsm_ccl_hand_active_trigger_to_work},  //人工激活触发	

	//FSM_STATE_CCL_CLOUD_INQUERY：启动SESSION定时4.5分钟 = 270秒，等待后台回传指令，开门授权则进入TO_OPEN_DOOR状态，否则关机
	{MSG_ID_SPS_CCL_CLOUD_FB,								FSM_STATE_CCL_CLOUD_INQUERY,         		fsm_ccl_sps_cloud_fb}, 							//后台反馈	
	
	//FSM_STATE_CCL_TO_OPEN_DOOR：启动30S开门定时，等待人工开门，超时则关门回归SLEEP
	{MSG_ID_DIDO_CCL_DOOR_OPEN_EVENT,				FSM_STATE_CCL_TO_OPEN_DOOR,         		fsm_ccl_dido_door_open_event},      //开门事件
	{MSG_ID_SPS_CCL_CLOSE_REPORT_CFM,				FSM_STATE_CCL_TO_OPEN_DOOR,         		fsm_ccl_sps_close_door_report_cfm},	//发送一次开关门报告的证实			
	
	//FSM_STATE_CCL_DOOR_OPEN：监控门限和门锁，大定时器超时进入FATAL FAULT。正常关门则发送报告给后台，然后关机
	{MSG_ID_DIDO_CCL_DOOR_CLOSE_EVENT,			FSM_STATE_CCL_DOOR_OPEN,         				fsm_ccl_door_close_event}, //关门事件
	{MSG_ID_SPS_CCL_CLOSE_REPORT_CFM,				FSM_STATE_CCL_DOOR_OPEN,         				fsm_ccl_sps_close_door_report_cfm},	//发送一次开关门报告的证实		
	
	//FSM_STATE_CCL_FATAL_FAULT：严重错误状态，发送报告给后台，等待人工干预。然后还需要修改闹铃定时器，以便再次唤醒并进行监控
	//需要启动5秒差错定时器，BEEP和LED持续告警，超时以后，修改闹铃到10分钟，然后关断自己
	//这个状态的处理逻辑是：将差错情况汇报给后台，并关机
	//需要不需要此事继续扫描，并判定关门的情形？如果不扫描，肯定简单多了，但系统比较肉。如果扫描，系统复杂，但系统反应灵敏
	//折中：CCL主动进行扫描，而不是等待DIDO模块进行事件性扫描，简化状态机。这样还是有可能收到CLOSE_REPORT_CFM消息的
	{MSG_ID_CCL_FAULT_STATE_TRIGGER,				FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_fault_state_trigger},	  				//10分钟定时差错处理
	{MSG_ID_SPS_CCL_FAULT_REPORT_CFM,				FSM_STATE_CCL_FATAL_FAULT,         			fsm_ccl_sps_fault_report_cfm},					//发送故障报告的证实
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             							NULL},  //Ending
};

//Global variables defination
strIhuCclTaskContext_t zIhuCclTaskContext;

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
	ihu_usleep(dest_id * IHU_SYSCFG_MODULE_START_DISTRIBUTION_DELAY_DURATION);
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
	memset(&zIhuCclTaskContext, 0, sizeof(strIhuCclTaskContext_t));
	
	//启动心跳定时器，确保喂狗的基本功能
	ret = ihu_timer_start(TASK_ID_CCL, TIMER_ID_1S_CCL_PERIOD_SCAN, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_CCL_PERIOD_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++;
		IhuErrorPrint("CCL: Error start timer!\n");
		return IHU_FAILURE;
	}
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("CCL: Enter FSM_STATE_CCL_ACTIVE status, Keeping refresh here!\n");
	}

	//尽快读取门按钮的状态，以防止丢失。其实就是扫描门按钮的高低电平，判定系统是被谁唤醒的
	zIhuCclTaskContext.handActFlag = ihu_l1hd_dido_f2board_lock_act_flag_read();
	
	//拉灯拉BEEP
	ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_GLOBAL_WORK_STATE, 20);
	ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_CCL_BEEP_PATTERN_SYS_START, 1);
	
	//等待3秒，以便其它任务进入稳定状态
	func_ccl_close_all_sensor_power();
	ihu_sleep(3);

	//判定是人工触发
	if (zIhuCclTaskContext.handActFlag == TRUE){
		IHU_DEBUG_PRINT_FAT("CCL: I am in the handal active state!!!\n");
		//必须人为的设置一次8小时定时RTC，不然有可能遇到系统第一次启动的情形，从未设置过
		//多次重复设置的问题，留给驱动解决：通过BOOT区的计数器判定是否设定过
		if (func_vmmw_rtc_pcf8563_init() == IHU_SUCCESS){
			IhuDebugPrint("CCL: OK set RTC PCF8563, set alarm to next %d minute!\n", IHU_CCL_ALARM_NORMAL_PERIOD_DURATION);
			func_vmmw_rtc_pcf8563_set_alarm_process(IHU_CCL_ALARM_NORMAL_PERIOD_DURATION);
			}
		else{
			IhuErrorPrint("CCL: Error set RTC PCF8563, Force to set alarm to next %d minute!\n", IHU_CCL_ALARM_NORMAL_PERIOD_DURATION);
			func_vmmw_rtc_pcf8563_set_alarm_process(IHU_CCL_ALARM_NORMAL_PERIOD_DURATION);
		}

		//再进行状态转移
		FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_ACTIVE);

		//发送触发消息给CCL模块
		msg_struct_ccl_hand_active_trigger_t snd;
		memset(&snd, 0, sizeof(msg_struct_ccl_hand_active_trigger_t));	
		snd.length = sizeof(msg_struct_ccl_hand_active_trigger_t);
		ret = ihu_message_send(MSG_ID_CCL_HAND_ACTIVE_TRIGGER, TASK_ID_CCL, TASK_ID_CCL, &snd, snd.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
	}
	
	//闹铃唤醒的，判定是哪一种闹铃：8小时周期报告定时，还是10分钟差错闹铃
	//else if (func_vmmw_rtc_pcf8563_judge_alarm_happen() == TRUE){
	else {
		//读取闹铃设置信息
		if (func_vmmw_rtc_pcf8563_get_alarm_duration() == IHU_CCL_ALARM_FAULT_PERIOD_DURATION){
			IHU_DEBUG_PRINT_FAT("CCL: I am in the FAULT ALARM active state!!!\n");
			//扫描门限
			if (ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_door_open_state(IHU_CCL_SENSOR_LOCK_NUMBER_MAX) == TRUE){

				if (func_vmmw_rtc_pcf8563_init() == IHU_SUCCESS){
					IhuDebugPrint("CCL: OK set RTC PCF8563, set alarm to next %d minute!\n", IHU_CCL_ALARM_FAULT_PERIOD_DURATION);
					func_vmmw_rtc_pcf8563_set_alarm_process(IHU_CCL_ALARM_FAULT_PERIOD_DURATION);
					}
				else{
					IhuErrorPrint("CCL: Error set RTC PCF8563, Force to set alarm to next %d minute!\n", IHU_CCL_ALARM_FAULT_PERIOD_DURATION);
					func_vmmw_rtc_pcf8563_set_alarm_process(IHU_CCL_ALARM_FAULT_PERIOD_DURATION);
					}


				
				//进入差错状态机
				FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_FATAL_FAULT);

				//发送触发消息给CCL模块
				msg_struct_ccl_fault_state_trigger_t snd;
				memset(&snd, 0, sizeof(msg_struct_ccl_fault_state_trigger_t));	
				snd.length = sizeof(msg_struct_ccl_fault_state_trigger_t);
				ret = ihu_message_send(MSG_ID_CCL_FAULT_STATE_TRIGGER, TASK_ID_CCL, TASK_ID_CCL, &snd, snd.length);
				if (ret == IHU_FAILURE)
					IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
			}
			
			//进入正常的8小时周期报告
			else{
				IHU_DEBUG_PRINT_FAT("CCL: I am in the FAULT recover to NORMAL ALARM active state!!!\n");				
				if (func_vmmw_rtc_pcf8563_init() == IHU_SUCCESS){
					IhuDebugPrint("CCL: OK set RTC PCF8563, set alarm to next %d minute!\n", IHU_CCL_ALARM_NORMAL_PERIOD_DURATION);
					func_vmmw_rtc_pcf8563_set_alarm_process(IHU_CCL_ALARM_NORMAL_PERIOD_DURATION);
					}
				else{
					IhuErrorPrint("CCL: Error set RTC PCF8563, Force to set alarm to next %d minute!\n", IHU_CCL_ALARM_NORMAL_PERIOD_DURATION);
					func_vmmw_rtc_pcf8563_set_alarm_process(IHU_CCL_ALARM_NORMAL_PERIOD_DURATION);
					}
				
				//进入8小时正常的报告状态
				FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_EVENT_REPORT);

				//发送触发消息给CCL模块
				msg_struct_ccl_period_reoport_trigger_t snd;
				memset(&snd, 0, sizeof(msg_struct_ccl_period_reoport_trigger_t));	
				snd.length = sizeof(msg_struct_ccl_period_reoport_trigger_t);
				ret = ihu_message_send(MSG_ID_CCL_PERIOD_REPORT_TRIGGER, TASK_ID_CCL, TASK_ID_CCL, &snd, snd.length);
				if (ret == IHU_FAILURE)
					IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
			}
		}
		
		//其它情况，非10分钟定时，可以是8小时定时，或者其它原因。都当做定时周期性汇报。为了确保安全性，重新设置ALARM定时报告
		else{
			IHU_DEBUG_PRINT_FAT("CCL: I am in the NORMAL ALARM active state!!!\n");
			//正常的进入周期性汇报状态机
			if (func_vmmw_rtc_pcf8563_init() == IHU_SUCCESS){
				IhuDebugPrint("CCL: OK set RTC PCF8563, set alarm to next %d minute!\n", IHU_CCL_ALARM_NORMAL_PERIOD_DURATION);
				func_vmmw_rtc_pcf8563_set_alarm_process(IHU_CCL_ALARM_NORMAL_PERIOD_DURATION);
			}
			else{
				IhuErrorPrint("CCL: Error set RTC PCF8563, Force to set alarm to next %d minute!\n", IHU_CCL_ALARM_NORMAL_PERIOD_DURATION);
				func_vmmw_rtc_pcf8563_set_alarm_process(IHU_CCL_ALARM_NORMAL_PERIOD_DURATION);
			}
			
			//进入8小时正常的报告状态
			FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_EVENT_REPORT);	

			//发送触发消息给CCL模块
			msg_struct_ccl_period_reoport_trigger_t snd;
			memset(&snd, 0, sizeof(msg_struct_ccl_period_reoport_trigger_t));	
			snd.length = sizeof(msg_struct_ccl_period_reoport_trigger_t);
			ret = ihu_message_send(MSG_ID_CCL_PERIOD_REPORT_TRIGGER, TASK_ID_CCL, TASK_ID_CCL, &snd, snd.length);
			if (ret == IHU_FAILURE)
				IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
		}
	}
	
	//测试整体硬件进入休眠状态
	//ihu_l1hd_pwr_sav_enter_into_stop_mode();
	
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
		zIhuCclTaskContext.sensor.doorState[i] = rcv.sensor.doorState[i];
	}
	zIhuCclTaskContext.sensor.batteryValue = rcv.sensor.batteryValue;
	zIhuCclTaskContext.sensor.fallState = rcv.sensor.fallState;
	zIhuCclTaskContext.sensor.tempValue = rcv.sensor.tempValue;
	zIhuCclTaskContext.sensor.humidValue = rcv.sensor.humidValue;
	
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
	zIhuCclTaskContext.sensor.rssiValue = rcv.sensor.rssiValue;
	
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
	zIhuCclTaskContext.sensor.rsv1Value = rcv.sensor.rsv1Value;
	zIhuCclTaskContext.sensor.rsv2Value = rcv.sensor.rsv2Value;
	
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
	
	//入参检查 //Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dcmi_ccl_sensor_status_rep_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dcmi_ccl_sensor_status_rep_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_RESP_STATUS_DCMI)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
		
	//获取报告数据
	for (i=0; i<IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
		zIhuCclTaskContext.sensor.cameraState[i] = rcv.sensor.cameraState[i];
	}
	
	//发送SPS数据给串口，形成发送的报告
	memset(&snd, 0, sizeof(msg_struct_ccl_sps_event_report_send_t));
	snd.cmdid = IHU_CCL_DH_CMDID_EVENT_REPORT;
	memcpy(&(snd.sensor), &zIhuCclTaskContext.sensor, sizeof(com_sensor_status_t));
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
	func_ccl_close_all_sensor_power();
	
	//延时并关断CPU系统
	ihu_sleep(2);
	func_cccl_cpu_power_off();
		
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

//周期性报告的触发来临
OPSTAT fsm_ccl_period_report_trigger(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_ccl_com_sensor_status_req_t snd;

	//打开所有接口
	func_ccl_open_all_sensor_power();
	
	//控制所有传感器，进入工作模式
	//作为周期采样模式，这里是不必要的，只有在工作模式下才需要这个过程
	
	//准备接收数据的缓冲区
	memset(&zIhuCclTaskContext.sensor, 0, sizeof(com_sensor_status_t));
	
	//由于消息队列的长度问题，这里采用串行发送接收模式，避免了多个接口的消息同时到达
	//发送第一组DIDO采样命令
	memset(&snd, 0, sizeof(msg_struct_ccl_com_sensor_status_req_t));
	snd.cmdid = IHU_CCL_DH_CMDID_REQ_STATUS_DIDO;
	snd.length = sizeof(msg_struct_ccl_com_sensor_status_req_t);
	ret = ihu_message_send(MSG_ID_CCL_COM_SENSOR_STATUS_REQ, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DIDOCAP].taskName);
	}
	
	//返回
	return IHU_SUCCESS;
}


//锁的工作周期之内：超时，则发送出错到各个下位机，因为此时门肯定没有正常被关闭，导致正常的工作流程没有完成
OPSTAT func_ccl_time_out_lock_work_active(void)
{
	int ret = 0;
	//msg_struct_ccl_com_ctrl_cmd_t snd;
	msg_struct_ccl_sps_close_report_send_t snd1;
	msg_struct_ccl_sps_fault_report_send_t snd2;
	
	//只需要去激活所有下位机
	if (FsmGetState(TASK_ID_CCL) == FSM_STATE_CCL_CLOUD_INQUERY){
		//关闭所有接口
		func_ccl_close_all_sensor_power();
		
		//延时并关断CPU系统
		ihu_sleep(2);
		func_cccl_cpu_power_off();

	}
	
	//待开门，意味着门还未开：这里面也有个逻辑，就是必须也得等待CLOSE_REPORT_CFM的到来，不然也会出现问题
	else if (FsmGetState(TASK_ID_CCL) == FSM_STATE_CCL_TO_OPEN_DOOR){
		//停止开门定时器：这是因为卡边，大定时器到点但开门定时器还未到点
		ret = ihu_timer_stop(TASK_ID_CCL, TIMER_ID_1S_CCL_LOCK_WORK_WAIT_TO_OPEN, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error start timer!\n");
		
		//发送关门报告给后台
		memset(&snd1, 0, sizeof(msg_struct_ccl_sps_close_report_send_t));
		snd1.cause = IHU_SYSMSG_CCL_CLOSE_DOOR_NOT_YET_OPEN;
		snd1.length = sizeof(msg_struct_ccl_sps_close_report_send_t);
		ret = ihu_message_send(MSG_ID_CCL_SPS_CLOSE_REPORT_SEND, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd1, snd1.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
		
		//然后将门关上
		//关门控制：独特的技巧，复用这些参数
		if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 1){
			ihu_usleep(200);
			ihu_l1hd_dido_f2board_lock1_do1_off();
		}
		if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 2){
			ihu_usleep(200);
			ihu_l1hd_dido_f2board_lock2_do1_off();
		}
		if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 4){
			ihu_usleep(200);
			ihu_l1hd_dido_f2board_lock3_do1_off();
			ihu_usleep(200);
			ihu_l1hd_dido_f2board_lock4_do1_off();
		}
		
	}
	
	//门开着，为收到关门信号，这个就要进入差错了
	else if (FsmGetState(TASK_ID_CCL) == FSM_STATE_CCL_DOOR_OPEN){
		//发送差错状态报告给后台
		memset(&snd2, 0, sizeof(msg_struct_ccl_sps_fault_report_send_t));
		snd2.cause = IHU_SYSMSG_CCL_FAULT_CAUSE_CLOSE_DOOR_TIME_OUT;
		snd2.length = sizeof(msg_struct_ccl_sps_fault_report_send_t);
		if (ihu_message_send(MSG_ID_CCL_SPS_FAULT_REPORT_SEND, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd2, snd2.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
		
		//然后将锁关上
		//关门控制：独特的技巧，复用这些参数
		if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 1){
			ihu_usleep(200);
			ihu_l1hd_dido_f2board_lock1_do1_off();
		}
		if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 2){
			ihu_usleep(200);
			ihu_l1hd_dido_f2board_lock2_do1_off();
		}
		if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 4){
			ihu_usleep(200);
			ihu_l1hd_dido_f2board_lock3_do1_off();
			ihu_usleep(200);
			ihu_l1hd_dido_f2board_lock4_do1_off();
		}		
		
		//状态转移：直接去FATAL_FAULT状态了
		if (FsmSetState(TASK_ID_CCL, FSM_STATE_CCL_FATAL_FAULT) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error Set FSM State!");
		
		//调整ALARM的时间长度，以便及时醒来，再监控	
		if (func_vmmw_rtc_pcf8563_init() == IHU_SUCCESS){
			func_vmmw_rtc_pcf8563_set_alarm_process(IHU_CCL_ALARM_FAULT_PERIOD_DURATION);
		}		
		
		//拉灯拉BEEP
		ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_GLOBAL_WORK_STATE, 5);
		ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_CCL_BEEP_PATTERN_SYS_SHUT_DOWN, 1);		
		
		//关机，等待FAULT REPORT收到以后，会自动干的
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
	msg_struct_ccl_dido_door_open_req_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_spsvirgo_ccl_cloud_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_spsvirgo_ccl_cloud_fb_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);	
	
	//对收到的后台指令反馈结果进行处理
	if (rcv.authResult == IHU_CCL_LOCK_AUTH_RESULT_NOK){		
		//关闭所有接口
		func_ccl_close_all_sensor_power();
		
		//停止定时器
		if (ihu_timer_stop(TASK_ID_CCL, TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE, TIMER_RESOLUTION_1S) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error start timer!\n");
		
		//拉灯拉BEEP，指示整个工作过程不成功
		ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_GLOBAL_WORK_STATE, 5);
		ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_CCL_BEEP_PATTERN_SYS_SHUT_DOWN, 1);		

		//延时并关断CPU系统
		ihu_sleep(5);
		func_cccl_cpu_power_off();
	}
		
	//如果是得到开门授权指令，则发送命令到DIDO模块
	else if (rcv.authResult == IHU_CCL_LOCK_AUTH_RESULT_OK){
		memset(&snd, 0, sizeof(msg_struct_ccl_dido_door_open_req_t));
		snd.length = sizeof(msg_struct_ccl_dido_door_open_req_t);
		snd.doorid = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
		if (ihu_message_send(MSG_ID_CCL_DIDO_DOOR_OPEN_REQ, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DIDOCAP].taskName);
		
		//开门控制：独特的技巧，复用这些参数
		//由于整机电源功率的限制，这里的锁具供电必须串行，而不能一起给电。所以我们采用顺序供电的模式，每一个保持5秒，如果不能及时开锁，就断电。
		if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 1){
			ihu_usleep(200);
			ihu_l1hd_dido_f2board_lock1_do1_on();
			ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_CCL_BEEP_PATTERN_LOCK1_TRIGGER, 30);
			ihu_sleep(5);
			ihu_l1hd_dido_f2board_lock1_do1_off();
		}
		
		if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 2){
			ihu_usleep(200);
			ihu_l1hd_dido_f2board_lock2_do1_on();
			ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_CCL_BEEP_PATTERN_LOCK2_TRIGGER, 30);
			ihu_sleep(5);
			ihu_l1hd_dido_f2board_lock2_do1_off();
		}

		if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 4){
			ihu_usleep(200);
			ihu_l1hd_dido_f2board_lock3_do1_on();
			ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_CCL_BEEP_PATTERN_LOCK3_TRIGGER, 30);
			ihu_sleep(5);
			ihu_l1hd_dido_f2board_lock3_do1_off();
			
			ihu_usleep(200);
			ihu_l1hd_dido_f2board_lock4_do1_on();
			ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_CCL_BEEP_PATTERN_LOCK4_TRIGGER, 30);
			ihu_sleep(5);
			ihu_l1hd_dido_f2board_lock4_do1_off();
		}
		
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
	
	//关锁，不然凭空耗电，不值得
	//关门控制：独特的技巧，复用这些参数
	if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 1){
		ihu_usleep(200);
		ihu_l1hd_dido_f2board_lock1_do1_off();
	}
	if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 2){
		ihu_usleep(200);
		ihu_l1hd_dido_f2board_lock2_do1_off();
	}
	if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 4){
		ihu_usleep(200);
		ihu_l1hd_dido_f2board_lock3_do1_off();
		ihu_usleep(200);
		ihu_l1hd_dido_f2board_lock4_do1_off();
	}
	
	//发送消息给DIDO
	msg_struct_ccl_dido_door_close_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_ccl_dido_door_close_req_t));
	snd.length = sizeof(msg_struct_ccl_dido_door_close_req_t);
	snd.doorid = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
	if (ihu_message_send(MSG_ID_CCL_DIDO_DOOR_CLOSE_REQ, TASK_ID_DIDOCAP, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_DIDOCAP].taskName);

	//启动监控定时器：其实这个定时器可以被大定时代替，所以暂时不启动
	
	//启动摄像头，并将数据存入到CCL的上下文中
	//只启动了缺省的第一个摄像头。更多的摄像头启动并传输，等待后续需求的明确，以及业务流程的设计。
	//这里的关键点是，摄像头产生的图像数据太多，需要共享内存，然后按照顺序传输。如果的确需要，需要建立起内部交付的业务流程才可以多次拍摄
	ihu_dcmiaris_take_picture(0);
	
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
	//msg_struct_ccl_com_ctrl_cmd_t snd;
	msg_struct_ccl_sps_close_report_send_t snd1;
	
	//关锁，不然凭空耗电，不值得
	//关门控制：独特的技巧，复用这些参数
	if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 1){
		ihu_usleep(200);
		ihu_l1hd_dido_f2board_lock1_do1_off();
	}
	if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 2){
		ihu_usleep(200);
		ihu_l1hd_dido_f2board_lock2_do1_off();
	}
	if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX >= 4){
		ihu_usleep(200);
		ihu_l1hd_dido_f2board_lock3_do1_off();
		ihu_usleep(200);
		ihu_l1hd_dido_f2board_lock4_do1_off();
	}	
	
	//发送关门报告给后台
	memset(&snd1, 0, sizeof(msg_struct_ccl_sps_close_report_send_t));
	snd1.cause = IHU_SYSMSG_CCL_CLOSE_DOOR_NOT_YET_OPEN;
	snd1.length = sizeof(msg_struct_ccl_sps_close_report_send_t);
	if (ihu_message_send(MSG_ID_CCL_SPS_CLOSE_REPORT_SEND, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd1, snd1.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
	
	//返回
	return IHU_SUCCESS;
}

//门和锁均被正常关闭
OPSTAT fsm_ccl_door_close_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//int ret = 0;
	msg_struct_dido_ccl_door_close_event_t rcv;
	msg_struct_ccl_sps_close_report_send_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_dido_ccl_door_close_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_dido_ccl_door_close_event_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
		
	//发送关门报告给后台
	memset(&snd, 0, sizeof(msg_struct_ccl_sps_close_report_send_t));
	snd.cause = IHU_SYSMSG_CCL_CLOSE_DOOR_NORMAL;
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
	//msg_struct_ccl_com_ctrl_cmd_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_sps_ccl_close_report_cfm_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sps_ccl_close_report_cfm_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
			
	//关闭所有接口
	func_ccl_close_all_sensor_power();
	
	//停止定时器
	//实际上只有在FSM_STATE_CCL_DOOR_OPEN状态下才有意义，在FSM_STATE_CCL_FATAL_FAULT状态下这个定时器本来就没有被激活
	//考虑到即便定时器没有启动，停止一次也没关系，就不做判定了。这种强制停止，也可以避免其它差错的潜在可能性
	ret = ihu_timer_stop(TASK_ID_CCL, TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error start timer!\n");
	
	//延时并关断CPU系统
	ihu_sleep(2);
	func_cccl_cpu_power_off();	
	
	//返回
	return IHU_SUCCESS;
}

//触发激活系统到工作
OPSTAT fsm_ccl_hand_active_trigger_to_work(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	int ret = 0;
	int temp = 0;
	msg_struct_ccl_hand_active_trigger_t rcv;
	msg_struct_ccl_sps_open_auth_inq_t snd1;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_hand_active_trigger_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_hand_active_trigger_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);	
	
	//打开所有接口
	func_ccl_open_all_sensor_power();
	
////////////////////TEST CODE, TO BE DELETE//////////////	
	
	ihu_dcmiaris_take_picture(0);
	
	//测试倾角传感器
	ihu_usleep(200);
	
	if (ihu_vmmw_navig_mpu6050_init() == IHU_SUCCESS){
		//采用NF2结构
		ihu_usleep(200);
		temp = (UINT16)(ihu_wmmw_navig_mpu6050_axis_z_angle_caculate_by_static_method() * 100);
		IHU_DEBUG_PRINT_NOR("I2CARIES: Fall Angle Read Result = [%4.2f]\n", (float)(temp*1.0)/100.0);
	}else{
		IhuErrorPrint("I2CARIES: Fall Angle Read Result error!\n");
	}
	
////////////////////TEST CODE, TO BE DELETE//////////////	
	
	//启动定时器：如果是在工作模式下，允许被重复触发
	ret = ihu_timer_start(TASK_ID_CCL, TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Error start timer!\n");
			
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


//差错情形下，将状态报告给后台
//这里采用简单模式，不控制所有模块都到FAULT状态，而只是控制SPSVIRGO，因为它要发送后台
OPSTAT fsm_ccl_fault_state_trigger(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_ccl_fault_state_trigger_t rcv;
	msg_struct_ccl_sps_fault_report_send_t snd;
	
	//入参检查
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_fault_state_trigger_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_fault_state_trigger_t)))
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);	
	
	//打开所有接口
	func_ccl_open_all_sensor_power();
	
	//拉灯拉BEEP
	ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_CCL_BEEP_PATTERN_SYS_FAULT, 6);
				
	//发送后台报告
		//发送差错状态报告给后台
	memset(&snd, 0, sizeof(msg_struct_ccl_sps_fault_report_send_t));
	snd.cause = IHU_SYSMSG_CCL_FAULT_CAUSE_CLOSE_DOOR_TIME_OUT;
	snd.length = sizeof(msg_struct_ccl_sps_fault_report_send_t);
	if (ihu_message_send(MSG_ID_CCL_SPS_FAULT_REPORT_SEND, TASK_ID_SPSVIRGO, TASK_ID_CCL, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_CCL_RECOVERY("CCL: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CCL].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
	
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

	//关闭所有接口
	func_ccl_close_all_sensor_power();
	
	//延时并关断CPU系统
	ihu_sleep(5);
	func_cccl_cpu_power_off();		
	
	//返回
	return IHU_SUCCESS;
}

//打开所有的外设
void func_ccl_open_all_sensor_power(void)
{
	//GPRS模块必须先上电，然后RST拉低2S的方波
	ihu_l1hd_dido_f2board_gprsmod_power_supply_on();
	ihu_usleep(100);
	ihu_l1hd_dido_f2board_gprsmod_power_key_off();
	ihu_sleep(2);
	ihu_l1hd_dido_f2board_gprsmod_power_key_on();
	//BLE等外设电源
	ihu_l1hd_dido_f2board_sensor_power_ctrl_on();
	ihu_l1hd_dido_f2board_mq2_cam_power_ctrl_on();
	//未来需要确定，温度传感器等耗电的传感器是否在用GPIO控制开关在控
	//打开温湿度传感器
	ihu_l1hd_dido_f2board_dht11_init();
	//打开ADC的控制
	ihu_l1hd_adc1_start();
	//其它的传感器，角度传感器MPU6050_SPI，震动传感器SW420等初始化，RTC I2C，均留给程序自己去搞定初始化
}

//关掉所有的外设
void func_ccl_close_all_sensor_power(void)
{
	//GPRSMOD的关电：先关闭POWER_KEY，防止数据丢失，然后再下电
	ihu_l1hd_dido_f2board_gprsmod_power_key_off();
	ihu_sleep(2);
	ihu_l1hd_dido_f2board_gprsmod_power_key_on();
	ihu_usleep(100);
	ihu_l1hd_dido_f2board_gprsmod_power_supply_off();
	//BLE等外设关电
	ihu_l1hd_dido_f2board_sensor_power_ctrl_off();
	ihu_l1hd_dido_f2board_mq2_cam_power_ctrl_off();
}

//关掉CPU的电源
void func_cccl_cpu_power_off(void)
{
	//判定是人工触发
	if (zIhuCclTaskContext.handActFlag == TRUE){
		ihu_l1hd_dido_f2board_cpu_power_ctrl_off();
	}
	
	//是RTC触发下电
	else{
		func_vmmw_rtc_pcf8563_clear_af_and_power_off_cpu();
	}

	return;
}


//由于错误，直接关机，等待再次被激活
void func_ccl_stm_main_recovery_from_fault(void)
{
	//msg_struct_ccl_com_ctrl_cmd_t snd;

	//关闭所有外部器件的电源
	func_ccl_close_all_sensor_power();
	
	//延时并关断CPU系统
	ihu_sleep(2);
	func_cccl_cpu_power_off();
	
	//初始化模块的任务资源
	//初始化定时器：暂时决定不做，除非该模块重新RESTART
	//初始化模块级全局变量：暂时决定不做，除非该模块重新RESTART
	
	return;
}




