/**
 ****************************************************************************************
 *
 * @file l1timer_ucos.c
 *
 * @brief L1 TIMER ucosIII
 *
 * BXXH team
 * Created by ZJL, 20160127
 *
 ****************************************************************************************
 */
 
#include "l1timer_ucos.h"

/*
** FSM of the TIMER
*/
FsmStateItem_t FsmTimer[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_timer_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_timer_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_timer_restart},
  {MSG_ID_COM_INIT_FB,										FSM_STATE_IDLE,            									fsm_com_do_nothing},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_TIMER_AVTIVE,         						fsm_timer_restart},
  {MSG_ID_COM_INIT_FB,										FSM_STATE_TIMER_AVTIVE,         						fsm_com_do_nothing},
  {MSG_ID_COM_STOP,												FSM_STATE_TIMER_AVTIVE,         						fsm_timer_stop_rcv},

  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
IhuTimerTable_t zIhuTimerTable;
OS_TMR zIhuL1timer1s, zIhuL1timer10ms, zIhuL1timer1ms;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_timer_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_TIMER, FSM_STATE_IDLE) == FAILURE){
		IhuErrorPrint("TIMER: Error Set FSM State at fsm_timer_task_entry.");
	}
	return SUCCESS;
}

OPSTAT fsm_timer_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret=0;
	OS_ERR err;
	
	//串行会送INIT_FB给VM，不然消息队列不够深度，此为节省内存机制
	if ((src_id > TASK_ID_MIN) && (src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM。由于消息队列不够深，所以不再回送FB证实。
		//暂时不删除以下代码，留待未来使用
//		msg_struct_com_init_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
//		snd.length = sizeof(msg_struct_com_init_fb_t);
//		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_TIMER, &snd, snd.length);
//		if (ret == FAILURE){
//			IhuErrorPrint("TIMER: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_TIMER], zIhuTaskNameList[src_id]);
//			return FAILURE;
//		}
	}

	//INIT this task global variables zIhuTimerTable
	memset(&zIhuTimerTable, 0, sizeof(IhuTimerTable_t));
	zIhuRunErrCnt[TASK_ID_TIMER] = 0;
	
	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_TIMER, FSM_STATE_TIMER_INITED) == FAILURE){
		zIhuRunErrCnt[TASK_ID_TIMER]++;
		IhuErrorPrint("TIMER: Error Set FSM State!");	
		return FAILURE;
	}

	//初始化硬件接口
	if (func_timer_hw_init() == FAILURE){	
		IhuErrorPrint("TIMER: Error initialize interface!");
		return FAILURE;
	}

	/*
	*   沿用HCU的程序代码风格，这里采用了ucos对应的定时器，然后生成自己的软定时器
	*   方法四：任务CALLBACK，实时系统的稳态性肯定要差点，但还是在某种限度内，没有太大的问题。
	*   为了生成10ms的任务级时钟，以下缺省配置从10u改为了100u，从而将任务级的时钟频率改为100Hz
	*   #define  OS_CFG_TMR_TASK_RATE_HZ          100u              // Rate for timers (10 Hz Typ.)
	*/

	//1s
	if (IHU_TIMER_CONFIG_START_RESOLUTION_1S == TRUE){
		OSTmrCreate(&zIhuL1timer1s, 
			"VMUO HW Timer 1S", 
			rand()%IHU_TIMER_TICK_1_SEC,   //Delay task level ticks
			IHU_TIMER_TICK_1_SEC,          //Period task level ticks
			OS_OPT_TMR_PERIODIC, 
			(OS_TMR_CALLBACK_PTR)func_timer_routine_handler_1s, 
			NULL,         //Arguments
			&err); 
		if (err != OS_ERR_NONE){
				zIhuRunErrCnt[TASK_ID_TIMER]++;
				IhuErrorPrint("TIMER: Create 1s timers error, err=%d, errno=%d, %s\n", err, errno, strerror(err));
				return FAILURE;
		}
		OSTmrStart((OS_TMR *)&zIhuL1timer1s, (OS_ERR *)&err);
		if (err != OS_ERR_NONE){
			zIhuRunErrCnt[TASK_ID_TIMER]++;
			IhuErrorPrint("TIMER: Start 1s timers error, err=%d, errno=%d, %s\n", err, errno, strerror(err));
			return FAILURE;
		}
	}

  //10ms帧协议定时器
	if (IHU_TIMER_CONFIG_START_RESOLUTION_10MS == TRUE){
		OSTmrCreate(&zIhuL1timer10ms, 
			"VMUO HW Timer 10MS", 
			rand()%IHU_TIMER_TICK_10_MS,   //Delay task level ticks
			IHU_TIMER_TICK_10_MS,          //Period task level ticks
			OS_OPT_TMR_PERIODIC, 
			(OS_TMR_CALLBACK_PTR)func_timer_routine_handler_10ms, 
			NULL,         //Arguments
			&err); 
		if (err != OS_ERR_NONE){
			zIhuRunErrCnt[TASK_ID_TIMER]++;
			IhuErrorPrint("TIMER: Create 10ms timers error, err=%d, errno=%d, %s\n", err, errno, strerror(err));
			return FAILURE;
		}
		OSTmrStart((OS_TMR *)&zIhuL1timer10ms, (OS_ERR *)&err);
		if (err != OS_ERR_NONE){
			zIhuRunErrCnt[TASK_ID_TIMER]++;
			IhuErrorPrint("TIMER: Start 10ms timers error, err=%d, errno=%d, %s\n", err, errno, strerror(err));
			return FAILURE;
		}
	}

  //1ms时钟，这个应该是挺影响性能的，如果系统中没有用到，可以去掉
	if (IHU_TIMER_CONFIG_START_RESOLUTION_1MS == TRUE){
		OSTmrCreate(&zIhuL1timer1ms, 
			"VMUO HW Timer 1MS", 
			rand()%IHU_TIMER_TICK_1_MS,   //Delay task level ticks
			IHU_TIMER_TICK_1_MS,          //Period task level ticks
			OS_OPT_TMR_PERIODIC, 
			(OS_TMR_CALLBACK_PTR)func_timer_routine_handler_1ms, 
			NULL,         //Arguments
			&err); 
		if (err != OS_ERR_NONE){
			zIhuRunErrCnt[TASK_ID_TIMER]++;
			IhuErrorPrint("TIMER: Create 1ms timers error, err=%d, errno=%d, %s\n", err, errno, strerror(err));
			return FAILURE;
		}
		OSTmrStart((OS_TMR *)&zIhuL1timer1ms, (OS_ERR *)&err);
		if (err != OS_ERR_NONE){
			zIhuRunErrCnt[TASK_ID_TIMER]++;
			IhuErrorPrint("TIMER: Start 1ms timers error, err=%d, errno=%d, %s\n", err, errno, strerror(err));
			return FAILURE;
		}
	}
	
	//进入等待反馈状态
	if (FsmSetState(TASK_ID_TIMER, FSM_STATE_TIMER_AVTIVE) == FAILURE){
		zIhuRunErrCnt[TASK_ID_TIMER]++;
		IhuErrorPrint("TIMER: Error Set FSM State!");
		return FAILURE;
	}
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("TIMER: Enter FSM_STATE_TIMER_ACTIVE status, Keeping refresh here!");
	}

	//进入永久的监控状态
	//如果这里进入永久不出来的状态，意味着L1TIMER任务将不受控，该任务一直阻塞在这儿，无法再能接受到任何其他消息
	//如果这就是我们想要的，那正常。如果不是，那这里就不能如此设计。
