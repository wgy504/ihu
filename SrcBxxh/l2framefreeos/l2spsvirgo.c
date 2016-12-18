/**
 ****************************************************************************************
 *
 * @file l2spsvirgo.c
 *
 * @brief L2 SPSVIRGO
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
#include "stm32f2xx.h"
#include "l2spsvirgo.h"
 
/*
** FSM of the SPSVIRGO
*/
FsmStateItem_t FsmSpsvirgo[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            									fsm_spsvirgo_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            										fsm_spsvirgo_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            										fsm_spsvirgo_restart},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_SPSVIRGO_INITED,         						fsm_spsvirgo_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_SPSVIRGO_INITED,         						fsm_spsvirgo_restart},

	//Task level actived status
  {MSG_ID_COM_RESTART,        						FSM_STATE_SPSVIRGO_ACTIVED,         					fsm_spsvirgo_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_SPSVIRGO_ACTIVED,         					fsm_spsvirgo_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_time_out},
	{MSG_ID_SPS_L2FRAME_RCV,								FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_l2frame_rcv},
	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	{MSG_ID_CCL_SPS_OPEN_AUTH_INQ,					FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_ccl_open_auth_inq},
	{MSG_ID_CCL_COM_SENSOR_STATUS_REQ,			FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_ccl_sensor_status_req},
	{MSG_ID_CCL_SPS_EVENT_REPORT_SEND,			FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_ccl_event_report_send},
	{MSG_ID_CCL_COM_CTRL_CMD,								FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_ccl_ctrl_cmd},	
	{MSG_ID_CCL_SPS_FAULT_REPORT_SEND,			FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_ccl_fault_report_send},
	{MSG_ID_CCL_SPS_CLOSE_REPORT_SEND,			FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_ccl_close_door_report_send},
#endif
	
	//通信状态：为了让其它任务有个判断，到底系统是否还处于通信状态，还是正常的激活状态
	//特别是CCL中基于GPRS的通信，有可能超时非常长，必须等待和状态判定，不然会造成状态重入从而影响状态机的正常运转
  {MSG_ID_COM_RESTART,        						FSM_STATE_SPSVIRGO_COMMU,         						fsm_spsvirgo_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_SPSVIRGO_COMMU,         						fsm_spsvirgo_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_SPSVIRGO_COMMU,         				  	fsm_spsvirgo_time_out},
		
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	{MSG_ID_CCL_SPS_OPEN_AUTH_INQ,					FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_ccl_open_auth_inq},
	{MSG_ID_CCL_COM_SENSOR_STATUS_REQ,			FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_ccl_sensor_status_req},
	{MSG_ID_CCL_SPS_EVENT_REPORT_SEND,			FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_ccl_event_report_send},
	{MSG_ID_CCL_COM_CTRL_CMD,								FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_ccl_ctrl_cmd},	
	{MSG_ID_CCL_SPS_FAULT_REPORT_SEND,			FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_ccl_fault_report_send},
	{MSG_ID_CCL_SPS_CLOSE_REPORT_SEND,			FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_ccl_close_door_report_send},
#endif

	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
//extern vu8 SPS_GPRS_R_Buff[SPS_GPRS_REC_MAXLEN];	//串口1数据接收缓冲区 
//extern vu8 SPS_GPRS_R_State;						//串口1接收状态
//extern vu16 SPS_GPRS_R_Count;						//当前接收数据的字节数 	 
//extern vu8 SPS_RFID_R_Buff[SPS_RFID_REC_MAXLEN];	//串口1数据接收缓冲区 
//extern vu8 SPS_RFID_R_State;						//串口1接收状态
//extern vu16 SPS_RFID_R_Count;						//当前接收数据的字节数 	 
//extern vu8 SPS_BLE_R_Buff[SPS_BLE_REC_MAXLEN];	//串口1数据接收缓冲区 
//extern vu8 SPS_BLE_R_State;						//串口1接收状态
//extern vu16 SPS_BLE_R_Count;						//当前接收数据的字节数 	 
//extern vu8 SPS_SPARE1_R_Buff[SPS_SPARE1_REC_MAXLEN];	//串口1数据接收缓冲区 
//extern vu8 SPS_SPARE1_R_State;						//串口1接收状态
//extern vu16 SPS_SPARE1_R_Count;						//当前接收数据的字节数
UINT8 zIhuGprsOperationFlag = 0;
strIhuCclSpsPar_t zIhuCclSpsvirgoCtrlTable;
#else
#endif


