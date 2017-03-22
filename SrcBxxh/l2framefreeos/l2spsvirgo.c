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
IhuFsmStateItem_t IhuFsmSpsvirgo[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            									fsm_spsvirgo_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            										fsm_spsvirgo_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_IDLE,            							      fsm_com_do_nothing},

  //Task level initialization
  {MSG_ID_COM_INIT,       								FSM_STATE_SPSVIRGO_INITED,            				fsm_spsvirgo_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_SPSVIRGO_INITED,            				fsm_com_do_nothing},

	//ANY state entry
  {MSG_ID_COM_INIT_FB,                    FSM_STATE_COMMON,                           fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,                 FSM_STATE_COMMON,                           fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,              FSM_STATE_COMMON,                           fsm_com_do_nothing},
	{MSG_ID_COM_STOP,                       FSM_STATE_COMMON,                           fsm_spsvirgo_stop_rcv},
  {MSG_ID_COM_RESTART,                    FSM_STATE_COMMON,                           fsm_spsvirgo_restart},
	{MSG_ID_COM_TIME_OUT,                   FSM_STATE_COMMON,                           fsm_spsvirgo_time_out},

	//Task level actived status
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
	//在这个状态下，还不允许有新的L2FRAME收到
  {MSG_ID_COM_RESTART,        						FSM_STATE_SPSVIRGO_COMMU,         						fsm_spsvirgo_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_SPSVIRGO_COMMU,         						fsm_spsvirgo_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_SPSVIRGO_COMMU,         				  	fsm_spsvirgo_time_out},
		
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	UINT8 zIhuGprsOperationFlag = 0;
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	strIhuCclSpsPar_t zIhuCclSpsvirgoCtrlTable;
	msg_struct_ccl_com_cloud_data_rx_t zIhuSpsvirgoMsgRcvBuf;
#else
	#error Un-correct constant definition
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
	ihu_usleep(dest_id * IHU_SYSCFG_MODULE_START_DISTRIBUTION_DELAY_DURATION);
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VMFO
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_SPSVIRGO, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[src_id].taskName);
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
	zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPSVIRGO] = 0;
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	memset(&zIhuCclSpsvirgoCtrlTable, 0, sizeof(strIhuCclSpsPar_t));
	memset(&zIhuSpsvirgoMsgRcvBuf, 0, sizeof(msg_struct_ccl_com_cloud_data_rx_t));
	zIhuCclSpsvirgoCtrlTable.cclSpsWorkingMode = IHU_CCL_SPS_WORKING_MODE_SLEEP;  //初始化就进入SLEEP，然后就看是否有触发
#else
	#error Un-correct constant definition
#endif
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED) == IHU_FAILURE)
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Error Set FSM State!\n");

	if (IHU_SPSVIRGO_PERIOD_TIMER_SET == IHU_SPSVIRGO_PERIOD_TIMER_ACTIVE){
		//测试性启动周期性定时器：正式工作后可以删掉这个工作逻辑机制
		ret = ihu_timer_start(TASK_ID_SPSVIRGO, TIMER_ID_1S_SPSVIRGO_PERIOD_SCAN, \
			zIhuSysEngPar.timer.array[TIMER_ID_1S_SPSVIRGO_PERIOD_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Error start timer!\n");
	}
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("SPSVIRGO: Enter FSM_STATE_SPSVIRGO_ACTIVE status, Keeping refresh here!\n");
	}
	
	//生成测试数据
	//func_cloud_standard_xml_generate_message_test_data();
	
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
	if ((param_ptr == NULL) || (dest_id != TASK_ID_SPSVIRGO))
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Wrong input paramters!\n");
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_IDLE) == IHU_FAILURE)
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Error Set FSM State!\n");
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_spsvirgo_hw_init(void)
{
	ihu_l1hd_sps_slave_hw_init();
	//在CCL项目中，没有SPI模块，SPI接口用来完成RFID外设的读取，所以初始化需要挂载这里
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	ihu_l1hd_spi_slave_hw_init();
#endif
	return IHU_SUCCESS;
}

//TIMER_OUT Processing
OPSTAT fsm_spsvirgo_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t)))
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPSVIRGO]是否超限
	if (zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPSVIRGO] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPSVIRGO] = zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPSVIRGO] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		if (ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_SPSVIRGO, TASK_ID_SPSVIRGO, &snd0, snd0.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_SPSVIRGO_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_SPSVIRGO) != FSM_STATE_SPSVIRGO_ACTIVED){
			if (FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED) == IHU_FAILURE)
				IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Error Set FSM State!\n");
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
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_VMFO].taskName);
		return;
	}
	
	return;	
}

//L2FRAME Receive Processing
OPSTAT fsm_spsvirgo_l2frame_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_spsvirgo_l2frame_rcv_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_spsvirgo_l2frame_rcv_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_spsvirgo_l2frame_rcv_t)))
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)	
	//对于缓冲区的数据，进行分别处理，将帧变成不同的消息，分门别类发送到L3模块进行处理
	//注意，这里是CHAR数据，不是L2FRAME的比特数据
	msg_struct_ccl_com_cloud_data_rx_t *pMsgBuf;
	memset(pMsgBuf, 0, sizeof(msg_struct_ccl_com_cloud_data_rx_t));
	pMsgBuf = (msg_struct_ccl_com_cloud_data_rx_t *)(&rcv);
	pMsgBuf->length = rcv.length;
	//忽略期望的消息，任何返回消息都是可以的
	ret = func_cloud_standard_xml_unpack(pMsgBuf, -1);
	//差错情形，由发送者自己控制，因为已经是双向通信机制，非互锁的通信机制，这里无法保证出错时必须送回信号给发送者
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Unpack and processing receiving message error!\n");
	//后面对于收到的每一个消息的基础处理，都由CCL的消息处理函数具体完成
