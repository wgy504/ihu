/**
 ****************************************************************************************
 *
 * @file l3emc.c
 *
 * @brief L3 EMC
 *
 * BXXH team
 * Created by ZJL, 20160127
 *
 ****************************************************************************************
 */
 
#include "l3emcwx.h"
#include "i2c_led.h"
 
/*
** FSM of the EMC
*/
FsmStateItem_t FsmEmc[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_emc_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_emc_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_emc_restart},

  //Task level initialization and get into OFFLINE
  {MSG_ID_COM_RESTART,        						FSM_STATE_EMC_OFFLINE,         							fsm_emc_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_EMC_OFFLINE,         							fsm_emc_stop_rcv},  //消息逻辑的来源待定
  {MSG_ID_ASYLIBRA_EMC_CON,								FSM_STATE_EMC_OFFLINE,         							fsm_emc_con_rcv},
  {MSG_ID_COM_TIME_OUT,										FSM_STATE_EMC_OFFLINE,         							fsm_emc_time_out},	

  //Task level online
  {MSG_ID_COM_RESTART,        						FSM_STATE_EMC_ONLINE,         							fsm_emc_restart},
  {MSG_ID_ASYLIBRA_EMC_DISC,							FSM_STATE_EMC_ONLINE,         							fsm_emc_disc_rcv},
  {MSG_ID_COM_TIME_OUT,										FSM_STATE_EMC_ONLINE,         							fsm_emc_time_out},
  {MSG_ID_ASYLIBRA_EMC_DLK_BLOCK,					FSM_STATE_EMC_ONLINE,         							fsm_emc_dlk_block_rcv},
  {MSG_ID_ASYLIBRA_EMC_PUSH_CMD,					FSM_STATE_EMC_ONLINE,         							fsm_emc_push_cmd_rcv},

  //Task level online_block: 通过引入该状态机，很好的解决了定时触发离线数据重发的问题
  {MSG_ID_COM_RESTART,        						FSM_STATE_EMC_ONLINE_BLOCK,         				fsm_emc_restart},
  {MSG_ID_ASYLIBRA_EMC_DISC,							FSM_STATE_EMC_ONLINE_BLOCK,         				fsm_emc_disc_rcv},
  {MSG_ID_COM_TIME_OUT,										FSM_STATE_EMC_ONLINE_BLOCK,         				fsm_emc_time_out},
  {MSG_ID_ASYLIBRA_EMC_DLK_UNBLOCK,				FSM_STATE_EMC_ONLINE_BLOCK,         				fsm_emc_dlk_block_rcv},
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
IhuDiscDataSampleStorage_t zIhuMemStorageBuf;

/*
 *
*   状态转移机API函数过程
 *
 */

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_emc_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_EMC, FSM_STATE_IDLE) == FAILURE){
		IhuErrorPrint("EMC: Error Set FSM_STATE_IDLE State at fsm_emc_task_entry.");
	}
	return SUCCESS;
}

//任务模块初始化过程
OPSTAT fsm_emc_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	int ret=0;
	char strDebug[IHU_PRINT_CHAR_SIZE];

	//串行会送INIT_FB给VMDA，不然消息队列不够深度，此为节省内存机制
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VMDA
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_EMC, &snd, snd.length);
		if (ret == FAILURE){
			sprintf(strDebug, "EMC: Send MSG_ID_COM_INIT_FB message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_EMC], zIhuTaskNameList[src_id]);
			IhuErrorPrint(strDebug);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_EMC, FSM_STATE_EMC_INITED) == FAILURE){
		IhuErrorPrint("EMC: Error Set FSM_STATE_EMC_INITED State!");	
		return FAILURE;
	}

	//初始化硬件接口
	if (func_emc_hw_init() == FAILURE){	
		IhuErrorPrint("EMC: Error func_emc_hw_init initialize interface!");
		return FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_EMC] = 0;
	memset(&zIhuMemStorageBuf, 0, sizeof(IhuDiscDataSampleStorage_t));
	
	//直接进入OFFLINE状态
	if (FsmSetState(TASK_ID_EMC, FSM_STATE_EMC_OFFLINE) == FAILURE){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: Error Set FSM_STATE_EMC_OFFLINE State!");
		return FAILURE;
	}
	
	//启动本地定时器，如果有必要
	ret = ihu_timer_start(TASK_ID_ASYLIBRA, TIMER_ID_1S_EMC_PERIOD_READ, EMC_TIMER_DURATION_PERIOD_READ, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_ASYLIBRA]++;
		IhuErrorPrint("ASYLIBRA: Error ihu_timer_start start timer!");
		return FAILURE;
	}	
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("EMC: Enter FSM_STATE_EMC_ACTIVE status, Keeping refresh here!");
	}
	
	//返回
	return SUCCESS;
}

