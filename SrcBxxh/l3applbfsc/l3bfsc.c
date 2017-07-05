/**
 ****************************************************************************************
 *
 * @file l3bfsc.c
 *
 * @brief L3 BFSC
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
#include "l3bfsc.h"
#include "l3bfsc_msg.h"
#include "main.h"
#include "l2adc_cs5532.h"
#include "blk230.h"
#include "cmsis_os.h"

/*
** FSM of the BFSC
*/
IhuFsmStateItem_t IhuFsmBfsc[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
  {MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            							fsm_bfsc_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            								fsm_bfsc_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_IDLE,            							  fsm_com_do_nothing},

  //Task level initialization
  //{MSG_ID_COM_INIT,       								FSM_STATE_BFSC_INITED,            				fsm_bfsc_init},
  //{MSG_ID_COM_INIT_FB,       							FSM_STATE_BFSC_INITED,            				fsm_com_do_nothing},
  
	//ANY state entry
  {MSG_ID_COM_INIT_FB,                    FSM_STATE_COMMON,                         fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,                 FSM_STATE_COMMON,                         fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,              FSM_STATE_COMMON,                         fsm_com_do_nothing},
	{MSG_ID_COM_STOP,                       FSM_STATE_COMMON,                         fsm_bfsc_stop_rcv},
  {MSG_ID_COM_RESTART,                    FSM_STATE_COMMON,                         fsm_bfsc_restart},

	//Task level actived status：等待初始化指令的到来，然后进行初始化，完成后进入SCAN工作状态
//	{MSG_ID_CAN_L3BFSC_CMD_CTRL,						FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_canvela_cmd_ctrl},  //参数配置设置
//	{MSG_ID_ADC_L3BFSC_MEAS_CMD_RESP,				FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_adc_meas_cmd_resp}, //参数配置反馈
//	{MSG_ID_I2C_L3BFSC_MOTO_CMD_RESP,				FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_i2c_moto_cmd_resp}, //参数配置反馈
//	{MSG_ID_CAN_L3BFSC_INIT_REQ,						FSM_STATE_BFSC_ACTIVED,         					fsm_bfsc_canvela_init_req},  //初始化完成

//	//扫描模式工作状态：等待ADC上报合法的称重结果
//	{MSG_ID_ADC_NEW_MATERIAL_WS,						FSM_STATE_BFSC_SCAN,         				  		fsm_bfsc_adc_new_material_ws}, //新的称重结果
//	{MSG_ID_ADC_MATERIAL_DROP,							FSM_STATE_BFSC_SCAN,         				  		fsm_bfsc_adc_material_drop},   //物料失重被拿走
//	
//	//称重上报工作状态：等待上层指令，收到后进入出料状态
//	{MSG_ID_ADC_NEW_MATERIAL_WS,						FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_adc_new_material_ws}, //新的称重结果，此时也是允许上报的
//	{MSG_ID_CAN_L3BFSC_ROLL_OUT_REQ,				FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_canvela_roll_out_req},//正常出料
//	{MSG_ID_CAN_L3BFSC_GIVE_UP_REQ,					FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_canvela_give_up_req}, //抛弃物料
//	{MSG_ID_ADC_MATERIAL_DROP,							FSM_STATE_BFSC_WEIGHT_REPORT,         		fsm_bfsc_adc_material_drop},   //物料失重被拿走
//	
//	//出料输出等待状态：完成后进入SCAN状态。如果连续N次都未能成功，停止马达和称重传感器，并进入ERROR_TRAP状态。
//	{MSG_ID_ADC_MATERIAL_DROP,							FSM_STATE_BFSC_ROLL_OUT,         				  fsm_bfsc_adc_material_drop},   //出料完成

//	//放弃物料输出等待状态：完成后进入SCAN状态。如果连续N次都未能成功，停止马达和称重传感器，并进入ERROR_TRAP状态。
//	{MSG_ID_ADC_MATERIAL_DROP,							FSM_STATE_BFSC_GIVE_UP,         				  fsm_bfsc_adc_material_drop},   //放弃物料完成

//	//硬件出错，该传感器进入错误陷阱状态，不再工作，等待人工干预
//  {MSG_ID_COM_RESTART,        						FSM_STATE_BFSC_ERROR_TRAP,         				fsm_bfsc_restart},
//  {MSG_ID_COM_STOP,												FSM_STATE_BFSC_ERROR_TRAP,         				fsm_bfsc_stop_rcv},
//	{MSG_ID_COM_TIME_OUT,										FSM_STATE_BFSC_ERROR_TRAP,         				fsm_bfsc_time_out},
	
	//MYC ADDED 20170131
	{MSG_ID_COM_TIME_OUT,                   FSM_STATE_BFSC_INITED,                    fsm_bfsc_wmc_startind_time_out},
  {MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,			FSM_STATE_BFSC_INITED,										fsm_bfsc_wmc_inited_config_req},
  {MSG_ID_L3BFSC_WMC_START_REQ,				    FSM_STATE_BFSC_INITED,							      fsm_bfsc_wmc_start_req},	//MYC
  {MSG_ID_L3BFSC_WMC_COMMAND_REQ,				  FSM_STATE_BFSC_INITED,						      	fsm_bfsc_wmc_command_req},	//MYC
  {MSG_ID_L3BFSC_WMC_STOP_REQ,				    FSM_STATE_BFSC_INITED,						        fsm_bfsc_wmc_stop_req},	//MYC	
  {MSG_ID_L3BFSC_WMC_COMBIN_REQ,				  FSM_STATE_BFSC_INITED,							    	fsm_bfsc_wmc_combin_req},	//MYC	
	{MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_REQ,			FSM_STATE_BFSC_INITED,								    fsm_bfsc_wmc_err_inq_req},	//MYC	
  
  {MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,			FSM_STATE_BFSC_CONFIGURATION,							fsm_bfsc_wmc_set_config_req},	//MYC
	{MSG_ID_L3BFSC_WMC_START_REQ,				    FSM_STATE_BFSC_CONFIGURATION,							fsm_bfsc_wmc_start_req},	//MYC
  {MSG_ID_L3BFSC_WMC_COMMAND_REQ,				  FSM_STATE_BFSC_CONFIGURATION,							fsm_bfsc_wmc_command_req},	//MYC
  {MSG_ID_L3BFSC_WMC_STOP_REQ,				    FSM_STATE_BFSC_CONFIGURATION,						  fsm_bfsc_wmc_stop_req},	//MYC	
  {MSG_ID_L3BFSC_WMC_COMBIN_REQ,				  FSM_STATE_BFSC_CONFIGURATION,							fsm_bfsc_wmc_combin_req},	//MYC	
	{MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_REQ,			FSM_STATE_BFSC_CONFIGURATION,							fsm_bfsc_wmc_err_inq_req},	//MYC	
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_BFSC_CONFIGURATION,							fsm_bfsc_wmc_heart_beat_timeout},
	{MSG_ID_L3BFSC_WMC_HEART_BEAT_CONFIRM,	FSM_STATE_BFSC_CONFIGURATION,							fsm_bfsc_wmc_heart_beat_confirm},
	
  {MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,			FSM_STATE_BFSC_SCAN,							        fsm_bfsc_wmc_set_config_req},	//MYC
	{MSG_ID_L3BFSC_WMC_START_REQ,				    FSM_STATE_BFSC_SCAN,							        fsm_bfsc_wmc_start_req},	//MYC
  {MSG_ID_L3BFSC_WMC_COMMAND_REQ,				  FSM_STATE_BFSC_SCAN,							        fsm_bfsc_wmc_command_req},	//MYC
  {MSG_ID_L3BFSC_WMC_STOP_REQ,				    FSM_STATE_BFSC_SCAN,						          fsm_bfsc_wmc_stop_req},	//MYC	
  {MSG_ID_L3BFSC_WMC_COMBIN_REQ,				  FSM_STATE_BFSC_SCAN,								      fsm_bfsc_wmc_combin_req},	//MYC	
  {MSG_ID_L3BFSC_WMC_WEIGHT_IND,				  FSM_STATE_BFSC_SCAN,								      fsm_bfsc_wmc_weight_ind},	//MYC	
	{MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_REQ,			FSM_STATE_BFSC_SCAN,								    	fsm_bfsc_wmc_err_inq_req},	//MYC
	{MSG_ID_COM_TIME_OUT,										FSM_STATE_BFSC_SCAN,											fsm_bfsc_wmc_heart_beat_timeout},
	{MSG_ID_L3BFSC_WMC_HEART_BEAT_CONFIRM,	FSM_STATE_BFSC_SCAN,											fsm_bfsc_wmc_heart_beat_confirm},
  
  {MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,			FSM_STATE_BFSC_COMBINATION,			  				fsm_bfsc_wmc_set_config_req},	//MYC
	{MSG_ID_L3BFSC_WMC_START_REQ,				    FSM_STATE_BFSC_COMBINATION,					  		fsm_bfsc_wmc_start_req},	//MYC
  {MSG_ID_L3BFSC_WMC_COMMAND_REQ,				  FSM_STATE_BFSC_COMBINATION,								fsm_bfsc_wmc_command_req},	//MYC	
  {MSG_ID_L3BFSC_WMC_STOP_REQ,				    FSM_STATE_BFSC_COMBINATION,								fsm_bfsc_wmc_stop_req},	//MYC	
  {MSG_ID_L3BFSC_WMC_COMBIN_REQ,				  FSM_STATE_BFSC_COMBINATION,								fsm_bfsc_wmc_combin_req},	//MYC	
  {MSG_ID_L3BFSC_WMC_WEIGHT_IND,				  FSM_STATE_BFSC_COMBINATION,								fsm_bfsc_wmc_weight_ind},	//MYC	
	{MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_REQ,			FSM_STATE_BFSC_COMBINATION,								fsm_bfsc_wmc_err_inq_req},	//MYC	
  {MSG_ID_COM_TIME_OUT,                   FSM_STATE_BFSC_COMBINATION,               fsm_bfsc_wmc_heart_beat_timeout},
	{MSG_ID_L3BFSC_WMC_HEART_BEAT_CONFIRM,	FSM_STATE_BFSC_COMBINATION,								fsm_bfsc_wmc_heart_beat_confirm},
  
	{MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,			FSM_STATE_BFSC_ACTIVED,										fsm_bfsc_wmc_set_config_req},	//MYC
	{MSG_ID_L3BFSC_WMC_START_REQ,						FSM_STATE_BFSC_ACTIVED,										fsm_bfsc_wmc_start_req},	//MYC
	{MSG_ID_L3BFSC_WMC_COMMAND_REQ,					FSM_STATE_BFSC_ACTIVED,										fsm_bfsc_wmc_command_req},	//MYC
	{MSG_ID_L3BFSC_WMC_STOP_REQ,						FSM_STATE_BFSC_ACTIVED,										fsm_bfsc_wmc_stop_req},	//MYC
	{MSG_ID_L3BFSC_WMC_COMBIN_REQ,					FSM_STATE_BFSC_ACTIVED,										fsm_bfsc_wmc_combin_req},	//MYC
	{MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_REQ,			FSM_STATE_BFSC_ACTIVED,								    fsm_bfsc_wmc_err_inq_req},	//MYC	
  {MSG_ID_COM_TIME_OUT,                   FSM_STATE_BFSC_ACTIVED,               		fsm_bfsc_wmc_heart_beat_timeout},
	{MSG_ID_L3BFSC_WMC_HEART_BEAT_CONFIRM,	FSM_STATE_BFSC_ACTIVED,										fsm_bfsc_wmc_heart_beat_confirm},
	
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             								NULL},  //Ending
};