#endif
	
	//返回
	return IHU_SUCCESS;
}

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
OPSTAT fsm_spsvirgo_ccl_open_auth_inq(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_ccl_sps_open_auth_inq_t rcv;
	msg_struct_spsvirgo_ccl_cloud_fb_t snd;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_sps_open_auth_inq_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_sps_open_auth_inq_t))){
		IhuErrorPrint("SPSVIRGO: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//先进入通信状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_COMMU);
	
	//对接收到的上层命令进行分解处理，并组装消息发送给后台
	StrMsg_HUITP_MSGID_uni_ccl_lock_auth_inq_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_auth_inq_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_ccl_lock_auth_inq>>8)&0xFF;
	pMsgProc.msgId.optId = HUITP_MSGID_uni_ccl_lock_auth_inq&0xFF;
	pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	//StrIe_HUITP_IEID_uni_com_req_t
	pMsgProc.baseReq.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_req);
	pMsgProc.baseReq.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4);
	pMsgProc.baseReq.comReq = HUITP_IEID_UNI_COM_REQUEST_YES;
	//StrIe_HUITP_IEID_uni_ccl_lock_auth_req_t
	pMsgProc.authReq.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_lock_auth_req);
	pMsgProc.authReq.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_lock_auth_req_t) - 4);
	
	//统一考虑三种触发方式：BLE/RFID/LOCK, 后台会再加上PID/电话号码方式，总共5种方式
	
	//不用使用透传工作方式，而采用ATCMD方式
	//if (ihu_vmmw_blemod_hc05_uart_fetch_mac_addr_in_transparant_mode(pMsgProc.authReq.bleMacAddr, 6) == IHU_SUCCESS){
	if (ihu_vmmw_blemod_hc05_uart_fetch_mac_addr_in_AT_cmd_mode(pMsgProc.authReq.bleMacAddr, 6) == IHU_SUCCESS){
		pMsgProc.authReq.bleAddrLen = 6;
		pMsgProc.authReq.authReqType = HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_TYPE_BLE;
		IHU_DEBUG_PRINT_IPT("SPSVORGO: MAC Address read = [%x:%x:%x:%x:%x:%x]\n", pMsgProc.authReq.bleMacAddr[0], pMsgProc.authReq.bleMacAddr[1],  pMsgProc.authReq.bleMacAddr[2],  pMsgProc.authReq.bleMacAddr[3],  pMsgProc.authReq.bleMacAddr[4],  pMsgProc.authReq.bleMacAddr[5]); 
	}
	
	//RFID DEVICE，将根据实际板子，只配置使用一种，这里是为了两种硬件的测试而已
	//未来需要禁止掉一个，只留下一个
	
	//采用无源RFID待调测=>目前的长度为16
	else if (ihu_vmmw_rfidmod_mf522_spi_read_id(pMsgProc.authReq.rfidAddr, IHU_SPSVIRGO_RFID_LEN_FIX) == IHU_SUCCESS){
		pMsgProc.authReq.rfidAddrLen = IHU_SPSVIRGO_RFID_LEN_FIX;
		pMsgProc.authReq.authReqType = HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_TYPE_RFID;
	}

	//采用有源RFID待调测=>目前的长度为16
	else if (ihu_vmmw_rfidmod_nrf24l01_spi_read_id(pMsgProc.authReq.rfidAddr, IHU_SPSVIRGO_RFID_LEN_FIX) == IHU_SUCCESS){
		pMsgProc.authReq.rfidAddrLen = IHU_SPSVIRGO_RFID_LEN_FIX;
		pMsgProc.authReq.authReqType = HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_TYPE_RFID;
	}
	
	else{
		pMsgProc.authReq.authReqType = HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_TYPE_LOCK;	
	}
	//Pack message
	StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
	memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
	memcpy(&pMsgInput, &pMsgProc, msgProcLen);
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));	
	ret = func_cloud_standard_xml_pack(HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID, NULL, HUITP_MSGID_uni_ccl_lock_auth_inq, &pMsgInput, msgProcLen, &pMsgOutput);
	if (ret == IHU_FAILURE){
		FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED);
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Package message error!\n");
	}
	
	//将组装好的消息发送到GPRSMOD模组中去，送往后台
	memset(&zIhuSpsvirgoMsgRcvBuf, 0, sizeof(msg_struct_ccl_com_cloud_data_rx_t));
	//ret = ihu_vmmw_gprsmod_http_data_transmit_with_receive((char *)(pMsgOutput.buf), pMsgOutput.bufferLen, zIhuSpsvirgoMsgRcvBuf.buf, &(zIhuSpsvirgoMsgRcvBuf.length));	
	ret = ihu_vmmw_gprsmod_tcp_text_data_transmit_with_receive((char *)(pMsgOutput.buf), pMsgOutput.bufferLen, zIhuSpsvirgoMsgRcvBuf.buf, &(zIhuSpsvirgoMsgRcvBuf.length));	
	
	
	//这里有个挺有意思的现象：这里的命令还未执行完成，实际上后台的数据已经通过UART回来了，并通过ISR服务程序发送到SPSVIRGO的QUEUE中，但只有这里执行结束后，
	//才会去接那个消息并执行结果。当然也存在着不正确或者没有结果的情况，那就靠CCL的状态机进行恢复了。
	//目前在GPRSMOD工作状态下，我们采用了半双工查询的方式，所以必然是主动收数据，而非通过ISR直接回送
		
	//再进入正常状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED);
	
	//如果RET成功，意味着USART从ISR通道将成功的消息发送给了L3，如果失败了，这里必须发送失败回馈消息给上层，不然上层就死在了当前的状态机中
	//AT CMD采用了半双工通信方式 - 下位机主动发送消息给后台，然后等反馈，这个反馈必然等得来，要么立即成功，要么立即失败或者超时失败。
	//所以，这里的设计机制采用反馈的方式，将使得L3不需要为这个单设超时时钟，有利于L3状态机设计的简化
	//如果上层L3还有超时机制，那就是双保险了
	//从GPRSMOD发送AT CMD采用了同步互锁机制，所以必然会有反馈
	if (ret == IHU_FAILURE){
		memset(&snd, 0, sizeof(msg_struct_spsvirgo_ccl_cloud_fb_t));
		//如果采用随机工作方式
		//snd.authResult = ((rand()%2 == 1)?IHU_CCL_LOCK_AUTH_RESULT_OK:IHU_CCL_LOCK_AUTH_RESULT_NOK);
		snd.authResult = IHU_CCL_LOCK_AUTH_RESULT_NOK;
		snd.length = sizeof(msg_struct_spsvirgo_ccl_cloud_fb_t);
		ret = ihu_message_send(MSG_ID_SPS_CCL_CLOUD_FB, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
	}
	
	//如果从后台收到有价值的反馈
	else{
		//对于缓冲区的数据，进行分别处理，将帧变成不同的消息，分门别类发送到L3模块进行处理
		//注意，这里是CHAR数据，不是L2FRAME的比特数据
		ret = func_cloud_standard_xml_unpack(&zIhuSpsvirgoMsgRcvBuf, HUITP_MSGID_uni_ccl_lock_auth_resp);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPSVIRGO]++;
			IhuErrorPrint("SPSVIRGO: Unpack and processing receiving message error!\n");
			
			//不能直接返回差错，因为上层还巴巴的等着回复这个消息，不然状态机会出错
			memset(&snd, 0, sizeof(msg_struct_spsvirgo_ccl_cloud_fb_t));
			snd.authResult = IHU_CCL_LOCK_AUTH_RESULT_NOK;
			snd.length = sizeof(msg_struct_spsvirgo_ccl_cloud_fb_t);
			ret = ihu_message_send(MSG_ID_SPS_CCL_CLOUD_FB, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
			if (ret == IHU_FAILURE)
				IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
		}
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_spsvirgo_ccl_sensor_status_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_ccl_com_sensor_status_req_t rcv;
	msg_struct_sps_ccl_sensor_status_rep_t snd;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_com_sensor_status_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_com_sensor_status_req_t)))
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_REQ_STATUS_SPS)
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Receive message error!\n");
	
	//具体扫描处理
	ihu_usleep(300);
	
	//扫描后将结果发给上层
	memset(&snd, 0, sizeof(msg_struct_sps_ccl_sensor_status_rep_t));
	snd.cmdid = IHU_CCL_DH_CMDID_RESP_STATUS_SPS;
	snd.sensor.rssiValue = rand()%100;
	snd.length = sizeof(msg_struct_sps_ccl_sensor_status_rep_t);
	ret = ihu_message_send(MSG_ID_SPS_CCL_SENSOR_STATUS_RESP, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
			
	//返回
	return IHU_SUCCESS;
}

