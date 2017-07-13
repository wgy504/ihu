/**
 ****************************************************************************************
 *
 * @file l3iap.c
 *
 * @brief L3 IAP
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
#include "l3iap.h"
 
/*
** FSM of the L3IAP
*/
IhuFsmStateItem_t IhuFsmL3iap[] =
{
  //MessageId                 						//State                   		 							//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       										FSM_STATE_ENTRY,            						fsm_l3iap_task_entry}, //Starting

	//System level initialization, only controlled by VMDA
  {MSG_ID_COM_INIT,       								FSM_STATE_IDLE,            							fsm_l3iap_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_IDLE,            							fsm_com_do_nothing},

  //Task level initialization
  {MSG_ID_COM_INIT,       								FSM_STATE_L3IAP_INITED,            			fsm_l3iap_init},
  {MSG_ID_COM_INIT_FB,       							FSM_STATE_L3IAP_INITED,            			fsm_com_do_nothing},

	//ANY state entry
  {MSG_ID_COM_INIT_FB,                    FSM_STATE_COMMON,                       fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,                 FSM_STATE_COMMON,                       fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,              FSM_STATE_COMMON,                       fsm_com_do_nothing},
	{MSG_ID_COM_STOP,                       FSM_STATE_COMMON,                       fsm_l3iap_stop_rcv},
  {MSG_ID_COM_RESTART,                    FSM_STATE_COMMON,                       fsm_l3iap_restart},
	{MSG_ID_COM_TIME_OUT,                   FSM_STATE_COMMON,                       fsm_l3iap_time_out},
	
	{MSG_ID_IAP_SW_INVENTORY_CONFIRM,  		  FSM_STATE_L3IAP_INITED,                 fsm_l3iap_sw_inventory_confirm},        
	{MSG_ID_COM_TIME_OUT,                   FSM_STATE_L3IAP_INITED,                 fsm_l3iap_sw_inventory_timeout},
	
	{MSG_ID_IAP_SW_PACKAGE_CONFIRM,  	      FSM_STATE_L3IAP_ACTIVE,                 fsm_l3iap_sw_package_confirm},
  {MSG_ID_COM_TIME_OUT,                   FSM_STATE_L3IAP_ACTIVE,                 fsm_l3iap_sw_package_timeout},		
		
  //结束点，固定定义，不要改动
  {MSG_ID_END,            								FSM_STATE_END,             							NULL},  //Ending
};

//Global variables defination
strIhuL3iapTaskContext_t zIhuL3iapTaskContext;
SysEngParElementHwBurnPhyIdAddr_t zIhuSysEngElementHwBurnContext;
WmcInventory_t										zWmcInvenory;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_l3iap_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_L3IAP, FSM_STATE_IDLE) == IHU_FAILURE){
		IhuErrorPrint("L3IAP: Error Set FSM State at fsm_l3iap_task_entry.\n");
	}
	return IHU_SUCCESS;
}

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
	
	srand(zWmcInvenory.wmc_id.wmc_id); /* For generating ramdon value for test */
	
	return IHU_SUCCESS;
}


OPSTAT fsm_l3iap_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//串行回送INIT_FB给VMFO
	ihu_usleep(dest_id * IHU_SYSCFG_MODULE_START_DISTRIBUTION_DELAY_DURATION);
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FB to VMFO
		msg_struct_com_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_fb_t));
		snd.length = sizeof(msg_struct_com_init_fb_t);
		ret = ihu_message_send(MSG_ID_COM_INIT_FB, src_id, TASK_ID_L3IAP, &snd, snd.length);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3IAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_L3IAP].taskName, zIhuVmCtrTab.task[src_id].taskName);
			return IHU_FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_L3IAP, FSM_STATE_L3IAP_INITED) == IHU_FAILURE){
		IhuErrorPrint("L3IAP: Error Set FSM State!");	
		return IHU_FAILURE;
	}

	//初始化硬件接口
	if (func_l3iap_hw_init() == IHU_FAILURE){	
		IhuErrorPrint("L3IAP: Error initialize interface!");
		return IHU_FAILURE;
	}
	
	if (func_bfsc_hw_init(&zWmcInvenory) == IHU_FAILURE){	
		IhuErrorPrint("L3BFSC: Error initialize interface!");
		return IHU_FAILURE;
	}

	//Global Variables
	zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP] = 0;
	memset(&zIhuL3iapTaskContext, 0, sizeof(strIhuL3iapTaskContext_t));
	IhuDebugPrint("L3IAP: Init zIhuL3iapTaskContext, sizeof(strIhuL3iapTaskContext_t) = %d Bytes\n", sizeof(strIhuL3iapTaskContext_t));

	//Init Local Variables
	zIhuL3iapTaskContext.NextFlashSaveAddress = FLASH_ADDRESS_APP_LOAD;
	
	memset(&zIhuSysEngElementHwBurnContext, 0, sizeof(SysEngParElementHwBurnPhyIdAddr_t));
	IhuDebugPrint("L3IAP: Init zIhuSysEngElementHwBurnContext, sizeof(SysEngParElementHwBurnPhyIdAddr_t) = %d Bytes\n", sizeof(SysEngParElementHwBurnPhyIdAddr_t));
	
	//启动心跳定时器，确保喂狗的基本功能
	ret = ihu_timer_start(TASK_ID_L3IAP, TIMER_ID_1S_L3IAP_PERIOD_SCAN, \
		zIhuSysEngPar.timer.array[TIMER_ID_1S_L3IAP_PERIOD_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
		IhuErrorPrint("L3IAP: Error start timer!\n");
		return IHU_FAILURE;
	}
	
	//打印报告进入常规状态
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("L3IAP: Enter FSM_STATE_L3IAP_ACTIVE status, Keeping refresh here!\n");
	}
	
	//拉灯拉BEEP
	ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_GLOBAL_WORK_STATE, 20);
	ihu_ledpisces_galowag_start(GALOWAG_CTRL_ID_L3IAP_BEEP_PATTERN_SYS_START, 6);
	
	// Init Local Variables
	ret = LoadIhuSysEngElementHwBurnContext(&zIhuSysEngElementHwBurnContext, FLASH_ADDRESS_SW_CONTROL_TABLE);
	if(SUCCESS == ret)
	{
	    //if 
			switch(zIhuSysEngElementHwBurnContext.swUpgradeFlag)
			{
			    case IAP_SW_UPGRADE_TYPE_JUMP_TO_APP_LOAD:
						  IhuDebugPrint("L3IAP: about to call ihu_iap_sw_jump_to_app_load(), leaving IAP ...\n");
					    ihu_iap_sw_jump_to_app_load();
					    break;
				
					case IAP_SW_UPGRADE_TYPE_JUMP_TO_FACTORY_LOAD:
						  IhuDebugPrint("L3IAP: about to call ihu_iap_sw_jump_to_factory_load(), leaving IAP ...\n");
					    ihu_iap_sw_jump_to_factory_load();
              break;
					
					case IAP_SW_UPGRADE_TYPE_UPGRADE_APP_LOAD:
					case IAP_SW_UPGRADE_TYPE_UPGRADE_FACTORY_LOAD:
						  IhuDebugPrint("L3IAP: about to call ihu_send_iap_sw_inventory_report()\n");
						  ihu_send_iap_sw_inventory_report();
					
					    zIhuL3iapTaskContext.CurrentSegmentTransTimes++;
					    IhuDebugPrint("L3IAP: ihu_send_iap_sw_inventory_report() for %d times\n", zIhuL3iapTaskContext.CurrentSegmentTransTimes++);
					
						  //启动TIMER_ID_10MS_L3IAP_RETRANS_TIMEOUT定时器
					    IhuDebugPrint("L3IAP: TIMER_ID_10MS_L3IAP_RETRANS_TIMEOUT start ...\n");
	            ret = ihu_timer_start(TASK_ID_L3IAP, TIMER_ID_10MS_L3IAP_RETRANS_TIMEOUT, \
		               zIhuSysEngPar.timer.array[TIMER_ID_10MS_L3IAP_RETRANS_TIMEOUT].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_10MS);
	            if (ret == IHU_FAILURE){
		              zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
		              IhuErrorPrint("L3IAP: Error start timer!\n");
		              return IHU_FAILURE;
	             }
	
					    break;	

					default:
					    break;	
			}
	}
	else //(FAILURE == ret)
	{
		
	}
	
	
	
	//返回
	return IHU_SUCCESS;
}