//	while(1) {
//		ihu_sleep(60);
//		ihu_vm_check_task_que_status_and_action();
//	  }
//	
	//返回，由于到达不了，所以这里就隐去，以便消除COMPILE WARNING
	return SUCCESS;
}

//是否能起到作用，待定
OPSTAT fsm_timer_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	//msg_struct_com_restart_t snd;
	
	IhuErrorPrint("TIMER: Internal error counter reach DEAD level, SW-RESTART soon!");
	fsm_timer_init(0, 0, NULL, 0);
	
	//由于时钟是系统工作的基础，该模块启动后，必须让VM启动，并初始化所有任务模块
//	memset(&snd, 0, sizeof(msg_struct_com_restart_t));
//	snd.length = sizeof(msg_struct_com_restart_t);
//	ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_VMUO, TASK_ID_TIMER, &snd, snd.length);
//	if (ret == FAILURE){
//		zIhuRunErrCnt[TASK_ID_TIMER]++;
//		sprintf(strDebug, "TIMER: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_TIMER], zIhuTaskNameList[TASK_ID_VMFO]);
//		IhuErrorPrint(strDebug);
//		return FAILURE;
//	}
	
	return SUCCESS;
}

OPSTAT fsm_timer_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_TIMER)){
		zIhuRunErrCnt[TASK_ID_TIMER]++;
		IhuErrorPrint("TIMER: Wrong input paramters!");
		return FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_TIMER, FSM_STATE_IDLE) == FAILURE){
		zIhuRunErrCnt[TASK_ID_TIMER]++;
		IhuErrorPrint("TIMER: Error Set FSM State!");
		return FAILURE;
	}
	
	//返回
	return SUCCESS;
}