//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_spsvirgo_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("SPSVIRGO: Error Set FSM State at fsm_spsvirgo_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_spsvirgo_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//串行回送INIT_FB给VMFO
	ihu_usleep(dest_id * IHU_MODULE_START_DISTRIBUTION_DELAY_DURATION);
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VMFO
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_SPSVIRGO, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_SPSVIRGO], zIhuTaskNameList[src_id]);
			return IHU_FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_INITED) == IHU_FAILURE){
		IhuErrorPrint("SPSVIRGO: Error Set FSM State!\n");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_spsvirgo_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("SPSVIRGO: Error initialize interface!\n");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_SPSVIRGO] = 0;
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	zIhuGprsOperationFlag = 0;
	memset(&zIhuCclSpsvirgoCtrlTable, 0, sizeof(strIhuCclSpsPar_t));
	zIhuCclSpsvirgoCtrlTable.cclSpsWorkingMode = IHU_CCL_SPS_WORKING_MODE_SLEEP;  //初始化就进入SLEEP，然后就看是否有触发
#else
#endif
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//启动本地定时器，如果有必要
	//测试性启动周期性定时器
	ret = ihu_timer_start(TASK_ID_SPSVIRGO, TIMER_ID_1S_SPSVIRGO_PERIOD_SCAN, zIhuSysEngPar.timer.spsvirgoPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Error start timer!\n");
		return IHU_FAILURE;
	}	
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("SPSVIRGO: Enter FSM_STATE_SPSVIRGO_ACTIVE status, Keeping refresh here!\n");
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_spsvirgo_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("SPSVIRGO: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_spsvirgo_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_spsvirgo_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_SPSVIRGO)){
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_spsvirgo_hw_init(void)
{
	//测试目的，PA9/PA10 = TX/RX
//	ihu_l1hd_rtc_init(); //初始化时钟
//	ihu_l1hd_sps_gprs_init_config(115200);//USART_GPRS初始化配置
//	ihu_l1hd_sps_rfid_init_config(115200);//USART_RFID初始化配置
//	ihu_l1hd_sps_ble_init_config(115200);//UART_BLE初始化配置
//	ihu_l1hd_sps_spare1_init_config(115200);//UART_SPARE1初始化配置
//	TIM_USART_GPRS_Init_Config();  //初始化跟GPRS时钟相关的TIM
	ihu_l1hd_sps_slave_hw_init();
	return IHU_SUCCESS;
}

//TIMER_OUT Processing
OPSTAT fsm_spsvirgo_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("SPSVIRGO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuRunErrCnt[TASK_ID_SPSVIRGO]是否超限
	if (zIhuRunErrCnt[TASK_ID_SPSVIRGO] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuRunErrCnt[TASK_ID_SPSVIRGO] = zIhuRunErrCnt[TASK_ID_SPSVIRGO] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_SPSVIRGO, TASK_ID_SPSVIRGO, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
			IhuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_SPSVIRGO], zIhuTaskNameList[TASK_ID_SPSVIRGO]);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_SPSVIRGO_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_SPSVIRGO) != FSM_STATE_SPSVIRGO_ACTIVED){
			ret = FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error Set FSM State!\n");
				return IHU_FAILURE;
			}//FsmSetState
		}
		func_spsvirgo_time_out_period_scan();
	}

	return IHU_SUCCESS;
}

void func_spsvirgo_time_out_period_scan(void)
{
	int ret = 0;
	
	//发送HeartBeat消息给VMFO模块，实现喂狗功能
	msg_struct_com_heart_beat_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_heart_beat_t));
	snd.length = sizeof(msg_struct_com_heart_beat_t);
	ret = ihu_message_send(MSG_ID_COM_HEART_BEAT, TASK_ID_VMFO, TASK_ID_SPSVIRGO, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_SPSVIRGO], zIhuTaskNameList[TASK_ID_VMFO]);
		return;
	}
	
	ihu_l1hd_sps_gprs_send_data("This is my GPRS test!\n", 20);