OPSTAT fsm_l3iap_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	IhuErrorPrint("L3IAP: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	fsm_l3iap_init(0, 0, NULL, 0);
	
	return IHU_SUCCESS;
}

OPSTAT fsm_l3iap_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{	
	//入参检查
	if ((param_ptr == NULL) || (dest_id != TASK_ID_L3IAP)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
		IhuErrorPrint("L3IAP: Wrong input paramters!");
		return IHU_FAILURE;
	}
	
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3IAP, FSM_STATE_IDLE) == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
		IhuErrorPrint("L3IAP: Error Set FSM State!");
		return IHU_FAILURE;
	}
	
	//返回
	return IHU_SUCCESS;
}

//Local APIs
OPSTAT func_l3iap_hw_init(void)
{
	return IHU_SUCCESS;
}

//TIMER_OUT Processing
OPSTAT fsm_l3iap_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret;
	msg_struct_com_restart_t snd0;
	msg_struct_com_time_out_t rcv;
	
	//Receive message and copy to local variable
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		IhuErrorPrint("L3IAP: Receive message error!\n");
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
		return IHU_FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]是否超限
	if (zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP] > IHU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP] = zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP] - IHU_RUN_ERROR_LEVEL_2_MAJOR;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, TASK_ID_L3IAP, TASK_ID_L3IAP, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
			IhuErrorPrint("L3IAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_L3IAP].taskName, zIhuVmCtrTab.task[TASK_ID_L3IAP].taskName);
			return IHU_FAILURE;
		}
	}

	//喂狗心跳定时器
	if ((rcv.timeId == TIMER_ID_1S_L3IAP_PERIOD_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//func_l3iap_time_out_period_event_report();
	}
	
	else{
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
			IhuErrorPrint("L3IAP: Wrong timer parameter received!\n");
			return IHU_FAILURE;		
	}

	return IHU_SUCCESS;
}



//由于错误，直接关机，等待再次被激活
void func_l3iap_stm_main_recovery_from_fault(void)
{
	//msg_struct_l3iap_com_ctrl_cmd_t snd;

	//关闭所有外部器件的电源
	//延时并关断CPU系统
	ihu_sleep(1);
	
	//初始化模块的任务资源
	//初始化定时器：暂时决定不做，除非该模块重新RESTART
	//初始化模块级全局变量：暂时决定不做，除非该模块重新RESTART
	
	return;
}