//模块RESTART过程
//需要在模块运行过程中的某些关键差错地方，设置RESTART机制，未来待进一步研究和完善
OPSTAT fsm_emc_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	IhuErrorPrint("EMC: Internal error counter reach DEAD level, SW-RESTART soon!");
	fsm_emc_init(0, 0, NULL, 0);
	
	return SUCCESS;
}

OPSTAT fsm_emc_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_EMC)){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: Wrong fsm_emc_stop_rcv input paramters!");
		return FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_EMC, FSM_STATE_IDLE) == FAILURE){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: Error Set FSM_STATE_IDLE State!");
		return FAILURE;
	}
	
	//返回
	return SUCCESS;
}

//断链通知收到，进入OFFLINE状态
OPSTAT fsm_emc_disc_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_EMC)){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: Wrong fsm_emc_disc_rcv input paramters!");
		return FAILURE;
	}

	//收消息
	msg_struct_asylibra_emc_disc_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_asylibra_emc_disc_t));
	if (param_len > sizeof(msg_struct_asylibra_emc_disc_t)){
		IhuErrorPrint("EMC: Receive MSG_ID_ASYLIBRA_EMC_DISC message error!");
		zIhuRunErrCnt[TASK_ID_EMC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//业务逻辑
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_EMC, FSM_STATE_EMC_OFFLINE) == FAILURE){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: Error Set FSM_STATE_EMC_OFFLINE State!");
		return FAILURE;
	}
	
	vmda1458x_led_off(LED_ID_7);

	//返回
	return SUCCESS;
}

//收到连接通知，进入ONLINE状态
OPSTAT fsm_emc_con_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{		
	int ret = 0;
	char strDebug[IHU_PRINT_CHAR_SIZE];	

	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_EMC)){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: Wrong fsm_emc_con_rcv input paramters!");
		return FAILURE;
	}

	//收消息
	msg_struct_asylibra_emc_con_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_asylibra_emc_con_t));
	if (param_len > sizeof(msg_struct_asylibra_emc_con_t)){
		IhuErrorPrint("EMC: Receive MSG_ID_ASYLIBRA_EMC_CON message error!");
		zIhuRunErrCnt[TASK_ID_EMC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//业务逻辑：发送时钟同步请求给后台
	//Send back MSG_ID_EMC_ASYLIBRA_DATA_REQ to VMDA
	msg_struct_emc_asylibra_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_emc_asylibra_data_req_t));	
	IhuL3DataFormatTag_t l3dataT;
	memset(&l3dataT, 0, sizeof(IhuL3DataFormatTag_t));
	l3dataT.UserCmdId = L3CI_time_sync;
	if (ihu_pack_l3_data_to_cloud(&l3dataT, snd.l3Data, snd.l3dataLen) == FAILURE){
		IhuErrorPrint("EMC: Pack L3 data error!");
		zIhuRunErrCnt[TASK_ID_EMC]++;
	}
	snd.length = sizeof(msg_struct_emc_asylibra_data_req_t);
	ret = ihu_message_send(MSG_ID_EMC_ASYLIBRA_DATA_REQ, TASK_ID_ASYLIBRA, TASK_ID_EMC, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "EMC: Send MSG_ID_EMC_ASYLIBRA_DATA_REQ message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_EMC], zIhuTaskNameList[TASK_ID_ASYLIBRA]);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_EMC, FSM_STATE_EMC_ONLINE) == FAILURE){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: Error Set FSM_STATE_EMC_ONLINE State!");
		return FAILURE;
	}
	
	//返回
	
	vmda1458x_led_on(LED_ID_7);
	return SUCCESS;
}

