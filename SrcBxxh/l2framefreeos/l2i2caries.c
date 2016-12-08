/**
 ****************************************************************************************
 *
 * @file l2i2caries.c
 *
 * @brief L2 I2CARIES
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
 #include "l2i2caries.h"
 
/*
** FSM of the I2CARIES
*/
FsmStateItem_t FsmI2caries[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            								fsm_i2caries_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            									fsm_i2caries_init},
  {MSG_ID_COM_RESTART,										FSM_STATE_IDLE,            									fsm_i2caries_restart},

 //Task level initialization
  {MSG_ID_COM_RESTART,        						FSM_STATE_I2CARIES_INITED,         					fsm_i2caries_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_I2CARIES_INITED,         					fsm_i2caries_stop_rcv},

	//Task level actived status
  {MSG_ID_COM_RESTART,        						FSM_STATE_I2CARIES_ACTIVED,         				fsm_i2caries_restart},
  {MSG_ID_COM_STOP,												FSM_STATE_I2CARIES_ACTIVED,         				fsm_i2caries_stop_rcv},
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_I2CARIES_ACTIVED,         				fsm_i2caries_time_out},
	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
  {MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL,				FSM_STATE_I2CARIES_ACTIVED,         				fsm_i2caries_bfsc_moto_cmd_ctrl},	
  {MSG_ID_CAN_I2C_MOTO_CMD_CTRL,					FSM_STATE_I2CARIES_ACTIVED,         				fsm_i2caries_i2c_moto_cmd_ctrl},	
	
#endif		
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             									NULL},  //Ending
};

//Global variables defination
strIhuBfscI2cMotoPar_t zIhuI2cBfscMoto;


//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_i2caries_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_I2CARIES, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("I2CARIES: Error Set FSM State at fsm_i2caries_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_i2caries_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//串行会送INIT_FB给VM，不然消息队列不够深度，此为节省内存机制
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VM。由于消息队列不够深，所以不再回送FB证实。
//		msg_struct_com_init_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
//		snd.length = sizeof(msg_struct_com_init_fb_t);
//		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_I2CARIES, &snd, snd.length);
//		if (ret == IHU_FAILURE){
//			IhuErrorPrint("I2CARIES: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_I2CARIES], zIhuTaskNameList[src_id]);
//			return IHU_FAILURE;
//		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_I2CARIES, FSM_STATE_I2CARIES_INITED) == IHU_FAILURE){
		IhuErrorPrint("I2CARIES: Error Set FSM State!\n");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_i2caries_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("I2CARIES: Error initialize interface!\n");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuRunErrCnt[TASK_ID_I2CARIES] = 0;
	memset(&zIhuI2cBfscMoto, 0, sizeof(strIhuBfscI2cMotoPar_t));

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_I2CARIES, FSM_STATE_I2CARIES_ACTIVED) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_I2CARIES]++;
		IhuErrorPrint("I2CARIES: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//启动本地定时器，如果有必要
	//测试性启动周期性定时器
	ret = ihu_timer_start(TASK_ID_I2CARIES, TIMER_ID_1S_I2CARIES_PERIOD_SCAN, zIhuSysEngPar.timer.i2cariesPeriodScanTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_I2CARIES]++;
		IhuErrorPrint("I2CARIES: Error start timer!\n");
		return IHU_FAILURE;
	}	
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("I2CARIES: Enter FSM_STATE_I2CARIES_ACTIVE status, Keeping refresh here!\n");
	}

	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_i2caries_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("I2CARIES: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_i2caries_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_i2caries_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_I2CARIES)){
		zIhuRunErrCnt[TASK_ID_I2CARIES]++;
		IhuErrorPrint("I2CARIES: Wrong input paramters!\n");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_I2CARIES, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_I2CARIES]++;
		IhuErrorPrint("I2CARIES: Error Set FSM State!\n");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_i2caries_hw_init(void)
{
	ihu_l1hd_i2c_slave_hw_init();
	return IHU_SUCCESS;
}

//TIMER_OUT Processing
OPSTAT fsm_i2caries_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("I2CARIES: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_I2CARIES]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuRunErrCnt[TASK_ID_I2CARIES]是否超限
	if (zIhuRunErrCnt[TASK_ID_I2CARIES] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuRunErrCnt[TASK_ID_I2CARIES] = zIhuRunErrCnt[TASK_ID_I2CARIES] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_I2CARIES, TASK_ID_I2CARIES, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_I2CARIES]++;
			IhuErrorPrint("I2CARIES: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_I2CARIES], zIhuTaskNameList[TASK_ID_I2CARIES]);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_I2CARIES_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_I2CARIES) != FSM_STATE_I2CARIES_ACTIVED){
			ret = FsmSetState(TASK_ID_I2CARIES, FSM_STATE_I2CARIES_ACTIVED);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_I2CARIES]++;
				IhuErrorPrint("I2CARIES: Error Set FSM State!\n");
				return IHU_FAILURE;
			}//FsmSetState
		}
		func_i2caries_time_out_period_scan();
	}

	return IHU_SUCCESS;
}