//发送周期性报告给后台
OPSTAT fsm_spsvirgo_ccl_event_report_send(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0, i = 0;
	msg_struct_ccl_sps_event_report_send_t rcv;
	msg_struct_sps_ccl_event_report_cfm_t snd;
		
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_sps_event_report_send_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_sps_event_report_send_t)))
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//入参检查
	if (rcv.cmdid != IHU_CCL_DH_CMDID_EVENT_REPORT)
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Receive message error!\n");
	
	//先进入通信状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_COMMU);

	//对接收到的上层命令进行分解处理，并组装消息发送给后台
	StrMsg_HUITP_MSGID_uni_ccl_state_report_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_report_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_ccl_state_report>>8)&0xFF;
	pMsgProc.msgId.optId = HUITP_MSGID_uni_ccl_state_report&0xFF;
	pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	//StrIe_HUITP_IEID_uni_com_report_t
	pMsgProc.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);
	pMsgProc.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);
	pMsgProc.baseReport.comReport = HUITP_IEID_UNI_COM_REPORT_YES;
	//StrIe_HUITP_IEID_uni_ccl_report_type_t
	pMsgProc.reportType.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_report_type);
	pMsgProc.reportType.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_report_type_t) - 4);
	pMsgProc.reportType.event = HUITP_IEID_UNI_CCL_REPORT_TYPE_PERIOD_EVENT;
	//StrIe_HUITP_IEID_uni_ccl_lock_state_t
	pMsgProc.lockState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_lock_state);
	pMsgProc.lockState.ieLen = HUITP_ENDIAN_EXG16(2 + IHU_CCL_SENSOR_LOCK_NUMBER_MAX); //实际上配置的锁的多少
	pMsgProc.lockState.maxLockNo = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
	pMsgProc.lockState.lockId = IHU_CCL_SENSOR_LOCK_NUMBER_MAX; //这个表示全部
	for (i = 0; i < IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
		pMsgProc.lockState.lockState[i] = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_lock_open_state(i) == TRUE)?HUITP_IEID_UNI_LOCK_STATE_OPEN:HUITP_IEID_UNI_LOCK_STATE_CLOSE);
	}
	//StrIe_HUITP_IEID_uni_ccl_door_state_t
	pMsgProc.doorState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_lock_state);
	pMsgProc.doorState.ieLen = HUITP_ENDIAN_EXG16(2 + IHU_CCL_SENSOR_LOCK_NUMBER_MAX); //实际上配置的门的多少
	pMsgProc.doorState.maxDoorNo = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
	pMsgProc.doorState.doorId = IHU_CCL_SENSOR_LOCK_NUMBER_MAX; //这个表示全部
	for (i = 0; i < IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
		pMsgProc.doorState.doorState[i] = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_door_open_state(i) == TRUE)?HUITP_IEID_UNI_DOOR_STATE_OPEN:HUITP_IEID_UNI_DOOR_STATE_CLOSE);
	}
	//StrIe_HUITP_IEID_uni_ccl_water_state_t
	pMsgProc.waterState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_water_state);
	pMsgProc.waterState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_water_state_t) - 4);
	pMsgProc.waterState.waterState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_water_state() == TRUE)?HUITP_IEID_UNI_WATER_STATE_ACTIVE:HUITP_IEID_UNI_WATER_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_fall_state_t
	pMsgProc.fallState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_fall_state);
	pMsgProc.fallState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_fall_state_t) - 4);
	pMsgProc.fallState.fallState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_fall_state() == TRUE)?HUITP_IEID_UNI_FALL_STATE_ACTIVE:HUITP_IEID_UNI_FALL_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_shake_state_t
	pMsgProc.shakeState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_shake_state);
	pMsgProc.shakeState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_shake_state_t) - 4);
	pMsgProc.shakeState.shakeState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_shake_state() == TRUE)?HUITP_IEID_UNI_SHAKE_STATE_ACTIVE:HUITP_IEID_UNI_SHAKE_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_smoke_state_t
	pMsgProc.smokeState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_smoke_state);
	pMsgProc.smokeState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_smoke_state_t) - 4);
	pMsgProc.smokeState.smokeState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_smoke_state() == TRUE)?HUITP_IEID_UNI_SMOKE_STATE_ACTIVE:HUITP_IEID_UNI_SMOKE_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_bat_state_t
	pMsgProc.batState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_bat_state);
	pMsgProc.batState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_bat_state_t) - 4);
	pMsgProc.batState.batState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_bat_state() == TRUE)?HUITP_IEID_UNI_BAT_STATE_WARNING:HUITP_IEID_UNI_BAT_STATE_NORMAL);
	//StrIe_HUITP_IEID_uni_ccl_temp_value_t
	pMsgProc.tempValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_temp_value);
	pMsgProc.tempValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_temp_value_t) - 4);
	pMsgProc.tempValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.tempValue.tempValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_temp_value());
	//StrIe_HUITP_IEID_uni_ccl_humid_value_t
	pMsgProc.humidValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_humid_value);
	pMsgProc.humidValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_humid_value_t) - 4);
	pMsgProc.humidValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.humidValue.humidValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_humid_value());
	//StrIe_HUITP_IEID_uni_ccl_bat_value_t
	pMsgProc.batValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_bat_value);
	pMsgProc.batValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_bat_value_t) - 4);
	pMsgProc.batValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.batValue.batValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_bat_value());	
	//StrIe_HUITP_IEID_uni_ccl_general_value1_t
	pMsgProc.general1Value.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_general_value1);
	pMsgProc.general1Value.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_general_value1_t) - 4);
	pMsgProc.general1Value.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.general1Value.generalValue1 = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rsv1_value());	
	//StrIe_HUITP_IEID_uni_ccl_general_value2_t
	pMsgProc.general2Value.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_general_value2);
	pMsgProc.general2Value.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_general_value2_t) - 4);
	pMsgProc.general2Value.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.general2Value.generalValue2 = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rsv2_value());	
	//StrIe_HUITP_IEID_uni_ccl_rssi_value_t
	pMsgProc.rssiValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_rssi_value);
	pMsgProc.rssiValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_rssi_value_t) - 4);
	pMsgProc.rssiValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.rssiValue.rssiValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rssi_value());	
	//StrIe_HUITP_IEID_uni_ccl_dcmi_value_t
	pMsgProc.dcmiValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_dcmi_value);
	pMsgProc.dcmiValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_dcmi_value_t) - 4);
	pMsgProc.dcmiValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.dcmiValue.dcmiValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_dcmi_value());	

	//Pack message
	StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
	memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
	memcpy(&pMsgInput, &pMsgProc, msgProcLen);
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));	
	ret = func_cloud_standard_xml_pack(HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID, NULL, HUITP_MSGID_uni_ccl_state_report, &pMsgInput, msgProcLen, &pMsgOutput);
	if (ret == IHU_FAILURE){
		FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED);
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Package message error!\n");
	}

	//具体的发送命令
	memset(&zIhuSpsvirgoMsgRcvBuf, 0, sizeof(msg_struct_ccl_com_cloud_data_rx_t));
	ret = ihu_vmmw_gprsmod_http_data_transmit_with_receive((char *)(pMsgOutput.buf), pMsgOutput.bufferLen, zIhuSpsvirgoMsgRcvBuf.buf, &(zIhuSpsvirgoMsgRcvBuf.length));	
	
	//再进入正常状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED);

	//如果干活失败，则发送差错消息回L3
	if (ret == IHU_FAILURE){
		memset(&snd, 0, sizeof(msg_struct_sps_ccl_event_report_cfm_t));
		snd.actionFlag = IHU_SYSMSG_CCL_EVENT_REPORT_SEND_FAILURE;
		snd.length = sizeof(msg_struct_sps_ccl_event_report_cfm_t);
		ret = ihu_message_send(MSG_ID_SPS_CCL_EVENT_REPORT_CFM, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
	}	

	//如果从后台收到有价值的反馈
	else{
		//对于缓冲区的数据，进行分别处理，将帧变成不同的消息，分门别类发送到L3模块进行处理
		//注意，这里是CHAR数据，不是L2FRAME的比特数据
		ret = func_cloud_standard_xml_unpack(&zIhuSpsvirgoMsgRcvBuf, HUITP_MSGID_uni_ccl_state_confirm);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPSVIRGO]++;
			IhuErrorPrint("SPSVIRGO: Unpack and processing receiving message error!\n");
			//不能直接返回差错，因为上层还巴巴的等着回复这个消息，不然状态机会出错
			memset(&snd, 0, sizeof(msg_struct_sps_ccl_event_report_cfm_t));
			snd.actionFlag = IHU_SYSMSG_CCL_EVENT_REPORT_SEND_FAILURE;
			snd.length = sizeof(msg_struct_sps_ccl_event_report_cfm_t);
			ret = ihu_message_send(MSG_ID_SPS_CCL_EVENT_REPORT_CFM, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
			if (ret == IHU_FAILURE)
				IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
		}
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
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_com_ctrl_cmd_t)))
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//操作状态
	if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_ACTIVE) zIhuCclSpsvirgoCtrlTable.cclSpsWorkingMode = IHU_CCL_SPS_WORKING_MODE_ACTIVE;
	else if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_SLEEP) zIhuCclSpsvirgoCtrlTable.cclSpsWorkingMode = IHU_CCL_SPS_WORKING_MODE_SLEEP;
	else if (rcv.workmode == IHU_CCL_DH_CMDID_WORK_MODE_FAULT) zIhuCclSpsvirgoCtrlTable.cclSpsWorkingMode = IHU_CCL_SPS_WORKING_MODE_FAULT;
	else IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Receive message error!\n");
	
	//返回
	return IHU_SUCCESS;
}