//Global variables defination
INT32 zIhuL3bfscLatestMeasureWeightValue = 0;
UINT32 zIhuL3bfscMotoRecoverTimes = 0;

//MYC Global Varible for WMC
WmcInventory_t										zWmcInvenory;
CombinationAlgorithmParamaters_t 	zCombAlgoParam;
WeightSensorParamaters_t					zWeightSensorParam;
MotorControlParamaters_t 					zMotorControlParam;
UINT16														zAwsCanIdPrefix;
UINT16														zWmcCanIdPrefix;
BfscWmcState_t										zBfscWmcState;
extern strIhuBfscAdcWeightPar_t 	zIhuAdcBfscWs;


//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_bfsc_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("L3BFSC: Error Set FSM State at fsm_bfsc_task_entry.\n");
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_bfsc_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//串行回送INIT_FB给VMFO
	ihu_usleep(dest_id * IHU_SYSCFG_MODULE_START_DISTRIBUTION_DELAY_DURATION);
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VMFO
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_BFSC, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[src_id].taskName);
			return IHU_FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_INITED) == IHU_FAILURE){
		IhuErrorPrint("L3BFSC: Error Set FSM State!");	
		return IHU_FAILURE;
	}
	
	//MYC Initialize globale variables:
	memset(&zWmcInvenory, 0, sizeof(WmcInventory_t));
	IhuDebugPrint("L3BFSC: fsm_bfsc_init: zWmcInvenory set 0, size = %d bytes\n", sizeof(WmcInventory_t));	
	
	memset(&zCombAlgoParam, 0, sizeof(CombinationAlgorithmParamaters_t));
	IhuDebugPrint("L3BFSC: fsm_bfsc_init: zCombAlgoParam set 0, size = %d bytes\n", sizeof(CombinationAlgorithmParamaters_t));	
	
	memset(&zBfscWmcState, 0, sizeof(BfscWmcState_t));
	IhuDebugPrint("L3BFSC: fsm_bfsc_init: zBfscWmcState set 0, size = %d bytes\n", sizeof(BfscWmcState_t));	
	
	/*  SHOULD BE SET TO BFSC_MWC_STATE_WAIT_FOR_COMBIN_OUT, every time receved START_REQ */
	zBfscWmcState.state = BFSC_MWC_STATE_WAIT_FOR_COMBIN_OUT;
	
	//初始化硬件接口
	if (func_bfsc_hw_init(&zWmcInvenory) == IHU_FAILURE){	
		IhuErrorPrint("L3BFSC: Error initialize interface!");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC] = 0;
	zIhuL3bfscLatestMeasureWeightValue = 0;
	zIhuL3bfscMotoRecoverTimes = 0;
  // start a timer to send startup ind 
	ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_10MS_BFSC_STARTUP_IND, \
		zIhuSysEngPar.timer.array[TIMER_ID_10MS_BFSC_STARTUP_IND].dur, TIMER_TYPE_PERIOD, 
		zIhuSysEngPar.timer.array[TIMER_ID_10MS_BFSC_STARTUP_IND].gradunarity);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Error start TIMER_ID_1S_BFSC_STARTUP_IND\n");
		return IHU_FAILURE;
	}
	
	ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_HEART_BEAT, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_BFSC_HEART_BEAT].dur, TIMER_TYPE_PERIOD, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_BFSC_HEART_BEAT].gradunarity);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Error start TIMER_ID_1S_BFSC_HEART_BEAT!\n");
		return IHU_FAILURE;
	}
	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_bfsc_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("L3BFSC: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_bfsc_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_bfsc_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_BFSC)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Wrong input paramters!");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Error Set FSM State!");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