//TIMEOUT超时消息收到
OPSTAT fsm_emc_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{	
	int ret=0;
	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_EMC)){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: Wrong fsm_emc_time_out input paramters!");
		return FAILURE;
	}

	//收消息
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if (param_len > sizeof(msg_struct_com_time_out_t)){
		IhuErrorPrint("EMC: Receive MSG_ID_COM_TIME_OUT message error!");
		zIhuRunErrCnt[TASK_ID_EMC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//RESTART钩子机制先去掉，暂时不考虑
	
	//定时长时钟进行链路检测的
	if ((rcv.timeId == TIMER_ID_1S_EMC_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (FsmGetState(TASK_ID_EMC) == FSM_STATE_EMC_ONLINE){
			ret = func_emc_send_out_data_online();
		}
		
		else if((FsmGetState(TASK_ID_EMC) == FSM_STATE_EMC_OFFLINE) || (FsmGetState(TASK_ID_EMC) == FSM_STATE_EMC_ONLINE_BLOCK)){	
			ret = func_emc_store_data_offline();
		}

		//时候需要强行恢复状态到ACTIVE，甚至启动INIT过程，需要未来仔细探究
		else{
			IhuErrorPrint("EMC: Wrong state on TIME_OUT message received!");
			zIhuRunErrCnt[TASK_ID_EMC]++;
			IhuErrorPrint("EMC: Fatal error occure, Module restart now...");
			fsm_emc_init(0, 0, NULL, 0);
			ret = FAILURE;
		}
	}
	
	//返回
	return ret;	
}

//收到链路BLOCK消息
OPSTAT fsm_emc_dlk_block_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{	
	int ret = 0;
	char strDebug[IHU_PRINT_CHAR_SIZE];	
	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_EMC)){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: Wrong fsm_emc_dlk_block_rcv input paramters!");
		return FAILURE;
	}

	//收消息
	msg_struct_asylibra_emc_dlk_unblock_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_asylibra_emc_dlk_unblock_t));
	if (param_len > sizeof(msg_struct_asylibra_emc_dlk_unblock_t)){
		IhuErrorPrint("EMC: Receive MSG_ID_ASYLIBRA_EMC_DLK_UNBLOCK message error!");
		zIhuRunErrCnt[TASK_ID_EMC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_EMC, FSM_STATE_EMC_ONLINE) == FAILURE){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: Error Set FSM_STATE_EMC_ONLINE State!");
		return FAILURE;
	}

	//业务逻辑：查询是否有离线数据，如果是的，就发送给后台
	IhuDiscDataSampleStorageArray_t record;

	//检查离线数据还有没有未送完的，如果没有，停止回送短定时器
	//先检查并停止，再读取数据，会造成多一次定时到达，不是坏事情，会让扫描更为完整
	if (zIhuMemStorageBuf.offlineNbr<=0){
		zIhuMemStorageBuf.offlineNbr = 0;
		return SUCCESS;
	}

	 //读取离线数据，通过参数进去无效，不得不使用返回RETURN FUNC的形式
	memset(&record, 0, sizeof(IhuDiscDataSampleStorageArray_t));
	ret = ihu_read_from_storage_mem(&record);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: No any more off-line data shall be sent to CLOUD!");
		return SUCCESS;
	}

	//将发送成功后的数据条目从OFFLINE状态设置为ONLINE状态，表示数据已经发送完成，留着只是为了本地备份
	//实际上，读取的过程已经将OFFLINE状态给设置了，这里不需要再处理，不然差错处理会导致状态机太多的报错
	//在线状态下，读取离线成功，但发送不成功，会导致离线数据不再重复发送给后台，因为已经被设置为ONLINE/BACKUP留作本地备份了
	//考虑到这种概率非常低下，不值得再做复杂的处理，反正数据还留着本地备份，万一需要，可以人工介入

	//RECORD数据参数检查，这里只做最为基本的检查，
	if ((record.equipid <=0) || (record.timestamp<=0) || (record.sensortype != L3CI_emc) ||(record.onOffLine != IHU_DISC_DATA_SAMPLE_OFFLINE)){
		IhuErrorPrint("EMC: Invalid record read from MEM-DISC, dropped!");
		zIhuRunErrCnt[TASK_ID_EMC]++;
		return FAILURE;
	}

	//离线数据回送，Send data MSG_ID_EMC_ASYLIBRA_DATA_REQ to ASYLIBRA
	msg_struct_emc_asylibra_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_emc_asylibra_data_req_t));
	
	IhuL3DataFormatTag_t l3dataT;
	memset(&l3dataT, 0, sizeof(IhuL3DataFormatTag_t));
	l3dataT.UserCmdId = record.sensortype;
	l3dataT.emcInfo.emcValue = record.emcValue;
	zIhuSystemTimeYmd = ihu_clock_unix_to_ymd(record.timestamp);
	l3dataT.emcInfo.timeYmd[0] = (zIhuSystemTimeYmd.tm_year+1900-2000) & 0xFF;
	l3dataT.emcInfo.timeYmd[1] = zIhuSystemTimeYmd.tm_mon+1;
	l3dataT.emcInfo.timeYmd[2] = zIhuSystemTimeYmd.tm_mday;
	l3dataT.emcInfo.timeYmd[3] = zIhuSystemTimeYmd.tm_hour;
	l3dataT.emcInfo.timeYmd[4] = zIhuSystemTimeYmd.tm_min;
	l3dataT.emcInfo.timeYmd[5] = zIhuSystemTimeYmd.tm_sec;	
	l3dataT.emcInfo.nTimes = record.sid;
	if (ihu_pack_l3_data_to_cloud(&l3dataT, snd.l3Data, snd.l3dataLen) == FAILURE){
		IhuErrorPrint("EMC: Pack L3 data error!");
		zIhuRunErrCnt[TASK_ID_EMC]++;
	}
	snd.length = sizeof(msg_struct_emc_asylibra_data_req_t);
	ret = ihu_message_send(MSG_ID_EMC_ASYLIBRA_DATA_REQ, TASK_ID_ASYLIBRA, TASK_ID_EMC, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "EMC: Send MSG_ID_EMC_ASYLIBRA_DATA_REQ message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_EMC], zIhuTaskNameList[TASK_ID_ASYLIBRA]);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}

	//结束打印信息
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_NOR_ON) != FALSE){
		IhuDebugPrint("EMC: Under online state, send off-line data to cloud success!");
	}	

	//返回
	return SUCCESS;
}

