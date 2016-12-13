/**
 ****************************************************************************************
 *
 * @file l2ledpisces.c
 *
 * @brief L2 LEDPISCES
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
 #include "l2ledpisces.h"
 
/*
** FSM of the LEDPISCES
*/
FsmStateItem_t FsmLedpisces[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_ledpisces_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_ledpisces_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_ledpisces_restart},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_LEDPISCES_INITED,         					fsm_ledpisces_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_LEDPISCES_INITED,         					fsm_ledpisces_stop_rcv},

	//Task level actived status
  {MSG_ID_COM_RESTART,        						FSM_STATE_LEDPISCES_ACTIVED,         					fsm_ledpisces_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_LEDPISCES_ACTIVED,         					fsm_ledpisces_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_LEDPISCES_ACTIVED,         				  fsm_ledpisces_time_out},
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_ledpisces_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_LEDPISCES, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("LEDPISCES: Error Set FSM State at fsm_ledpisces_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_ledpisces_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//串行回送INIT_FB给VMFO
	ihu_usleep(dest_id * IHU_MODULE_START_DISTRIBUTION_DELAY_DURATION);
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_LEDPISCES, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("LEDPISCES: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_LEDPISCES], zIhuTaskNameList[src_id]);
			return IHU_FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_LEDPISCES, FSM_STATE_LEDPISCES_INITED) == IHU_FAILURE){
		IhuErrorPrint("LEDPISCES: Error Set FSM State!\n");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_ledpisces_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("LEDPISCES: Error initialize interface!\n");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_LEDPISCES] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_LEDPISCES, FSM_STATE_LEDPISCES_ACTIVED) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_LEDPISCES]++;
		IhuErrorPrint("LEDPISCES: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//启动本地定时器，如果有必要
	ret = ihu_timer_start(TASK_ID_LEDPISCES, TIMER_ID_1S_LEDPISCES_PERIOD_SCAN, zIhuSysEngPar.timer.ledpiscesPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_LEDPISCES]++;
		IhuErrorPrint("LEDPISCES: Error start timer!\n");
		return IHU_FAILURE;
	}	
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("LEDPISCES: Enter FSM_STATE_LEDPISCES_ACTIVE status, Keeping refresh here!\n");
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_ledpisces_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("LEDPISCES: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_ledpisces_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_ledpisces_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_LEDPISCES)){
		zIhuRunErrCnt[TASK_ID_LEDPISCES]++;
		IhuErrorPrint("LEDPISCES: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_LEDPISCES, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_LEDPISCES]++;
		IhuErrorPrint("LEDPISCES: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_ledpisces_hw_init(void)
{
//	ihu_l1hd_led_f2board_init();                          /* LED初始化                          */
//	ihu_l1hd_led_f2timer_ind_init();
	//key_init();                                         /* 按键初始化                         */
	//spi_flash_init();                                   /* spi_flash硬件接口初始化            */
	//rtc_init();   	

	return IHU_SUCCESS;
}

//TIMER_OUT Processing
OPSTAT fsm_ledpisces_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("LEDPISCES: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_LEDPISCES]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuRunErrCnt[TASK_ID_LEDPISCES]是否超限
	if (zIhuRunErrCnt[TASK_ID_LEDPISCES] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuRunErrCnt[TASK_ID_LEDPISCES] = zIhuRunErrCnt[TASK_ID_LEDPISCES] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_LEDPISCES, TASK_ID_LEDPISCES, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_LEDPISCES]++;
			IhuErrorPrint("LEDPISCES: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_LEDPISCES], zIhuTaskNameList[TASK_ID_LEDPISCES]);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_LEDPISCES_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_LEDPISCES) != FSM_STATE_LEDPISCES_ACTIVED){
			ret = FsmSetState(TASK_ID_LEDPISCES, FSM_STATE_LEDPISCES_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_LEDPISCES]++;
				IhuErrorPrint("LEDPISCES: Error Set FSM State!\n");
				return IHU_FAILURE;
			}//FsmSetState
		}
		func_ledpisces_time_out_period_scan();
	}

	return IHU_SUCCESS;
}

void func_ledpisces_time_out_period_scan(void)
{
//	UINT8   second_buf;
//	UINT16  temp;  

	int ret = 0;
	
	//发送HeartBeat消息给VMFO模块，实现喂狗功能
	msg_struct_com_heart_beat_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_heart_beat_t));
	snd.length = sizeof(msg_struct_com_heart_beat_t);
	ret = ihu_message_send(MSG_ID_COM_HEART_BEAT, TASK_ID_VMFO, TASK_ID_LEDPISCES, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_LEDPISCES]++;
		IhuErrorPrint("LEDPISCES: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_LEDPISCES], zIhuTaskNameList[TASK_ID_VMFO]);
		return;
	}
	
//	if(W25Q32 == spi_flash_read_device_ID())
//	{
//			temp = 0;
//	}    
//	else
//	{
//			temp = 3000;
//	}   

//	while (1) 
//	{
//			if(KEY_VALUE_URSER_BUTTON == get_key_state())   /* 判断是否有按键按下                 */
//			{
//					led1_negation();
//					delay_ms(temp + 500);
//			}
//			else
//			{
//					rtc_get_time();
//					if(second_buf != g_current_time.second)
//					{
//							led1_negation();
//							second_buf = g_current_time.second;
//					}
//			}
//	}
//对LED灯进行反转性测试，暂时没有任何实际意义，只是为了点亮灯，表示整个板子还活着
	ihu_l1hd_led_f2board_negation();
	ihu_l1hd_led_f2timer_ind_negation();
	
	return;
}