//MYC
UINT32 GetWmcId()
{
	/* PA5,PA6,PA7,PA8 => SW0,1,2,3 */
	GPIO_PinState ps;
	UINT32 wmc_id = 0;
	
	/* SW0 <= PE10 */
	ps = HAL_GPIO_ReadPin(GPIOE, CUBEMX_PIN_ADD_SW0_NODEID_BIT0_Pin);
	if (GPIO_PIN_RESET == ps)
	{
		wmc_id = wmc_id & 0xFFFFFFFE;  //1110
	}
	else
	{
		wmc_id = wmc_id | 0x00000001;  //0001
	}
	
	/* SW1 <= PE11 */
	ps = HAL_GPIO_ReadPin(GPIOE, CUBEMX_PIN_ADD_SW1_NODEID_BIT1_Pin);
	if (GPIO_PIN_RESET == ps)
	{
		wmc_id = wmc_id & 0xFFFFFFFD;  //1101
	}
	else
	{
		wmc_id = wmc_id | 0x00000002;  //0010
	}
	
	/* SW2 <= PE12 */
	ps = HAL_GPIO_ReadPin(GPIOE, CUBEMX_PIN_ADD_SW2_NODEID_BIT2_Pin);
	if (GPIO_PIN_RESET == ps)
	{
		wmc_id = wmc_id & 0xFFFFFFFB;  //1011
	}
	else
	{
		wmc_id = wmc_id | 0x00000004;  //0100
	}
	
	/* SW3 <= PE13 */
	ps = HAL_GPIO_ReadPin(GPIOE, CUBEMX_PIN_ADD_SW3_NODEID_BIT3_Pin);
	if (GPIO_PIN_RESET == ps)
	{
		wmc_id = wmc_id & 0xFFFFFFF7;  //0111
	}
	else
	{
		wmc_id = wmc_id | 0x00000008;  //1000
	}
	
	/* SW4 <= PE14 */
	ps = HAL_GPIO_ReadPin(GPIOE, CUBEMX_PIN_ADD_SW4_NODEID_BIT4_Pin);
	if (GPIO_PIN_RESET == ps)
	{
		wmc_id = wmc_id & 0xFFFFFFEF;  //11101111
	}
	else
	{
		wmc_id = wmc_id | 0x00000010;  //00010000
	}
	
	/* SW5 <= PE15 */
	ps = HAL_GPIO_ReadPin(GPIOE, CUBEMX_PIN_ADD_SW5_RESERVED_Pin);
	if (GPIO_PIN_RESET == ps)
	{
		wmc_id = wmc_id & 0xFFFFFFDF;  //11011111
	}
	else
	{
		wmc_id = wmc_id | 0x00000020;  //00100000
	}
	
	/* make sure only the last 6 digit values */
	wmc_id = wmc_id & 0x2F;
	return wmc_id;
}
//MYC
OPSTAT func_bfsc_hw_init(WmcInventory_t *pwi)
{
	if(NULL == pwi)
	{
		IhuErrorPrint("L3BFSC: func_bfsc_hw_init: input parameter pwi == BULL");
		return IHU_FAILURE;
	}
	
	//MYC TODO
	pwi->hw_inventory_id = 0x0;
	pwi->sw_inventory_id = 0x1;
	
	//MYC Get CPU ID for STM32
	ihu_bsp_stm32_get_cpuid_f2board(&pwi->stm32_cpu_id);
	IhuDebugPrint("L3BFSC: func_bfsc_hw_init: stm32_cpu_id = [0x%08X]\n", pwi->stm32_cpu_id);
	
	//MYC TODO
	pwi->weight_sensor_type = 0x2;
	pwi->motor_type = 0x03;
	pwi->wmc_id.wmc_id = GetWmcId();               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	IhuDebugPrint("L3BFSC: func_bfsc_hw_init: wmc_id = [0x%02X]\n", pwi->wmc_id);

	extern CAN_HandleTypeDef hcan1;
	bsp_can_config_filter(&hcan1, WMC_CAN_ID);
	
	/* Load Default Parameters */
	WeightSensorInit(&zWeightSensorParam);
	IhuDebugPrint("L3BFSC: func_bfsc_hw_init: WeightSensorInit()\r\n");
	
	WeightSensorDefaultCalibrationValue(&zWeightSensorParam);
	IhuDebugPrint("L3BFSC: func_bfsc_hw_init: WeightSensorDefaultCalibrationValue()\r\n");
	
	srand(zWmcInvenory.wmc_id.wmc_id); /* For generating ramdon value for test */
	
	return IHU_SUCCESS;
}


//TIMER_OUT Processing
OPSTAT fsm_bfsc_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//IhuErrorPrint("L3BFSC: fsm_bfsc_time_out\n");
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("L3BFSC: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC]是否超限
	if (zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC] = zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_BFSC, TASK_ID_BFSC, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC]++;
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);
			return IHU_FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_BFSC_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_bfsc_time_out_period_scan();
	}

	else if ((rcv.timeId == TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_bfsc_time_out_wait_weight_command_process();
	}
	
	else if ((rcv.timeId == TIMER_ID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_bfsc_time_out_roll_out_process();
	}
	
	else if ((rcv.timeId == TIMER_ID_1S_BFSC_L3BFSC_GIVE_UP_TIMER) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_bfsc_time_out_give_up_process();
	}
			
	else{
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("L3BFSC: Receive error timer ID!\n");
		return IHU_FAILURE;
	}
	
	return IHU_SUCCESS;
}

//暂时无用，而只是为了回归状态的定时器
OPSTAT func_bfsc_time_out_period_scan(void)
{
	int ret = 0;
	uint32_t tempVal = 0;
	
	//发送HeartBeat消息给VMFO模块，实现喂狗功能
	msg_struct_com_heart_beat_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_heart_beat_t));
	snd.length = sizeof(msg_struct_com_heart_beat_t);
	ret = ihu_message_send(MSG_ID_COM_HEART_BEAT, TASK_ID_VMFO, TASK_ID_BFSC, &snd, snd.length);
	
	//读取假数据
	//需要直接访问ADC的HAL函数
	//tempVal = rand() % 1000;  // MYC comment 2017/04/30 
	tempVal = ihu_l1hd_adc_bfsc_get_value(); // MYC added 2017/04/30
	IhuDebugPrint("ADCLIBRA: func_bfsc_time_out_period_scan: tempVal = %d\n", tempVal);
	
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC]++;
		IhuErrorPrint("BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_VMFO].taskName);
		return IHU_FAILURE;
	}

	return IHU_SUCCESS;	
}

OPSTAT func_bfsc_time_out_wait_weight_command_process(void)
{
	int ret = 0;
	msg_struct_l3bfsc_canvela_new_ws_event_t snd;
	
	//重发数据
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t));
	snd.wsValue = zIhuL3bfscLatestMeasureWeightValue;
	snd.length = sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t);
	if (ihu_message_send(MSG_ID_L3BFSC_CAN_NEW_WS_EVENT, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);
	
	//启动定时器
	ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error start timer!\n");
	
	return IHU_SUCCESS;	
}

//[组合]出料TIME OUT处理
OPSTAT func_bfsc_time_out_roll_out_process(void)
{
	int ret = 0;
	msg_struct_l3bfsc_canvela_error_status_report_t snd;
	msg_struct_l3bfsc_adc_ws_cmd_ctrl_t snd1;
	msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t snd2;	
	
	//发送错误报告给上位机
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_error_status_report_t));
	snd.length = sizeof(msg_struct_l3bfsc_canvela_error_status_report_t);