//发送差错报告后的证实给CCL
OPSTAT fsm_spsvirgo_ccl_fault_report_send(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0, i = 0;
	msg_struct_ccl_sps_fault_report_send_t rcv;
	msg_struct_sps_ccl_fault_report_cfm_t snd;

	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_sps_fault_report_send_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_sps_fault_report_send_t)))
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//先进入通信状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_COMMU);
	
	//干活，成功了，自然通过ISR将返回发送到L3
	//差错信息，未来可考虑再根据情况，填入到下表中

	//对接收到的上层命令进行分解处理，并组装消息发送给后台
	StrMsg_HUITP_MSGID_uni_ccl_state_report_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_report_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_ccl_state_report>>8)&0xFF;
	pMsgProc.msgId.optId = HUITP_MSGID_uni_ccl_state_report&0xFF;
	pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	//StrIe_HUITP_IEID_uni_com_report_t
	pMsgProc.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);
	pMsgProc.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);
	pMsgProc.baseReport.comReport = HUITP_IEID_UNI_COM_REPORT_YES;
	//StrIe_HUITP_IEID_uni_ccl_report_type_t
	pMsgProc.reportType.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_report_type);
	pMsgProc.reportType.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_report_type_t) - 4);
	pMsgProc.reportType.event = HUITP_IEID_UNI_CCL_REPORT_TYPE_FAULT_EVENT;
	//StrIe_HUITP_IEID_uni_ccl_lock_state_t
	pMsgProc.lockState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_lock_state);
	pMsgProc.lockState.ieLen = HUITP_ENDIAN_EXG16(2 + IHU_CCL_SENSOR_LOCK_NUMBER_MAX); //实际上配置的锁的多少
	pMsgProc.lockState.maxLockNo = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
	pMsgProc.lockState.lockId = IHU_CCL_SENSOR_LOCK_NUMBER_MAX; //这个表示全部
	for (i = 0; i < IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
		pMsgProc.lockState.lockState[i] = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_lock_open_state(i) == TRUE)?HUITP_IEID_UNI_LOCK_STATE_OPEN:HUITP_IEID_UNI_LOCK_STATE_CLOSE);
	}
	//StrIe_HUITP_IEID_uni_ccl_door_state_t
	pMsgProc.doorState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_lock_state);
	pMsgProc.doorState.ieLen = HUITP_ENDIAN_EXG16(2 + IHU_CCL_SENSOR_LOCK_NUMBER_MAX); //实际上配置的门的多少
	pMsgProc.doorState.maxDoorNo = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
	pMsgProc.doorState.doorId = IHU_CCL_SENSOR_LOCK_NUMBER_MAX; //这个表示全部
	for (i = 0; i < IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
		pMsgProc.doorState.doorState[i] = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_door_open_state(i) == TRUE)?HUITP_IEID_UNI_DOOR_STATE_OPEN:HUITP_IEID_UNI_DOOR_STATE_CLOSE);
	}
	//StrIe_HUITP_IEID_uni_ccl_water_state_t
	pMsgProc.waterState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_water_state);
	pMsgProc.waterState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_water_state_t) - 4);
	pMsgProc.waterState.waterState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_water_state() == TRUE)?HUITP_IEID_UNI_WATER_STATE_ACTIVE:HUITP_IEID_UNI_WATER_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_fall_state_t
	pMsgProc.fallState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_fall_state);
	pMsgProc.fallState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_fall_state_t) - 4);
	pMsgProc.fallState.fallState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_fall_state() == TRUE)?HUITP_IEID_UNI_FALL_STATE_ACTIVE:HUITP_IEID_UNI_FALL_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_shake_state_t
	pMsgProc.shakeState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_shake_state);
	pMsgProc.shakeState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_shake_state_t) - 4);
	pMsgProc.shakeState.shakeState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_shake_state() == TRUE)?HUITP_IEID_UNI_SHAKE_STATE_ACTIVE:HUITP_IEID_UNI_SHAKE_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_smoke_state_t
	pMsgProc.smokeState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_smoke_state);
	pMsgProc.smokeState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_smoke_state_t) - 4);
	pMsgProc.smokeState.smokeState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_smoke_state() == TRUE)?HUITP_IEID_UNI_SMOKE_STATE_ACTIVE:HUITP_IEID_UNI_SMOKE_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_bat_state_t
	pMsgProc.batState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_bat_state);
	pMsgProc.batState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_bat_state_t) - 4);
	pMsgProc.batState.batState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_bat_state() == TRUE)?HUITP_IEID_UNI_BAT_STATE_WARNING:HUITP_IEID_UNI_BAT_STATE_NORMAL);
	//StrIe_HUITP_IEID_uni_ccl_temp_value_t
	pMsgProc.tempValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_temp_value);
	pMsgProc.tempValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_temp_value_t) - 4);
	pMsgProc.tempValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.tempValue.tempValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_temp_value());
	//StrIe_HUITP_IEID_uni_ccl_humid_value_t
	pMsgProc.humidValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_humid_value);
	pMsgProc.humidValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_humid_value_t) - 4);
	pMsgProc.humidValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.humidValue.humidValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_humid_value());
	//StrIe_HUITP_IEID_uni_ccl_bat_value_t
	pMsgProc.batValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_bat_value);
	pMsgProc.batValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_bat_value_t) - 4);
	pMsgProc.batValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.batValue.batValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_bat_value());	
	//StrIe_HUITP_IEID_uni_ccl_general_value1_t
	pMsgProc.general1Value.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_general_value1);
	pMsgProc.general1Value.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_general_value1_t) - 4);
	pMsgProc.general1Value.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.general1Value.generalValue1 = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rsv1_value());	
	//StrIe_HUITP_IEID_uni_ccl_general_value2_t
	pMsgProc.general2Value.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_general_value2);
	pMsgProc.general2Value.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_general_value2_t) - 4);
	pMsgProc.general2Value.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.general2Value.generalValue2 = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rsv2_value());	
	//StrIe_HUITP_IEID_uni_ccl_rssi_value_t
	pMsgProc.rssiValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_rssi_value);
	pMsgProc.rssiValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_rssi_value_t) - 4);
	pMsgProc.rssiValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.rssiValue.rssiValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rssi_value());	
	//StrIe_HUITP_IEID_uni_ccl_dcmi_value_t
	pMsgProc.dcmiValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_dcmi_value);
	pMsgProc.dcmiValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_dcmi_value_t) - 4);
	pMsgProc.dcmiValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.dcmiValue.dcmiValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_dcmi_value());	

	//Pack message
	StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
	memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
	memcpy(&pMsgInput, &pMsgProc, msgProcLen);
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));	
	ret = func_cloud_standard_xml_pack(HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID, NULL, HUITP_MSGID_uni_ccl_state_report, &pMsgInput, msgProcLen, &pMsgOutput);
	if (ret == IHU_FAILURE){
		FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED);
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Package message error!\n");
	}

	//具体的发送命令
	memset(&zIhuSpsvirgoMsgRcvBuf, 0, sizeof(msg_struct_ccl_com_cloud_data_rx_t));
	ret = ihu_vmmw_gprsmod_http_data_transmit_with_receive((char *)(pMsgOutput.buf), pMsgOutput.bufferLen, zIhuSpsvirgoMsgRcvBuf.buf, &(zIhuSpsvirgoMsgRcvBuf.length));	
	
	//再进入正常状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED);
	
	//如果干活失败，则发送差错消息回L3
	if (ret == IHU_FAILURE){
		memset(&snd, 0, sizeof(msg_struct_sps_ccl_fault_report_cfm_t));
		snd.actionFlag = IHU_SYSMSG_CCL_EVENT_FAULT_SEND_FAILUR;
		snd.length = sizeof(msg_struct_sps_ccl_fault_report_cfm_t);
		ret = ihu_message_send(MSG_ID_SPS_CCL_FAULT_REPORT_CFM, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
	}		

	//如果从后台收到有价值的反馈
	else{
		//对于缓冲区的数据，进行分别处理，将帧变成不同的消息，分门别类发送到L3模块进行处理
		//注意，这里是CHAR数据，不是L2FRAME的比特数据
		ret = func_cloud_standard_xml_unpack(&zIhuSpsvirgoMsgRcvBuf, HUITP_MSGID_uni_ccl_state_confirm);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPSVIRGO]++;
			IhuErrorPrint("SPSVIRGO: Unpack and processing receiving message error!\n");
			//不能直接返回差错，因为上层还巴巴的等着回复这个消息，不然状态机会出错
			memset(&snd, 0, sizeof(msg_struct_sps_ccl_fault_report_cfm_t));
			snd.actionFlag = IHU_SYSMSG_CCL_EVENT_FAULT_SEND_FAILUR;
			snd.length = sizeof(msg_struct_sps_ccl_fault_report_cfm_t);
			ret = ihu_message_send(MSG_ID_SPS_CCL_FAULT_REPORT_CFM, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
			if (ret == IHU_FAILURE)
				IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
		}
	}
	
	//返回
	return IHU_SUCCESS;
}