//Local APIs
OPSTAT func_timer_hw_init(void)
{
	return SUCCESS;
}

//定时器启动，支持远程调用
OPSTAT ihu_timer_start(UINT8 task_id, UINT8 timer_id, UINT32 t_dur, UINT8 t_type, UINT8 t_res)
{	
	//检查task_id是否合法
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		IhuErrorPrint("TIMER: Error on timer start src_id =%d [%s]!!!", task_id, zIhuTaskNameList[task_id]);
		zIhuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}

	//检查timer_duartion是否合法
	if ((t_dur > MAX_TIMER_SET_DURATION) || (t_dur <= 0)){
		IhuErrorPrint("TIMER: Error on timer start timer duration!!!");		
		zIhuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}

	//检查t_type是否合法
	if ((t_type != TIMER_TYPE_ONE_TIME) && (t_type != TIMER_TYPE_PERIOD)){
		IhuErrorPrint("TIMER: Error on timer start type!!!");
		zIhuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}

	//检查t_resolution & timer_id 是否合法
	if (t_res == TIMER_RESOLUTION_1S){
		if ((timer_id >= MAX_TIMER_NUM_IN_ONE_IHU_1S) || (timer_id <= TIMER_ID_1S_MIN)){
			IhuErrorPrint("TIMER: Error on timer start 1S timerId!!!\n");
			zIhuRunErrCnt[TASK_ID_TIMER]++;
			return FAILURE;
		}
	}else if (t_res == TIMER_RESOLUTION_10MS){
		if ((timer_id >= MAX_TIMER_NUM_IN_ONE_IHU_10MS) || (timer_id <= TIMER_ID_10MS_MIN)){
			IhuErrorPrint("TIMER: Error on timer start 10MS timerId!!!");
			zIhuRunErrCnt[TASK_ID_TIMER]++;
			return FAILURE;
		}
	}
	else if (t_res == TIMER_RESOLUTION_1MS){
		if ((timer_id >= MAX_TIMER_NUM_IN_ONE_IHU_1MS) || (timer_id <= TIMER_ID_1MS_MIN)){
			IhuErrorPrint("TIMER: Error on timer start 1MS timerId!!!");
			zIhuRunErrCnt[TASK_ID_TIMER]++;
			return FAILURE;
		}
	}
	else{
		zIhuRunErrCnt[TASK_ID_TIMER]++;
		IhuErrorPrint("TIMER: Error on timer start timer resolution!!!");
		return FAILURE;
	}

	//设置进全局数据表单
	if (t_res == TIMER_RESOLUTION_1S){
		zIhuTimerTable.timer1s[timer_id].taskId = task_id;
		zIhuTimerTable.timer1s[timer_id].timerId = timer_id;
		zIhuTimerTable.timer1s[timer_id].tDuration = t_dur;
		zIhuTimerTable.timer1s[timer_id].timerType = t_type;
		zIhuTimerTable.timer1s[timer_id].timerRes = TIMER_RESOLUTION_1S;
		zIhuTimerTable.timer1s[timer_id].status = TIMER_STATUS_START;
	}
	else if (t_res == TIMER_RESOLUTION_10MS){
		zIhuTimerTable.timer10ms[timer_id].taskId = task_id;
		zIhuTimerTable.timer10ms[timer_id].timerId = timer_id;
		zIhuTimerTable.timer10ms[timer_id].tDuration = t_dur;
		zIhuTimerTable.timer10ms[timer_id].timerType = t_type;
		zIhuTimerTable.timer10ms[timer_id].timerRes = TIMER_RESOLUTION_10MS;
		zIhuTimerTable.timer10ms[timer_id].status = TIMER_STATUS_START;
	}else if (t_res == TIMER_RESOLUTION_1MS){
		zIhuTimerTable.timer1ms[timer_id].taskId = task_id;
		zIhuTimerTable.timer1ms[timer_id].timerId = timer_id;
		zIhuTimerTable.timer1ms[timer_id].tDuration = t_dur;
		zIhuTimerTable.timer1ms[timer_id].timerType = t_type;
		zIhuTimerTable.timer1ms[timer_id].timerRes = TIMER_RESOLUTION_1MS;
		zIhuTimerTable.timer1ms[timer_id].status = TIMER_STATUS_START;
	}

	return SUCCESS;
}