OPSTAT fsm_l3iap_sw_inventory_confirm(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len) 
{
		OPSTAT ret;
		msg_struct_l3iap_sw_inventory_confirm_t rcv;
		
		//Receive message and copy to local variable
		memset(&rcv, 0, sizeof(msg_struct_l3iap_sw_inventory_confirm_t));
		if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3iap_sw_inventory_confirm_t))){
			IhuErrorPrint("L3IAP: Receive message error!\n");
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
			return IHU_FAILURE;
		}
		memcpy(&rcv, param_ptr, param_len);
			
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_inventory_confirm received: swRel=%d\n", rcv.swRel); 
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_inventory_confirm received: swVer=%d\n", rcv.swVer); 
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_inventory_confirm received: upgradeFlag=%d\n", rcv.upgradeFlag);
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_inventory_confirm received: swCheckSum=%d\n", rcv.swCheckSum);
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_inventory_confirm received: swTotalLengthInBytes=%d\n", rcv.swTotalLengthInBytes);
				
		zIhuL3iapTaskContext.swUpgradeType = rcv.upgradeFlag;					       //APP or FACTORY, or ... same as msg_struct_l3iap_iap_sw_upgrade_req
	  zIhuL3iapTaskContext.swRelId = rcv.swRel;
	  zIhuL3iapTaskContext.swVerId = rcv.swVer;
	  zIhuL3iapTaskContext.swCheckSum = rcv.swCheckSum;
	  zIhuL3iapTaskContext.swTotalLengthInBytes = rcv.swTotalLengthInBytes;
		
		if(rcv.swTotalLengthInBytes > L3IAP_MAX_LOAD_SIZE_IN_BYTES)
		{
			  IhuDebugPrint("L3IAP: fsm_l3iap_sw_inventory_confirm: rcv.swTotalLengthInBytes(%d) > L3IAP_MAX_LOAD_SIZE_IN_BYTES(%d)\n", \
			       rcv.swTotalLengthInBytes, L3IAP_MAX_LOAD_SIZE_IN_BYTES);
		    return IHU_FAILURE;
				//TO DO
			
		}
		
		/* Calculate Stop N protocal variables */
		zIhuL3iapTaskContext.swLengthInBytesPerSegment = MAX_LEN_PER_LOAD_SEGMENT;
		zIhuL3iapTaskContext.swTotalSegmentNumber = zIhuL3iapTaskContext.swTotalLengthInBytes / zIhuL3iapTaskContext.swLengthInBytesPerSegment;
		zIhuL3iapTaskContext.swLengthInBytesLastSegment = zIhuL3iapTaskContext.swTotalLengthInBytes % zIhuL3iapTaskContext.swLengthInBytesPerSegment;
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_inventory_confirm: swTotalSegmentNumber=%d, swLengthInBytesPerSegment=%d, swLengthInBytesLastSegment=%d\n", \
		    zIhuL3iapTaskContext.swTotalSegmentNumber, zIhuL3iapTaskContext.swLengthInBytesPerSegment, zIhuL3iapTaskContext.swLengthInBytesLastSegment);
		
		/* !!!! erase the flash block !!!! */
		if(IAP_SW_UPGRADE_TYPE_UPGRADE_APP_LOAD == zIhuL3iapTaskContext.swUpgradeType)
		{
				ret = ihu_iap_erase_flash_block(FLASH_ADDRESS_APP_LOAD, zIhuL3iapTaskContext.swTotalLengthInBytes);
				if(IHU_FAILURE == ret)
				{
						/* TO BE DISCUSSED */
						IhuDebugPrint("L3IAP: ihu_iap_erase_flash_block() failure, about to call ihu_iap_sw_jump_to_factory_load(), leaving IAP ...\n");
						ihu_iap_sw_jump_to_factory_load();
				}
		}
		else if(IAP_SW_UPGRADE_TYPE_UPGRADE_FACTORY_LOAD == zIhuL3iapTaskContext.swUpgradeType)
		{
				ret = ihu_iap_erase_flash_block(FLASH_ADDRESS_FACTORY_LOAD, zIhuL3iapTaskContext.swTotalLengthInBytes);
				if(IHU_FAILURE == ret)
				{
						/* TO BE DISCUSSED */
						IhuDebugPrint("L3IAP: ihu_iap_erase_flash_block() failure, about to call ihu_iap_sw_jump_to_app_load(), leaving IAP ...\n");
						ihu_iap_sw_jump_to_app_load();
				}
		}
		else
		{
			  //////
		}
		
		/* Send FIRST ihu_send_iap_sw_package_report */
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_inventory_confirm: Calling ihu_send_iap_sw_package_report\n");
		ihu_send_iap_sw_package_report();
		
		/* Initialized again the CurrentSegmentTransTimes */
		zIhuL3iapTaskContext.CurrentSegmentTransTimes = 1;
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_inventory_confirm: Calling ihu_send_iap_sw_package_report for %d times\n", zIhuL3iapTaskContext.CurrentSegmentTransTimes);
		
		/* Stats Change */
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_inventory_confirm: FsmSetState(TASK_ID_L3IAP, FSM_STATE_L3IAP_ACTIVE)\n");
		if (FsmSetState(TASK_ID_L3IAP, FSM_STATE_L3IAP_ACTIVE) == IHU_FAILURE){
				IhuErrorPrint("L3IAP: Error Set FSM State!");	
				return IHU_FAILURE;
	  }
		
		return IHU_SUCCESS;
}

OPSTAT fsm_l3iap_sw_inventory_timeout(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	  OPSTAT ret;
		msg_struct_com_time_out_t rcv;
	
	  //Receive message and copy to local variable
	  ret = ret;
	  memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	  if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		    IhuErrorPrint("L3IAP: Receive message error!\n");
		    zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
		    return IHU_FAILURE;
	   }
	  memcpy(&rcv, param_ptr, param_len);
	
 	  if ((rcv.timeId == TIMER_ID_10MS_L3IAP_RETRANS_TIMEOUT) &&(rcv.timeRes == TIMER_RESOLUTION_10MS))
		{
	      /* Check Transmition Times */
			  if(zIhuL3iapTaskContext.CurrentSegmentTransTimes >= L3IAP_MAX_RETRANSMISSION_TIME)
				{
						
					  IhuDebugPrint("L3IAP: fsm_l3iap_sw_inventory_timeout: CurrentSegmentTransTimes(%d) >= L3IAP_MAX_RETRANSMISSION_TIME(%d), About to Jump\n", \
					       zIhuL3iapTaskContext.CurrentSegmentTransTimes, L3IAP_MAX_RETRANSMISSION_TIME);
					
					  if(TRUE == zIhuSysEngElementHwBurnContext.bootLoad1Valid)
						{
							  IhuDebugPrint("L3IAP: fsm_l3iap_sw_inventory_timeout: bootLoad1Valid is TRUE, about to call ihu_iap_sw_jump_to_app_load(), leaving IAP ...\n"); 
								ihu_iap_sw_jump_to_app_load();
						}
						else if(TRUE == zIhuSysEngElementHwBurnContext.facLoadValid)
						{
							  IhuDebugPrint("L3IAP: fsm_l3iap_sw_inventory_timeout: facLoadValid is TRUE, about to call ihu_iap_sw_jump_to_factory_load(), leaving IAP ...\n");
								ihu_iap_sw_jump_to_factory_load();
						}
						else
						{
								IhuDebugPrint("L3IAP: No Valid load found, continue ...\n");
						}
				}
				
				/* Resend */
				IhuDebugPrint("L3IAP: about to call ihu_send_iap_sw_inventory_report()\n");
				ihu_send_iap_sw_inventory_report();
							
				zIhuL3iapTaskContext.CurrentSegmentTransTimes++;
				IhuDebugPrint("L3IAP: ihu_send_iap_sw_inventory_report() for %d times\n", zIhuL3iapTaskContext.CurrentSegmentTransTimes++);
	
	  }
	
  	return IHU_SUCCESS;
}