//收到链路UNBLOCK消息
OPSTAT fsm_emc_dlk_unblock_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{		
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_EMC)){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: Wrong fsm_emc_dlk_unblock_rcv input paramters!");
		return FAILURE;
	}

	//收消息
	msg_struct_asylibra_emc_dlk_block_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_asylibra_emc_dlk_block_t));
	if (param_len > sizeof(msg_struct_asylibra_emc_dlk_block_t)){
		IhuErrorPrint("EMC: Receive MSG_ID_ASYLIBRA_EMC_DLK_BLOCK message error!");
		zIhuRunErrCnt[TASK_ID_EMC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	
	//业务逻辑
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_EMC, FSM_STATE_EMC_ONLINE_BLOCK) == FAILURE){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: Error Set FSM_STATE_EMC_ONLINE_BLOCK State!");
		return FAILURE;
	}
	
	//返回
	return SUCCESS;
}

//收到链路PUSH_CMD消息
//		snd1.year = (z_system_time_ymd->tm_year+1900-2000) & 0xFF;
//		snd1.month = z_system_time_ymd->tm_mon+1;
//		snd1.day = z_system_time_ymd->tm_mday;
//		snd1.hour = z_system_time_ymd->tm_hour;
//		snd1.minute = z_system_time_ymd->tm_min;
//		snd1.second = z_system_time_ymd->tm_sec;
OPSTAT fsm_emc_push_cmd_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{		
	int ret=0;
	char strDebug[IHU_PRINT_CHAR_SIZE];

	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_EMC)){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		IhuErrorPrint("EMC: Wrong fsm_emc_push_cmd_rcv input paramters!");
		return FAILURE;
	}

	//收消息
	msg_struct_asylibra_emc_push_cmd_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_asylibra_emc_push_cmd_t));
	if (param_len > sizeof(msg_struct_asylibra_emc_push_cmd_t)){
		IhuErrorPrint("EMC: Receive MSG_ID_ASYLIBRA_EMC_PUSH_CMD message error!");
		zIhuRunErrCnt[TASK_ID_EMC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//解码L3DATA信息
	IhuL3DataFormatTag_t l3dataR;
	memset(&l3dataR, 0, sizeof(IhuL3DataFormatTag_t));
	if (ihu_unpack_l3_data_from_cloud(rcv.l3Data, rcv.l3dataLen, &l3dataR) == FAILURE){
		IhuErrorPrint("EMC: Unpack L3 DATA error!");
		zIhuRunErrCnt[TASK_ID_EMC]++;
		return FAILURE;
	}
	
	//业务逻辑处理	
	//时钟同步信息
	if (l3dataR.UserCmdId == L3CI_time_sync)
	{
		//时钟同步信息的消息结构，参考《设计要点》中的定义 //只取时间信息，其它信息忽略
		zIhuSystemTimeUnix = l3dataR.timeSync.timeStamp;
		zIhuSystemTimeYmd = ihu_clock_unix_to_ymd(zIhuSystemTimeUnix);
	}//UserCmid == L3CI_time_sync
		
	//设备基本信息
	if (l3dataR.UserCmdId == L3CI_equipment_info) 
	{
		//设备基本信息的消息结构，参考《设计要点》中的定义 //这里不处理任何细节参数部分，未来需要进一步增强
		//使用MSG_ID_EMC_ASYLIBRA_EQU_INFO_SYNC消息发送给ASYLIBRA
		msg_struct_emc_asylibra_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_emc_asylibra_data_req_t));
		IhuL3DataFormatTag_t l3dataT;
		memset(&l3dataT, 0, sizeof(IhuL3DataFormatTag_t));
		l3dataT.UserCmdId = l3dataR.UserCmdId;
		if (ihu_get_mac_addr(l3dataT.equInfo.mac) == FAILURE){
			IhuErrorPrint("EMC: Not get MAC address correctly, can not send back Equipment Info to cloud.");
			zIhuRunErrCnt[TASK_ID_EMC]++;
			return FAILURE;
		}
		l3dataT.equInfo.hwType = IHU_EMCWX_CURRENT_HW_TYPE;
		l3dataT.equInfo.hwVer = IHU_EMCWX_CURRENT_HW_PEM;
		l3dataT.equInfo.swRel = IHU_EMCWX_CURRENT_SW_RELEASE;
		l3dataT.equInfo.swVer = IHU_EMCWX_CURRENT_SW_DELIVERY;
		if (ihu_pack_l3_data_to_cloud(&l3dataT, snd.l3Data, snd.l3dataLen) == FAILURE){
			IhuErrorPrint("EMC: Pack L3 data error!");
			zIhuRunErrCnt[TASK_ID_EMC]++;
		}	
		snd.length = sizeof(msg_struct_emc_asylibra_data_req_t);
		ret = ihu_message_send(MSG_ID_EMC_ASYLIBRA_DATA_REQ, TASK_ID_ASYLIBRA, TASK_ID_EMC, &snd, snd.length);
		if (ret == FAILURE){
			sprintf(strDebug, "EMC: Send MSG_ID_EMC_ASYLIBRA_EQU_INFO_SYNC message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_EMC], zIhuTaskNameList[TASK_ID_ASYLIBRA]);
			IhuErrorPrint(strDebug);
			zIhuRunErrCnt[TASK_ID_EMC]++;
			return FAILURE;
		}
	}//UserCmid == L3CI_equipment_info
		
	//电磁辐射瞬时读取
	else if (l3dataR.UserCmdId == L3CI_emc)
	{
		//固定格式的消息结构，参考《设计要点》中的定义
		//使用MSG_ID_EMC_ASYLIBRA_DATA_REQ消息发送给ASYLIBRA		
		if ((rcv.EmDeviceDataType != 0) && (rcv.EmDeviceDataType != EDDT_wxDeviceHtmlChatView) && (rcv.EmDeviceDataType != EDDT_manufatureSvr)){
			IhuErrorPrint("EMC: Wrong input parameter [EmDeviceDataType] for instance reading.");
			zIhuRunErrCnt[TASK_ID_EMC]++;
			return FAILURE;
		}
		msg_struct_emc_asylibra_data_req_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_emc_asylibra_data_req_t));
		IhuL3DataFormatTag_t l3dataT1;
		memset(&l3dataT1, 0, sizeof(IhuL3DataFormatTag_t));
		l3dataT1.UserCmdId = l3dataR.UserCmdId;
		l3dataT1.emcInfo.emcValue = ihu_emc_adc_read();
		zIhuSystemTimeYmd = ihu_clock_unix_to_ymd(zIhuSystemTimeUnix);
		l3dataT1.emcInfo.timeYmd[0] = (zIhuSystemTimeYmd.tm_year+1900-2000) & 0xFF;
		l3dataT1.emcInfo.timeYmd[1] = zIhuSystemTimeYmd.tm_mon+1;
		l3dataT1.emcInfo.timeYmd[2] = zIhuSystemTimeYmd.tm_mday;
		l3dataT1.emcInfo.timeYmd[3] = zIhuSystemTimeYmd.tm_hour;
		l3dataT1.emcInfo.timeYmd[4] = zIhuSystemTimeYmd.tm_min;
		l3dataT1.emcInfo.timeYmd[5] = zIhuSystemTimeYmd.tm_sec;	
		l3dataT1.emcInfo.nTimes = 0;
		if (ihu_pack_l3_data_to_cloud(&l3dataT1, snd1.l3Data, snd1.l3dataLen) == FAILURE){
			IhuErrorPrint("EMC: Pack L3 data error!");
			zIhuRunErrCnt[TASK_ID_EMC]++;
		}			
		snd1.EmDeviceDataType = rcv.EmDeviceDataType;
		snd1.length = sizeof(msg_struct_emc_asylibra_data_req_t);        
		ret = ihu_message_send(MSG_ID_EMC_ASYLIBRA_DATA_REQ, TASK_ID_ASYLIBRA, TASK_ID_EMC, &snd1, snd1.length);
		if (ret == FAILURE){
			sprintf(strDebug, "EMC: Send MSG_ID_EMC_ASYLIBRA_DATA_REQ message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_EMC], zIhuTaskNameList[TASK_ID_ASYLIBRA]);
			IhuErrorPrint(strDebug);
			zIhuRunErrCnt[TASK_ID_EMC]++;
			return FAILURE;
		}
	}//UserCmid == L3CI_emc_indicator

	//不支持的CommandId
	else{
		sprintf(strDebug, "EMC: Not support Command, cmdId=%d!", l3dataR.UserCmdId);
		IhuErrorPrint(strDebug);
		zIhuRunErrCnt[TASK_ID_EMC]++;
		return FAILURE;		
	}
	
	vmda1458x_led_blink_once_on_off(LED_ID_6);
	//返回
	return SUCCESS;
}