void func_i2caries_time_out_period_scan(void)
{
	IhuDebugPrint("I2CARIES: Time Out Test!\n");
}

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
//MSG_ID_L3BFSC_I2C_CMD_STOP_MOTO Processing
OPSTAT fsm_i2caries_bfsc_moto_cmd_ctrl(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret = 0;
	msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t))){
		IhuErrorPrint("I2CARIES: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_I2CARIES]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//消息处理
	if (rcv.cmdId == IHU_BFSC_I2C_MOTO_CMD_TYPE_START)
	{
		zIhuI2cBfscMoto.turnMode = IHU_BFSC_I2C_MOTO_TURN_MODE_START;
	}
	else if (rcv.cmdId == IHU_BFSC_I2C_MOTO_CMD_TYPE_STOP)
	{
		zIhuI2cBfscMoto.turnMode = IHU_BFSC_I2C_MOTO_TURN_MODE_START;
	}
	else{
		IhuErrorPrint("I2CARIES: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_I2CARIES]++;
		return IHU_FAILURE;		
	}
	
	//命令下发给MOTO硬件

	return IHU_SUCCESS;
}

//MSG_ID_CAN_I2C_MOTO_CMD_CTRL
OPSTAT fsm_i2caries_i2c_moto_cmd_ctrl(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_canvela_i2caries_moto_cmd_ctrl_t rcv;
	msg_struct_i2caries_canvela_cmd_resp_t snd;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_canvela_i2caries_moto_cmd_ctrl_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_i2caries_moto_cmd_ctrl_t))){
		IhuErrorPrint("I2CARIES: Receive message error!\n");
		zIhuRunErrCnt[TASK_ID_I2CARIES]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//依赖不同的控制命令，分门别类的处理
	//分别针对不同的OPTID进行帧的分类处理
	switch(rcv.cmd.optid)
		{
			case IHU_CANVELA_OPTID_motor_turn_around:  //电机转动
				//具体读取重量的干活指令
				if (rcv.cmd.optpar == IHU_CANVELA_OPTPAR_motor_turn_around_normal)
				{
					zIhuI2cBfscMoto.turnDir = IHU_BFSC_I2C_MOTO_TURN_DIRECTION_NOR;
					zIhuI2cBfscMoto.turnMode = IHU_BFSC_I2C_MOTO_TURN_MODE_START;
				}
				else if (rcv.cmd.optpar == IHU_CANVELA_OPTPAR_motor_turn_around_normal)
				{
					zIhuI2cBfscMoto.turnDir = IHU_BFSC_I2C_MOTO_TURN_DIRECTION_REV;
					zIhuI2cBfscMoto.turnMode = IHU_BFSC_I2C_MOTO_TURN_MODE_START;
				}
				else if (rcv.cmd.optpar == IHU_CANVELA_OPTPAR_motor_turn_around_stop)
				{
					zIhuI2cBfscMoto.turnMode = IHU_BFSC_I2C_MOTO_TURN_MODE_STOP;
				}
				else{
					IhuErrorPrint("I2CARIES: Receive message error!\n");
					zIhuRunErrCnt[TASK_ID_I2CARIES]++;
					return IHU_FAILURE;
				}
				break;		
				
			case IHU_CANVELA_OPTID_motor_speed:  //电机设置速度
				//具体读取重量的干活指令
				zIhuI2cBfscMoto.speed = rcv.cmd.modbusVal;
				break;
			
			default:
				zIhuRunErrCnt[TASK_ID_CANVELA]++;
				IhuErrorPrint("I2CARIES: Input parameters error!\n");
				return IHU_FAILURE;
				//break;
		} //switch(rcv.cmd.optid)
	
	//发送回去消息
	memset(&snd, 0, sizeof(msg_struct_i2caries_canvela_cmd_resp_t));
	snd.length = sizeof(msg_struct_i2caries_canvela_cmd_resp_t);
	ret = ihu_message_send(MSG_ID_I2C_CAN_MOTO_CMD_RESP, TASK_ID_CANVELA, TASK_ID_I2CARIES, &snd, snd.length);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("I2CARIES: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_I2CARIES], zIhuTaskNameList[TASK_ID_CANVELA]);
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

#endif