//	if (ihu_message_send(MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);

	//出错次数统计
	//zIhuL3bfscMotoRecoverTimes++;
	zIhuL3bfscMotoRecoverTimes = zIhuL3bfscMotoRecoverTimes + 1;
	
	if (zIhuL3bfscMotoRecoverTimes < IHU_L3BFSC_MOTO_HW_ERROR_RECOVER_TIMES_MAX)
	{
		//继续启动定时器，并打开MOTO命令
		
		//重启定时器
		ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER, \
			zIhuSysEngPar.timer.array[TIMER_ID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error start timer!\n");
	}
	else
	{
		//状态强行转移到ERROR_TRAP模式，等待人工恢复
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_ERROR_TRAP) == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error Set FSM State!");	
		//发送命令给ADC，停止测量工作
		memset(&snd1, 0, sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t));
		snd1.cmdid = IHU_SYSMSG_BFSC_ADC_WS_CMD_TYPE_STOP;
		snd1.length = sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t);
//		if (ihu_message_send(MSG_ID_L3BFSC_ADC_WS_CMD_CTRL, TASK_ID_ADCLIBRA, TASK_ID_BFSC, &snd1, snd1.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_ADCLIBRA].taskName);
		//发送命令给I2C-MOTO，停止测量工作
		memset(&snd2, 0, sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t));
		snd2.cmdid = IHU_SYSMSG_BFSC_I2C_MOTO_CMD_TYPE_STOP;
		snd2.length = sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t);
//		if (ihu_message_send(MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL, TASK_ID_I2CARIES, TASK_ID_BFSC, &snd2, snd2.length) == IHU_FAILURE)
//			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_I2CARIES].taskName);
	}
	
	//返回
	return IHU_SUCCESS;	
}

//[放弃]出料TIME OUT处理
OPSTAT func_bfsc_time_out_give_up_process(void)
{
	int ret = 0;
	msg_struct_l3bfsc_canvela_error_status_report_t snd;
	msg_struct_l3bfsc_adc_ws_cmd_ctrl_t snd1;
	msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t snd2;		
	
	//发送错误报告给上位机
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_error_status_report_t));
	snd.length = sizeof(msg_struct_l3bfsc_canvela_error_status_report_t);
//	if (ihu_message_send(MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);
	
	//出错次数统计
	//zIhuL3bfscMotoRecoverTimes++;
	zIhuL3bfscMotoRecoverTimes = zIhuL3bfscMotoRecoverTimes + 1;
	
	if (zIhuL3bfscMotoRecoverTimes < IHU_L3BFSC_MOTO_HW_ERROR_RECOVER_TIMES_MAX)
	{
		//继续启动定时器，并打开MOTO命令
		
		//重启定时器
		ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_GIVE_UP_TIMER, \
			zIhuSysEngPar.timer.array[TIMER_ID_1S_BFSC_L3BFSC_GIVE_UP_TIMER].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error start timer!\n");
	}
	else
	{
		//状态强行转移到ERROR_TRAP模式，等待人工恢复
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_ERROR_TRAP) == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error Set FSM State!");	

		//发送命令给ADC，停止测量工作
		memset(&snd1, 0, sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t));
		snd1.length = sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t);
		snd1.cmdid = IHU_SYSMSG_BFSC_ADC_WS_CMD_TYPE_STOP;
//		if (ihu_message_send(MSG_ID_L3BFSC_ADC_WS_CMD_CTRL, TASK_ID_ADCLIBRA, TASK_ID_BFSC, &snd1, snd1.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_ADCLIBRA].taskName);

		//发送命令给I2C-MOTO，停止测量工作
		memset(&snd2, 0, sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t));
		snd2.cmdid = IHU_SYSMSG_BFSC_I2C_MOTO_CMD_TYPE_STOP;
		snd2.length = sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t);
//		if (ihu_message_send(MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL, TASK_ID_I2CARIES, TASK_ID_BFSC, &snd2, snd2.length) == IHU_FAILURE)
//			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_I2CARIES].taskName);
	}
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_CAN_L3BFSC_CMD_CTRL以后的处理过程
OPSTAT fsm_bfsc_canvela_cmd_ctrl(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret;
	msg_struct_canvela_l3bfsc_cmd_ctrl_t rcv;
	msg_struct_l3bfsc_canvela_cmd_resp_t snd;
	msg_struct_l3bfsc_adc_ws_cmd_ctrl_t snd1;
	msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t snd2;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_canvela_l3bfsc_cmd_ctrl_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_l3bfsc_cmd_ctrl_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查
	if ((rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_ws_ctrl) && (rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_motor_ctrl))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");


	//如果是单独的Node Control
	if (rcv.cmd.prefixcmdid == IHU_CANVELA_PREFIXH_node_ctrl)
	{
		//先干活，干什么待定
		
		//然后反馈给CAN模块
		memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t));
		snd.cmdid = IHU_SYSMSG_BFSC_ADC_WS_CMD_TYPE_CTRL;
		snd.cmd.optid = rcv.cmd.optid;
		snd.cmd.optpar = rcv.cmd.optpar;
		snd.cmd.modbusVal = rcv.cmd.modbusVal;
		snd.length = sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t);
//		if (ihu_message_send(MSG_ID_L3BFSC_CAN_CMD_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length) == IHU_FAILURE)
//			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);
	}
	
	//发送命令给ADC
	if (rcv.cmd.prefixcmdid == IHU_CANVELA_PREFIXH_ws_ctrl)
	{		
		memset(&snd1, 0, sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t));
		snd1.cmdid = IHU_SYSMSG_BFSC_ADC_WS_CMD_TYPE_CTRL;
		snd1.cmd.optid = rcv.cmd.optid;
		snd1.cmd.optpar = rcv.cmd.optpar;
		snd1.cmd.modbusVal = rcv.cmd.modbusVal;
		snd1.length = sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t);
//		if (ihu_message_send(MSG_ID_L3BFSC_ADC_WS_CMD_CTRL, TASK_ID_ADCLIBRA, TASK_ID_BFSC, &snd1, snd1.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_ADCLIBRA].taskName);
	}
	
	//发送命令给I2C-MOTO
	if (rcv.cmd.prefixcmdid == IHU_CANVELA_PREFIXH_motor_ctrl)
	{		
		memset(&snd2, 0, sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t));		
		snd2.cmdid = IHU_SYSMSG_BFSC_I2C_MOTO_CMD_TYPE_CTRL;
		snd2.cmd.optid = rcv.cmd.optid;
		snd2.cmd.optpar = rcv.cmd.optpar;
		snd2.cmd.modbusVal = rcv.cmd.modbusVal;
		snd2.length = sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t);
//		if (ihu_message_send(MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL, TASK_ID_I2CARIES, TASK_ID_BFSC, &snd2, snd2.length) == IHU_FAILURE)
//			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_I2CARIES].taskName);
	}
	
	//返回
	return IHU_SUCCESS;
}

//MSG_ID_ADC_L3BFSC_MEAS_CMD_RESP
OPSTAT fsm_bfsc_adc_meas_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret;
	msg_struct_adclibra_l3bfsc_meas_cmd_resp_t rcv;
	msg_struct_l3bfsc_canvela_cmd_resp_t snd;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_adclibra_l3bfsc_meas_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_adclibra_l3bfsc_meas_cmd_resp_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查：暂时不处理IHU_SYSMSG_ADC_BFSC_WS_CMD_TYPE_RESP之外其它的类型，未来待完善
	if ((rcv.cmdid != IHU_SYSMSG_ADC_BFSC_WS_CMD_TYPE_RESP) || (rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_ws_resp))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	
	//发送消息给CANVELA
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t));
	snd.cmdid = IHU_SYSMSG_BFSC_CAN_CMD_TYPE_RESP;
	snd.cmd.prefixcmdid = IHU_CANVELA_PREFIXH_ws_resp;
	snd.cmd.optid = rcv.cmd.optid;
	snd.cmd.optpar = rcv.cmd.optpar;
	snd.cmd.modbusVal = rcv.cmd.modbusVal;
	snd.length = sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t);