//全局基础函数，支持时钟的停止
OPSTAT ihu_timer_stop(UINT8 task_id, UINT8 timer_id, UINT8 t_res)
{
	//检查task_id是否合法
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		IhuErrorPrint("TIMER: Error on timer stop src_id =%d [%s]!!!", task_id, zIhuTaskNameList[task_id]);
		zIhuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}

	//检查t_resolution & timer_id 是否合法
	if (t_res == TIMER_RESOLUTION_1S){
		if ((timer_id >= MAX_TIMER_NUM_IN_ONE_IHU_1S) || (timer_id <= TIMER_ID_1S_MIN)){
			IhuErrorPrint("TIMER: Error on timer stop 1S timerId!!!");
			zIhuRunErrCnt[TASK_ID_TIMER]++;
			return FAILURE;
		}
	}else if (t_res == TIMER_RESOLUTION_10MS){
		if ((timer_id >= MAX_TIMER_NUM_IN_ONE_IHU_10MS) || (timer_id <= TIMER_ID_10MS_MIN)){
			IhuErrorPrint("TIMER: Error on timer stop 10MS timerId!!!");
			zIhuRunErrCnt[TASK_ID_TIMER]++;
			return FAILURE;
		}
	}else if (t_res == TIMER_RESOLUTION_1MS){
		if ((timer_id >= MAX_TIMER_NUM_IN_ONE_IHU_1MS) || (timer_id <= TIMER_ID_1MS_MIN)){
			IhuErrorPrint("TIMER: Error on timer stop 1MS timerId!!!");
			zIhuRunErrCnt[TASK_ID_TIMER]++;
			return FAILURE;
		}
	}else{
		IhuErrorPrint("TIMER: Error on timer stop timer resolution!!!");
		zIhuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}

	//设置进全局数据表单
	if (t_res == TIMER_RESOLUTION_1S){
		if ((zIhuTimerTable.timer1s[timer_id].taskId == task_id) && (zIhuTimerTable.timer1s[timer_id].timerId == timer_id)){
			zIhuTimerTable.timer1s[timer_id].status = TIMER_STATUS_STOP;
		}else{
			zIhuTimerTable.timer1s[timer_id].status = TIMER_STATUS_DEACTIVE;
		}
	}else if (t_res == TIMER_RESOLUTION_10MS){
		if ((zIhuTimerTable.timer10ms[timer_id].taskId == task_id) && (zIhuTimerTable.timer10ms[timer_id].timerId == timer_id)){
			zIhuTimerTable.timer10ms[timer_id].status = TIMER_STATUS_STOP;
		}else{
			zIhuTimerTable.timer10ms[timer_id].status = TIMER_STATUS_DEACTIVE;
		}
	}else if (t_res == TIMER_RESOLUTION_1MS){
		if ((zIhuTimerTable.timer1ms[timer_id].taskId == task_id) && (zIhuTimerTable.timer1ms[timer_id].timerId == timer_id)){
			zIhuTimerTable.timer1ms[timer_id].status = TIMER_STATUS_STOP;
		}else{
			zIhuTimerTable.timer1ms[timer_id].status = TIMER_STATUS_DEACTIVE;
		}
	}
	return SUCCESS;
}