/*
 *
 *   本地API函数过程
 *
 */

//Local APIs
OPSTAT func_emc_hw_init(void)
{
	return SUCCESS;
}

//发送在线数据给后台
OPSTAT func_emc_send_out_data_online(void)
{
	int ret=0;
	char strDebug[IHU_PRINT_CHAR_SIZE];
	
	//发送数据给底层，状态由调用者决定
	//Send data MSG_ID_EMC_ASYLIBRA_DATA_REQ to ASYLIBRA
	msg_struct_emc_asylibra_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_emc_asylibra_data_req_t));
	IhuL3DataFormatTag_t l3dataT;
	memset(&l3dataT, 0, sizeof(IhuL3DataFormatTag_t));
	l3dataT.UserCmdId = L3CI_emc;
	l3dataT.emcInfo.emcValue = ihu_emc_adc_read();
	zIhuSystemTimeYmd = ihu_clock_unix_to_ymd(zIhuSystemTimeUnix);
	l3dataT.emcInfo.timeYmd[0] = (zIhuSystemTimeYmd.tm_year+1900-2000) & 0xFF;
	l3dataT.emcInfo.timeYmd[1] = zIhuSystemTimeYmd.tm_mon+1;
	l3dataT.emcInfo.timeYmd[2] = zIhuSystemTimeYmd.tm_mday;
	l3dataT.emcInfo.timeYmd[3] = zIhuSystemTimeYmd.tm_hour;
	l3dataT.emcInfo.timeYmd[4] = zIhuSystemTimeYmd.tm_min;
	l3dataT.emcInfo.timeYmd[5] = zIhuSystemTimeYmd.tm_sec;	
	l3dataT.emcInfo.nTimes = 0;
	if (ihu_pack_l3_data_to_cloud(&l3dataT, snd.l3Data, snd.l3dataLen) == FAILURE){
		IhuErrorPrint("EMC: Pack L3 data error!");
		zIhuRunErrCnt[TASK_ID_EMC]++;
	}
	snd.length = sizeof(msg_struct_emc_asylibra_data_req_t);    
	ret = ihu_message_send(MSG_ID_EMC_ASYLIBRA_DATA_REQ, TASK_ID_ASYLIBRA, TASK_ID_EMC, &snd, snd.length);
	if (ret == FAILURE){
		sprintf(strDebug, "EMC: Send MSG_ID_EMC_ASYLIBRA_DATA_REQ message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_EMC], zIhuTaskNameList[TASK_ID_ASYLIBRA]);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}	
	
	return SUCCESS;
}

//存储离线数据到本地
//什么时候存储，完全由应用程序决定，这里不做检查
//我们的逻辑是，定时数据才存储，瞬时数据则不存
OPSTAT func_emc_store_data_offline(void)
{
	int ret=0;
	
	IhuDiscDataSampleStorageArray_t record;
	memset(&record, 0, sizeof(IhuDiscDataSampleStorageArray_t));
	record.equipid = IHU_EMC_EQUID_DEFAULT;
	record.sensortype = L3CI_emc;
	record.onOffLine = IHU_DISC_DATA_SAMPLE_OFFLINE;
	record.timestamp = zIhuSystemTimeUnix;
	record.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
	record.emcValue = ihu_emc_adc_read();
	record.gpsx = 111;
	record.gpsy = 222;
	record.gpsz = 333;

	//RECORD存入内存盘
	if (IHU_MEM_SENSOR_SAVE_FLAG == IHU_MEM_SENSOR_SAVE_FLAG_YES)
	{
		ret = ihu_save_to_storage_mem(&record);
		if (ret == FAILURE){
			zIhuRunErrCnt[TASK_ID_EMC]++;
			IhuErrorPrint("EMC: Can not save data into memory buffer, might par error!");
		}
	}
	//RECORD存入硬盘
	if (IHU_DISC_SENSOR_SAVE_FLAG == IHU_DISC_SENSOR_SAVE_FLAG_YES)
	{
		ret = ihu_save_to_storage_disc(&record, sizeof(IhuDiscDataSampleStorageArray_t));
		if (ret == FAILURE){
			zIhuRunErrCnt[TASK_ID_EMC]++;
			IhuErrorPrint("EMC: Can not save data into flash/hard disk!");
		}
	}	

	//返回
	return SUCCESS;
}

//存入数据到本地内存磁盘
OPSTAT ihu_save_to_storage_mem(IhuDiscDataSampleStorageArray_t *record)
{
	char strDebug[IHU_PRINT_CHAR_SIZE];
	UINT32 readCnt=0, wrtCnt=0, totalNbr=0, sid=0;

	//先检查输入数据的合法性，以下三项必须填写，其它的无所谓
	if (((record->sensortype)<=0) || ((record->timestamp)<=0)){
		IhuErrorPrint("EMC: Error input of data save to memory, on Sensor type or Time stamp!");
		zIhuRunErrCnt[TASK_ID_EMC]++;
		return FAILURE;
	}
	if (((record->onOffLine) != IHU_DISC_DATA_SAMPLE_ONLINE) && ((record->onOffLine) != IHU_DISC_DATA_SAMPLE_OFFLINE)){
		IhuErrorPrint("EMC: Error input of data save to memory, on on/off line attribute!");
		zIhuRunErrCnt[TASK_ID_EMC]++;
		return FAILURE;
	}

	//取得缓冲区数据的状态
	readCnt = zIhuMemStorageBuf.rdCnt;
	wrtCnt = zIhuMemStorageBuf.wrtCnt;
	totalNbr = zIhuMemStorageBuf.recordNbr;
	sid = zIhuMemStorageBuf.lastSid+1; //达到32位的最大值，需要连续工作几百年，几乎不可能

	//先清零原先的记录，然后再贯穿写，不然之前的记录会污染当前的记录
	//但是一股脑儿的全部拷贝，反而不会出错，就不需要清零原来的记录了
	memcpy(&zIhuMemStorageBuf.recordItem[wrtCnt], record, sizeof(IhuDiscDataSampleStorageArray_t));
	if ((record->onOffLine) == IHU_DISC_DATA_SAMPLE_OFFLINE) {
		zIhuMemStorageBuf.offlineNbr = zIhuMemStorageBuf.offlineNbr + 1;
		if (zIhuMemStorageBuf.offlineNbr >= IHU_DISC_DATA_SAMPLE_STORAGE_NBR_MAX){
			zIhuMemStorageBuf.offlineNbr = IHU_DISC_DATA_SAMPLE_STORAGE_NBR_MAX;
		}
	}

	//判断是否满，从而回写
	wrtCnt++;
	wrtCnt = wrtCnt % IHU_DISC_DATA_SAMPLE_STORAGE_NBR_MAX;
	if (totalNbr >= IHU_DISC_DATA_SAMPLE_STORAGE_NBR_MAX)
	{
		totalNbr = IHU_DISC_DATA_SAMPLE_STORAGE_NBR_MAX;
		readCnt++;
		readCnt = readCnt % IHU_DISC_DATA_SAMPLE_STORAGE_NBR_MAX;
	}else{
		totalNbr++;
	}
	zIhuMemStorageBuf.rdCnt = readCnt;
	zIhuMemStorageBuf.wrtCnt = wrtCnt;
	zIhuMemStorageBuf.recordNbr = totalNbr;
	zIhuMemStorageBuf.lastSid = sid;  //最新一个写入记录的SID数值

	//Always successful, as the storage is a cycle buffer!
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_NOR_ON) != FALSE){
		sprintf(strDebug, "EMC: Data record save to MEM-DISC, sid=%d, totalNbr=%d, offNbr=%d.", sid, totalNbr, zIhuMemStorageBuf.offlineNbr);
		IhuDebugPrint(strDebug);
	}
	
	//返回
	return SUCCESS;
}