//	if (ihu_message_send(MSG_ID_L3BFSC_CAN_CMD_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length) == IHU_FAILURE)
//		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);
	
	//返回
	return IHU_SUCCESS;
}

//MSG_ID_I2C_L3BFSC_MOTO_CMD_RESP
OPSTAT fsm_bfsc_i2c_moto_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret;
	msg_struct_i2caries_l3bfsc_cmd_resp_t rcv;
	msg_struct_l3bfsc_canvela_cmd_resp_t snd;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_i2caries_l3bfsc_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_i2caries_l3bfsc_cmd_resp_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	//入参检查：暂时不处理IHU_SYSMSG_I2C_BFSC_WS_CMD_TYPE_RESP之外其它的类型，未来待完善
	if ((rcv.cmdid != IHU_SYSMSG_I2C_BFSC_WS_CMD_TYPE_RESP) || (rcv.cmd.prefixcmdid != IHU_CANVELA_PREFIXH_motor_resp))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	
	//发送消息给CANVELA
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t));
	snd.cmdid = IHU_SYSMSG_BFSC_CAN_CMD_TYPE_RESP;
	snd.cmd.prefixcmdid = IHU_CANVELA_PREFIXH_motor_resp;
	snd.cmd.optid = rcv.cmd.optid;
	snd.cmd.optpar = rcv.cmd.optpar;
	snd.cmd.modbusVal = rcv.cmd.modbusVal;
	snd.length = sizeof(msg_struct_l3bfsc_canvela_cmd_resp_t);
//	if (ihu_message_send(MSG_ID_L3BFSC_CAN_CMD_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length) == IHU_FAILURE)
//		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_CAN_L3BFSC_INIT_REQ以后的处理过程
OPSTAT fsm_bfsc_canvela_init_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//int ret;
	msg_struct_canvela_l3bfsc_init_req_t rcv;
	msg_struct_l3bfsc_canvela_init_resp_t snd;
	msg_struct_l3bfsc_adc_ws_cmd_ctrl_t snd1;
	msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t snd2;			
	
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_canvela_l3bfsc_init_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_l3bfsc_init_req_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//处理消息：命令并指令WS、MOTO开始干活
	//发送命令给ADC，开始测量工作
	memset(&snd1, 0, sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t));
	snd1.length = sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t);
	snd1.cmdid = IHU_SYSMSG_BFSC_ADC_WS_CMD_TYPE_START;
//	if (ihu_message_send(MSG_ID_L3BFSC_ADC_WS_CMD_CTRL, TASK_ID_ADCLIBRA, TASK_ID_BFSC, &snd1, snd1.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_ADCLIBRA].taskName);

	//发送命令给I2C-MOTO，开始测量工作
	memset(&snd2, 0, sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t));
	snd2.cmdid = IHU_SYSMSG_BFSC_I2C_MOTO_CMD_TYPE_START;
	snd2.length = sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t);
//	if (ihu_message_send(MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL, TASK_ID_I2CARIES, TASK_ID_BFSC, &snd2, snd2.length) == IHU_FAILURE)
//		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_I2CARIES].taskName);
	
	//发送反馈消息出去
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_init_resp_t));
	snd.length = sizeof(msg_struct_l3bfsc_canvela_init_resp_t);
	if (ihu_message_send(MSG_ID_L3BFSC_CAN_INIT_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);

	//状态转移
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN) == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error Set FSM State!");	
	
	//MYC add the first message to AWS
//	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
//		//Send back MSG_ID_COM_INIT_FB to VMFO
//		msg_struct_l3bfsc_wmc_startup_ind_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfsc_wmc_startup_ind_t));
//		snd.length = sizeof(msg_struct_l3bfsc_wmc_startup_ind_t);
//		snd.msgid = MSG_ID_L3BFSC_WMC_STARTUP_IND;
//		snd.wmc_state = FSM_STATE_BFSC_SCAN;
//		memcpy(&snd.wmc_inventory, &zWmcInvenory, sizeof(WmcInventory_t));
//		
//		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_STARTUP_IND, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
//		if (ret == IHU_FAILURE){
//			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_CANVELA]);
//			return IHU_FAILURE;
//		}
//	}
	
	//返回
	return IHU_SUCCESS;
}


//收到MSG_ID_ADC_NEW_MATERIAL_WS以后的处理过程
OPSTAT fsm_bfsc_adc_new_material_ws(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_adc_new_material_ws_t rcv;
	msg_struct_l3bfsc_canvela_new_ws_event_t snd;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_adc_new_material_ws_t));
	if (param_ptr == NULL || param_len > sizeof(msg_struct_adc_new_material_ws_t))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//处理消息，存储最后收到的新测量值，以便定时超时后再复送
	zIhuL3bfscLatestMeasureWeightValue = rcv.wsValue;
	
	//停止定时器：没有定时器可以停止
	
	//发送消息出去
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t));
	snd.wsValue = rcv.wsValue;
	snd.length = sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t);
	if (ihu_message_send(MSG_ID_L3BFSC_CAN_NEW_WS_EVENT, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length) == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);
	
	//启动定时器
	ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error start timer!\n");
	}	
	
	//状态转移
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_WEIGHT_REPORT) == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error Set FSM State!");	
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_ADC_MATERIAL_DROP以后的处理过程
OPSTAT fsm_bfsc_adc_material_drop(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_adc_material_drop_t rcv;
	msg_struct_l3bfsc_canvela_new_ws_event_t snd;
	msg_struct_l3bfsc_canvela_roll_out_resp_t snd1;
	msg_struct_l3bfsc_canvela_give_up_resp_t snd2;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_adc_material_drop_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_adc_material_drop_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//处理消息
	//在称重条件下，直接返回SCAN状态，没有任何处理过程
	if (FsmGetState(TASK_ID_BFSC) == FSM_STATE_BFSC_WEIGHT_REPORT){
		//发送状态命令给上位机，表示这个空闲了	
		memset(&snd, 0, sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t));
		snd.length = sizeof(msg_struct_l3bfsc_canvela_new_ws_event_t);
		if (ihu_message_send(MSG_ID_L3BFSC_CAN_NEW_WS_EVENT, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);
	
		//状态转移
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN) == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error Set FSM State!");	
	}
	
	//在[组合]出料条件下
	else if (FsmGetState(TASK_ID_BFSC) == FSM_STATE_BFSC_ROLL_OUT){
		//停止定时器
		ret = ihu_timer_stop(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error stop timer!\n");
		
		//发送消息出去
		memset(&snd1, 0, sizeof(msg_struct_l3bfsc_canvela_roll_out_resp_t));
		snd1.length = sizeof(msg_struct_l3bfsc_canvela_roll_out_resp_t);
		if (ihu_message_send(MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd1, snd1.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);
		
		//状态转移
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN) == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error Set FSM State!");	
	}

	//在[放弃]出料条件下
	else if (FsmGetState(TASK_ID_BFSC) == FSM_STATE_BFSC_GIVE_UP){
		//停止定时器
		ret = ihu_timer_stop(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_GIVE_UP_TIMER, TIMER_RESOLUTION_1S);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error stop timer!\n");
		
		//发送消息出去
		memset(&snd2, 0, sizeof(msg_struct_l3bfsc_canvela_give_up_resp_t));
		snd2.length = sizeof(msg_struct_l3bfsc_canvela_give_up_resp_t);
		if (ihu_message_send(MSG_ID_L3BFSC_CAN_GIVE_UP_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, &snd2, snd2.length) == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);
		
		//状态转移
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN) == IHU_FAILURE)
			IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error Set FSM State!");	
	}
	
	else{
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive command at wrong status!");	
	}
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_CAN_L3BFSC_ROLL_OUT_REQ以后的处理过程
OPSTAT fsm_bfsc_canvela_roll_out_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret = 0;
	msg_struct_canvela_l3bfsc_roll_out_req_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_canvela_l3bfsc_roll_out_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_l3bfsc_roll_out_req_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//停止定时器
	ret = ihu_timer_stop(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error stop timer!\n");
	
	//发送马达运转的指令
	//等待出料完成
	//如果差错，需要送出差错消息给上位机
	//处理消息：发送质量给马达，进行出料处理
	
	//启动定时器
	ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error start timer!\n");
	
	//状态转移
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_ROLL_OUT) == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error Set FSM State!");	
	
	//返回
	return IHU_SUCCESS;
}