//	ihu_l1hd_sps_rfid_send_data("This is my RFID test!\n", 20);
//	ihu_l1hd_sps_ble_send_data("This is my BLE test!\n", 20);
//	ihu_l1hd_sps_spare1_send_data("This is my SPARE1 test!\n", 30);

	zIhuGprsOperationFlag++;
	
	//不干、只干活一次
	//if (zIhuGprsOperationFlag == 0)	GPRS_UART_GSM_working_procedure_selection(2, 0);
	
	//IhuDebugPrint("SPSVIRGO: Time Out Test!\n");
	
	return;	
}

//L2FRAME Receive Processing
OPSTAT fsm_spsvirgo_l2frame_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret;
	msg_struct_spsvirgo_l2frame_rcv_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_spsvirgo_l2frame_rcv_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_spsvirgo_l2frame_rcv_t))){
		IhuErrorPrint("SPSVIRGO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//对于缓冲区的数据，进行分别处理，将帧变成不同的消息，分门别类发送到L3模块进行处理
	//MSG_ID_SPS_TO_CCL_CLOUD_FB
	
	
	return IHU_SUCCESS;
}

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
OPSTAT fsm_spsvirgo_ccl_open_auth_inq(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_ccl_sps_open_auth_inq rcv;
	msg_struct_spsvirgo_ccl_cloud_fb_t snd;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_sps_open_auth_inq));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_sps_open_auth_inq))){
		IhuErrorPrint("SPSVIRGO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//先进入通信状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_COMMU);
	
	//对接收到的上层命令进行分解处理
	
	//干活
	ihu_sleep(2);
	//GPRS_UART_GSM_working_procedure_selection(2, 0);
	//这里有个挺有意思的现象：这里的命令还未执行完成，实际上后台的数据已经通过UART回来了，并通过ISR服务程序发送到SPSVIRGO的QUEUE中，但只有这里执行结束后，
	//才会去接那个消息并执行结果。当然也存在着不正确或者没有结果的情况，那就靠CCL的状态机进行恢复了。

		
	//再进入正常状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED);
	
	//干完了之后，结果发送给CCL
	memset(&snd, 0, sizeof(msg_struct_spsvirgo_ccl_cloud_fb_t));
	snd.authResult = ((rand()%2 == 1)?IHU_CCL_LOCK_AUTH_RESULT_OK:IHU_CCL_LOCK_AUTH_RESULT_NOK);
	snd.length = sizeof(msg_struct_spsvirgo_ccl_cloud_fb_t);
	ret = ihu_message_send(MSG_ID_SPS_CCL_CLOUD_FB, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_SPSVIRGO], zIhuTaskNameList[TASK_ID_CCL]);
		return IHU_FAILURE;
	}

	return IHU_SUCCESS;
}

OPSTAT fsm_spsvirgo_ccl_sensor_status_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_ccl_com_sensor_status_req_t rcv;
	msg_struct_sps_ccl_sensor_status_rep_t snd;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_com_sensor_status_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_com_sensor_status_req_t))){
		IhuErrorPrint("SPSVIRGO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_REQ_STATUS_SPS){
		IhuErrorPrint("SPSVIRGO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	
	//具体扫描处理
	ihu_usleep(300);
	
	//扫描后将结果发给上层
	memset(&snd, 0, sizeof(msg_struct_sps_ccl_sensor_status_rep_t));
	snd.cmdid = IHU_CCL_DH_CMDID_RESP_STATUS_SPS;
	snd.sensor.rssiValue = rand()%100;
	snd.length = sizeof(msg_struct_sps_ccl_sensor_status_rep_t);
	ret = ihu_message_send(MSG_ID_SPS_CCL_SENSOR_STATUS_RESP, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_SPSVIRGO], zIhuTaskNameList[TASK_ID_CCL]);
		return IHU_FAILURE;
	}
			
	//返回
	return IHU_SUCCESS;
}