//从本地内存磁盘，读取数据并送往后台
OPSTAT ihu_read_from_storage_mem(IhuDiscDataSampleStorageArray_t *record)
{
	char strDebug[IHU_PRINT_CHAR_SIZE];
	UINT32 readCnt=0, totalNbr=0;

	readCnt = zIhuMemStorageBuf.rdCnt;
	totalNbr = zIhuMemStorageBuf.recordNbr;
	memset(record, 0, sizeof(IhuDiscDataSampleStorageArray_t));

	//先检查输入数据的合法性，以下三项必须填写，其它的无所谓
	//总控数量优先
	if (totalNbr <=0 ){
		zIhuRunErrCnt[TASK_ID_EMC]++;
		return FAILURE;
	}

	//读取一个记录
	//总数totalNbr和离线计数offlineNbr双重控制，如果出现不一致，一定是存储读取错误
	bool flag = FALSE;
	while(totalNbr>0){
		if (zIhuMemStorageBuf.recordItem[readCnt].onOffLine == IHU_DISC_DATA_SAMPLE_OFFLINE){
			memcpy(record, &zIhuMemStorageBuf.recordItem[readCnt], sizeof(IhuDiscDataSampleStorageArray_t));
			zIhuMemStorageBuf.recordItem[readCnt].onOffLine = IHU_DISC_DATA_SAMPLE_ONLINE; //used as backup
			if ((zIhuMemStorageBuf.offlineNbr <=0) || (zIhuMemStorageBuf.offlineNbr > totalNbr)){
				IhuErrorPrint("EMC: Error occurs during totalNbr/offlineNbr control MEM STORAGE, recover!");
				zIhuMemStorageBuf.offlineNbr = totalNbr;//强制将offlineNbr设置为跟totalNbr一致，但基本上没啥作用，而且还不影响继续发送数据到后台
			}
			totalNbr--;
			readCnt++;
			readCnt = readCnt % IHU_DISC_DATA_SAMPLE_STORAGE_NBR_MAX;
			zIhuMemStorageBuf.rdCnt = readCnt;
			zIhuMemStorageBuf.recordNbr = totalNbr;
			zIhuMemStorageBuf.offlineNbr--;
			flag = TRUE;
			break;
		}else{
			totalNbr--;
			readCnt++;
			readCnt = readCnt % IHU_DISC_DATA_SAMPLE_STORAGE_NBR_MAX;
			zIhuMemStorageBuf.rdCnt = readCnt;
			zIhuMemStorageBuf.recordNbr = totalNbr;
		}
	}
	//如果offlineNbr>0，数据又没有读到，这个也是正常情况，因为Offline数据可能被桶形ONLINE数据给覆盖掉了
	//所以没有读到数据的情形，也得当正常情形来看待，而不能当出错，不然处理方式就不对
	if (flag == FALSE){
		sprintf(strDebug, "EMC: Data read nothing, rdCnt=%d, totalNbr = %d, offNbr=%d.", readCnt, totalNbr, zIhuMemStorageBuf.offlineNbr);
		IhuDebugPrint(strDebug);
		zIhuRunErrCnt[TASK_ID_EMC]++;
		return FAILURE;
	}

	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_NOR_ON) != FALSE){
		sprintf(strDebug, "EMC: Data record read from MEM-DISC, rdCnt=%d, totalNbr = %d, offNbr=%d.", readCnt, totalNbr, zIhuMemStorageBuf.offlineNbr);		
		IhuDebugPrint(strDebug);
	}
	
	//返回
	return SUCCESS;
}