//收到MSG_ID_CAN_L3BFSC_GIVE_UP_REQ以后的处理过程
OPSTAT fsm_bfsc_canvela_give_up_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_canvela_l3bfsc_give_up_req_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_canvela_l3bfsc_give_up_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canvela_l3bfsc_give_up_req_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//停止定时器
	ret = ihu_timer_stop(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error stop timer!\n");
	
	//发送马达运转的指令
	//等待出料完成
	//如果差错，需要送出差错消息给上位机
	//处理消息：发送质量给马达，进行出料处理
	
	//启动定时器
	ret = ihu_timer_start(TASK_ID_BFSC, TIMER_ID_1S_BFSC_L3BFSC_GIVE_UP_TIMER, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_BFSC_L3BFSC_GIVE_UP_TIMER].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error start timer!\n");
	
	//状态转移
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_GIVE_UP) == IHU_FAILURE)
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Error Set FSM State!");	

	//返回
	return IHU_SUCCESS;
}

//由于错误，直接从差错中转入扫描状态，确保业务一直处理工作状态
void func_bfsc_stm_main_recovery_from_fault(void)
{	
	msg_struct_l3bfsc_adc_ws_cmd_ctrl_t snd1;
	msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t snd2;

	//将传感器情形归化到工作状态	
	//发送命令给ADC，开始测量工作
	memset(&snd1, 0, sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t));
	snd1.length = sizeof(msg_struct_l3bfsc_adc_ws_cmd_ctrl_t);
	snd1.cmdid = IHU_SYSMSG_BFSC_ADC_WS_CMD_TYPE_START;
//	ihu_message_send(MSG_ID_L3BFSC_ADC_WS_CMD_CTRL, TASK_ID_ADCLIBRA, TASK_ID_BFSC, &snd1, snd1.length);
	//发送命令给I2C-MOTO，开始测量工作
	memset(&snd2, 0, sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t));
	snd2.cmdid = IHU_SYSMSG_BFSC_I2C_MOTO_CMD_TYPE_START;
	snd2.length = sizeof(msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t);
//	ihu_message_send(MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL, TASK_ID_I2CARIES, TASK_ID_BFSC, &snd2, snd2.length);
	
	//状态转移到SCAN状态
	FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN);
	
	//初始化模块的任务资源
	//初始化定时器：暂时决定不做，除非该模块重新RESTART
	//初始化模块级全局变量：暂时决定不做，除非该模块重新RESTART
	
	return;
}

static long number_of_wmc_startind = 0;
OPSTAT fsm_bfsc_wmc_startind_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
  OPSTAT ret;
	msg_struct_com_time_out_t rcv;

  if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("L3BFSC: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	if(rcv.timeId == TIMER_ID_10MS_BFSC_STARTUP_IND)
	{
		msg_struct_l3bfsc_wmc_startup_ind_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfsc_wmc_startup_ind_t));
		snd.length = sizeof(msg_struct_l3bfsc_wmc_startup_ind_t);
		snd.msgid = (MSG_ID_L3BFSC_WMC_STARTUP_IND);
		memcpy(&snd.wmc_inventory, &zWmcInvenory, sizeof(WmcInventory_t));

		number_of_wmc_startind++;
    IhuDebugPrint("send WMC_START_IND (%d bytes, %d times): 0x%08x\n", snd.length, number_of_wmc_startind, *(UINT32 *)&snd);
		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_STARTUP_IND, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
 		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_CANVELA]);
			return IHU_FAILURE;
		}
	}

	
	return IHU_SUCCESS;
}

OPSTAT fsm_bfsc_wmc_inited_config_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	OPSTAT ret;

	// stop the timer TIMER_ID_1S_BFSC_STARTUP_IND
	ihu_timer_stop(TASK_ID_BFSC, TIMER_ID_10MS_BFSC_STARTUP_IND, zIhuSysEngPar.timer.array[TIMER_ID_10MS_BFSC_STARTUP_IND].gradunarity);
	
	// forward the config message to next state
	ret = ihu_message_send(MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ, TASK_ID_BFSC, TASK_ID_BFSC, param_ptr, param_len);
	if (ret == IHU_FAILURE){
		IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_BFSC]);
		return IHU_FAILURE;
	}
	
	// change state from inited to configuration
	if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_CONFIGURATION) == IHU_FAILURE){
		IhuErrorPrint("L3BFSC: Error Set FSM State!");	
		return IHU_FAILURE;
	}

	return IHU_SUCCESS;
}

/*
**
*/
static long number_of_wmc_set_config_req = 0;
OPSTAT fsm_bfsc_wmc_set_config_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)	//MYC
{
	OPSTAT ret = IHU_SUCCESS;
	error_code_t error_code;
	msg_struct_l3bfsc_wmc_set_config_req_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_wmc_set_config_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_wmc_set_config_req_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	number_of_wmc_set_config_req++;	
	IhuDebugPrint("L3BFSC: msg_struct_l3bfsc_wmc_set_config_req_t (%d times): rcv.msgid = 0x%08X, rcv.length = %d\r\n", 
									number_of_wmc_set_config_req, rcv.msgid, rcv.length);
	
	/* PROCESS TO BE ADDED */
	/* !!!!!!!!!!!!!!!!!!!!*/
	/* Process Message */
	msg_wmc_set_config_req_process(param_ptr, &error_code);
	
	/* STATE CHANGE IF OK */
	if (ERROR_CODE_NO_ERROR == error_code)
	{
			FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_CONFIGURATION);
			IhuDebugPrint("L3BFSC: fsm_bfsc_wmc_set_config_req: Set to FSM_STATE_BFSC_CONFIGURATION\r\n");
	}
	
	/* Send back the response */
	msg_wmc_set_config_resp(error_code);
	
	//返回
	return ret;
}

//OPSTAT fsm_bfsc_wmc_get_config_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)	//MYC
//{
//	OPSTAT ret = IHU_SUCCESS;
//	error_code_t error_code;
//	msg_struct_l3bfsc_wmc_get_config_req_t rcv;
//	
//	//收到消息并做参数检查
//	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_wmc_get_config_req_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_wmc_get_config_req_t)))
//		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
//	memcpy(&rcv, param_ptr, param_len);
//	
//	IhuDebugPrint("L3BFSC: msg_struct_l3bfsc_wmc_get_config_req_t: rcv.msgid = 0x%08X, rcv.length = %d\r\n", rcv.msgid, rcv.length);

//	/* PROCESS TO BE ADDED */
//	/* !!!!!!!!!!!!!!!!!!!!*/
//	/* Process Message */
//	msg_wmc_get_config_req_process(param_ptr, &error_code);
//	
//	/* Send back the response */
//	msg_wmc_get_config_resp(error_code);
//	
//	//FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_CONFIGURATION);

//	//返回
//	return ret;
//}