//发送周期性报告给后台
OPSTAT fsm_spsvirgo_ccl_event_report_send(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_ccl_sps_event_report_send_t rcv;
	msg_struct_sps_ccl_event_report_cfm_t snd;
		
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_sps_event_report_send_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_sps_event_report_send_t))){
		IhuErrorPrint("SPSVIRGO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_EVENT_REPORT){
		IhuErrorPrint("SPSVIRGO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	
	//先进入通信状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_COMMU);

	//干活
	//具体的发送命令
	ihu_sleep(2);
	
	//再进入正常状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED);

	//干完了之后，结果发送给CCL
	memset(&snd, 0, sizeof(msg_struct_sps_ccl_event_report_cfm_t));
	snd.length = sizeof(msg_struct_sps_ccl_event_report_cfm_t);
	ret = ihu_message_send(MSG_ID_SPS_CCL_EVENT_REPORT_CFM, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_SPSVIRGO], zIhuTaskNameList[TASK_ID_CCL]);
		return IHU_FAILURE;
	}	
	
	//返回
	return IHU_SUCCESS;
}


//控制状态
OPSTAT fsm_spsvirgo_ccl_ctrl_cmd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret;
	msg_struct_ccl_com_ctrl_cmd_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_com_ctrl_cmd_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_com_ctrl_cmd_t))){
		IhuErrorPrint("SPSVIRGO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//操作状态
	if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_ACTIVE) zIhuCclSpsvirgoCtrlTable.cclSpsWorkingMode = IHU_CCL_SPS_WORKING_MODE_ACTIVE;
	else if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_SLEEP) zIhuCclSpsvirgoCtrlTable.cclSpsWorkingMode = IHU_CCL_SPS_WORKING_MODE_SLEEP;
	else if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_FAULT) zIhuCclSpsvirgoCtrlTable.cclSpsWorkingMode = IHU_CCL_SPS_WORKING_MODE_FAULT;
	else{
		IhuErrorPrint("DIDOCAP: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_DIDOCAP]++;
		return IHU_FAILURE;		
	}
	
	//返回
	return IHU_SUCCESS;
}

//发送差错报告后的证实给CCL
OPSTAT fsm_spsvirgo_ccl_fault_report_send(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_ccl_sps_fault_report_send_t rcv;
	msg_struct_sps_ccl_fault_report_cfm_t snd;

	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_sps_fault_report_send_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_sps_fault_report_send_t))){
		IhuErrorPrint("SPSVIRGO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//先进入通信状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_COMMU);
	
	//干活
	//具体的发送命令
	ihu_sleep(2);

	//再进入正常状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED);
	
	//干完了之后，结果发送给CCL
	memset(&snd, 0, sizeof(msg_struct_sps_ccl_fault_report_cfm_t));
	snd.length = sizeof(msg_struct_sps_ccl_fault_report_cfm_t);
	ret = ihu_message_send(MSG_ID_SPS_CCL_FAULT_REPORT_CFM, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_SPSVIRGO], zIhuTaskNameList[TASK_ID_CCL]);
		return IHU_FAILURE;
	}	

	//返回
	return IHU_SUCCESS;
}


//发送关门报告后的证实给CCL
OPSTAT fsm_spsvirgo_ccl_close_door_report_send(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_ccl_sps_close_report_send_t rcv;
	msg_struct_sps_ccl_close_report_cfm_t snd;

	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_sps_close_report_send_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_sps_close_report_send_t))){
		IhuErrorPrint("SPSVIRGO: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//先进入通信状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_COMMU);
	
	//干活
	//具体的发送命令
	ihu_sleep(2);
	
	//再进入正常状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED);
	
	//干完了之后，结果发送给CCL
	memset(&snd, 0, sizeof(msg_struct_sps_ccl_close_report_cfm_t));
	snd.length = sizeof(msg_struct_sps_ccl_close_report_cfm_t);
	ret = ihu_message_send(MSG_ID_SPS_CCL_CLOSE_REPORT_CFM, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_SPSVIRGO], zIhuTaskNameList[TASK_ID_CCL]);
		return IHU_FAILURE;
	}	

	//返回
	return IHU_SUCCESS;
}

#endif