//记录多少条，是否超限，过时清理等等，都在这儿完成
//存入FLASH文件系统
OPSTAT ihu_save_to_storage_disc(void *dataBuffer, UINT32 dataLen)
{
	return SUCCESS;
}

OPSTAT ihu_read_from_storage_disc(void *dataBuffer, UINT32 dataLen)
{
	return SUCCESS;
}

//基础数学函数，不应该搞的过于复杂
//完全基于设计要点的定义
OPSTAT ihu_unpack_l3_data_from_cloud(UINT8 *buf, UINT8 len, IhuL3DataFormatTag_t *output)
{
	if ((buf == NULL) || (len <=0) || (len > MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL)) return FAILURE;
	
	if (*buf == L3CI_time_sync){
		output->UserCmdId = L3CI_time_sync;
		if (*(buf+1) != 4) return FAILURE;
		output->timeSync.timeStamp = ((*(buf+2)) << 24) & 0xFF000000 + ((*(buf+3)) << 16) & 0xFF0000 + ((*(buf+4)) << 8) & 0xF000 + (*(buf+5) & 0xFF);
	}
	else if (*buf == L3CI_equipment_info){
		output->UserCmdId = L3CI_equipment_info;
		if (*(buf+1) != 0) return FAILURE;
	}
	else if (*buf == L3CI_emc){
		output->UserCmdId = L3CI_emc;
		if (*(buf+1) != 0) return FAILURE;
	}
	else{
		return FAILURE;
	}
	return SUCCESS;
}

//基础数学函数，不应该搞的过于复杂
//完全基于设计要点的定义
OPSTAT ihu_pack_l3_data_to_cloud(IhuL3DataFormatTag_t *input, UINT8 *buf, UINT8 len)
{
	if ((buf == NULL) || (input == NULL)) return FAILURE;
	
	if (input->UserCmdId == L3CI_time_sync){
		*buf = L3CI_time_sync;
		*(buf+1) = 0;
		len = 2;
	}
	else if (input->UserCmdId == L3CI_equipment_info){
		*buf = L3CI_equipment_info;
		*(buf+1) = sizeof(input->equInfo);
		memcpy(buf+2, &(input->equInfo), sizeof(input->equInfo));
		len = sizeof(input->equInfo) + 2;
	}
	else if (input->UserCmdId == L3CI_emc){
		*buf = L3CI_emc;
		*(buf+1) = sizeof(input->emcInfo);
		memcpy(buf+2, &(input->emcInfo), sizeof(input->emcInfo));
		len = sizeof(input->emcInfo) + 2;
	}
	else{
		len = 0;
		return FAILURE;
	}
	
	return len; //纯粹为了消除WARNING，未来待完善这里的业务逻辑
	//return SUCCESS;
}