OPSTAT fsm_bfsc_wmc_start_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)	//MYC
{
	OPSTAT ret = IHU_SUCCESS;
	error_code_t error_code;
	msg_struct_l3bfsc_wmc_start_req_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_wmc_start_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_wmc_start_req_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	IhuDebugPrint("L3BFSC: msg_struct_l3bfsc_wmc_start_req_t: rcv.msgid = 0x%08X, rcv.length = %d\r\n", rcv.msgid, rcv.length);

	/* PROCESS TO BE ADDED */
	/* !!!!!!!!!!!!!!!!!!!!*/
	/* Process Message */
	msg_wmc_start_req_process(param_ptr, &error_code);
	
	/* STATE CHANGE IF OK */
	if (ERROR_CODE_NO_ERROR == error_code)
	{
    //weight_sensor_send_cmd(WIGHT_SENSOR_CMD_TYPE_START);
	}
	
	/* Send back the response */
	msg_wmc_start_resp(error_code);

	//返回
	return ret;
}

OPSTAT fsm_bfsc_wmc_command_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)	//MYC
{
	OPSTAT ret = IHU_SUCCESS;
	error_code_t error_code;
	msg_struct_l3bfsc_wmc_command_req_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_wmc_command_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_wmc_command_req_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	IhuDebugPrint("L3BFSC: msg_struct_l3bfsc_wmc_command_req_t: rcv.msgid = 0x%08X, rcv.length = %d cmd_flags=0x%08x\r\n", rcv.msgid, rcv.length, rcv.cmdid);

	/* PROCESS TO BE ADDED */
	/* !!!!!!!!!!!!!!!!!!!!*/
	/* Process Message */
	msg_wmc_command_req_process(param_ptr, &error_code);
	
	/* Send back the response */
	msg_wmc_command_resp(error_code, &rcv);
	
	//FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_CONFIGURATION);

	//返回
	return ret;
}


OPSTAT fsm_bfsc_wmc_stop_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)	//MYC
{
	OPSTAT ret = IHU_SUCCESS;
	error_code_t error_code;
	msg_struct_l3bfsc_wmc_stop_req_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_wmc_stop_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_wmc_stop_req_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	IhuDebugPrint("L3BFSC: msg_struct_l3bfsc_wmc_stop_req_t: rcv.msgid = 0x%08X, rcv.length = %d\r\n", rcv.msgid, rcv.length);

	/* PROCESS TO BE ADDED */
	/* !!!!!!!!!!!!!!!!!!!!*/
	/* Process Message */
	msg_wmc_stop_req_process(param_ptr, &error_code);
	
	/* STATE CHANGE IF OK */
	if (ERROR_CODE_NO_ERROR == error_code)
	{

    
		FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_CONFIGURATION);
		IhuDebugPrint("L3BFSC: msg_struct_l3bfsc_wmc_stop_req_t: Set to FSM_STATE_BFSC_CONFIGURATION\r\n");
	}
	
	/* Send back the response */
	msg_wmc_stop_resp(error_code);

	//返回
	return ret;
}

OPSTAT fsm_bfsc_wmc_combin_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)	//MYC
{
	OPSTAT ret = IHU_SUCCESS;
	error_code_t error_code;
	msg_struct_l3bfsc_wmc_combin_out_req_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_wmc_combin_out_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_wmc_combin_out_req_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	IhuDebugPrint("L3BFSC: msg_struct_l3bfsc_wmc_combin_out_req_t: rcv.msgid = 0x%08X, rcv.length = %d\r\n", rcv.msgid, rcv.length);

	/* PROCESS TO BE ADDED */
	/* !!!!!!!!!!!!!!!!!!!!*/
	/* Process Message */
	msg_wmc_combin_req_process(param_ptr, &error_code);
	
	/* Send back the response */
	msg_wmc_combin_resp(error_code);

	//返回
	return ret;
}

extern WeightSensorCalirationKB_t wsckb;
// new stable weight event coming from sensor
OPSTAT fsm_bfsc_wmc_weight_ind(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)	//MYC
{
  OPSTAT ret = IHU_SUCCESS;
	msg_struct_l3bfsc_weight_ind_t rcv;
  msg_struct_l3bfsc_wmc_ws_event_t msg_wmc_ws_event;
  //int weight;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_weight_ind_t));
	if ((param_ptr == NULL || param_len != sizeof(msg_struct_l3bfsc_weight_ind_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

  //weight = weight_sensor_map_adc_to_weight(rcv.adc_filtered);
	// removed by MYC, because the mapping is done in WeightThred !!!!
  
	/* Process Message */
	/* Check If it is the right/valid state to process the message */
	if( FSM_STATE_BFSC_SCAN != FsmGetState(TASK_ID_BFSC) && FSM_STATE_BFSC_COMBINATION != FsmGetState(TASK_ID_BFSC))
	{
			IhuErrorPrint("L3BFSC: fsm_bfsc_wmc_weight_ind: in wrong state %d, return\r\n", FsmGetState(TASK_ID_BFSC));
			return IHU_FAILURE;
	};

  if(FSM_STATE_BFSC_SCAN == FsmGetState(TASK_ID_BFSC))
  {
    // send new weight event
    //IhuDebugPrint("L3BFSC: fsm_bfsc_wmc_weight_ind: weight=%d adc_filtered=%d rep_times=%d k=%f b=%d\r\n", rcv.average_weight, rcv.adc_filtered, rcv.repeat_times, wsckb.k, wsckb.b);
    
		/* Build Message Content Header */
    if(rcv.repeat_times > 0)
      msg_wmc_ws_event.msgid = (MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT);
    else
      msg_wmc_ws_event.msgid = (MSG_ID_L3BFSC_WMC_NEW_WS_EVENT);
		
    msg_wmc_ws_event.wmc_id = zWmcInvenory.wmc_id;
    msg_wmc_ws_event.length = sizeof(msg_struct_l3bfsc_wmc_ws_event_t);
    msg_wmc_ws_event.weight_ind.average_weight = rcv.average_weight;
    msg_wmc_ws_event.weight_ind.weight_event = rcv.weight_event;
    msg_wmc_ws_event.weight_ind.repeat_times = rcv.repeat_times;
    msg_wmc_ws_event.weight_combin_type.ActionDelayMs = 0;
    msg_wmc_ws_event.weight_combin_type.WeightCombineType = HUITP_IEID_SUI_BFSC_COMINETYPE_NULL;
    
    /* Send Message to CAN Task */
    /* Build Message Content Header */
    if(rcv.repeat_times > 0)
		{
      msg_wmc_ws_event.msgid = (MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT);
			ret = ihu_message_send(MSG_ID_L3BFSC_WMC_NEW_WS_EVENT, TASK_ID_CANVELA, TASK_ID_BFSC, \
															&msg_wmc_ws_event, MSG_SIZE_L3BFSC_WMC_NEW_WS_EVENT);
			if (ret == IHU_FAILURE){
				IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_CANVELA]);
				return ret;
    }
    
		}
		else
    {
			msg_wmc_ws_event.msgid = (MSG_ID_L3BFSC_WMC_NEW_WS_EVENT);
			ret = ihu_message_send(MSG_ID_L3BFSC_WMC_NEW_WS_EVENT, TASK_ID_CANVELA, TASK_ID_BFSC, \
															&msg_wmc_ws_event, MSG_SIZE_L3BFSC_WMC_NEW_WS_EVENT);
			if (ret == IHU_FAILURE){
				IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_CANVELA]);
				return ret;
			}
		}
  }
  else if (FSM_STATE_BFSC_COMBINATION == FsmGetState(TASK_ID_BFSC))
  {
    IhuDebugPrint("L3BFSC: fsm_bfsc_wmc_weight_ind(rep): weight=%d emptyThred=%d, adc_filtered=%d rep_times=%d k=%f b=%d\r\n", \
							rcv.average_weight, zWeightSensorParam.WeightSensorEmptyThread, rcv.adc_filtered, rcv.repeat_times, wsckb.k, wsckb.b);
    
    //if(weight < zWeightSensorParam.WeightSensorEmptyThread)
		//if(rcv.average_weight < 500) // ONLY FOR TEST
		if(WEIGHT_EVENT_ID_EMPTY == rcv.weight_event)
		{
      // send combination response
      msg_struct_l3bfsc_wmc_combin_out_resp_t msg_wmc_combin_resp;
        
      /* Build Message Content Header */
      msg_wmc_combin_resp.msgid = (MSG_ID_L3BFSC_WMC_COMBIN_RESP);
      //msg_wmc_combin_resp.wmc_id = zWmcInvenory.wmc_id;
      msg_wmc_combin_resp.length = sizeof(msg_struct_l3bfsc_wmc_resp_t);
			msg_wmc_combin_resp.weight_combin_type.ActionDelayMs = 0;
			msg_wmc_combin_resp.weight_combin_type.WeightCombineType = zBfscWmcState.last_combin_type.WeightCombineType;
      
//			#define HUITP_IEID_SUI_BFSC_COMINETYPE_NULL 0
//			#define HUITP_IEID_SUI_BFSC_COMINETYPE_ROOLOUT 1
//			#define HUITP_IEID_SUI_BFSC_COMINETYPE_DROP 2
//			#define HUITP_IEID_SUI_BFSC_COMINETYPE_WARNING 3
//			#define HUITP_IEID_SUI_BFSC_COMINETYPE_ERROR 4
//			#define HUITP_IEID_SUI_BFSC_COMINETYPE_INVALID 0xFF
			
      //osDelay(1000); ///THIS
			IhuDebugPrint("L3BFSC: msg_wmc_combin_resp: msgid = 0x%08X\r\n", msg_wmc_combin_resp.msgid);
      //blk230_set_lamp(WMC_LAMP_OUT2_GREEN, WMC_LAMP_OFF);
			blk230_led_send_cmd(WMC_LAMP_OUT2_GREEN, LED_COMMNAD_OFF);
		
      /* Send Message to CAN Task */
      ret = ihu_message_send(MSG_ID_L3BFSC_WMC_COMBIN_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
                              &msg_wmc_combin_resp, MSG_SIZE_L3BFSC_WMC_COMBIN_RESP);
      if (ret == IHU_FAILURE){
        IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_CANVELA]);
        return ret;
      }
      
      // switch to SCAN state
      FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN);
			zBfscWmcState.last_combin_type.WeightCombineType = HUITP_IEID_SUI_BFSC_COMINETYPE_NULL;
			
    }
		else if(WEIGHT_EVENT_ID_LOAD == rcv.weight_event)  //2017/06/25 "15s" GIVE GREEN LIGHT BLINK SLOW
		{
				if( (zIhuAdcBfscWs.WeightCurrentTicks - zIhuAdcBfscWs.WeightCombinOutReceivedTicks) >= \
						MAX_WEIGHT_TICKS_TO_REMOVE_AFTER_COMBIN_OUT )
				{
						blk230_led_send_cmd(WMC_LAMP_OUT2_GREEN, LED_COMMNAD_BINKING_LOWSPEED);
						FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN); // BACK TO SCAN STATE
						//besides blink, do nothing else
				}
		}
		else
		{
				IhuErrorPrint("L3BFSC: fsm_bfsc_wmc_weight_ind: should NOT enter here.\n");
		}
  }
  else{
    IhuErrorPrint("L3BFSC: fsm_bfsc_wmc_weight_ind: in wrong state: %d, return\r\n", FsmGetState(TASK_ID_BFSC));
    ret = IHU_FAILURE;
  }
  
	//返回
	return ret;
}