OPSTAT fsm_l3iap_sw_package_confirm(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{

		//typedef struct msg_struct_l3iap_sw_package_confirm
		//{
		//	UINT16 msgid;
		//	UINT16 length;
		//	UINT16 swRelId;
		//	UINT16 swVerId;
		//	UINT8  upgradeFlag;
		//	UINT16 segIndex;
		//	UINT16 segTotal;
		//	UINT16 segSplitLen;
		//	UINT16 segValidLen;
		//	UINT16 segCheckSum;
		//	UINT8  swPkgBody[MAX_LEN_PER_LOAD_SEGMENT];
		//}msg_struct_l3iap_sw_package_confirm_t;

  	OPSTAT ret;
		msg_struct_l3iap_sw_package_confirm_t rcv;
	  UINT32 SegmentLengthInBytes = rcv.length - sizeof(msg_struct_l3iap_sw_package_confirm_t);
	  UINT16 segCheckSumRcv = 0;
	  UINT16 segCheckSumCalculated = 0;
		
		//Receive message and copy to local variable
		memset(&rcv, 0, sizeof(msg_struct_l3iap_sw_package_confirm_t));
		if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3iap_sw_package_confirm_t))){
			IhuErrorPrint("L3IAP: Receive message error!\n");
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
			return IHU_FAILURE;
		}
		memcpy(&rcv, param_ptr, param_len);
		
		//	UINT16 swRelId;
		//	UINT16 swVerId;
		//	UINT8  upgradeFlag;
		//	UINT16 segIndex;
		//	UINT16 segTotal;
		//	UINT16 segSplitLen;
		//	UINT16 segValidLen;
		//	UINT16 segCheckSum;
		
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm received: swRelId=%d\n", rcv.swRelId); 
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm received: swVerId=%d\n", rcv.swVerId); 
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm received: upgradeFlag=%d\n", rcv.upgradeFlag); 
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm received: segIndex=%d\n", rcv.segIndex); 
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm received: segTotal=%d\n", rcv.segTotal);
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm received: segSplitLen=%d\n", rcv.segSplitLen); 
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm received: segValidLen=%d\n", rcv.segValidLen);
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm received: segCheckSum=%d\n", rcv.segCheckSum);
		
		SegmentLengthInBytes = rcv.segValidLen;
		segCheckSumRcv = rcv.segCheckSum;
		
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm received: SegmentLength=%d\n", SegmentLengthInBytes);

		/* Step 1: Segment Length Check */
		if(rcv.segIndex == zIhuL3iapTaskContext.swTotalSegmentNumber)
		{
				IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm: Segment (%d) is the last Segment (%d)\n", \
			      rcv.segIndex, zIhuL3iapTaskContext.swTotalSegmentNumber);
			  
			  if(SegmentLengthInBytes != zIhuL3iapTaskContext.swLengthInBytesLastSegment)
				{
					  IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm: Last Segment Length Received (%d) is not same as expected (%d), return\n", \
			          SegmentLengthInBytes, zIhuL3iapTaskContext.swLengthInBytesLastSegment);
				     return IHU_FAILURE;
				}
			  
		}
		else/* IF IT IS NOT LAST SEGMENT */
		{
			
			  IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm: Segment (%d) is not the last Segment (%d)\n", \
			      rcv.segIndex, zIhuL3iapTaskContext.swTotalSegmentNumber);
			
			  if(SegmentLengthInBytes != zIhuL3iapTaskContext.swLengthInBytesPerSegment)
				{
					  IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm: Last segment length received (%d) is not same as expected (%d), return\n", \
			          SegmentLengthInBytes, zIhuL3iapTaskContext.swLengthInBytesPerSegment);
				     return IHU_FAILURE;
				}			  		
		}
		
		/* Step 1.1 Checksum */
		segCheckSumCalculated == CalculateCheckSum(rcv.swPkgBody, SegmentLengthInBytes);
		if(segCheckSumCalculated != segCheckSumRcv)
		{
				IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm: segCheckSumCalculated(0x%04x)!=segCheckSumRcv(0x%04x), return\n", \
			          segCheckSumCalculated, segCheckSumRcv);
				return IHU_FAILURE;
		}
		
		/* Step 2: START TO CHECK RECEIVED INFORMATION */
		if( (zIhuL3iapTaskContext.swRelId != rcv.swRelId) ||
		    (zIhuL3iapTaskContext.swVerId != rcv.swVerId) ||
		    //(zIhuL3iapTaskContext.swTotalLengthInBytes != rcv.swTotalLengthInBytes) ||
		    (zIhuL3iapTaskContext.CurrentSegmentIndex != rcv.segIndex) )
		{
		    IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm expected: swRelId=%d\n", zIhuL3iapTaskContext.swRelId); 
		    IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm expected: swVerId=%d\n", zIhuL3iapTaskContext.swVerId); 
		    //IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm expected: swTotalLengthInBytes=%d\n", zIhuL3iapTaskContext.swTotalLengthInBytes);
		    IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm expected: CurrentSegmentIndex=%d\n", zIhuL3iapTaskContext.CurrentSegmentIndex);
			
			  IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm: expected != received, do nothing, wait for retransmission\n");
			
			  /* THIS IS IMPORTANT, RETURN FAILURE WILL TRIGER TIMEOUT, zIhuL3iapTaskContext.CurrentSegmentTransTimes++ WILL BE CALLED THERE */
			  return IHU_FAILURE;
		}

		/* Step 3: SAVE SEGMENT TO FLASH */
		IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm: expected == received, save to FLASH address 0x%08x, length=%d\n", \
		    zIhuL3iapTaskContext.NextFlashSaveAddress, SegmentLengthInBytes);

		ret = ihu_iap_write_flash(zIhuL3iapTaskContext.NextFlashSaveAddress, SegmentLengthInBytes, rcv.swPkgBody);
		if(IHU_FAILURE == ret)
		{
			  /* TO DO */
		}
		else
		{
				/* point to next flash address */
			  zIhuL3iapTaskContext.NextFlashSaveAddress = zIhuL3iapTaskContext.NextFlashSaveAddress + SegmentLengthInBytes;
		}
		
		/* Step 4: CHECK WHETHER IT IS THE LAST SEGMENT */
    zIhuL3iapTaskContext.swCalculatedCheckSum = zIhuL3iapTaskContext.swCalculatedCheckSum ^ segCheckSumCalculated;
			  		
		/* IF IT IS LAST SEGMENT, JUMP TO IT */
		if(rcv.segIndex == zIhuL3iapTaskContext.swTotalSegmentNumber)
		{
				IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm: Segment (%d) is the last Segment (%d), request for Segment(%d)\n", \
			      rcv.segIndex, zIhuL3iapTaskContext.swTotalSegmentNumber, zIhuL3iapTaskContext.CurrentSegmentIndex);
			
				/* Check Checksum */
			  /* TO DO */
			  if(zIhuL3iapTaskContext.swCalculatedCheckSum!=zIhuL3iapTaskContext.swCheckSum)
				{
					  /* TO DO */
					  IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm() swCalculatedCheckSum(0x%04x)!=swCheckSum(0x%04x), Checksum failure\n", \
					       zIhuL3iapTaskContext.swCalculatedCheckSum!=zIhuL3iapTaskContext.swCheckSum);
					  /* TO DO */
					  
				}
			  /* TO DO */
			
				if(IAP_SW_UPGRADE_TYPE_UPGRADE_APP_LOAD == zIhuL3iapTaskContext.swUpgradeType)
				{
						IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm() last segment (%d), about to call ihu_iap_sw_jump_to_factory_load(), leaving IAP with SUCCESS RESULT...\n", rcv.segIndex);
					  zIhuSysEngElementHwBurnContext.bootLoad1RelId = zIhuL3iapTaskContext.swRelId;
					  zIhuSysEngElementHwBurnContext.bootLoad1VerId = zIhuL3iapTaskContext.swVerId;
					  zIhuSysEngElementHwBurnContext.bootLoad1Valid = TRUE;

  					ihu_iap_sw_save_hw_sw_control_table();
						ihu_iap_sw_jump_to_app_load();
				}
				else if(IAP_SW_UPGRADE_TYPE_UPGRADE_FACTORY_LOAD == zIhuL3iapTaskContext.swUpgradeType)
				{
						IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm() last segment (%d), about to call ihu_iap_sw_jump_to_app_load(), leaving IAP with SUCCESS RESULT...\n", rcv.segIndex);
						zIhuSysEngElementHwBurnContext.facLoadSwRel = zIhuL3iapTaskContext.swRelId;
					  zIhuSysEngElementHwBurnContext.facLoadSwVer = zIhuL3iapTaskContext.swVerId;
					  zIhuSysEngElementHwBurnContext.facLoadValid = TRUE;

  					ihu_iap_sw_save_hw_sw_control_table();
						ihu_iap_sw_jump_to_factory_load();
				}
				else
				{
						//////
				}			  
		}
		else/* IF IT IS NOT LAST SEGMENT */
		{
			  zIhuL3iapTaskContext.CurrentSegmentIndex++;        /* Request for next segment */
			  zIhuL3iapTaskContext.CurrentSegmentTransTimes = 1; /* Reset Retrans to 1 */
			
			  IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_confirm: Segment (%d) is not the last Segment (%d), request for Segment(%d)\n", \
			      rcv.segIndex, zIhuL3iapTaskContext.swTotalSegmentNumber, zIhuL3iapTaskContext.CurrentSegmentIndex);
			  
			  /* Request for next segment */ 
			  ihu_send_iap_sw_package_report();
		}
		
		return IHU_SUCCESS;
}