//发送关门报告后的证实给CCL
OPSTAT fsm_spsvirgo_ccl_close_door_report_send(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0, i = 0;
	msg_struct_ccl_sps_close_report_send_t rcv;
	msg_struct_sps_ccl_close_report_cfm_t snd;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_ccl_sps_close_report_send_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ccl_sps_close_report_send_t)))
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//先进入通信状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_COMMU);
	
	//干活，成功了，自然通过ISR将返回发送到L3
	//关门独特信息，未来可考虑再根据情况，填入到下表中
	
	//对接收到的上层命令进行分解处理，并组装消息发送给后台
	StrMsg_HUITP_MSGID_uni_ccl_state_report_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_report_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_ccl_state_report>>8)&0xFF;
	pMsgProc.msgId.optId = HUITP_MSGID_uni_ccl_state_report&0xFF;
	pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	//StrIe_HUITP_IEID_uni_com_report_t
	pMsgProc.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);
	pMsgProc.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);
	pMsgProc.baseReport.comReport = HUITP_IEID_UNI_COM_REPORT_YES;
	//StrIe_HUITP_IEID_uni_ccl_report_type_t
	pMsgProc.reportType.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_report_type);
	pMsgProc.reportType.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_report_type_t) - 4);
	pMsgProc.reportType.event = HUITP_IEID_UNI_CCL_REPORT_TYPE_CLOSE_EVENT;
	//StrIe_HUITP_IEID_uni_ccl_lock_state_t
	pMsgProc.lockState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_lock_state);
	pMsgProc.lockState.ieLen = HUITP_ENDIAN_EXG16(2 + IHU_CCL_SENSOR_LOCK_NUMBER_MAX); //实际上配置的锁的多少
	pMsgProc.lockState.maxLockNo = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
	pMsgProc.lockState.lockId = IHU_CCL_SENSOR_LOCK_NUMBER_MAX; //这个表示全部
	for (i = 0; i < IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
		pMsgProc.lockState.lockState[i] = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_lock_open_state(i) == TRUE)?HUITP_IEID_UNI_LOCK_STATE_OPEN:HUITP_IEID_UNI_LOCK_STATE_CLOSE);
	}
	//StrIe_HUITP_IEID_uni_ccl_door_state_t
	pMsgProc.doorState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_lock_state);
	pMsgProc.doorState.ieLen = HUITP_ENDIAN_EXG16(2 + IHU_CCL_SENSOR_LOCK_NUMBER_MAX); //实际上配置的门的多少
	pMsgProc.doorState.maxDoorNo = IHU_CCL_SENSOR_LOCK_NUMBER_MAX;
	pMsgProc.doorState.doorId = IHU_CCL_SENSOR_LOCK_NUMBER_MAX; //这个表示全部
	for (i = 0; i < IHU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
		pMsgProc.doorState.doorState[i] = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_door_open_state(i) == TRUE)?HUITP_IEID_UNI_DOOR_STATE_OPEN:HUITP_IEID_UNI_DOOR_STATE_CLOSE);
	}
	//StrIe_HUITP_IEID_uni_ccl_water_state_t
	pMsgProc.waterState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_water_state);
	pMsgProc.waterState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_water_state_t) - 4);
	pMsgProc.waterState.waterState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_water_state() == TRUE)?HUITP_IEID_UNI_WATER_STATE_ACTIVE:HUITP_IEID_UNI_WATER_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_fall_state_t
	pMsgProc.fallState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_fall_state);
	pMsgProc.fallState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_fall_state_t) - 4);
	pMsgProc.fallState.fallState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_fall_state() == TRUE)?HUITP_IEID_UNI_FALL_STATE_ACTIVE:HUITP_IEID_UNI_FALL_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_shake_state_t
	pMsgProc.shakeState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_shake_state);
	pMsgProc.shakeState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_shake_state_t) - 4);
	pMsgProc.shakeState.shakeState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_shake_state() == TRUE)?HUITP_IEID_UNI_SHAKE_STATE_ACTIVE:HUITP_IEID_UNI_SHAKE_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_smoke_state_t
	pMsgProc.smokeState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_smoke_state);
	pMsgProc.smokeState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_smoke_state_t) - 4);
	pMsgProc.smokeState.smokeState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_smoke_state() == TRUE)?HUITP_IEID_UNI_SMOKE_STATE_ACTIVE:HUITP_IEID_UNI_SMOKE_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_bat_state_t
	pMsgProc.batState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_bat_state);
	pMsgProc.batState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_bat_state_t) - 4);
	pMsgProc.batState.batState = ((ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_bat_state() == TRUE)?HUITP_IEID_UNI_BAT_STATE_WARNING:HUITP_IEID_UNI_BAT_STATE_NORMAL);
	//StrIe_HUITP_IEID_uni_ccl_temp_value_t
	pMsgProc.tempValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_temp_value);
	pMsgProc.tempValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_temp_value_t) - 4);
	pMsgProc.tempValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.tempValue.tempValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_temp_value());
	//StrIe_HUITP_IEID_uni_ccl_humid_value_t
	pMsgProc.humidValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_humid_value);
	pMsgProc.humidValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_humid_value_t) - 4);
	pMsgProc.humidValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.humidValue.humidValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_humid_value());
	//StrIe_HUITP_IEID_uni_ccl_bat_value_t
	pMsgProc.batValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_bat_value);
	pMsgProc.batValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_bat_value_t) - 4);
	pMsgProc.batValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.batValue.batValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_bat_value());	
	//StrIe_HUITP_IEID_uni_ccl_general_value1_t
	pMsgProc.general1Value.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_general_value1);
	pMsgProc.general1Value.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_general_value1_t) - 4);
	pMsgProc.general1Value.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.general1Value.generalValue1 = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rsv1_value());	
	//StrIe_HUITP_IEID_uni_ccl_general_value2_t
	pMsgProc.general2Value.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_general_value2);
	pMsgProc.general2Value.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_general_value2_t) - 4);
	pMsgProc.general2Value.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.general2Value.generalValue2 = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rsv2_value());	
	//StrIe_HUITP_IEID_uni_ccl_rssi_value_t
	pMsgProc.rssiValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_rssi_value);
	pMsgProc.rssiValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_rssi_value_t) - 4);
	pMsgProc.rssiValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.rssiValue.rssiValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rssi_value());	
	//StrIe_HUITP_IEID_uni_ccl_dcmi_value_t
	pMsgProc.dcmiValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_dcmi_value);
	pMsgProc.dcmiValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_dcmi_value_t) - 4);
	pMsgProc.dcmiValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc.dcmiValue.dcmiValue = HUITP_ENDIAN_EXG16(ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_dcmi_value());	

	//Pack message
	StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
	memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
	memcpy(&pMsgInput, &pMsgProc, msgProcLen);
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));	
	ret = func_cloud_standard_xml_pack(HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID, NULL, HUITP_MSGID_uni_ccl_state_report, &pMsgInput, msgProcLen, &pMsgOutput);
	if (ret == IHU_FAILURE){
		FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED);
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Package message error!\n");
	}
	
	//具体的发送命令
	memset(&zIhuSpsvirgoMsgRcvBuf, 0, sizeof(msg_struct_ccl_com_cloud_data_rx_t));
	ret = ihu_vmmw_gprsmod_http_data_transmit_with_receive((char *)(pMsgOutput.buf), pMsgOutput.bufferLen, zIhuSpsvirgoMsgRcvBuf.buf, &(zIhuSpsvirgoMsgRcvBuf.length));	
	
	//再进入正常状态
	FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_ACTIVED);
	
	//如果干活失败，则发送差错消息回L3
	if (ret == IHU_FAILURE){
		memset(&snd, 0, sizeof(msg_struct_sps_ccl_close_report_cfm_t));
		snd.actionFlag = IHU_SYSMSG_CCL_EVENT_CLOSE_SEND_FAILURE;
		snd.length = sizeof(msg_struct_sps_ccl_close_report_cfm_t);
		ret = ihu_message_send(MSG_ID_SPS_CCL_CLOSE_REPORT_CFM, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
	}

	//如果从后台收到有价值的反馈
	else{
		//对于缓冲区的数据，进行分别处理，将帧变成不同的消息，分门别类发送到L3模块进行处理
		//注意，这里是CHAR数据，不是L2FRAME的比特数据
		ret = func_cloud_standard_xml_unpack(&zIhuSpsvirgoMsgRcvBuf, HUITP_MSGID_uni_ccl_state_confirm);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPSVIRGO]++;
			IhuErrorPrint("SPSVIRGO: Unpack and processing receiving message error!\n");
			//不能直接返回差错，因为上层还巴巴的等着回复这个消息，不然状态机会出错
			memset(&snd, 0, sizeof(msg_struct_sps_ccl_close_report_cfm_t));
			snd.actionFlag = IHU_SYSMSG_CCL_EVENT_CLOSE_SEND_FAILURE;
			snd.length = sizeof(msg_struct_sps_ccl_close_report_cfm_t);
			ret = ihu_message_send(MSG_ID_SPS_CCL_CLOSE_REPORT_CFM, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
			if (ret == IHU_FAILURE)
				IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
		}
	}
	
	//返回
	return IHU_SUCCESS;
}