long number_of_wmc_combin_timeout = 0;
long number_of_heart_beat_report = 0;
long number_of_heart_beat_confirm = 0;
extern long counter_can_tx_nok;
extern long counter_can_tx_total;
extern long counter_can_rx_nok;
extern long counter_can_rx_total;

OPSTAT fsm_bfsc_wmc_heart_beat_timeout(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
{
  OPSTAT ret = SUCCESS;
  msg_struct_com_time_out_t rcv;
	msg_struct_l3bfsc_wmc_heart_beat_report_t snd;

  //Receive message and copy to local variable
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("L3BFSC: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_BFSC]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
  
	if(TIMER_ID_10MS_BFSC_PERIOD_SCAN == rcv.timeId)
	{
		{
			number_of_wmc_combin_timeout++;
			if(0 == (number_of_wmc_combin_timeout % 100))
			{
				printf("%d:T:%d,%d,R:%d,%d,W+:%d\r\n",osKernelSysTick(), counter_can_tx_nok, counter_can_tx_total, counter_can_rx_nok, counter_can_rx_total, number_of_wmc_combin_timeout);
				//IhuErrorPrint("L3BFSC: Send Weight Ind (%d times)\r\n", number_of_wmc_combin_timeout);
			}
		}
	}
	
	if(TIMER_ID_1S_BFSC_HEART_BEAT == rcv.timeId)
	{
			number_of_heart_beat_report++;
		
			snd.msgid = MSG_ID_L3BFSC_WMC_HEART_BEAT_REPORT;
			snd.length = MSG_SIZE_L3BFSC_WMC_HEART_BEAT_REP;
			snd.wmc_id.wmc_id = zWmcInvenory.wmc_id.wmc_id;
			/* TimeStemp TO be added */
		
			IhuDebugPrint("send MSG_ID_L3BFSC_WMC_HEART_BEAT_REPORT, wmc_id=%d, report=%d, confirm=%d, SysTick=%d\n", zWmcInvenory.wmc_id.wmc_id, number_of_heart_beat_report, number_of_heart_beat_confirm, osKernelSysTick());
			ret = ihu_message_send(MSG_ID_L3BFSC_WMC_HEART_BEAT_REPORT, TASK_ID_CANVELA, TASK_ID_BFSC, &snd, snd.length);
			if (ret == IHU_FAILURE)
			{
					IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_CANVELA]);
					return IHU_FAILURE;
			}	
	}
	return IHU_SUCCESS;
}

OPSTAT fsm_bfsc_wmc_heart_beat_confirm(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
{
	
	number_of_heart_beat_confirm++;
	IhuDebugPrint("Receive MSG_ID_L3BFSC_WMC_HEART_BEAT_CONFIRM, wmc_id=%d, report=%d, confirm=%d, SysTick=%d\n", zWmcInvenory.wmc_id.wmc_id, number_of_heart_beat_report, number_of_heart_beat_confirm, osKernelSysTick());
	return IHU_SUCCESS;
}

OPSTAT fsm_bfsc_wmc_err_inq_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)	//MYC
{
	OPSTAT ret = IHU_SUCCESS;
	error_code_t error_code;
	msg_struct_l3bfsc_wmc_err_inq_req_t rcv;
	
	//收到消息并做参数检查
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_wmc_err_inq_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_wmc_err_inq_req_t)))
		IHU_ERROR_PRINT_BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	
	IhuDebugPrint("L3BFSC: msg_struct_l3bfsc_wmc_err_inq_req_t: rcv.msgid = 0x%08X, rcv.length = %d\r\n", rcv.msgid, rcv.length);

	/* PROCESS TO BE ADDED */
	/* !!!!!!!!!!!!!!!!!!!!*/
	/* Process Message */
	msg_wmc_err_inq_req_process(param_ptr, &error_code);
	
	/* Send back the response */
	msg_wmc_err_inq_resp(error_code);

	//返回
	return ret;
}