OPSTAT fsm_l3iap_sw_package_timeout(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	  OPSTAT ret;
		msg_struct_com_time_out_t rcv;
	
	  //Receive message and copy to local variable
	  ret = ret;
	  memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	  if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		    IhuErrorPrint("L3IAP: Receive message error!\n");
		    zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
		    return IHU_FAILURE;
	   }
	  memcpy(&rcv, param_ptr, param_len);
	
 	  if ((rcv.timeId == TIMER_ID_10MS_L3IAP_RETRANS_TIMEOUT) &&(rcv.timeRes == TIMER_RESOLUTION_10MS))
		{
	      /* Check Transmition Times */
			  if(zIhuL3iapTaskContext.CurrentSegmentTransTimes >= L3IAP_MAX_RETRANSMISSION_TIME)
				{
						
					IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_timeout: CurrentSegmentTransTimes(%d) >= L3IAP_MAX_RETRANSMISSION_TIME(%d), About to Jump\n", \
					       zIhuL3iapTaskContext.CurrentSegmentTransTimes, L3IAP_MAX_RETRANSMISSION_TIME);
					
					  if(TRUE == zIhuSysEngElementHwBurnContext.bootLoad1Valid)
						{
							  IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_timeout: bootLoad1Valid is TRUE, about to call ihu_iap_sw_jump_to_app_load(), leaving IAP ...\n"); 
								ihu_iap_sw_jump_to_app_load();
						}
						else if(TRUE == zIhuSysEngElementHwBurnContext.facLoadValid)
						{
							  IhuDebugPrint("L3IAP: fsm_l3iap_sw_package_timeout: facLoadValid is TRUE, about to call ihu_iap_sw_jump_to_factory_load(), leaving IAP ...\n");
								ihu_iap_sw_jump_to_factory_load();
						}
						else
						{
								IhuDebugPrint("L3IAP: No Valid load found, continue ...\n");
						}
				}
				
				/* Resend */
				IhuDebugPrint("L3IAP: about to call ihu_send_iap_sw_packege_report()\n");
				ihu_send_iap_sw_package_report();
							
				zIhuL3iapTaskContext.CurrentSegmentTransTimes++;
				IhuDebugPrint("L3IAP: ihu_send_iap_sw_packege_report() for %d times\n", zIhuL3iapTaskContext.CurrentSegmentTransTimes++);
	
	  }
	
  	return IHU_SUCCESS;
}



