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
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_spsvirgo_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_spsvirgo_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_spsvirgo_restart},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_SPSVIRGO_INITED,         					fsm_spsvirgo_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_SPSVIRGO_INITED,         					fsm_spsvirgo_restart},

	//Task level actived status
  {MSG_ID_COM_RESTART,        						FSM_STATE_SPSVIRGO_ACTIVED,         					fsm_spsvirgo_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_SPSVIRGO_ACTIVED,         					fsm_spsvirgo_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_SPSVIRGO_ACTIVED,         				  fsm_spsvirgo_time_out},
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
extern vu8 SPS_GPRS_R_Buff[SPS_GPRS_REC_MAXLEN];	//串口1数据接收缓冲区 
extern vu8 SPS_GPRS_R_State;						//串口1接收状态
extern vu16 SPS_GPRS_R_Count;						//当前接收数据的字节数 	 
extern vu8 SPS_RFID_R_Buff[SPS_RFID_REC_MAXLEN];	//串口1数据接收缓冲区 
extern vu8 SPS_RFID_R_State;						//串口1接收状态
extern vu16 SPS_RFID_R_Count;						//当前接收数据的字节数 	 
extern vu8 SPS_BLE_R_Buff[SPS_BLE_REC_MAXLEN];	//串口1数据接收缓冲区 
extern vu8 SPS_BLE_R_State;						//串口1接收状态
extern vu16 SPS_BLE_R_Count;						//当前接收数据的字节数 	 
extern vu8 SPS_SPARE1_R_Buff[SPS_SPARE1_REC_MAXLEN];	//串口1数据接收缓冲区 
extern vu8 SPS_SPARE1_R_State;						//串口1接收状态
extern vu16 SPS_SPARE1_R_Count;						//当前接收数据的字节数

UINT8 zIhuGprsOperationFlag = 0;

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

	//串行会送INIT_FB给VM，不然消息队列不够深度，此为节省内存机制
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM。由于消息队列不够深，所以不再回送FB证实。
//		msg_struct_com_init_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
//		snd.length = sizeof(msg_struct_com_init_fb_t);
//		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_SPSVIRGO, &snd, snd.length);
//		if (ret == IHU_FAILURE){
//			IhuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_SPSVIRGO], zIhuTaskNameList[src_id]);
//			return IHU_FAILURE;
//		}
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
	zIhuGprsOperationFlag = 0;

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
	ihu_l1hd_sps_gprs_init_config(115200);//USART_GPRS初始化配置
	ihu_l1hd_sps_rfid_init_config(115200);//USART_RFID初始化配置
	ihu_l1hd_sps_ble_init_config(115200);//UART_BLE初始化配置
	ihu_l1hd_sps_spare1_init_config(115200);//UART_SPARE1初始化配置
	TIM_USART_GPRS_Init_Config();  //初始化跟GPRS时钟相关的TIM
	
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
	ihu_l1hd_sps_gprs_send_data("This is my GPRS test!\n", 20);
	ihu_l1hd_sps_rfid_send_data("This is my RFID test!\n", 20);
	ihu_l1hd_sps_ble_send_data("This is my BLE test!\n", 20);
	ihu_l1hd_sps_spare1_send_data("This is my SPARE1 test!\n", 30);

	zIhuGprsOperationFlag++;
	
	//只干活一次
	if (zIhuGprsOperationFlag == 1)	SPS_GPRS_GSM_test_selection(2);
	
//	if(SPS_GPRS_R_State == 1)//一帧数据接收完成
//	{
//		SPS_GPRS_SendData((INT8 *)SPS_GPRS_R_Buff, SPS_GPRS_R_Count);//USART1发送数据缓冲区数据(发送刚接收完成的一帧数据)
//		SPS_GPRS_R_State =0;
//		SPS_GPRS_R_Count =0;
//	}
	IhuDebugPrint("SPSVIRGO: Time Out Test!\n");
}


