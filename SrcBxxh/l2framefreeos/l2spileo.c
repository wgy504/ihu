/**
 ****************************************************************************************
 *
 * @file l2spileo.c
 *
 * @brief L2 SPILEO
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#include "l2spileo.h"
#include "bsp_spi.h"

/*
** FSM of the SPILEO
*/
IhuFsmStateItem_t IhuFsmSpileo[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_spileo_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_spileo_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_spileo_restart},

  //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_SPILEO_INITED,         						fsm_spileo_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_SPILEO_INITED,         						fsm_spileo_stop_rcv},

	//Task level actived status
  {MSG_ID_COM_RESTART,        						FSM_STATE_SPILEO_ACTIVED,         					fsm_spileo_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_SPILEO_ACTIVED,         					fsm_spileo_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_SPILEO_ACTIVED,         				  fsm_spileo_time_out},
  {MSG_ID_SPI_L2FRAME_RCV,								FSM_STATE_SPILEO_ACTIVED,         					fsm_spileo_l2frame_rcv},
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
#else
	#error Un-correct constant definition
#endif

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_spileo_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_SPILEO, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("SPILEO: Error Set FSM State at fsm_spileo_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_spileo_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//串行回送INIT_FB给VMFO
	ihu_usleep(dest_id * IHU_MODULE_START_DISTRIBUTION_DELAY_DURATION);
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VMFO
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_SPILEO, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("SPILEO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPILEO].taskName, zIhuVmCtrTab.task[src_id].taskName);
			return IHU_FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_SPILEO, FSM_STATE_SPILEO_INITED) == IHU_FAILURE){
		IhuErrorPrint("SPILEO: Error Set FSM State!\n");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_spileo_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("SPILEO: Error initialize interface!\n");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO] = 0;
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
#else
	#error Un-correct constant definition
#endif
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SPILEO, FSM_STATE_SPILEO_ACTIVED) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO]++;
		IhuErrorPrint("SPILEO: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	if (IHU_SPILEO_PERIOD_TIMER_SET == IHU_SPILEO_PERIOD_TIMER_ACTIVE){
		//测试性启动周期性定时器
		ret = ihu_timer_start(TASK_ID_SPILEO, TIMER_ID_1S_SPILEO_PERIOD_SCAN, \
			zIhuSysEngPar.timer.array[TIMER_ID_1S_SPILEO_PERIOD_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO]++;
			IhuErrorPrint("SPILEO: Error start timer!\n");
			return IHU_FAILURE;
		}
	}
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("SPILEO: Enter FSM_STATE_SPILEO_ACTIVE status, Keeping refresh here!\n");
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_spileo_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("SPILEO: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_spileo_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_spileo_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_SPILEO)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO]++;
		IhuErrorPrint("SPILEO: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SPILEO, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO]++;
		IhuErrorPrint("SPILEO: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_spileo_hw_init(void)
{
	//初始化需要再完善，待完成：到底是放入到底层，还是在MAIN中完成？
	ihu_l1hd_spi_slave_hw_init();
	return IHU_SUCCESS;
}

OPSTAT fsm_spileo_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("SPILEO: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO]是否超限
	if (zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO] = zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_SPILEO, TASK_ID_SPILEO, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO]++;
			IhuErrorPrint("SPILEO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPILEO].taskName, zIhuVmCtrTab.task[TASK_ID_SPILEO].taskName);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_SPILEO_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_SPILEO) != FSM_STATE_SPILEO_ACTIVED){
			ret = FsmSetState(TASK_ID_SPILEO, FSM_STATE_SPILEO_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO]++;
				IhuErrorPrint("SPILEO: Error Set FSM State!\n");
				return IHU_FAILURE;
			}//FsmSetState
		}
		func_spileo_time_out_period_scan();
	}

	return IHU_SUCCESS;
}

void func_spileo_time_out_period_scan(void)
{
	int ret = 0;
	
	//发送HeartBeat消息给VMFO模块，实现喂狗功能
	msg_struct_com_heart_beat_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_heart_beat_t));
	snd.length = sizeof(msg_struct_com_heart_beat_t);
	ret = ihu_message_send(MSG_ID_COM_HEART_BEAT, TASK_ID_VMFO, TASK_ID_SPILEO, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO]++;
		IhuErrorPrint("SPILEO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_SPILEO].taskName, zIhuVmCtrTab.task[TASK_ID_VMFO].taskName);
		return;
	}
	
	return;
}

//L2FRAME Receive Processing
OPSTAT fsm_spileo_l2frame_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret;
	msg_struct_spileo_l2frame_rcv_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_spileo_l2frame_rcv_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_spileo_l2frame_rcv_t))){
		IhuErrorPrint("SPILEO: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_SPILEO]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//对于缓冲区的数据，进行分别处理，将帧变成不同的消息，分门别类发送到L3模块进行处理
	
	
	return IHU_SUCCESS;
}