void ihu_send_iap_sw_inventory_report(void)
{
	  OPSTAT ret;
		msg_struct_l3iap_sw_upgrade_report_t snd0;
	
		memset(&snd0, 0, sizeof(msg_struct_l3iap_sw_upgrade_report_t));
		snd0.length = sizeof(msg_struct_l3iap_sw_upgrade_report_t);
	  snd0.msgid = MSG_ID_IAP_SW_INVENTORY_REPORT;
	
		snd0.hwId  = zIhuSysEngElementHwBurnContext.hwPemId;
		snd0.hwType = zIhuSysEngElementHwBurnContext.hwType;
		snd0.upgradeFlag = 0; //////////!!!!!!/////////???????/////////
	
	  if(TRUE == zIhuSysEngElementHwBurnContext.bootLoad1Valid)
		{
				snd0.swRel = zIhuSysEngElementHwBurnContext.bootLoad1RelId;
				snd0.swVer = zIhuSysEngElementHwBurnContext.bootLoad1VerId;
		}
		else
		{
				snd0.swRel = 0;
				snd0.swVer = 0;
		}
	
	  IhuDebugPrint("L3IAP: ihu_send_iap_sw_inventory_report: swRel=%d, swVer=%d\n", snd0.swRel, snd0.swVer);
		ret = ihu_message_send(MSG_ID_IAP_SW_INVENTORY_REPORT, TASK_ID_CANVELA, TASK_ID_L3IAP, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
			IhuErrorPrint("L3IAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_L3IAP].taskName, zIhuVmCtrTab.task[TASK_ID_L3IAP].taskName);
			return;
		}
	
}

extern OPSTAT LoadIhuSysEngElementHwBurnContext(SysEngParElementHwBurnPhyIdAddr_t *psepehb, UINT32 SysEngElementHwBurnContextAddress)
{
	
		SysEngParElementHwBurnPhyIdAddr_t *pFlash;
		if(NULL == psepehb)
		{
			  IhuErrorPrint("L3IAP: LoadIhuSysEngElementHwBurnContext: NULL == psepehb, return failure\n");
			  return IHU_FAILURE;
		}
		
		if(SysEngElementHwBurnContextAddress != FLASH_ADDRESS_SW_CONTROL_TABLE)
		{
				IhuErrorPrint("L3IAP: SysEngElementHwBurnContextAddress != FLASH_ADDRESS_SW_CONTROL_TABLE, return failure\n");
			  return IHU_FAILURE;
		}
	
		pFlash = (SysEngParElementHwBurnPhyIdAddr_t *)FLASH_ADDRESS_SW_CONTROL_TABLE;
		if( (0 == pFlash->hwType) &&
			  (0 == pFlash->hwPemId) &&
		    (0 == pFlash->swRelId) &&
		    (0 == pFlash->swVerId) )
    {
		    SysEngElementHwBurnFlashContentInit();
		}
		
		memcpy((void *)psepehb, (void *)pFlash, sizeof(SysEngParElementHwBurnPhyIdAddr_t));
	
	  return IHU_SUCCESS;
}

void ihu_send_iap_sw_package_report(void)
{
	  OPSTAT ret;
		msg_struct_l3iap_sw_package_report_t snd0;
	
		memset(&snd0, 0, sizeof(msg_struct_l3iap_sw_package_report_t));
		snd0.length = sizeof(msg_struct_l3iap_sw_package_report_t);
	  snd0.msgid = MSG_ID_IAP_SW_PACKAGE_REPORT;

		snd0.swRelId = zIhuL3iapTaskContext.swRelId;
		snd0.swVerId = zIhuL3iapTaskContext.swVerId;
		snd0.upgradeFlag = IAP_SW_UPGRADE_PREPARE_STATUS_APP_LOAD_FLASH_ERASED_OK;
		snd0.segIndex = zIhuL3iapTaskContext.CurrentSegmentIndex;
		snd0.segTotal = zIhuL3iapTaskContext.swTotalSegmentNumber;
		snd0.segSplitLen = zIhuL3iapTaskContext.swLengthInBytesPerSegment;

		IhuDebugPrint("L3IAP: ihu_send_iap_sw_package_report: request segment(%d of %d) for total size of (%d) bytes\n", \
	      zIhuL3iapTaskContext.CurrentSegmentIndex, zIhuL3iapTaskContext.swTotalSegmentNumber, zIhuL3iapTaskContext.swTotalLengthInBytes);
		ret = ihu_message_send(MSG_ID_IAP_SW_PACKAGE_REPORT, TASK_ID_CANVELA, TASK_ID_L3IAP, &snd0, snd0.length);
		if (ret == IHU_FAILURE){
			  zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;
			  IhuErrorPrint("L3IAP: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_L3IAP].taskName, zIhuVmCtrTab.task[TASK_ID_L3IAP].taskName);
			  return;
		}
}

void SysEngElementHwBurnFlashContentInit(void)
{
		IhuDebugPrint("L3IAP: SysEngElementHwBurnFlashContentInit start ...\n");
		//zIhuSysEngElementHwBurnContext.equLable[20];
		zIhuSysEngElementHwBurnContext.hwType = 1;
		zIhuSysEngElementHwBurnContext.hwPemId = 2;
		zIhuSysEngElementHwBurnContext.swRelId = 3;
		zIhuSysEngElementHwBurnContext.swVerId = 4;
		zIhuSysEngElementHwBurnContext.swUpgradeFlag = 0;
		zIhuSysEngElementHwBurnContext.swUpgPollId = 0;
		zIhuSysEngElementHwBurnContext.bootIndex = 1;
		zIhuSysEngElementHwBurnContext.bootAreaMax = 2;   //32
		zIhuSysEngElementHwBurnContext.facLoadAddr = 0; 
		zIhuSysEngElementHwBurnContext.facLoadSwRel = 0;
		zIhuSysEngElementHwBurnContext.facLoadSwVer = 0;	
		zIhuSysEngElementHwBurnContext.facLoadCheckSum = 0;
		zIhuSysEngElementHwBurnContext.facLoadValid = 0;
		zIhuSysEngElementHwBurnContext.spare2 = 0;
		zIhuSysEngElementHwBurnContext.bootLoad1Addr = FLASH_ADDRESS_APP_LOAD;
		zIhuSysEngElementHwBurnContext.bootLoad1RelId = 2;
		zIhuSysEngElementHwBurnContext.bootLoad1VerId = 3;
		zIhuSysEngElementHwBurnContext.bootLoad1CheckSum = 0;
		zIhuSysEngElementHwBurnContext.bootLoad1Valid = 1;
		zIhuSysEngElementHwBurnContext.spare3  = 0;   //32
		zIhuSysEngElementHwBurnContext.bootLoad2Addr = 0;
		zIhuSysEngElementHwBurnContext.bootLoad2RelId = 0;
		zIhuSysEngElementHwBurnContext.bootLoad2VerId = 0;
		zIhuSysEngElementHwBurnContext.bootLoad2CheckSum = 0;
		zIhuSysEngElementHwBurnContext.bootLoad2Valid = 0;
		zIhuSysEngElementHwBurnContext.spare4 = 0;
		zIhuSysEngElementHwBurnContext.bootLoad3Addr = 0;
		zIhuSysEngElementHwBurnContext.bootLoad3RelId = 0;
		zIhuSysEngElementHwBurnContext.bootLoad3VerId = 0;
		zIhuSysEngElementHwBurnContext.bootLoad3CheckSum = 0;
		zIhuSysEngElementHwBurnContext.bootLoad3Valid = 0;
		zIhuSysEngElementHwBurnContext.spare5 = 0;       //32
		//zIhuSysEngElementHwBurnContext.cipherKey[16];
		//zIhuSysEngElementHwBurnContext.rsv[16];   //32
		
		ihu_iap_sw_save_hw_sw_control_table();
		IhuDebugPrint("L3IAP: SysEngElementHwBurnFlashContentInit complete\n");
}
	

void ihu_iap_sw_save_hw_sw_control_table(void)
{
		OPSTAT ret;

		ret = ihu_iap_erase_flash_block(FLASH_ADDRESS_SW_CONTROL_TABLE, sizeof(SysEngParElementHwBurnPhyIdAddr_t));
		if(IHU_FAILURE == ret)
		{
				/* TO BE DISCUSSED */
				IhuDebugPrint("L3IAP: ihu_iap_sw_save_hw_sw_control_table(), ihu_iap_erase_flash_block() failure\n");
		}
		
		ret = ihu_iap_write_flash(FLASH_ADDRESS_SW_CONTROL_TABLE, sizeof(SysEngParElementHwBurnPhyIdAddr_t), (UINT8 *)&zIhuSysEngElementHwBurnContext);
		if(IHU_FAILURE == ret)
		{
			  /* TO BE DISCUSSED */
			  IhuDebugPrint("L3IAP: ihu_iap_sw_save_hw_sw_control_table(), ihu_iap_write_flash() failure\n");
		}
}

UINT16 CalculateCheckSum(UINT8 *pdata, UINT32 length_in_bytes)
{
	 return 0;
}
	
void ihu_iap_sw_jump_to_app_load(void)
{
	  IhuDebugPrint("L3IAP: disable_irq and about to jump to app(0x%08X)\n", FLASH_ADDRESS_APP_LOAD);
		__disable_irq();
    IAP_ExecuteApp_fun(FLASH_ADDRESS_APP_LOAD);
}

void ihu_iap_sw_jump_to_factory_load(void)
{
	  IhuDebugPrint("L3IAP: disable_irq and about to jump to factory(0x%08X)\n", FLASH_ADDRESS_FACTORY_LOAD);
		HAL_FLASH_Lock();
		__disable_irq();
    IAP_ExecuteApp_fun(FLASH_ADDRESS_FACTORY_LOAD);
}

void ihu_iap_sw_jump_to_iap_load(void)
{
	  IhuDebugPrint("L3IAP: disable_irq and about to jump to iap(0x%08X)\n", FLASH_ADDRESS_IAP_LOAD);
	  HAL_FLASH_Lock();
		__disable_irq();
    IAP_ExecuteApp_fun(FLASH_ADDRESS_IAP_LOAD);
}

OPSTAT ihu_iap_erase_flash_block(uint32_t start_address, uint32_t length_in_bytes)
{
//#define FLASH_ADDRESS_SW_CONTROL_TABLE             (0x080E0000)  // TO BE UPDATE
//#define FLASH_ADDRESS_IAP_LOAD                     (0x08000000)
//#define FLASH_ADDRESS_FACTORY_LOAD                 (0x08020000)  // TO BE UPDATE
//#define FLASH_ADDRESS_APP_LOAD                     (0x08080000)  // TO BE UPDATE

//#define FLASH_SECTOR_HW_CONTROL_TABLE              FLASH_SECTOR_11
//#define FLASH_SECTOR_IAP_LOAD                      FLASH_SECTOR_0
//#define FLASH_SECTOR_FACTORY_LOAD                  FLASH_SECTOR_5
//#define FLASH_SECTOR_APP_LOAD                      FLASH_SECTOR_8
    UINT32 sector_start_index = 0;
	  UINT32 sector_stop_index = 0;
	  UINT32 index = 0;
    
		switch(start_address)
		{
				case FLASH_ADDRESS_SW_CONTROL_TABLE:
					  if(length_in_bytes > FLASH_MAX_SIZE_HW_CONTROL_TABLE)
						{
								IhuErrorPrint("L3IAP: ihu_iap_erase_flash_block, length_in_bytes(%d) > FLASH_MAX_SIZE_HW_CONTROL_TABLE*%d), return failure\n", length_in_bytes, FLASH_MAX_SIZE_HW_CONTROL_TABLE);
								return IHU_FAILURE;
						}
						sector_start_index = FLASH_SECTOR_11;
				    sector_stop_index = FLASH_SECTOR_11;
						IhuDebugPrint("L3IAP: ihu_iap_erase_flash_block, FLASH_ADDRESS_SW_CONTROL_TABLE(0x%08x), sector_start_index=%d, sector_stop_index=%d\n", start_address, sector_start_index, sector_stop_index);
				    break;
						
				case FLASH_ADDRESS_IAP_LOAD:
					  if(length_in_bytes > FLASH_MAX_SIZE_IAP_LOAD)
						{
								IhuErrorPrint("L3IAP: ihu_iap_erase_flash_block, length_in_bytes(%d) > FLASH_MAX_SIZE_HW_CONTROL_TABLE*%d), return failure\n", length_in_bytes, FLASH_MAX_SIZE_HW_CONTROL_TABLE);
								return IHU_FAILURE;
						}
						
						sector_start_index = FLASH_SECTOR_0;
				    if(length_in_bytes > (0x00010000)) // > 64K
						{
								sector_stop_index = FLASH_SECTOR_4;
						}
						else
						{
							  sector_stop_index = length_in_bytes / (0x400);
						}
					  IhuDebugPrint("L3IAP: ihu_iap_erase_flash_block, FLASH_ADDRESS_IAP_LOAD(0x%08x), sector_start_index=%d, sector_stop_index=%d\n", start_address, sector_start_index, sector_stop_index);
						break;
						
				case FLASH_ADDRESS_FACTORY_LOAD:
					 	if(length_in_bytes > FLASH_MAX_SIZE_FACTORY_LOAD)
						{
								IhuErrorPrint("L3IAP: ihu_iap_erase_flash_block, length_in_bytes(%d) > FLASH_MAX_SIZE_HW_CONTROL_TABLE*%d), return failure\n", length_in_bytes, FLASH_MAX_SIZE_HW_CONTROL_TABLE);
								return IHU_FAILURE;
						}
						
						sector_start_index = FLASH_SECTOR_5;
					  sector_stop_index = sector_start_index + length_in_bytes / (0x2000); /* 128K */
						IhuDebugPrint("L3IAP: ihu_iap_erase_flash_block, FLASH_ADDRESS_FACTORY_LOAD(0x%08x), sector_start_index=%d, sector_stop_index=%d\n", start_address, sector_start_index, sector_stop_index);
					  break;
						
				case FLASH_ADDRESS_APP_LOAD:
						if(length_in_bytes > FLASH_MAX_SIZE_APP_LOAD)
						{
								IhuErrorPrint("L3IAP: ihu_iap_erase_flash_block, length_in_bytes(%d) > FLASH_MAX_SIZE_HW_CONTROL_TABLE*%d), return failure\n", length_in_bytes, FLASH_MAX_SIZE_HW_CONTROL_TABLE);
								return IHU_FAILURE;
						}
						
						sector_start_index = FLASH_SECTOR_8;
					  sector_stop_index = sector_start_index + length_in_bytes / (0x2000); /* 128K */
						IhuDebugPrint("L3IAP: ihu_iap_erase_flash_block, FLASH_ADDRESS_APP_LOAD(0x%08x), sector_start_index=%d, sector_stop_index=%d\n", start_address, sector_start_index, sector_stop_index);
					  
	          break;
				default:
						IhuErrorPrint("L3IAP: ihu_iap_erase_flash_block, invalid start_address(0x%08x), return failure\n", start_address);
						return IHU_FAILURE;	
				
					  //break;
		}
		
		IhuDebugPrint("L3IAP: ihu_iap_erase_flash_block, HAL_FLASH_Unlock() start at SysTick(%d)\n", osKernelSysTick());
		HAL_FLASH_Unlock();
    IhuDebugPrint("L3IAP: ihu_iap_erase_flash_block, HAL_FLASH_Unlock() complete at SysTick(%d)\n", osKernelSysTick());
		
	  for(index = sector_start_index; index <= sector_stop_index; index++)
		{
				IhuDebugPrint("L3IAP: ihu_iap_erase_flash_block, FLASH_Erase_Sector(%) start at SysTick(%d)\n", index, osKernelSysTick());
				FLASH_Erase_Sector(index, (uint8_t) FLASH_VOLTAGE_RANGE_2);
			  IhuDebugPrint("L3IAP: ihu_iap_erase_flash_block, FLASH_Erase_Sector(%) complete at SysTick(%d)\n", index, osKernelSysTick());
		}
	
		HAL_FLASH_Lock();
	  return IHU_SUCCESS;
}

OPSTAT ihu_iap_write_flash(uint32_t start_address, uint32_t length_in_bytes, uint8_t *data)
{
    UINT32 i;
	  UINT16 val;
	  UINT32 write_len;
	  UINT32 write_addr;
	  HAL_StatusTypeDef status;
	
	  if(start_address > (1024*1024-2))
		{
				IhuErrorPrint("L3IAP: ihu_iap_write_flash, start_address(%d) > (1024*1024-2)(%d), return failure\n", start_address, (1024*1024-2));
			  return IHU_FAILURE;
		}

    if(length_in_bytes > (1024*1024))
		{
				IhuErrorPrint("L3IAP: ihu_iap_write_flash, length_in_bytes(%d) > (1024*1024)(%d), return failure\n", length_in_bytes, (1024*1024));
			  return IHU_FAILURE;
		}
	
    if(NULL == data)
		{
				IhuErrorPrint("L3IAP: ihu_iap_write_flash, NULL == data, return failure\n");
			  return IHU_FAILURE;
		}				
	
		write_len = length_in_bytes;
		write_addr = start_address;
		IhuDebugPrint("L3IAP: ihu_iap_write_flash: write_addr=0x%08X, write value=0x%04X start at sysTick(%d)\n", write_addr, val, osKernelSysTick());//*(uint16_t *)(iap_pac_conf->payload.swPackageSegmentContent[i]));

		HAL_FLASH_Unlock();
		for(i = 0; i < write_len; i = i + 2)
    {
        val =  ((uint16_t)data[i] + ((uint16_t)data[i+1]<<8));
			  status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, write_addr, val);
			  if(HAL_OK != status)
				{
						IhuErrorPrint("L3IAP: ihu_iap_write_flash, HAL_FLASH_Program failure with status=%d, return failure\n", status);
					  HAL_FLASH_Lock();
					  return IHU_FAILURE;
				}
        write_addr = write_addr + 2;
    }
		
		HAL_FLASH_Lock();
		IhuDebugPrint("L3IAP: ihu_iap_write_flash: write_addr=0x%08X, write value=0x%04X complete at sysTick(%d)\n", write_addr, val, osKernelSysTick());//*(uint16_t *)(iap_pac_conf->payload.swPackageSegmentContent[i]));
		
		return IHU_SUCCESS;
		
}