//SLEEP&FAULT模式下扫描：扫描RSSI, 数据格式HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2
INT16 ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rssi_value(void)
{
	return ihu_vmmw_gprsmod_get_rssi_value();
}

//解码接收到的消息部分，其它模块没有这个，因为SPS用来管理GPRSMOD的L2FRAME通道
OPSTAT func_cloud_spsvirgo_ccl_msg_heart_beat_req_received_handle(StrMsg_HUITP_MSGID_uni_heart_beat_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_heart_beat_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_heart_beat_confirm_received_handle(StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_lock_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_lock_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_lock_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_lock_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_lock_auth_resp_received_handle(StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_t *rcv)
{
	int ret = 0;
	
	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseResp.ieId = HUITP_ENDIAN_EXG16(rcv->baseResp.ieId);
	rcv->baseResp.ieLen = HUITP_ENDIAN_EXG16(rcv->baseResp.ieLen);
	rcv->respState.ieId = HUITP_ENDIAN_EXG16(rcv->respState.ieId);
	rcv->respState.ieLen = HUITP_ENDIAN_EXG16(rcv->respState.ieLen);
	
	//IE参数检查
	if ((rcv->baseResp.ieId != HUITP_IEID_uni_com_resp) || (rcv->baseResp.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_resp_t) - 4)))
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Cloud raw message content unpack error!\n");
	if ((rcv->respState.ieId != HUITP_IEID_uni_ccl_lock_auth_resp) || (rcv->respState.ieLen != (sizeof(StrIe_HUITP_IEID_uni_ccl_lock_auth_resp_t) - 4)))
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Cloud raw message content unpack error!\n");
	
	//逻辑处理
	if ((rcv->respState.authResp != HUITP_IEID_UNI_CCL_LOCK_AUTH_RESP_YES) && (rcv->respState.authResp != HUITP_IEID_UNI_CCL_LOCK_AUTH_RESP_NO)) 
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Cloud raw message content unpack error!\n");
	msg_struct_spsvirgo_ccl_cloud_fb_t snd;
	memset(&snd, 0, sizeof(msg_struct_spsvirgo_ccl_cloud_fb_t));
	if (rcv->respState.authResp == HUITP_IEID_UNI_CCL_LOCK_AUTH_RESP_YES) snd.authResult = IHU_CCL_LOCK_AUTH_RESULT_OK;
	else snd.authResult = IHU_CCL_LOCK_AUTH_RESULT_NOK;
	snd.length = sizeof(msg_struct_spsvirgo_ccl_cloud_fb_t);
	ret = ihu_message_send(MSG_ID_SPS_CCL_CLOUD_FB, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
	
	//返回
	return IHU_SUCCESS;
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_door_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_door_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_door_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_door_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_door_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_door_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_rfid_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_rfid_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_rfid_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_rfid_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_rfid_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_rfid_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_ble_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_ble_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_ble_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_ble_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_ble_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_ble_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_gprs_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_gprs_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_gprs_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_gprs_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_gprs_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_gprs_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_battery_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_battery_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_battery_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_battery_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_battery_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_battery_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_shake_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_shake_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_shake_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_shake_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_shake_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_shake_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_smoke_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_smoke_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_smoke_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_smoke_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_smoke_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_smoke_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_water_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_water_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_water_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_water_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_water_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_water_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_temp_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_temp_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_temp_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_temp_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_temp_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_temp_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_humid_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_humid_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_humid_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_humid_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_humid_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_humid_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_fall_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_fall_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_fall_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_fall_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_fall_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_fall_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_state_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_state_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_ccl_state_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_state_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_state_confirm_t *rcv)
{
	int ret = 0;
	
	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);
	rcv->reportType.ieId = HUITP_ENDIAN_EXG16(rcv->reportType.ieId);
	rcv->reportType.ieLen = HUITP_ENDIAN_EXG16(rcv->reportType.ieLen);
	
	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Cloud raw message content unpack error!\n");
	if ((rcv->reportType.ieId != HUITP_IEID_uni_ccl_report_type) || (rcv->reportType.ieLen != (sizeof(StrIe_HUITP_IEID_uni_ccl_report_type_t) - 4)))
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Cloud raw message content unpack error!\n");
	
	//逻辑处理
	if ((rcv->reportType.event != HUITP_IEID_UNI_CCL_REPORT_TYPE_PERIOD_EVENT) && (rcv->reportType.event != HUITP_IEID_UNI_CCL_REPORT_TYPE_CLOSE_EVENT)\
		&& (rcv->reportType.event != HUITP_IEID_UNI_CCL_REPORT_TYPE_FAULT_EVENT))
		IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Cloud raw message content unpack error!\n");
		
	if (rcv->reportType.event == HUITP_IEID_UNI_CCL_REPORT_TYPE_PERIOD_EVENT){
		msg_struct_sps_ccl_event_report_cfm_t snd;
		memset(&snd, 0, sizeof(msg_struct_sps_ccl_event_report_cfm_t));
		snd.actionFlag = IHU_SYSMSG_CCL_EVENT_REPORT_SEND_SUCCESS;
		snd.length = sizeof(msg_struct_sps_ccl_event_report_cfm_t);
		ret = ihu_message_send(MSG_ID_SPS_CCL_EVENT_REPORT_CFM, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
	}
	else if (rcv->reportType.event == HUITP_IEID_UNI_CCL_REPORT_TYPE_CLOSE_EVENT){
		msg_struct_sps_ccl_close_report_cfm_t snd;
		memset(&snd, 0, sizeof(msg_struct_sps_ccl_close_report_cfm_t));
		snd.actionFlag = IHU_SYSMSG_CCL_EVENT_CLOSE_SEND_SUCCESS;
		snd.length = sizeof(msg_struct_sps_ccl_close_report_cfm_t);
		ret = ihu_message_send(MSG_ID_SPS_CCL_CLOSE_REPORT_CFM, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
	}
	else if (rcv->reportType.event == HUITP_IEID_UNI_CCL_REPORT_TYPE_FAULT_EVENT){
		msg_struct_sps_ccl_fault_report_cfm_t snd;
		memset(&snd, 0, sizeof(msg_struct_sps_ccl_fault_report_cfm_t));
		snd.actionFlag = IHU_SYSMSG_CCL_EVENT_FAULT_SEND_SUCCESS;
		snd.length = sizeof(msg_struct_sps_ccl_fault_report_cfm_t);
		ret = ihu_message_send(MSG_ID_SPS_CCL_FAULT_REPORT_CFM, TASK_ID_CCL, TASK_ID_SPSVIRGO, &snd, snd.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName, zIhuVmCtrTab.task[TASK_ID_CCL].taskName);
	}
	else  //不可能到这儿
	{
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

OPSTAT func_cloud_spsvirgo_ccl_msg_inventory_req_received_handle(StrMsg_HUITP_MSGID_uni_inventory_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_inventory_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_inventory_confirm_received_handle(StrMsg_HUITP_MSGID_uni_inventory_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_inventory_confirm_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_sw_package_req_received_handle(StrMsg_HUITP_MSGID_uni_sw_package_req_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_sw_package_req_t received!\n");
}

OPSTAT func_cloud_spsvirgo_ccl_msg_sw_package_confirm_received_handle(StrMsg_HUITP_MSGID_uni_sw_package_confirm_t *rcv)
{
	IHU_ERROR_PRINT_SPSVIRGO("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_sw_package_confirm_t received!\n");
}


#endif //#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)