//线程机制，搞定时器
//OS_TMR *p_tmr该参数起到啥作用，如何起作用，暂时不清楚，等待未来进一步测试和挖掘
void func_timer_routine_handler_1s(OS_TMR *p_tmr, void *p_arg)
{
	int i=0, ret=0;
	msg_struct_com_time_out_t snd;

	//入参检查
	
	//扫描
	for (i=0;i<MAX_TIMER_NUM_IN_ONE_IHU_1S;i++)
	{		
		//Active status
		if (zIhuTimerTable.timer1s[i].status == TIMER_STATUS_ACTIVE){
			if (zIhuTimerTable.timer1s[i].elapse > zIhuTimerTable.timer1s[i].tDuration){
				IhuErrorPrint("TIMER: Error elapse value meet!\n");
				zIhuTimerTable.timer1s[i].elapse = zIhuTimerTable.timer1s[i].elapse % zIhuTimerTable.timer1s[i].tDuration;
			}
			if (zIhuTimerTable.timer1s[i].elapse >= 1){
				//防止出现潜在问题，求余再减1
				zIhuTimerTable.timer1s[i].elapse--;
			}else{
				//change ACTIVE to STOP, no
				if (zIhuTimerTable.timer1s[i].timerType == TIMER_TYPE_ONE_TIME){
					zIhuTimerTable.timer1s[i].status = TIMER_STATUS_STOP;
					zIhuTimerTable.timer1s[i].elapse = 0;
				}
				//No change for PERIOD, re-start this timer
				//本轮也算1秒，所以需要在周期的基础上减1
				else if (zIhuTimerTable.timer1s[i].timerType == TIMER_TYPE_PERIOD){
					zIhuTimerTable.timer1s[i].elapse = zIhuTimerTable.timer1s[i].tDuration-1;
				}
				//Send message time_out
				memset(&snd, 0, sizeof(msg_struct_com_time_out_t));
				snd.length = sizeof(msg_struct_com_time_out_t);
				snd.timeId = zIhuTimerTable.timer1s[i].timerId;
				snd.timeRes = zIhuTimerTable.timer1s[i].timerRes;
				ret = ihu_message_send(MSG_ID_COM_TIME_OUT, zIhuTimerTable.timer1s[i].taskId, TASK_ID_TIMER, &snd, snd.length);
				if (ret == FAILURE){
					zIhuRunErrCnt[TASK_ID_TIMER]++;
					IhuErrorPrint("TIMER: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_TIMER], zIhuTaskNameList[zIhuTimerTable.timer1s[i].taskId]);
					return;
				}
			}//Elapse <= 0, timeout reach
		}
		//START status
		else if(zIhuTimerTable.timer1s[i].status == TIMER_STATUS_START){
			zIhuTimerTable.timer1s[i].elapse = zIhuTimerTable.timer1s[i].tDuration;
			zIhuTimerTable.timer1s[i].status = TIMER_STATUS_ACTIVE;
		}//Not meet ACTIVE or START timer
	}//for-MAX_TIMER_NUM_IN_ONE_IHU_1S
}


//线程机制，搞定时器
void func_timer_routine_handler_10ms(OS_TMR *p_tmr, void *p_arg)
{
	int i=0, ret=0;
	msg_struct_com_time_out_t snd;

	//入参检查

	//扫描
	for (i=0;i<MAX_TIMER_NUM_IN_ONE_IHU_10MS;i++)
	{
		//Active status
		if (zIhuTimerTable.timer10ms[i].status == TIMER_STATUS_ACTIVE){
			if (zIhuTimerTable.timer10ms[i].elapse > zIhuTimerTable.timer10ms[i].tDuration){
				IhuErrorPrint("TIMER: Error elapse value meet!\n");
				zIhuTimerTable.timer10ms[i].elapse = zIhuTimerTable.timer10ms[i].elapse % zIhuTimerTable.timer10ms[i].tDuration;
			}
			if (zIhuTimerTable.timer10ms[i].elapse >= 1){
				//防止出现潜在问题，求余再减1
				zIhuTimerTable.timer10ms[i].elapse--;
			}else{
				//change ACTIVE to STOP, no
				if (zIhuTimerTable.timer10ms[i].timerType == TIMER_TYPE_ONE_TIME){
					zIhuTimerTable.timer10ms[i].status = TIMER_STATUS_STOP;
					zIhuTimerTable.timer10ms[i].elapse = 0;
				}
				//No change for PERIOD, re-start this timer
				//本轮也算1次，所以需要在周期的基础上减1，不然会出现问题
				else if (zIhuTimerTable.timer10ms[i].timerType == TIMER_TYPE_PERIOD){
					zIhuTimerTable.timer10ms[i].elapse = zIhuTimerTable.timer10ms[i].tDuration-1;
				}
				//Send message time_out
				memset(&snd, 0, sizeof(msg_struct_com_time_out_t));
				snd.length = sizeof(msg_struct_com_time_out_t);
				snd.timeId = zIhuTimerTable.timer10ms[i].timerId;
				snd.timeRes = zIhuTimerTable.timer10ms[i].timerRes;
				ret = ihu_message_send(MSG_ID_COM_TIME_OUT, zIhuTimerTable.timer10ms[i].taskId, TASK_ID_TIMER, &snd, snd.length);
				if (ret == FAILURE){
					zIhuRunErrCnt[TASK_ID_TIMER]++;
					IhuErrorPrint("TIMER: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_TIMER], zIhuTaskNameList[zIhuTimerTable.timer10ms[i].taskId]);
					return;
				}
			}//Elapse <= 0, timeout reach
		}
		//START status
		else if(zIhuTimerTable.timer10ms[i].status == TIMER_STATUS_START){
			zIhuTimerTable.timer10ms[i].elapse = zIhuTimerTable.timer10ms[i].tDuration;
			zIhuTimerTable.timer10ms[i].status = TIMER_STATUS_ACTIVE;
		}//Not meet ACTIVE or START timer
	}//for-MAX_TIMER_NUM_IN_ONE_IHU_10MS
}

//线程机制，搞定时器
void func_timer_routine_handler_1ms(OS_TMR *p_tmr, void *p_arg)
{
	int i=0, ret=0;
	msg_struct_com_time_out_t snd;

	//入参检查

	//扫描
	for (i=0;i<MAX_TIMER_NUM_IN_ONE_IHU_1MS;i++)
	{
		//Active status
		if (zIhuTimerTable.timer1ms[i].status == TIMER_STATUS_ACTIVE){
			if (zIhuTimerTable.timer1ms[i].elapse > zIhuTimerTable.timer1ms[i].tDuration){
				IhuErrorPrint("TIMER: Error elapse value meet!\n");
				zIhuTimerTable.timer1ms[i].elapse = zIhuTimerTable.timer1ms[i].elapse % zIhuTimerTable.timer1ms[i].tDuration;
			}
			if (zIhuTimerTable.timer1ms[i].elapse >= 1){
				//防止出现潜在问题，求余再减1
				zIhuTimerTable.timer1ms[i].elapse--;
			}else{
				//change ACTIVE to STOP, no
				if (zIhuTimerTable.timer1ms[i].timerType == TIMER_TYPE_ONE_TIME){
					zIhuTimerTable.timer1ms[i].status = TIMER_STATUS_STOP;
					zIhuTimerTable.timer1ms[i].elapse = 0;
				}
				//No change for PERIOD, re-start this timer
				//本轮也算1次，所以需要在周期的基础上减1，不然会出现问题
				else if (zIhuTimerTable.timer1ms[i].timerType == TIMER_TYPE_PERIOD){
					zIhuTimerTable.timer1ms[i].elapse = zIhuTimerTable.timer1ms[i].tDuration-1;
				}
				//Send message time_out
				memset(&snd, 0, sizeof(msg_struct_com_time_out_t));
				snd.length = sizeof(msg_struct_com_time_out_t);
				snd.timeId = zIhuTimerTable.timer1ms[i].timerId;
				snd.timeRes = zIhuTimerTable.timer1ms[i].timerRes;
				ret = ihu_message_send(MSG_ID_COM_TIME_OUT, zIhuTimerTable.timer1ms[i].taskId, TASK_ID_TIMER, &snd, snd.length);
				if (ret == FAILURE){
					zIhuRunErrCnt[TASK_ID_TIMER]++;
					IhuErrorPrint("TIMER: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_TIMER], zIhuTaskNameList[zIhuTimerTable.timer1ms[i].taskId]);
					return;
				}
			}//Elapse <= 0, timeout reach
		}
		//START status
		else if(zIhuTimerTable.timer1ms[i].status == TIMER_STATUS_START){
			zIhuTimerTable.timer1ms[i].elapse = zIhuTimerTable.timer1ms[i].tDuration;
			zIhuTimerTable.timer1ms[i].status = TIMER_STATUS_ACTIVE;
		}//Not meet ACTIVE or START timer
	}//for-MAX_TIMER_NUM_IN_ONE_IHU_1MS
}


