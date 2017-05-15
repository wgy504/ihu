/*
 * vmfreeoslayer.c
 *
 *  Created on: 2016年9月18日
 *      Author: test
 */

#include "vmfreeoslayer.h"

/**********************************************************************************
 *
 *   =====VM的使用方式=====
 *   1. 定义全局变量、任务模块、消息结构
 *   2. 完善系统DIMENSION，工程参数定义
 *   3. 调用硬件初始化  ihu_vm_system_ctr_table_init();
 *   4. 调用任务创建 ihu_task_create_all(void);
 *   5. 进入while(1)循环，并sleep状态，每1分钟醒来一次，检查各个任务的状态以及messageque，如果非正常，则重新启动该任务
 *   6. 象征性的清理环境，调用 ihu_task_delete_all_and_queue();
 *   7. 结束
 *
 **********************************************************************************/


/**********************************************************************************
 *
 *   VMLAYER全局三张变量控制表
 *
 **********************************************************************************/

IhuVmCtrTab_t 			zIhuVmCtrTab;  		//全局系统总控表
IhuSysEngParTab_t 	zIhuSysEngPar; 		//全局工程参数控制表
IhuSysStaPm_t				zIhuSysStaPm;			//全局性能统计表

/*
 *
 *
 *   任务初始化配置参数
 *
 *
 */

//任务初始化配置参数
//从极致优化内存的角度，这里浪费了2个TASK对应的内存空间（MIN=0/MAX=n+1)，但它却极大的改善了程序编写的效率，值得浪费！！！
IhuVmCtrTaskStaticCfg_t zIhuVmCtrTaskStaticCfg[] =
{
	//TASK_ID,    				状态控制				状态机入口 					   控制启动             TRACE_CTRL        //注释
	{TASK_ID_MIN, 				"TASKMIN", 			NULL,                  IHU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},	 //Starting
	{TASK_ID_VMFO, 				"VMFO", 				&IhuFsmVmfo,           IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_TIMER, 			"TIMER", 				&IhuFsmTimer,          IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	{TASK_ID_EMC68X, 			"EMC68X", 			&IhuFsmEmc68x,         IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)		
	{TASK_ID_ADCLIBRA, 		"ADCLIBRA", 		&IhuFsmAdclibra,       IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_I2CARIES, 		"I2CARIES", 		&IhuFsmI2caries,       IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SPSVIRGO, 		"SPSVIRGO", 		&IhuFsmSpsvirgo,       IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_DIDOCAP, 		"DIDOCAP", 			&IhuFsmDidocap,        IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_LEDPISCES, 	"LEDPISCES", 		&IhuFsmLedpisces,      IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_DCMIARIS, 		"DCMIARIS", 		&IhuFsmDcmiaris,       IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CCL, 				"CCL", 					&IhuFsmCcl,            IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	{TASK_ID_ADCLIBRA, 		"ADCLIBRA", 		&IhuFsmAdclibra,       IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SPILEO, 			"SPILEO", 			&IhuFsmSpileo,         IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CANVELA, 		"CANVELA", 			&IhuFsmCanvela,        IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_I2CARIES, 		"I2CARIES", 		&IhuFsmI2caries,       IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_LEDPISCES, 	"LEDPISCES", 		&IhuFsmLedpisces,      IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_BFSC, 				"BFSC", 				&IhuFsmBfsc,           IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
//为了提供完成列表，而且需要按照顺序来                        
#else 
	{TASK_ID_ADCLIBRA, 		"ADCLIBRA", 		&IhuFsmAdclibra,       IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SPILEO, 			"SPILEO", 			&IhuFsmSpileo,         IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_I2CARIES, 		"I2CARIES", 		&IhuFsmI2caries,       IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_PWMTAURUS, 	"PWMTAURUS", 		&IhuFsmPwmtaurus,      IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CANVELA, 		"CANVELA", 			&IhuFsmCanvela,        IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},	
	{TASK_ID_SPSVIRGO, 		"SPSVIRGO", 		&IhuFsmSpsvirgo,       IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_DIDOCAP, 		"DIDOCAP", 			&IhuFsmDidocap,        IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_LEDPISCES, 	"LEDPISCES", 		&IhuFsmLedpisces,      IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_ETHORION, 		"LEDPISCES", 		&IhuFsmEthorion,       IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_DCMIARIS, 		"DCMIARIS", 		&IhuFsmDcmiaris,       IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_EMC68X, 			"EMC68X", 			&IhuFsmEmc68x,         IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CCL, 				"CCL", 					&IhuFsmCcl,            IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_BFSC, 				"BFSC", 				&IhuFsmBfsc,           IHU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#endif
  {TASK_ID_MAX,					"TASKMAX", 		  NULL,                  IHU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},	 //Ending
};
		
//消息ID的定义全局表，方便TRACE函数使用
//请服从IHU_SYSDIM_MSGID_NAME_LEN_MAX的最长定义，不然出错
//全局变量：消息打印命名
IhuVmCtrMsgStaticCfg_t zIhuVmCtrMsgStaticCfg[] = {
	//MSG_ID,    				                            消息名字				                         TRACE_CTRL   //注释
	//START FLAG
  {MSG_ID_COM_MIN,                                "MSG_ID_COM_MIN",                        1, 1, 1},    //STARTING
  {MSG_ID_COM_INIT,                               "MSG_ID_COM_INIT",                       1, 1, 1},
  {MSG_ID_COM_INIT_FB,                            "MSG_ID_COM_INIT_FB",                    1, 1, 1},
  {MSG_ID_COM_RESTART,                            "MSG_ID_COM_RESTART",                    1, 1, 1},
  {MSG_ID_COM_TIME_OUT,                           "MSG_ID_COM_TIME_OUT",                   1, 1, 1},
  {MSG_ID_COM_STOP,                               "MSG_ID_COM_STOP",                       1, 1, 1},
  {MSG_ID_COM_HEART_BEAT,                         "MSG_ID_COM_HEART_BEAT",                 1, 1, 1},
  {MSG_ID_COM_HEART_BEAT_FB,                      "MSG_ID_COM_HEART_BEAT_FB",              1, 1, 1},
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
  {MSG_ID_XXX_NULL,                               "MSG_ID_XXX_NULL",                       1, 1, 1},
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
  //VMFO                                                                                    
   {MSG_ID_VMFO_TIMER_1S_PERIOD,                  "MSG_ID_VMFO_TIMER_1S_PERIOD",            1, 1, 1},
  //ADC
  //LED
  //DIDO
  {MSG_ID_DIDO_CCL_SENSOR_STATUS_RESP,            "MSG_ID_DIDO_CCL_SENSOR_STATUS_RESP",     1, 1, 1},
  //{MSG_ID_DIDO_CCL_EVENT_LOCK_TRIGGER,            "MSG_ID_DIDO_CCL_EVENT_LOCK_TRIGGER",     1, 1, 1},
  //{MSG_ID_DIDO_CCL_EVENT_FAULT_TRIGGER,           "MSG_ID_DIDO_CCL_EVENT_FAULT_TRIGGER",    1, 1, 1},
  //{MSG_ID_DIDO_CCL_EVENT_STATUS_UPDATE,           "MSG_ID_DIDO_CCL_EVENT_STATUS_UPDATE",    1, 1, 1},
  {MSG_ID_DIDO_CCL_DOOR_OPEN_EVENT,               "MSG_ID_DIDO_CCL_DOOR_OPEN_EVENT",        1, 1, 1},
  {MSG_ID_DIDO_CCL_DOOR_CLOSE_EVENT,              "MSG_ID_DIDO_CCL_DOOR_CLOSE_EVENT",    1, 1, 1},
  //SPS                                                                                    
  {MSG_ID_SPS_L2FRAME_SEND,                       "MSG_ID_SPS_L2FRAME_SEND",                1, 1, 1},
  {MSG_ID_SPS_L2FRAME_RCV,                        "MSG_ID_SPS_L2FRAME_RCV",                 1, 1, 1},
  {MSG_ID_SPS_CCL_CLOUD_FB,                       "MSG_ID_SPS_CCL_CLOUD_FB",                1, 1, 1},
  {MSG_ID_SPS_CCL_SENSOR_STATUS_RESP,             "MSG_ID_SPS_CCL_SENSOR_STATUS_RESP",      1, 1, 1},
  {MSG_ID_SPS_CCL_EVENT_REPORT_CFM,               "MSG_ID_SPS_CCL_EVENT_REPORT_CFM",        1, 1, 1},
  {MSG_ID_SPS_CCL_FAULT_REPORT_CFM,               "MSG_ID_SPS_CCL_FAULT_REPORT_CFM",        1, 1, 1},
  {MSG_ID_SPS_CCL_CLOSE_REPORT_CFM,               "MSG_ID_SPS_CCL_CLOSE_REPORT_CFM",        1, 1, 1},
  //SPI
  {MSG_ID_SPI_L2FRAME_SEND,                       "MSG_ID_SPI_L2FRAME_SEND",                1, 1, 1},
  {MSG_ID_SPI_L2FRAME_RCV,                        "MSG_ID_SPI_L2FRAME_RCV",                 1, 1, 1},
  //I2C
  {MSG_ID_I2C_L2FRAME_SEND,                       "MSG_ID_I2C_L2FRAME_SEND",                1, 1, 1},
  {MSG_ID_I2C_L2FRAME_RCV,                        "MSG_ID_I2C_L2FRAME_RCV",                 1, 1, 1},
  {MSG_ID_I2C_CCL_SENSOR_STATUS_RESP,             "MSG_ID_I2C_CCL_SENSOR_STATUS_RESP",      1, 1, 1},
  //CAN
  {MSG_ID_CAN_L2FRAME_SEND,                       "MSG_ID_CAN_L2FRAME_SEND",                1, 1, 1},
  {MSG_ID_CAN_L2FRAME_RCV,                        "MSG_ID_CAN_L2FRAME_RCV",                 1, 1, 1},
  //DCMI
  {MSG_ID_DCMI_CCL_SENSOR_STATUS_RESP,            "MSG_ID_DCMI_CCL_SENSOR_STATUS_RESP",     1, 1, 1},
  //CCL
  {MSG_ID_CCL_PERIOD_REPORT_TRIGGER,        			"MSG_ID_CCL_PERIOD_REPORT_TRIGGER", 			1, 1, 1},
  {MSG_ID_CCL_FAULT_STATE_TRIGGER,                "MSG_ID_CCL_FAULT_STATE_TRIGGER",         1, 1, 1},
  {MSG_ID_CCL_HAND_ACTIVE_TRIGGER,                "MSG_ID_CCL_HAND_ACTIVE_TRIGGER",         1, 1, 1},
  {MSG_ID_CCL_SPS_OPEN_AUTH_INQ,                  "MSG_ID_CCL_SPS_OPEN_AUTH_INQ",           1, 1, 1},
  {MSG_ID_CCL_COM_SENSOR_STATUS_REQ,              "MSG_ID_CCL_COM_SENSOR_STATUS_REQ",       1, 1, 1},
  //{MSG_ID_CCL_COM_CTRL_CMD,                       "MSG_ID_CCL_COM_CTRL_CMD",                1, 1, 1},
  {MSG_ID_CCL_DIDO_DOOR_OPEN_REQ,                	"MSG_ID_CCL_DIDO_DOOR_OPEN_REQ",        	1, 1, 1},
  {MSG_ID_CCL_DIDO_DOOR_CLOSE_REQ,                "MSG_ID_CCL_DIDO_DOOR_CLOSE_REQ",         1, 1, 1},
  {MSG_ID_CCL_SPS_EVENT_REPORT_SEND,              "MSG_ID_CCL_SPS_EVENT_REPORT_SEND",       1, 1, 1},
  {MSG_ID_CCL_SPS_FAULT_REPORT_SEND,              "MSG_ID_CCL_SPS_FAULT_REPORT_SEND",       1, 1, 1},
  {MSG_ID_CCL_SPS_CLOSE_REPORT_SEND,              "MSG_ID_CCL_SPS_CLOSE_REPORT_SEND",       1, 1, 1},
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
  //VMFO                                                                                 
  {MSG_ID_VMFO_TIMER_1S_PERIOD,                   "MSG_ID_VMFO_TIMER_1S_PERIOD",            1, 1, 1},
  //ADC                                                                                    
  {MSG_ID_ADC_NEW_MATERIAL_WS,                    "MSG_ID_ADC_NEW_MATERIAL_WS",             1, 1, 1},
  {MSG_ID_ADC_MATERIAL_DROP,                      "MSG_ID_ADC_MATERIAL_DROP",               1, 1, 1},
  {MSG_ID_ADC_L3BFSC_MEAS_CMD_RESP,               "MSG_ID_ADC_L3BFSC_MEAS_CMD_RESP",        1, 1, 1},
  //LED                                                                                    
  //UART                                                                                  
  {MSG_ID_SPS_L2FRAME_SEND,                       "MSG_ID_SPS_L2FRAME_SEND",                1, 1, 1},
  {MSG_ID_SPS_L2FRAME_RCV,                        "MSG_ID_SPS_L2FRAME_RCV",                 1, 1, 1},
  //SPI                                                                                    
  {MSG_ID_SPI_L2FRAME_SEND,                       "MSG_ID_SPI_L2FRAME_SEND",                1, 1, 1},
  {MSG_ID_SPI_L2FRAME_RCV,                        "MSG_ID_SPI_L2FRAME_RCV",                 1, 1, 1},
  //I2C                                                                                    
  {MSG_ID_I2C_L2FRAME_SEND,                       "MSG_ID_I2C_L2FRAME_SEND",                1, 1, 1},
  {MSG_ID_I2C_L2FRAME_RCV,                        "MSG_ID_I2C_L2FRAME_RCV",                 1, 1, 1},
  {MSG_ID_I2C_L3BFSC_MOTO_CMD_RESP,               "MSG_ID_I2C_L3BFSC_MOTO_CMD_RESP",        1, 1, 1},
  //CAN                                                                                    
  {MSG_ID_CAN_L2FRAME_SEND,                       "MSG_ID_CAN_L2FRAME_SEND",                1, 1, 1},
  {MSG_ID_CAN_L2FRAME_RCV,                        "MSG_ID_CAN_L2FRAME_RCV",                 1, 1, 1},
  {MSG_ID_CAN_L3BFSC_CMD_CTRL,                    "MSG_ID_CAN_L3BFSC_CMD_CTRL",             1, 1, 1},
  {MSG_ID_CAN_L3BFSC_INIT_REQ,                    "MSG_ID_CAN_L3BFSC_INIT_REQ",             1, 1, 1},
  {MSG_ID_CAN_L3BFSC_ROLL_OUT_REQ,                "MSG_ID_CAN_L3BFSC_ROLL_OUT_REQ",         1, 1, 1},
  {MSG_ID_CAN_L3BFSC_GIVE_UP_REQ,                 "MSG_ID_CAN_L3BFSC_GIVE_UP_REQ",          1, 1, 1},
  {MSG_ID_CAN_ADC_WS_MAN_SET_ZERO,                "MSG_ID_CAN_ADC_WS_MAN_SET_ZERO",         1, 1, 1},
  //L3BFSC                                                                                 
  {MSG_ID_L3BFSC_CAN_INIT_RESP,                   "MSG_ID_L3BFSC_CAN_INIT_RESP",            1, 1, 1},
  {MSG_ID_L3BFSC_CAN_NEW_WS_EVENT,                "MSG_ID_L3BFSC_CAN_NEW_WS_EVENT",         1, 1, 1},
  {MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP,               "MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP",        1, 1, 1},
  {MSG_ID_L3BFSC_CAN_GIVE_UP_RESP,                "MSG_ID_L3BFSC_CAN_GIVE_UP_RESP",         1, 1, 1},
  {MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT,         "MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT",  1, 1, 1},
  {MSG_ID_L3BFSC_ADC_WS_CMD_CTRL,                 "MSG_ID_L3BFSC_ADC_WS_CMD_CTRL",          1, 1, 1},
  {MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL,               "MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL",        1, 1, 1},
  {MSG_ID_L3BFSC_CAN_CMD_RESP,                    "MSG_ID_L3BFSC_CAN_CMD_RESP",             1, 1, 1},
  //WMC <-> AWS //MYC                                                                      
  {MSG_ID_L3BFSC_WMC_STARTUP_IND,                 "MSG_ID_L3BFSC_WMC_STARTUP_IND",          1, 1, 1},
  {MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,              "MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ",       1, 1, 1},
  {MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP,             "MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP",      1, 1, 1},
  {MSG_ID_L3BFSC_WMC_GET_CONFIG_REQ,              "MSG_ID_L3BFSC_WMC_GET_CONFIG_REQ",       1, 1, 1},
  {MSG_ID_L3BFSC_WMC_GET_CONFIG_RESP,             "MSG_ID_L3BFSC_WMC_GET_CONFIG_RESP",      1, 1, 1},
  {MSG_ID_L3BFSC_WMC_START_REQ,                   "MSG_ID_L3BFSC_WMC_START_REQ",            1, 1, 1},
  {MSG_ID_L3BFSC_WMC_START_RESP,                  "MSG_ID_L3BFSC_WMC_START_RESP",           1, 1, 1},
  {MSG_ID_L3BFSC_WMC_STOP_REQ,                    "MSG_ID_L3BFSC_WMC_STOP_REQ",             1, 1, 1},
  {MSG_ID_L3BFSC_WMC_STOP_RESP,                   "MSG_ID_L3BFSC_WMC_STOP_RESP",            1, 1, 1},
  {MSG_ID_L3BFSC_WMC_WEIGHT_IND,                  "MSG_ID_L3BFSC_WMC_WEIGHT_IND",           1, 1, 1},
  {MSG_ID_L3BFSC_WMC_COMBIN_REQ,                  "MSG_ID_L3BFSC_WMC_COMBIN_REQ",           1, 1, 1},
  {MSG_ID_L3BFSC_WMC_COMBIN_RESP,                 "MSG_ID_L3BFSC_WMC_COMBIN_RESP",          1, 1, 1},
  {MSG_ID_L3BFSC_WMC_FAULT_IND,                   "MSG_ID_L3BFSC_WMC_FAULT_IND",            1, 1, 1},
  {MSG_ID_L3BFSC_WMC_COMMAND_REQ,                 "MSG_ID_L3BFSC_WMC_COMMAND_REQ",          1, 1, 1},
  {MSG_ID_L3BFSC_WMC_COMMAND_RESP,                "MSG_ID_L3BFSC_WMC_COMMAND_RESP",         1, 1, 1},
#else
  #error Un-correct constant definition
#endif
	//END
  {MSG_ID_COM_MAX,                                "MSG_ID_COM_MAX",                         1, 1, 1}, 	   //END     
};                                                                                         

//消息ID的定义全局表，方便TRACE函数使用
//请服从IHU_SYSDIM_TIMERID_NAME_LEN_MAX的最长长度，否则出错
//设置的技巧是：基础部分不要动，配置不一样的，请单独在分项目中，再设置一遍
IhuSysEngTimerStaticCfg_t zIhuSysEngTimerStaticCfg[] ={
  //TIMER_ID                                       定时器名字                                定时长度 精度                      注释
	//STARING公共基础部分，不要轻易改动，可能会影响其它项目！！！各个项目重复的定时器且不同数值部分，
	//可以在分项目中第二次进行设置，后一次设置的数据将覆盖前一次的数值
  {TIMER_ID_MIN,                                   "TID_MIN",                                0,       TIMER_RESOLUTION_1S},    //STARTING
  {TIMER_ID_1S_MIN,                                "TID_1S_MIN",                             0,       TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_VMFO_INIT_FB,                       "TID_1S_VMFO_INIT_FB",                    0,       TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_VMFO_PERIOD_SCAN,                   "TID_1S_VMFO_PERIOD_SCAN",                10,      TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_ADCLIBRA_PERIOD_SCAN,               "TID_1S_ADCLIBRA_PERIOD_SCAN",            300,     TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_DIDOCAP_PERIOD_SCAN,                "TID_1S_DIDOCAP_PERIOD_SCAN",             200,     TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_ETHORION_PERIOD_SCAN,               "TID_1S_ETHORION_PERIOD_SCAN",            200,     TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_CANVELA_PERIOD_SCAN,                "TID_1S_CANVELA_PERIOD_SCAN",             200,     TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_I2CARIES_PERIOD_SCAN,               "TID_1S_I2CARIES_PERIOD_SCAN",            200,     TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_LEDPISCES_PERIOD_SCAN,              "TID_1S_LEDPISCES_PERIOD_SCAN",           200,     TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_LEDPISCES_GALOWAG_SCAN,             "TID_1S_LEDPISCES_GALOWAG_SCAN",          1,       TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_PWMTAURUS_PERIOD_SCAN,              "TID_1S_PWMTAURUS_PERIOD_SCAN",           200,     TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_SPILEO_PERIOD_SCAN,                 "TID_1S_SPILEO_PERIOD_SCAN",              200,     TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_SPSVIRGO_PERIOD_SCAN,               "TID_1S_SPSVIRGO_PERIOD_SCAN",            200,     TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_DCMIARIS_PERIOD_SCAN,               "TID_1S_DCMIARIS_PERIOD_SCAN",            200,     TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_MAX,                                "TID_1S_MAX",                             0,       TIMER_RESOLUTION_1S}, 
  {TIMER_ID_10MS_MIN,                              "TID_10MS_MIN",                           0,       TIMER_RESOLUTION_10MS},
  {TIMER_ID_10MS_VMFO_TEST,                        "TID_10MS_VMFO_TEST",                     0,       TIMER_RESOLUTION_10MS},
  {TIMER_ID_10MS_MAX,                              "TID_10MS_MAX",                           0,       TIMER_RESOLUTION_10MS},
  {TIMER_ID_1MS_MIN,                               "TID_1MS_MIN",                            0,       TIMER_RESOLUTION_1MS},
  {TIMER_ID_1MS_VMFO_TEST,                         "TID_1MS_VMFO_TEST",                      1000,    TIMER_RESOLUTION_1MS},
  {TIMER_ID_1MS_MAX,                               "TID_1MS_MAX",                            0,       TIMER_RESOLUTION_1MS},
	//END基础部分结束
	//以下为分项目部分
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
  {TIMER_ID_1S_EMC68X_PERIOD_SCAN,                 "TID_1S_EMC68X_PERIOD_SCAN",              10,      TIMER_RESOLUTION_1S},
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
  {TIMER_ID_1S_CCL_PERIOD_SCAN,                    "TID_1S_CCL_PERIOD_SCAN",                 5,       TIMER_RESOLUTION_1S},
	//G502设计使用RTC，而非CPU定时器来触发周期性汇报
  //{TIMER_ID_1S_CCL_EVENT_REPORT_PEROID_SCAN,       "TID_1S_CCL_EVENT_REPORT_PEROID_SCAN",    60,     TIMER_RESOLUTION_1S},  //为了测试目的，改为60秒，实际8小时
  {TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE,               "TID_1S_CCL_LOCK_WORK_ACTIVE",            268,     TIMER_RESOLUTION_1S},  
  {TIMER_ID_1S_CCL_LOCK_WORK_WAIT_TO_OPEN,         "TID_1S_CCL_LOCK_WORK_WAIT_TO_OPEN",      30,      TIMER_RESOLUTION_1S},  
  //{TIMER_ID_1S_CCL_DIDO_TRIGGER_PERIOD_SCAN,       "TID_1S_CCL_DIDO_TRIGGER_PERIOD_SCAN",    5,       TIMER_RESOLUTION_1S},  
  //{TIMER_ID_1S_CCL_DIDO_WORKING_PERIOD_SCAN,       "TID_1S_CCL_DIDO_WORKING_PERIOD_SCAN",    3,       TIMER_RESOLUTION_1S},  
  //{TIMER_ID_1S_CCL_SPS_WORKING_PERIOD_SCAN,        "TID_1S_CCL_SPS_WORKING_PERIOD_SCAN",     3,       TIMER_RESOLUTION_1S},  
  //{TIMER_ID_1S_CCL_I2C_WORKING_PERIOD_SCAN,        "TID_1S_CCL_I2C_WORKING_PERIOD_SCAN",     3,       TIMER_RESOLUTION_1S},  
  //{TIMER_ID_1S_CCL_DCMI_WORKING_PERIOD_SCAN,       "TID_1S_CCL_DCMI_WORKING_PERIOD_SCAN",    3,       TIMER_RESOLUTION_1S},  
  //{TIMER_ID_1S_CCL_ADC_WORKING_PERIOD_SCAN,        "TID_1S_CCL_ADC_WORKING_PERIOD_SCAN",     3,       TIMER_RESOLUTION_1S},  
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID) 
  {TIMER_ID_1S_LEDPISCES_PERIOD_SCAN,              "TID_1S_LEDPISCES_PERIOD_SCAN",           4,       TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_BFSC_PERIOD_SCAN,                   "TID_1S_BFSC_PERIOD_SCAN",                5,       TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER,      "TID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER",   25,      TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER,         "TID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER",      5,       TIMER_RESOLUTION_1S},
  {TIMER_ID_1S_BFSC_L3BFSC_GIVE_UP_TIMER,          "TID_1S_BFSC_L3BFSC_GIVE_UP_TIMER",       5,       TIMER_RESOLUTION_1S},
  {TIMER_ID_10MS_BFSC_ADCLIBRA_SCAN_TIMER,         "TID_10MS_BFSC_ADCLIBRA_SCAN_TIMER",      25,      TIMER_RESOLUTION_10MS}, 
#else
#endif  
	//END FLAG
  {TIMER_ID_MAX,                                   "TID_MAX",                                0,       TIMER_RESOLUTION_1S},   //END
};



/*
 *
 *
 *   VM核心函数，提供全局服务
 *
 *
 */

/*******************************************************************************
**
**	全局公用的API函数，不依赖于任何操作系统，只依赖于POSIX支持的标准函数集
**
**  已经使用了线程安全的方法，采用的是MUTEX互锁机制，确保不同打印之间调用的不相关性和单入性
**
**********************************************************************************/
//正常打印
void IhuDebugPrintFo(UINT8 index, char *format, ...)
{
	va_list marker;
	char strDebug[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX];
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	char *ptrPrintBuffer;
#endif
	//UINT8 index=0;

	//index = zIhuVmCtrTab.print.prtIndex;
	memset(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, 0, IHU_SYSDIM_PRINT_CHAR_SIZE_MAX);
	
	va_start(marker, format );
	vsnprintf(strDebug, IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-1, format, marker);
	va_end(marker);	

	//试图将实时时间打印出来，可惜没成功
////在CCL项目中，已经打开了RTC，所以可以调用RTC函数输出真实的时间，其它项目可以类似干，如果没有打开，则依然可以继续使用编译时间
//#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
//	char ss[20];
//	sprintf(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, "%s, [DBG: ", zIhuVmCtrTab.print.prtBuf[index].PrintHeader);
//	memset(ss, 0, sizeof(ss));
//	ihu_l1hd_rtc_get_current_ymdhms_time(ss);
//	strcat(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, ss);
//	strcat(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, "] ");
//#endif
	sprintf(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, "%s, [DBG: %s, %s] ", zIhuVmCtrTab.print.prtBuf[index].PrintHeader, __DATE__, __TIME__);
	strncat(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, strDebug, IHU_SYSDIM_PRINT_CHAR_SIZE_MAX - strlen(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer) - 1);

	// The trace is limited to 128 characters as defined at SYSDIM.H
	if( (zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-2] != 0) && (zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-1] != 0) )
	{
		zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-3] = '!';
		zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-2] = '\n';
		zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-1] = '\0';
	}
	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	printf("%s", ptrPrintBuffer);
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	/* 互斥信号量 */
	if (OS_MUTEX_GET(zIhuVmCtrTab.print.prtMutex, IHU_SYSDIM_PRINT_MUTEX_TIME_OUT_DUR) != OS_MUTEX_TAKEN){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		return;
	}
	ihu_l1hd_sps_print_send_data(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer);
	OS_MUTEX_PUT(zIhuVmCtrTab.print.prtMutex);
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	if (OS_MUTEX_GET(zIhuVmCtrTab.print.prtMutex, IHU_SYSDIM_PRINT_MUTEX_TIME_OUT_DUR) != OS_MUTEX_TAKEN){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		return;
	}
	ihu_l1hd_sps_print_send_data(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer);
	OS_MUTEX_PUT(zIhuVmCtrTab.print.prtMutex);	
#else
	#error Un-correct constant definition
#endif
	
	zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[0] = '\0';
}

//错误打印
void IhuErrorPrintFo(UINT8 index, char *format, ...)
{
	va_list marker;
	char strDebug[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX];
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
  char *ptrPrintBuffer;
#endif
	//UINT8 index=0;

	va_start(marker, format );
	vsnprintf(strDebug, IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-1, format, marker);
	va_end(marker);	
	
	//index = zIhuVmCtrTab.print.prtIndex;
	memset(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, 0, IHU_SYSDIM_PRINT_CHAR_SIZE_MAX);

	//试图将实时时间打印出来，可惜没成功
//#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
//	char ss[20];
//	sprintf(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, "%s, [ERR: ", zIhuVmCtrTab.print.prtBuf[index].PrintHeader);
//	memset(ss, 0, sizeof(ss));
//	ihu_l1hd_rtc_get_current_ymdhms_time(ss);
//	strcat(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, ss);
//	strcat(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, "] ");
//#endif		
	sprintf(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, "%s, [ERR: %s, %s] ", zIhuVmCtrTab.print.prtBuf[index].PrintHeader, __DATE__, __TIME__);
	strncat(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, strDebug, IHU_SYSDIM_PRINT_CHAR_SIZE_MAX - strlen(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer) - 1);

	// The trace is limited to 128 characters as defined at SYSDIM.H
	if( (zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-2] != 0) && (zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-1] != 0) )
	{
		zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-3] = '!';
		zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-2] = '\n';
		zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-1] = '\0';
	}
	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	printf("%s", ptrPrintBuffer);
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)	
	if (OS_MUTEX_GET(zIhuVmCtrTab.print.prtMutex, IHU_SYSDIM_PRINT_MUTEX_TIME_OUT_DUR) != OS_MUTEX_TAKEN){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		return;
	}
	ihu_l1hd_sps_print_send_data(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer);
	OS_MUTEX_PUT(zIhuVmCtrTab.print.prtMutex);
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	if (OS_MUTEX_GET(zIhuVmCtrTab.print.prtMutex, IHU_SYSDIM_PRINT_MUTEX_TIME_OUT_DUR) != OS_MUTEX_TAKEN){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		return;
	}
	ihu_l1hd_sps_print_send_data(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer);
	OS_MUTEX_PUT(zIhuVmCtrTab.print.prtMutex);		
#else
	#error Un-correct constant definition
#endif
	
	zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[0] = '\0';
}

//独特的ID打印
UINT8 IhuDebugPrintId(char *file, int line)
{
	char strLine[9];
	UINT8 index=0;
	
	zIhuVmCtrTab.print.prtIndex++;
	if (zIhuVmCtrTab.print.prtIndex >= IHU_SYSDIM_PRINT_BUF_NBR_MAX) zIhuVmCtrTab.print.prtIndex = 0;
	index = zIhuVmCtrTab.print.prtIndex;
	memset(zIhuVmCtrTab.print.prtBuf[index].PrintHeader, 0, IHU_SYSDIM_PRINT_FILE_LINE_NBR_MAX);
	
	snprintf(zIhuVmCtrTab.print.prtBuf[index].PrintHeader, 50, "[%s]", file);
	sprintf(strLine, "[%6d]", line);
	strcat(zIhuVmCtrTab.print.prtBuf[index].PrintHeader, strLine);
	return index;
}

void IhuDebugPrintFoEmc68x(char *format, ...)
{
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
        va_list marker;
        char strDebug[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX];

        char *ptrPrintBuffer;
        UINT8 index=0;

        //index = zIhuVmCtrTab.print.prtIndex;
        memset(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, 0, IHU_SYSDIM_PRINT_CHAR_SIZE_MAX);

        va_start(marker, format );
        vsnprintf(strDebug, IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-1, format, marker);
        va_end(marker);

        sprintf(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, "%s, [DBG: %s, %s] ", zIhuVmCtrTab.print.prtBuf[index].PrintHeader, __DATE__, __TIME__);
        strncat(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, strDebug, IHU_SYSDIM_PRINT_CHAR_SIZE_MAX - strlen(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer) - 1);

        // The trace is limited to 128 characters as defined at SYSDIM.H
        if( (zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-2] != 0) && (zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-1] != 0) )
        {
                zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-3] = '!';
                zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-2] = '\n';
                zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-1] = '\0';
        }

        printf("%s", ptrPrintBuffer);
        zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[0] = '\0';
#endif

}

//错误打印
void IhuErrorPrintFoEmc68x(char *format, ...)
{
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
        va_list marker;
        char strDebug[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX];
        char *ptrPrintBuffer;
        UINT8 index=0;

        va_start(marker, format );
        vsnprintf(strDebug, IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-1, format, marker);
        va_end(marker);

        //index = zIhuVmCtrTab.print.prtIndex;
        memset(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, 0, IHU_SYSDIM_PRINT_CHAR_SIZE_MAX);

        sprintf(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, "%s, [ERR: %s, %s] ", zIhuVmCtrTab.print.prtBuf[index].PrintHeader, __DATE__, __TIME__);
        strncat(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer, strDebug, IHU_SYSDIM_PRINT_CHAR_SIZE_MAX - strlen(zIhuVmCtrTab.print.prtBuf[index].PrintBuffer) - 1);

        // The trace is limited to 128 characters as defined at SYSDIM.H
        if( (zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-2] != 0) && (zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-1] != 0) )
        {
                zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-3] = '!';
                zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-2] = '\n';
                zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX-1] = '\0';
        }

        printf("%s", ptrPrintBuffer);
        zIhuVmCtrTab.print.prtBuf[index].PrintBuffer[0] = '\0';
#endif

}


//API abstract
//这里采用系统提供的时钟函数
void ihu_sleep(UINT32 second)
{
	if (second <= (UINT32)0) second =0;
	if (second >= IHU_SYSDIM_SLEEP_UP_LIMIT_CNT_MAX) second = IHU_SYSDIM_SLEEP_UP_LIMIT_CNT_MAX;
	second = second*1000;
	OS_DELAY_MS(second);
}

void ihu_usleep(UINT32 usecond)
{
	if (usecond <= 0) usecond =0;
	if (usecond >= IHU_SYSDIM_SLEEP_UP_LIMIT_CNT_MAX) usecond = IHU_SYSDIM_SLEEP_UP_LIMIT_CNT_MAX;
	OS_DELAY_MS(usecond);
}



/*
 *
 *
 *   VM系统初始化函数
 *
 *
 */


//INIT the whole system
void ihu_vm_system_ctr_table_init(void)
{	
	int i = 0, item = 0;
	UINT8 taskid = 0, msgid = 0, tid = 0;
	/*
	 *  SYSTEM DIMENSION检查：边界在IHU_SYSDIM_TASK_NBR_MAX-1，注意！
	 */

	if ((TASK_ID_MAX >= IHU_SYSDIM_TASK_NBR_MAX) || (MSG_ID_COM_MAX >= IHU_SYSDIM_MSGID_NBR_MAX) || (TIMER_ID_MAX >= IHU_SYSDIM_TIMERID_NBR_MAX)){
		IhuErrorPrint("VMFO: Initialize failure, configuration of system dimension error!\n");
		return;
	}	
	
	/*
	 *  初始化三表
	 */

	memset(&zIhuVmCtrTab, 0, sizeof(IhuVmCtrTab_t));
	memset(&zIhuSysEngPar, 0, sizeof(IhuSysEngParTab_t));
	memset(&zIhuSysStaPm, 0, sizeof(IhuSysStaPm_t));
	 	

	/*
	 *  初始化打印缓冲区 
	 */

	//采用OS_MUTEX_CREATE居然不成功，怪哉。。。
	//OS_MUTEX_CREATE(zIhuVmCtrTab.print.prtMutex);
	zIhuVmCtrTab.print.prtMutex = xSemaphoreCreateRecursiveMutex();
	if (zIhuVmCtrTab.print.prtMutex == NULL){
		IhuErrorPrint("VMFO: Initialize VMFO failure, not continue any more...!\n");
		return;
	}

	/*
	 *  INIT IHU itself
	 */

	IhuDebugPrint("VMFO: User task starting, compiled load Info: CURRENT_PRJ=[%s], PRODUCT_CAT=[0x%x], HW_TYPE=[%d], SW_REL=[%d], SW_DELIVER=[%d].\n", IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT, \
		IHU_SYSCFG_HW_PRODUCT_CAT_TYPE, IHU_CURRENT_HW_TYPE, IHU_CURRENT_SW_RELEASE, IHU_CURRENT_SW_DELIVERY);
	IhuDebugPrint("VMFO: BXXH(TM) IHU(c) Application Layer start and initialized, build at %s, %s.\n", __DATE__, __TIME__);
	//初始化全局变量TASK_ID/QUE_ID/TASK_STAT
	for (i=TASK_ID_MIN; i<=TASK_ID_MAX; i++){
		zIhuVmCtrTab.task[i].TaskId = i;
		zIhuVmCtrTab.task[i].pnpState = IHU_SYSCFG_TASK_PNP_OFF;
	}

	/*
	 *  扫描TASKID的静态配置
	 */

	//起始必须是TASK_ID_MIN条目
	if (zIhuVmCtrTaskStaticCfg[0].taskInputId != TASK_ID_MIN){
		IhuErrorPrint("VMFO: Initialize VMFO failure, task input configuration error!\n");
		return;
	}
	strcpy(zIhuVmCtrTab.task[TASK_ID_MIN].taskName, zIhuVmCtrTaskStaticCfg[0].taskInputName);
	//以TASK_ID_MAX为终止条目：最大为2倍，因为初始化表单中出现了两次，一次赋基础，一次控制激活与否
	for(item=1; item <= (2*IHU_SYSDIM_TASK_NBR_MAX); item++)
	{
		if(zIhuVmCtrTaskStaticCfg[item].taskInputId == TASK_ID_MAX)
		{
			break;
		}
		if ((zIhuVmCtrTaskStaticCfg[item].taskInputId <= TASK_ID_MIN) || (zIhuVmCtrTaskStaticCfg[item].taskInputId > TASK_ID_MAX)){
			IhuErrorPrint("VMFO: Initialize VMFO failure, task static input configuration error!\n");
			return;			
		}		
		if (zIhuVmCtrTaskStaticCfg[item].fsmFuncEntry == NULL){
			IhuErrorPrint("VMFO: Initialize VMFO failure, task static input configuration error!\n");
			return;			
		}
		if ((zIhuVmCtrTaskStaticCfg[item].pnpFlag != IHU_SYSCFG_TASK_PNP_ON) && (zIhuVmCtrTaskStaticCfg[item].pnpFlag != IHU_SYSCFG_TASK_PNP_OFF)){
			IhuErrorPrint("VMFO: Initialize VMFO failure, task static input configuration error!\n");
			return;
		}
	}
	//从任务配置输入区域读取参数到系统任务表，一旦遇到TASK_ID_MAX就终止
	item = 0;
	while(zIhuVmCtrTaskStaticCfg[item].taskInputId != TASK_ID_MAX){
		taskid = zIhuVmCtrTaskStaticCfg[item].taskInputId;
		zIhuVmCtrTab.task[taskid].pnpState = zIhuVmCtrTaskStaticCfg[item].pnpFlag;
		strcpy(zIhuVmCtrTab.task[taskid].taskName, zIhuVmCtrTaskStaticCfg[item].taskInputName);
		zIhuVmCtrTab.task[taskid].taskFuncEntry = zIhuVmCtrTaskStaticCfg[item].fsmFuncEntry;
		zIhuSysEngPar.traceList.mod[taskid].moduleId = taskid;
		strcpy(zIhuSysEngPar.traceList.mod[taskid].moduleName, zIhuVmCtrTaskStaticCfg[item].taskInputName);
		zIhuSysEngPar.traceList.mod[taskid].moduleCtrFlag = zIhuVmCtrTaskStaticCfg[item].traceCtrFlag;
		zIhuSysEngPar.traceList.mod[taskid].moduleToAllow = zIhuVmCtrTaskStaticCfg[item].traceModToAllowFlag;
		zIhuSysEngPar.traceList.mod[taskid].moduleToRestrict = zIhuVmCtrTaskStaticCfg[item].traceModToRestrictFlag;
		zIhuSysEngPar.traceList.mod[taskid].moduleFromAllow = zIhuVmCtrTaskStaticCfg[item].traceModFromAllowFlag;
		zIhuSysEngPar.traceList.mod[taskid].moduleFromRestrict = zIhuVmCtrTaskStaticCfg[item].traceModFromRestrictFlag;
		item++;
	}
	//最后一项必定是TASK_ID_MAX
	strcpy(zIhuVmCtrTab.task[TASK_ID_MAX].taskName, zIhuVmCtrTaskStaticCfg[item].taskInputName);
	taskid = zIhuVmCtrTaskStaticCfg[item].taskInputId;
	zIhuSysEngPar.traceList.mod[taskid].moduleId = TASK_ID_MAX;
	strcpy(zIhuSysEngPar.traceList.mod[taskid].moduleName, zIhuVmCtrTaskStaticCfg[item].taskInputName);
	zIhuSysEngPar.traceList.mod[taskid].moduleCtrFlag = zIhuVmCtrTaskStaticCfg[item].traceCtrFlag;
	zIhuSysEngPar.traceList.mod[taskid].moduleToAllow = zIhuVmCtrTaskStaticCfg[item].traceModToAllowFlag;
	zIhuSysEngPar.traceList.mod[taskid].moduleToRestrict = zIhuVmCtrTaskStaticCfg[item].traceModToRestrictFlag;
	zIhuSysEngPar.traceList.mod[taskid].moduleFromAllow = zIhuVmCtrTaskStaticCfg[item].traceModFromAllowFlag;
	zIhuSysEngPar.traceList.mod[taskid].moduleFromRestrict = zIhuVmCtrTaskStaticCfg[item].traceModFromRestrictFlag;

	
	/*
	 *  扫描MSGID的静态配置
	 */

	//起始必须是MSG_ID_COM_MIN条目
	if (zIhuVmCtrMsgStaticCfg[0].msgId != MSG_ID_COM_MIN){
		IhuErrorPrint("VMFO: Initialize VMFO failure, task input configuration error!\n");
		return;
	}
	//以MSG_ID_COM_MAX为终止条目：2倍最大配置数量
	for(item=1; item < (2*IHU_SYSDIM_MSGID_NBR_MAX); item++)
	{
		if(zIhuVmCtrMsgStaticCfg[item].msgId == MSG_ID_COM_MAX)
		{
			break;
		}
		if ((zIhuVmCtrMsgStaticCfg[item].msgId <= MSG_ID_COM_MIN) || (zIhuVmCtrMsgStaticCfg[item].msgId > MSG_ID_COM_MAX)){
			IhuErrorPrint("VMFO: Initialize VMFO failure, msg static input configuration error!\n");
			return;			
		}
	}
	//从消息配置输入区域读取参数到系统任务表，一旦遇到MSG_ID_COM_MAX就终止
	item = 0;
	while(zIhuVmCtrMsgStaticCfg[item].msgId != MSG_ID_COM_MAX){
		msgid = zIhuVmCtrMsgStaticCfg[item].msgId;
		zIhuSysEngPar.traceList.msg[msgid].msgId = msgid;
		strcpy(zIhuSysEngPar.traceList.msg[msgid].msgName, zIhuVmCtrMsgStaticCfg[item].msgName);
		zIhuSysEngPar.traceList.msg[msgid].msgCtrFlag = zIhuVmCtrMsgStaticCfg[item].traceCtrFlag;
		zIhuSysEngPar.traceList.msg[msgid].msgAllow = zIhuVmCtrMsgStaticCfg[item].traceMsgAllowFlag;
		zIhuSysEngPar.traceList.msg[msgid].msgRestrict = zIhuVmCtrMsgStaticCfg[item].traceMsgRestrictFlag;
		item++;
	}
	//最后一项必定是MSG_ID_COM_MAX
	msgid = zIhuVmCtrMsgStaticCfg[item].msgId;
	zIhuSysEngPar.traceList.msg[msgid].msgId = msgid;
	strcpy(zIhuSysEngPar.traceList.msg[msgid].msgName, zIhuVmCtrMsgStaticCfg[item].msgName);
	zIhuSysEngPar.traceList.msg[msgid].msgCtrFlag = zIhuVmCtrMsgStaticCfg[item].traceCtrFlag;
	zIhuSysEngPar.traceList.msg[msgid].msgAllow = zIhuVmCtrMsgStaticCfg[item].traceMsgAllowFlag;
	zIhuSysEngPar.traceList.msg[msgid].msgRestrict = zIhuVmCtrMsgStaticCfg[item].traceMsgRestrictFlag;


	/*
	 *  扫描TIMERID的静态配置
	 */

	//起始必须是TIMER_ID_MIN条目
	if (zIhuSysEngTimerStaticCfg[0].timerId != TIMER_ID_MIN){
		IhuErrorPrint("VMFO: Initialize VMFO failure, task input configuration error!\n");
		return;
	}
	//以TIMER_ID_MAX为终止条目：2倍最大配置数量
	for(item=1; item < (2*IHU_SYSDIM_TIMERID_NBR_MAX); item++)
	{
		if(zIhuSysEngTimerStaticCfg[item].timerId == TIMER_ID_MAX)
		{
			break;
		}
		if ((zIhuSysEngTimerStaticCfg[item].timerId <= TIMER_ID_MIN) || (zIhuSysEngTimerStaticCfg[item].timerId > TIMER_ID_MAX)){
			IhuErrorPrint("VMFO: Initialize VMFO failure, msg static input configuration error!\n");
			return;			
		}
	}
	//从消息配置输入区域读取参数到系统任务表，一旦遇到MSG_ID_COM_MAX就终止
	item = 0;
	while(zIhuSysEngTimerStaticCfg[item].timerId != TIMER_ID_MAX){
		tid = zIhuSysEngTimerStaticCfg[item].timerId;
		zIhuSysEngPar.timer.array[tid].id = tid;
		strcpy(zIhuSysEngPar.timer.array[tid].name, zIhuSysEngTimerStaticCfg[item].timerName);
		zIhuSysEngPar.timer.array[tid].gradunarity = zIhuSysEngTimerStaticCfg[item].timerGranularity;
		zIhuSysEngPar.timer.array[tid].dur = zIhuSysEngTimerStaticCfg[item].timerDur;
		item++;
	}
	//最后一项必定是TIMER_ID_MAX
		tid = zIhuSysEngTimerStaticCfg[item].timerId;
		zIhuSysEngPar.timer.array[tid].id = tid;
		strcpy(zIhuSysEngPar.timer.array[tid].name, zIhuSysEngTimerStaticCfg[item].timerName);
		zIhuSysEngPar.timer.array[tid].gradunarity = zIhuSysEngTimerStaticCfg[item].timerGranularity;
		zIhuSysEngPar.timer.array[tid].dur = zIhuSysEngTimerStaticCfg[item].timerDur;
	
	/*
	 *  Init Fsm
	 */

	FsmInit();

	/*
	 *  分项目对工程参数进行配置
	 */

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)	
	//烧录的过程，EMC68x是不需要的
	return;
#endif

	/*
	 *  工程参数初始化
	 */

	//初始化全局工参配置信息，这里给出了大部分用到的参数的初始化结构，以便未来可以更加方便的添加完善
	//后台通信部分
	zIhuSysEngPar.comm.commBackHawlCon = IHU_SYSCFG_COMM_BACK_HAWL_CON;
	//Series Port config
	zIhuSysEngPar.serialport.SeriesPortForGPS = IHU_SYSCFG_GPS_SERIES_PORT_DEFAULT;
	//后台部分
	strcpy(zIhuSysEngPar.cloud.cloudHttpAddLocal, IHU_SYSCFG_BH_HTTP_LOCAL_ADDR);
	strcpy(zIhuSysEngPar.cloud.cloudBhServerAddr, IHU_SYSCFG_BH_HTTP_SVR_ENTRY_ADDR);
	strcpy(zIhuSysEngPar.cloud.cloudBhServerName, IHU_SYSCFG_BH_HTTP_SVR_NAME);
	strcpy(zIhuSysEngPar.cloud.cloudBhFtpSvrAddr, IHU_SYSCFG_BH_FTP_SVR_ADDR);	
	//local SW storage address
	strcpy(zIhuSysEngPar.swDownload.ihuSwDownloadDir, IHU_SYSCFG_SW_DOWNLOAD_DIR_DEFAULT);
	strcpy(zIhuSysEngPar.swDownload.ihuSwActiveDir, IHU_SYSCFG_SW_ACTIVE_DIR_DEFAULT);
	strcpy(zIhuSysEngPar.swDownload.ihuSwBackupDir, IHU_SYSCFG_SW_BACKUP_DIR_DEFAULT);
	//DEBUG部分
	zIhuSysEngPar.debugMode = IHU_SYSCFG_TRACE_DEBUG_SET;
	//TRACE部分
	zIhuSysEngPar.traceMode = IHU_SYSCFG_TRACE_MSG_SET;
	IHU_DEBUG_PRINT_NOR("VMFO: Set basic engineering data correctly from SYSTEM DEFAULT parameters!\n");
	//后台协议接口
	zIhuSysEngPar.cloud.cloudBhItfFrameStd = IHU_SYSCFG_CLOUD_BH_ITF_STD_SET;
	

	/*
   *  烧录区信息读取
	 */
	
	//硬件烧录区域，系统唯一标识部分，后面程序中访问到这些系统参数都必须从这个地方读取
	ihu_l1hd_f2board_equid_get((UINT8*)&(zIhuSysEngPar.hwBurnId));
	//对硬件类型进行相同性检查，如果不一致，必然发生了生产性错误，或者硬件搞错，或者Factory Load用错，应该严重警告
	if ((IHU_SYSCFG_HW_MASSIVE_PRODUCTION_SET == IHU_SYSCFG_HW_MASSIVE_PRODUCTION_YES) && (zIhuSysEngPar.hwBurnId.hwType != IHU_SYSCFG_HW_PRODUCT_CAT_TYPE)){
		IhuDebugPrint("VMFO: Fatal error, using wrong hardware type or factory load!!!\n");
		return;
	}
	//由于硬件部分并没有真正起作用，所以暂时需要从系统定义区重复写入，一旦批量生产这部分可以去掉
	if (IHU_SYSCFG_HW_MASSIVE_PRODUCTION_SET == IHU_SYSCFG_HW_MASSIVE_PRODUCTION_NO){
		strncpy(zIhuSysEngPar.hwBurnId.equLable, IHU_SYSCFG_BH_HCU_NAME_SELF, (sizeof(IHU_SYSCFG_BH_HCU_NAME_SELF)<sizeof(zIhuSysEngPar.hwBurnId.equLable))?(sizeof(IHU_SYSCFG_BH_HCU_NAME_SELF)):(sizeof(zIhuSysEngPar.hwBurnId.equLable)));
		zIhuSysEngPar.hwBurnId.hwType = IHU_SYSCFG_HW_PRODUCT_CAT_TYPE;
		zIhuSysEngPar.hwBurnId.hwPemId = IHU_CURRENT_HW_TYPE; //PEM小型号
		zIhuSysEngPar.hwBurnId.swUpgradeFlag = IHU_SYSCFG_HDD_FW_UPGRADE_SET;
		zIhuSysEngPar.hwBurnId.swUpgPollId = IHU_SYSCFG_HBB_FW_UPGRADE_METHOD_UART_GPRS;
		zIhuSysEngPar.hwBurnId.bootIndex = 0; //研发永远从FACTORY LOAD启动
		zIhuSysEngPar.hwBurnId.bootAreaMax = 1; //研发状态下只有1个启动区
		zIhuSysEngPar.hwBurnId.facLoadAddr = IHU_SYSCFG_BOOT_FAC_LOAD_START_ADDRESS;
		zIhuSysEngPar.hwBurnId.facLoadSwRel = IHU_CURRENT_SW_RELEASE;
		zIhuSysEngPar.hwBurnId.facLoadSwVer = IHU_CURRENT_SW_DELIVERY;
		zIhuSysEngPar.hwBurnId.facLoadCheckSum = 0;
		zIhuSysEngPar.hwBurnId.facLoadValid = true;
		zIhuSysEngPar.hwBurnId.bootLoad1Addr = IHU_SYSCFG_BOOT_BLOCK1_LOAD_START_ADDRESS;
		zIhuSysEngPar.hwBurnId.bootLoad1Valid = false;
		zIhuSysEngPar.hwBurnId.bootLoad2Addr = IHU_SYSCFG_BOOT_BLOCK2_LOAD_START_ADDRESS;
		zIhuSysEngPar.hwBurnId.bootLoad2Valid = false;
		//cipherKey[16];
	}

	/*
   *  初始化结束
	 */
	
	//初始化之后的系统标识信息
	IhuDebugPrint("VMFO: Initialized Accomplish! Hardware Burn Physical Id/Address: CURRENT_PRJ=[%s], HW_LABLE=[%s], PRODUCT_CAT=[0x%x], HW_TYPE=[0x%x], SW_RELEASE_VER=[%d.%d], FW_UPGRADE_FLAG=[%d].\n", \
		IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT, \
		zIhuSysEngPar.hwBurnId.equLable, \
		zIhuSysEngPar.hwBurnId.hwType, \
		zIhuSysEngPar.hwBurnId.hwPemId, \
		zIhuSysEngPar.hwBurnId.swRelId, \
		zIhuSysEngPar.hwBurnId.swVerId, \
		zIhuSysEngPar.hwBurnId.swUpgradeFlag);	
	return;
}

//TaskId transfer to string
//正确的做法也许应该使用zIhuVmCtrTab.task[id].TaskName这个信息来表达
OPSTAT ihu_taskid_to_string(UINT8 id, char *string)
{
	char tmp[IHU_SYSDIM_TASK_NAME_LEN_MAX-2]="";
	
	if (id>=TASK_ID_MAX) //id<TASK_ID_MIN || 
		IHU_ERROR_PRINT_TASK(TASK_ID_VMFO, "VMFO: Error task Id input!\n");

	strcpy(string, "[");
	if (strlen(zIhuSysEngPar.traceList.mod[id].moduleName)>0){
		strncpy(tmp, zIhuSysEngPar.traceList.mod[id].moduleName, IHU_SYSDIM_TASK_NAME_LEN_MAX-3);
		strcat(string, tmp);
	}else{
		strcat(string, "TASK_ID_XXX");
	}
	strcat(string, "]");
	return IHU_SUCCESS;
}

//MsgId transfer to string
//输入的string参数，其内存地址空间和长度预留，是否足够
OPSTAT ihu_msgid_to_string(UINT8 id, char *string)
{
	char tmp[IHU_SYSDIM_MSGID_NAME_LEN_MAX-2]="";
	
	if (id < MSG_ID_COM_MIN || id >= MSG_ID_COM_MAX)
		IHU_ERROR_PRINT_TASK(TASK_ID_VMFO, "VMFO: Error Message Id input!\n");

	strcpy(string, "[");
	if (strlen(zIhuSysEngPar.traceList.msg[id].msgName)>0){
		strncpy(tmp, zIhuSysEngPar.traceList.msg[id].msgName, IHU_SYSDIM_MSGID_NAME_LEN_MAX-3);
		strcat(string, tmp);
	}else{
		strcat(string, "MSG_ID_XXX");
	}
	strcat(string, "]");

	return IHU_SUCCESS;
}

//TimerId transfer to string
//输入的string参数，其内存地址空间和长度预留，是否足够
OPSTAT ihu_timerid_to_string(UINT8 id, char *string)
{
	char tmp[IHU_SYSDIM_TIMERID_NAME_LEN_MAX-2]="";
	
	if (id <= TIMER_ID_MIN || id >= TIMER_ID_MAX)
		IHU_ERROR_PRINT_TASK(TASK_ID_VMFO, "VMFO: Error Timer Id input!\n");

	strcpy(string, "[");
	if (strlen(zIhuSysEngPar.timer.array[id].name)>0){
		strncpy(tmp, zIhuSysEngPar.timer.array[id].name, IHU_SYSDIM_TIMERID_NAME_LEN_MAX-3);
		strcat(string, tmp);
	}else{
		strcat(string, "TID_XXX");
	}
	strcat(string, "]");

	return IHU_SUCCESS;
}


/*******************************************************************************
**
**	全局公共的状态机机制，任务和队列相关的API, 不依赖于任何操作系统
**
**********************************************************************************/

/*******************************************************************************
**	状态机API
**
**	METHOD :FsmInit
**------------------------------------------------------------------------------
** Description :
**    Initialize the module of the fsm.
**------------------------------------------------------------------------------
** Arguments :
**    IN:	 none
**    OUT:   none
**    INOUT: none
**------------------------------------------------------------------------------
** Return value : IHU_SUCCESS OR IHU_FAILURE
*******************************************************************************/
OPSTAT FsmInit(void)
{
	UINT32 i;
	IhuDebugPrint("VMFO: >>Start init FSM.\n");
	zIhuVmCtrTab.fsm.numOfIhuFsmCtrlTable = 0;
	for(i=0; i<IHU_SYSDIM_TASK_NBR_MAX; i++)
	{
		zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[i].taskId = TASK_ID_INVALID;
		zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[i].numOfIhuFsmArrayElement = 0;
	}
	zIhuVmCtrTab.fsm.currentTaskId = TASK_ID_INVALID;

	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("VMFO: Maxium (%d) tasks/modules supported.\n", IHU_SYSDIM_TASK_NBR_MAX);
	}
    return IHU_SUCCESS;
}

/*******************************************************************************
** METHOD : FsmAddNew
**------------------------------------------------------------------------------
** Description :
**    fill the FSM table information of entity
**------------------------------------------------------------------------------
** Arguments :
**    IN:	 Taskid : entity identification
**			 pIhuFsmStateItem: pointer to message table of entity
**    OUT:   none
**    INOUT: none
**------------------------------------------------------------------------------
** Return value : IHU_SUCCESS OR IHU_FAILURE
*******************************************************************************/
OPSTAT FsmAddNew(UINT8 task_id, IhuFsmStateItem_t* pIhuFsmStateItem)
{
	OPSTAT ret;
	UINT32 state;
	UINT32 msgid;
	UINT32 item, itemNo, i, j;

	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		IhuDebugPrint("VMFO: >>Register new FSM. TaskId:(%d), pFsm(0x%x).\n", task_id, (UINT32)pIhuFsmStateItem);
	}
	/*
	** Check the array of the state machine.
	*/
	if( pIhuFsmStateItem == NULL)
	{
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Can not init FSM machine.\n");
		return IHU_FAILURE;
	}
	if((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX))
	{
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: The task_ID is invalid.\n");
		return IHU_FAILURE;
	}
	if( zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[task_id].taskId != TASK_ID_INVALID )
	{
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: This task_id has been already inited.\n");
		return IHU_FAILURE;
	}

	/*
	** Check the first entry. 最初一个FSM表单必须以FSM_STATE_ENTRY开始
	*/
	if( pIhuFsmStateItem[FSM_STATE_ENTRY].state != FSM_STATE_ENTRY )
	{
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: The format of the FSM table is invalid.\n");
		return IHU_FAILURE;
	}

    /*
    ** Check each item of the state entry. 最后一个FSM表单使用FSM_STATE_END结束
    */
	for(item=1; item < IHU_SYSDIM_FSM_ENTRY_NBR_MAX; item++)
	{
		if( FSM_STATE_END == pIhuFsmStateItem[item].state )
		{
			break;
		}
		if( FSM_STATE_ENTRY == pIhuFsmStateItem[item].state )
		{
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
			IhuErrorPrint("VMFO: Invalid FSM machine.\n");
			return IHU_FAILURE;
		}
		if( NULL == pIhuFsmStateItem[item].stateFunc)
		{
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
			IhuErrorPrint("VMFO: Invalid state function pointer.\n");
			return IHU_FAILURE;
		}
	}

	/*
	** Check the number of the item.
	** If we can not find the ENDING flag of the state machine after more than
	** IHU_SYSDIM_FSM_ENTRY_NBR_MAX entry, raise fatal alarm.
	*/
	if( IHU_SYSDIM_FSM_ENTRY_NBR_MAX == item)
	{
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Invalid FSM machine -- Can not find the end of the FSM.\n");
		return IHU_FAILURE;
	}
	zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[task_id].numOfIhuFsmArrayElement = item-1; //有效STATE-MSG条目数，不包括START/END两条

	/*
	**  Insert this new fsm item into fsm table.
	**  The position in the fsm table is based on the task_id.
	*/
	zIhuVmCtrTab.fsm.numOfIhuFsmCtrlTable++;

	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE)
	{
		IhuDebugPrint("VMFO: FsmAddNew: task_id = 0x%x [%s], src_id= %x, dest_id= %X.\n", task_id, zIhuVmCtrTab.task[task_id].taskName, 0, 0);
		IhuDebugPrint("VMFO: After add this one, Total (%d) FSM in the table.\n", zIhuVmCtrTab.fsm.numOfIhuFsmCtrlTable);
	}

	/*
	** Save the state machine info.
	*/
	zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[task_id].taskId = task_id;
	for(i=0; i<IHU_SYSDIM_TASK_STATE_NBR_MAX; i++)
	{
		for(j=0; j<IHU_SYSDIM_MSGID_NBR_MAX; j++)
		{
			zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[task_id].pFsmArray[i][j].stateFunc = NULL;
		}
	}

	//检查Msg_id的合法性，不能超过FF，这里很有技巧，全靠消息ID不超过FF来设定，小心消息过多的情况
	//内部消息也不能使用任意定义的数值，因为这里将采用MsgId进行索引，以提高效率
	for(itemNo=0; itemNo < item; itemNo++)
	{
		state = pIhuFsmStateItem[itemNo].state;
		if(state >= IHU_SYSDIM_TASK_STATE_NBR_MAX)
		{
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
			IhuErrorPrint("VMFO: The State number > %d.\n", IHU_SYSDIM_TASK_STATE_NBR_MAX);
			return IHU_FAILURE;
		}
		msgid = pIhuFsmStateItem[itemNo].msg_id & IHU_SYSDIM_MSGID_MASK_SET;
		zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[task_id].pFsmArray[state][msgid].stateFunc = pIhuFsmStateItem[itemNo].stateFunc;
	}

	/*
	** Start the task entry init api/function
	*/
	//Input message id = 0, param_ptr = NULL, param_len = 0.
    /*
    ** call the entry
    ** 这一块代码不能复用，因为这个任务和被执行任务并不再同一个空间之内，它只能被目标任务的入口函数进行执行
    ** 该函数API必须在每个任务中进行表达，否则一定会出错， 该代码需要移动到任务创建成功后的入口函数中进行调用
    */
	//(zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_COM_MIN].stateFunc)(task_id, 0, NULL, 0);

	/*
	** Set the state to init state(FSM_STATE_IDLE).
	** 一旦任务创建，自动转移到IDLE状态，因而每个任务独特的初始化不能处理状态，也不能被FsmRunEngine调度，而必须被任务创建的入口函数所执行
	*/
	ret = FsmSetState(task_id, FSM_STATE_IDLE);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error set FSM State!\n");
	}

    return IHU_SUCCESS;
}

OPSTAT FsmRemove(UINT8 task_id)
{
	//入参检查
	if((task_id <= (UINT8)TASK_ID_MIN) || (task_id >= TASK_ID_MAX))
	{
		IhuErrorPrint("VMFO: The task_ID is invalid.\n");
		return IHU_FAILURE;
	}

	//设置无效
	zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[task_id].taskId = TASK_ID_INVALID;

    return IHU_SUCCESS;
}

/*******************************************************************************
** METHOD : FsmRunEngine
**------------------------------------------------------------------------------
** Description :
**    supply action function entry of task that receive current Event in current state
**------------------------------------------------------------------------------
** Arguments :
**    IN:	 src_id: identification of task that send out current message
** 		     dest_id: identification of task that receive current message
** 		     commandId: identification of current message
** 		     param_ptr: pointer to current message structure
** 		     param_len: message length
**    OUT:   none
**    INOUT: none
**------------------------------------------------------------------------------
** Return value : IHU_SUCCESS OR IHU_FAILURE
*******************************************************************************/
OPSTAT FsmRunEngine(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
{
	UINT8  state;
	OPSTAT ret;
	UINT32 mid;
	/*
	** Check the task_id, message_id and par_length
	*/
	
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, src_id=[0x%x], dest_id=[0x%x], msgid=[0x%x][%s]!!!\n", src_id, dest_id, msg_id, zIhuVmCtrMsgStaticCfg[msg_id].msgName);
		return IHU_FAILURE;
	}
	if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, src_id=[0x%x], dest_id=[0x%x], msgid=[0x%x][%s]!!!\n", src_id, dest_id, msg_id, zIhuVmCtrMsgStaticCfg[msg_id].msgName);
		return IHU_FAILURE;
	}
	if (param_len>IHU_SYSDIM_MSG_BODY_LEN_MAX){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Too large message length than IHU set capability, param_len=%d!!!\n", param_len);
		return IHU_FAILURE;
	}
	if ((msg_id <= MSG_ID_COM_MIN) || (msg_id >= MSG_ID_COM_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on Msg_Id, msg_id=%d!!!\n", msg_id);
		return IHU_FAILURE;
	}

	/*
	** Search for the dest_id task state
	*/

	if( zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[dest_id].taskId != dest_id )
	{
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: The destination process does not exist.\n");
		return IHU_FAILURE;
	}

	/*
	** Get current FSM state
	*/
	state = FsmGetState(dest_id);
	mid = msg_id & IHU_SYSDIM_MSGID_MASK_SET;

	//check the state and messageId of task
	if((state >= IHU_SYSDIM_TASK_STATE_NBR_MAX)||(mid >= IHU_SYSDIM_MSGID_NBR_MAX))
	{
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: The state(%d) or msgId(0x%x) of task(0x%x) is error.\n", state, mid, dest_id);
		return IHU_FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE)
	{
		//消息ID的顺序跟commsgxxx.h的中的定义可能不完全一样，导致这里直接POP出消息名字会出错，注意！
		IhuDebugPrint("VMFO: Call state function(0x%x) in state(%d) of dest task(0x%x[%s]) for msg(0x%x[%s]), src=(0x%x[%s]).\n", \
		(UINT32)zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc, state, dest_id, zIhuVmCtrTab.task[dest_id].taskName,\
		mid, zIhuVmCtrMsgStaticCfg[mid-1].msgName, src_id, zIhuVmCtrTab.task[src_id].taskName);
	}

	/*
	** Call the state function.
	*/
	//Following FSM_STATE_COMMON state could contain any MSGID, as to avoid the restriction of message incoming
	if(zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[dest_id].pFsmArray[FSM_STATE_COMMON][mid].stateFunc != NULL)
	{
		ret = (zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[dest_id].pFsmArray[FSM_STATE_COMMON][mid].stateFunc)(dest_id, src_id, param_ptr, param_len);
		if( IHU_FAILURE == ret)
		{
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;			
			IhuErrorPrint("VMFO: Internal error is found in the FSM_STATE_COMMON state function.\n");
			return IHU_FAILURE;
		}
	}
	else if(zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc != NULL)
	{
		ret = (zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc)(dest_id, src_id, param_ptr, param_len);
		if( IHU_FAILURE == ret)
		{
			zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;			
			IhuErrorPrint("VMFO: Internal error is found in the state function.\n");
			return IHU_FAILURE;
		}
	}
	else
	{
		if(NULL != param_ptr)
		{
			//Free memory, here do nothing.
		}
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		//消息ID的顺序跟commsgxxx.h的中的定义可能不完全一样，导致这里直接POP出消息名字会出错，注意！		
		IhuErrorPrint("VMFO: Receive invalid msg(%x)[%s] in state(%d) of task(0x%x)[%s] from task(0x%x)[%s].\n", mid, zIhuVmCtrMsgStaticCfg[mid-1].msgName, state, dest_id, zIhuVmCtrTab.task[dest_id].taskName, src_id, zIhuVmCtrTab.task[src_id].taskName);
		return IHU_FAILURE;
	}

    return IHU_SUCCESS;
}

/*******************************************************************************
** METHOD : FsmProcessingLaunch
**------------------------------------------------------------------------------
** Description :
**    check and handle message loop as each task entry
**------------------------------------------------------------------------------
** Arguments :
**    IN:	 none
**    OUT:   none
**    INOUT: none
**------------------------------------------------------------------------------
** Return value : IHU_SUCCESS OR IHU_FAILURE
*******************************************************************************/
OPSTAT FsmProcessingLaunch(void *task)
{
	OPSTAT ret;
	IhuMsgSruct_t rcv; //MYC
	UINT8 task_id = 0; //Get current working task_id
	
	/*
	** Check the task_id
	*/
	//这里的currentTaskId通过currentTaskId是传不过来的，必须采用arg参数传递
	//task_id = zIhuVmCtrTab.fsm.currentTaskId;
	//zIhuVmCtrTab.fsm.currentTaskId = TASK_ID_INVALID;
	task_id = (UINT8)*(char *)task;

	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}

	/*
	** Run each task entry API, give each task a chance to init any specific information
	*/
	if (zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_ENTRY].stateFunc != NULL){
		(zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_ENTRY].stateFunc)(task_id, 0, NULL, 0);
	}else{
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Task (ID=%d) get no init entry fetched!\n", task_id);
	}
	
	//任务中的消息轮循
	//从当前TASKID对应的消息队列读取消息
	//每一次循环，只从消息队列中读取一个消息并进行执行，这相当于所有任务的优先级都是均等的，不存在优先级高的问题
	//如果消息过多，可能会造成消息队列的溢出，这里没有考虑消息队列的共享问题，未来可以考虑继续优化
	//小喇叭响起来，这里是资源优化集中营！
		//Loop for ever
	while (1){
		memset(&rcv, 0, sizeof(IhuMsgSruct_t));
		ret = ihu_message_rcv(task_id, &rcv);
		//以下方式是为了调查出现无效接收消息时的方式，就是扎到出现问题消息的特征，进而帮助分析产生无效消息的来源
//		if (task_id == TASK_ID_SPSVIRGO) IhuDebugPrint("VMFO: Received message at task_id=%d [%s], &rcv=[0x%x]\n", task_id, zIhuVmCtrTab.task[task_id].taskName, &rcv);
//		if ((rcv.src_id <=TASK_ID_MIN) || (rcv.src_id >= TASK_ID_MAX) || (rcv.dest_id <=TASK_ID_MIN) || (rcv.dest_id >= TASK_ID_MAX)){
//			IhuErrorPrint("VMFO: TEST msg_id=[0x%x], &msg_id=[0x%x]\n", rcv.msgType, &(rcv.msgType));
//			IhuErrorPrint("VMFO: TEST dest_id=[0x%x], &dest_id=[0x%x]\n", rcv.dest_id, &(rcv.dest_id));
//			IhuErrorPrint("VMFO: TEST src_id=[0x%x], &src_id=[0x%x]\n", rcv.src_id, &(rcv.src_id));
//			IhuErrorPrint("VMFO: TEST msg_len=[0x%x], &msg_len=[0x%x]\n", rcv.msgLen, &(rcv.msgLen));
//		}
		
		//纯粹是为了消除最后返回RETURN的WARNING告警问题
		if (ret == -3){
			break;
		}
		//留一个rcv函数返回的口子，以便继续执行
		else if (ret == -2){
			continue;
		}
		else if (ret == IHU_FAILURE){
			IhuDebugPrint("VMFO: STM kernal receive message error, hold for further action!\n");
			//To be defined here the error case, to recover everything
			ihu_sleep(1); 
		}else{
			ret = FsmRunEngine(rcv.msgType, rcv.dest_id, rcv.src_id, rcv.msgBody, rcv.msgLen);
			if (ret == IHU_FAILURE){
				zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;	
				IhuErrorPrint("VMFO: Error execute FsmRun state machine!\n");
			}
		}
	}//While(1)
	
	return IHU_SUCCESS;
}


//设置状态
OPSTAT FsmSetState(UINT8 task_id, UINT8 newState)
{
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: FsmSetState error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}

	//Checking newState range
	if (newState > IHU_SYSDIM_TASK_STATE_NBR_MAX){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: FsmSetState error on state, State=%d!!!\n", newState);
		return IHU_FAILURE;
	}

	zIhuVmCtrTab.task[task_id].state = newState;
	return IHU_SUCCESS;
}

//获取状态
UINT8 FsmGetState(UINT8 task_id)
{
	/*
	** Check the task_id
	*/
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return FSM_STATE_INVALID;
	}
	/*
	** Check the state info
	*/
	if (zIhuVmCtrTab.task[task_id].state <= IHU_SYSDIM_TASK_STATE_NBR_MAX){
		return zIhuVmCtrTab.task[task_id].state;
	}else{
		return FSM_STATE_INVALID;
	}
}

/**********************************************************************************
 *
 *   IHU VMLAYER全局函数过程
 *   依赖于FreeRTOS，面对底层SDK所定义的过程API函数
 *
 **********************************************************************************/
/**************************************************************************************
 * 创建任务队列，以便用于任务之间消息通信, 每个任务创建唯一一个队列，所以使用TASKID作为索引
 * void  Ihu_message_queue_create( UINT task_id)
 ***************************************************************************************/
OPSTAT ihu_message_queue_create(UINT8 task_id)
{
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}

	//OS_QUEUE_CREATE(queue, item_size, max_items)
	OS_QUEUE_CREATE(zIhuVmCtrTab.task[task_id].QueId, IHU_SYSDIM_MSG_QUEUE_SIZE_MAX, IHU_SYSDIM_MSG_QUEUE_NBR_MAX);
	if(zIhuVmCtrTab.task[task_id].QueId == NULL){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Failed to create msg-queue | errno=%d [%s]\n", errno, strerror(errno));
		return IHU_FAILURE;
	}

	return IHU_SUCCESS;
}

//kill message queue
OPSTAT ihu_message_queue_delete(UINT8 task_id)
{
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}

	//Not exist
	if (zIhuVmCtrTab.task[task_id].QueId == NULL) {
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Null msg-queue and no need delete | errno=%d [%s]\n", errno, strerror(errno));
	  return IHU_FAILURE;
	}

	//Delete queue
	OS_QUEUE_DELETE(zIhuVmCtrTab.task[task_id].QueId);
	zIhuVmCtrTab.task[task_id].QueId = NULL;

	return IHU_SUCCESS;
}

//查询消息队列
QueueHandle_t ihu_message_queue_inquery(UINT8 task_id)
{
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return 0;
	}

	return zIhuVmCtrTab.task[task_id].QueId;
}

//这个过程用在同步不同进程之间的消息队列，确保不同进程之间的任务可以互发消息
//如果队列本来就不存在，就不用同步或者重新填入全局控制表了
OPSTAT ihu_message_queue_resync(void)
{
  //此函数在VMFO中没有实质意义

	return IHU_SUCCESS;
}

//清理某一个任务的消息队列
OPSTAT ihu_message_queue_clean(UINT8 dest_id)
{
	//入参检查
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return IHU_FAILURE;
	}

	//先删除消息队列
  if (zIhuVmCtrTab.task[dest_id].QueId == NULL) {
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Null msg-queue and no need clean | errno=%d [%s]\n", errno, strerror(errno));
    return IHU_FAILURE;
  }
	OS_QUEUE_DELETE(zIhuVmCtrTab.task[dest_id].QueId);

	//再建立消息队列
	OS_QUEUE_CREATE(zIhuVmCtrTab.task[dest_id].QueId, IHU_SYSDIM_MSG_QUEUE_SIZE_MAX, IHU_SYSDIM_MSG_QUEUE_NBR_MAX);
  if (zIhuVmCtrTab.task[dest_id].QueId == NULL) {
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Not successfully re-build the msg queue | errno=%d [%s]\n", errno, strerror(errno));
    return IHU_FAILURE;
  }

	return IHU_SUCCESS;
}

//message send basic processing
//All in parameters
OPSTAT ihu_message_send(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
{
	//int ret = 0;
	char s1[IHU_SYSDIM_TASK_NAME_LEN_MAX+2]="", s2[IHU_SYSDIM_TASK_NAME_LEN_MAX+2]="", s3[IHU_SYSDIM_MSGID_NAME_LEN_MAX]="", s4[IHU_SYSDIM_TIMERID_NAME_LEN_MAX]="";
	UINT32 tid = 0;
	IhuMsgSruct_t msg;
	
	//Checking task_id range
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return IHU_FAILURE;
	}
	if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, src_id=%d!!!\n", src_id);
		return IHU_FAILURE;
	}
	if (param_len > IHU_SYSDIM_MSG_BODY_LEN_MAX){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Too large message length than IHU set capability, param_len=%d!!!\n", param_len);
		return IHU_FAILURE;
	}

	//Init to clean this memory area
  //初始化消息内容，是为了稳定可靠安全
	//IhuDebugPrint("VMFO: Calling memset(&msg, 0, sizeof(IhuMsgSruct_t));\r\n");
  memset(&msg, 0, sizeof(IhuMsgSruct_t));
  msg.msgType = msg_id;
  msg.dest_id = dest_id;
  msg.src_id = src_id;
  msg.msgLen = param_len;
	//IhuDebugPrint("VMFO: Calling memcpy(&(msg.msgBody[0]), param_ptr, param_len);\r\n");
  memcpy(&(msg.msgBody[0]), param_ptr, param_len);

  //正式发送QUEUE
	//IhuDebugPrint("VMFO: Calling OS_QUEUE_PUT()\r\n");
  if (OS_QUEUE_PUT(zIhuVmCtrTab.task[dest_id].QueId, &msg, 0) != OS_QUEUE_OK){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: msgsnd() write msg failed, errno=%d[%s], dest_id = %d [%s]\n",errno,strerror(errno), dest_id, zIhuVmCtrTab.task[dest_id].taskName);
    //zIhuVmCtrTab.task[dest_id].QueFullFlag = IHU_SYSCFG_TASK_QUEUE_FULL_TRUE;
    return IHU_FAILURE;
  }

	/*
	 *  Message Trace processing
	 *  注意字符串长度，太小会出现内存飞掉的情形，MessageTrace的数据配置来源于数据库，层次比这个还要高，虽然有点怪怪的，但不影响总体架构
	 *  最开始采用IFDEF的形式，后期完善后改为更为直接的代码方式
	 *  软件开发中，DEBUG和TRACE是两种最常用/最有用的调试模式，比单步还有用，这不仅是因为熟手不需要单步执行，而且也是因为多线程多进程单步执行环境制造的复杂性
	 *
	 *  有关MESSAGE TRACE的效率，是一个要注意的问题，当系统负载不高时，打开所有的TRACE是合适的，但一旦部署实际系统，TRACE需要减少到最低程度，这是原则，实际需要
	 *  维护人员根据情况灵活把我
	 *
	 *  本TRACE功能，提供了多种工作模式
	 *
	 */
	switch (zIhuSysEngPar.traceMode)
	{
		case IHU_SYSCFG_TRACE_MSG_MODE_OFF:
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_INVALID:
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_ALL:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_ALL_BUT_TIME_OUT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if (msg_id != MSG_ID_COM_TIME_OUT){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_ALL_BUT_HEART_BEAT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_ALL_BUT_TIME_OUT_AND_HEART_BEAT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((msg_id != MSG_ID_COM_TIME_OUT) && (msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;
			
		case IHU_SYSCFG_TRACE_MSG_MODE_ALL_WITH_TIMERID:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((msg_id != MSG_ID_COM_TIME_OUT)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			else{
				//正好将TIMERID定义为4B，就跟编译器和处理器大小端无关了
				memcpy(&tid, param_ptr, 4);
				ihu_timerid_to_string(tid, s4);
			  IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d, TID=%02X%s\n", msg_id, s3, dest_id, s1, src_id, s2, param_len, tid, s4);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_TIMERID_ONLY:
			if (msg_id == MSG_ID_COM_TIME_OUT){
				ihu_taskid_to_string(dest_id, s1);
				ihu_taskid_to_string(src_id, s2);
				ihu_msgid_to_string(msg_id, s3);
				//正好将TIMERID定义为4B，就跟编译器和处理器大小端无关了
				memcpy(&tid, param_ptr, 4);
				ihu_timerid_to_string(tid, s4);
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d, TID=%02X%s\n", msg_id, s3, dest_id, s1, src_id, s2, param_len, tid, s4);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_TO_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)\
				&& (zIhuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_TO_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict!= TRUE)\
				&& (zIhuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_FROM_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)\
				&& (zIhuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_FROM_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToRestrict!= TRUE)\
				&& (zIhuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_DOUBLE_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)\
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE) && (zIhuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_DOUBLE_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToRestrict != TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)\
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE) && (zIhuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MSGID_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)\
				&& (zIhuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MSGID_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)\
				&& (zIhuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_TO_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)\
					&& (zIhuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE) && (zIhuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_TO_RESTRIC:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)\
					&& (zIhuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE) && (zIhuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_FROM_ALLOWC:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)\
					&& (zIhuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE) && (zIhuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_FROM_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)\
					&& (zIhuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE) && (zIhuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_DOUBLE_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)\
					&& (zIhuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE) && (zIhuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE)\
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_DOUBLE_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)\
					&& (zIhuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE) && (zIhuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE)\
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		default:
			if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
				zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: System Engineering Parameter Trace Mode setting error! DebugMode=%d\n", zIhuSysEngPar.debugMode);
			}
			break;
	}

	return IHU_SUCCESS;
}

//MYC
OPSTAT ihu_message_send_isr(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
{
	//int ret = 0;
	char s1[IHU_SYSDIM_TASK_NAME_LEN_MAX+2]="", s2[IHU_SYSDIM_TASK_NAME_LEN_MAX+2]="", s3[IHU_SYSDIM_MSGID_NAME_LEN_MAX]="";
	IhuMsgSruct_t msg;
	
	//Checking task_id range
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		printf("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return IHU_FAILURE;
	}
	if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		printf("VMFO: Error on task_id, src_id=%d!!!\n", src_id);
		return IHU_FAILURE;
	}
	if (param_len > IHU_SYSDIM_MSG_BODY_LEN_MAX){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		printf("VMFO: Too large message length than IHU set capability, param_len=%d!!!\n", param_len);
		return IHU_FAILURE;
	}

	//Init to clean this memory area
  //初始化消息内容，是为了稳定可靠安全
	//printf("VMFO: Calling memset(&msg, 0, sizeof(IhuMsgSruct_t));\r\n");
  memset(&msg, 0, sizeof(IhuMsgSruct_t));
  msg.msgType = msg_id;
  msg.dest_id = dest_id;
  msg.src_id = src_id;
  msg.msgLen = param_len;
	//printf("VMFO: Calling memcpy(&(msg.msgBody[0]), param_ptr, param_len);\r\n");
  memcpy(&(msg.msgBody[0]), param_ptr, param_len);

  //正式发送QUEUE
	//printf("VMFO: Calling OS_QUEUE_PUT_FROM_ISR() start ...\r\n");
	
	{ 
			BaseType_t need_switch;
		  int ret; 
			ret = xQueueSendToBackFromISR(zIhuVmCtrTab.task[dest_id].QueId, &msg, &need_switch); 
			portEND_SWITCHING_ISR(need_switch); 
			if (OS_QUEUE_OK != ret)
			{
					zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
					printf("VMFO: msgsnd() write msg failed, errno=%d[%s], dest_id = %d [%s]\n",errno,strerror(errno), dest_id, zIhuVmCtrTab.task[dest_id].taskName);
					return IHU_FAILURE;
			}
	}
//  if ( OS_QUEUE_PUT_FROM_ISR(zIhuVmCtrTab.task[dest_id].QueId, &msg) != OS_QUEUE_OK)
//	{
//    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
//    printf("VMFO: msgsnd() write msg failed, errno=%d[%s], dest_id = %d [%s]\n",errno,strerror(errno), dest_id, zIhuVmCtrTab.task[dest_id].taskName);
//    //zIhuVmCtrTab.task[dest_id].QueFullFlag = IHU_SYSCFG_TASK_QUEUE_FULL_TRUE;
//    return IHU_FAILURE;
//  }
	printf("VMFO: Calling OS_QUEUE_PUT_FROM_ISR() end.\r\n");
	/*
	 *  Message Trace processing
	 *  注意字符串长度，太小会出现内存飞掉的情形，MessageTrace的数据配置来源于数据库，层次比这个还要高，虽然有点怪怪的，但不影响总体架构
	 *  最开始采用IFDEF的形式，后期完善后改为更为直接的代码方式
	 *  软件开发中，DEBUG和TRACE是两种最常用/最有用的调试模式，比单步还有用，这不仅是因为熟手不需要单步执行，而且也是因为多线程多进程单步执行环境制造的复杂性
	 *
	 *  有关MESSAGE TRACE的效率，是一个要注意的问题，当系统负载不高时，打开所有的TRACE是合适的，但一旦部署实际系统，TRACE需要减少到最低程度，这是原则，实际需要
	 *  维护人员根据情况灵活把我
	 *
	 *  本TRACE功能，提供了多种工作模式
	 *
	 */
	switch (zIhuSysEngPar.traceMode)
	{
		case IHU_SYSCFG_TRACE_MSG_MODE_OFF:
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_INVALID:
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_ALL:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_ALL_BUT_TIME_OUT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if (msg_id != MSG_ID_COM_TIME_OUT){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_ALL_BUT_HEART_BEAT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_ALL_BUT_TIME_OUT_AND_HEART_BEAT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((msg_id != MSG_ID_COM_TIME_OUT) && (msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_TO_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_TO_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict!= TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_FROM_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_FROM_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToRestrict!= TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_DOUBLE_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_DOUBLE_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToRestrict != TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MSGID_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_MSGID_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_TO_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_TO_RESTRIC:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_FROM_ALLOWC:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_FROM_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_DOUBLE_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_DOUBLE_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)){
				printf("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		default:
			if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
				zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
				printf("VMFO: System Engineering Parameter Trace Mode setting error! DebugMode=%d\n", zIhuSysEngPar.debugMode);
			}
			break;
	}

	return IHU_SUCCESS;
}


//Reveive message
//dest_id: in
//*msg: out
// ret: 接受消息中的errno=4/EINTR需要单独处理，不然会出现被SIGALRM打断的情形
OPSTAT ihu_message_rcv(UINT8 dest_id, IhuMsgSruct_t *msg)
{
	//Checking task_id range
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return IHU_FAILURE;
	}

	//未来调试时，需要注意，这里的BUFFER长度是否合理。原先在LINUX系统中，需要扣掉头部
	//ret = msgrcv(zIhuVmCtrTab.task[dest_id].QueId, msg, (sizeof(IhuMsgSruct_t) - sizeof(long)), 0, 0);
	if (OS_QUEUE_GET(zIhuVmCtrTab.task[dest_id].QueId, msg, OS_QUEUE_FOREVER) != OS_QUEUE_OK){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: msgrcv() receive msg failed, Qid=%d, msg=%08X, errno=%d[%s]\n", zIhuVmCtrTab.task[dest_id].QueId, msg, errno, strerror(errno));
    //防止某个任务在接受消息时，既不阻塞，也接受出错，所以休眠100ms，保持其他任务的正常运行
    ihu_usleep(100);
    return IHU_FAILURE;
	}

	return IHU_SUCCESS;
}


/**************************************************************************************
 * 创建任务，使用的是线程模拟，各个任务之间可以共享内存
 * To Create a thread
 * void  ihu_task_create( UINT8 task_id,          		out: pointer to zIhuTaskId[task_id]
 *                   void *(*task_func)(void *),        in: task's entry point
 *                   void *arg,                         in: parameter of the task's function
 *                   int prio )                         in: priority of the task*
 ***************************************************************************************/
OPSTAT ihu_task_create(UINT8 task_id, void *(*task_func)(void *), void *arg, int prio)
{
	int                     err;
	OS_BASE_TYPE ret;

	// Checking task_id
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id!! err=%d, errno=%d, %s, task_id=%d\n", err, errno, strerror(err), task_id);
		return IHU_FAILURE;
	}

	//As FsmProcessLaunch has to transfer task_id in, we will use global variance of zIhuVmCtrTab.fsm.currentTaskId
	//So task_id has to store into there
	//Wonderful mechanism!!!
	//在FreeRTOS下，任务创建好了之后，并没有立即启动，而是要等待osKernelStart()执行之后才能启动调度，所以这个机制对FreeRTOS并不适合
//	if (zIhuVmCtrTab.fsm.currentTaskId != TASK_ID_INVALID){
//		ihu_sleep(1);
//	}
//	if (zIhuVmCtrTab.fsm.currentTaskId != TASK_ID_INVALID){
//		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
//		IhuErrorPrint("VMFO: Task_id not yet ready by running process, new task create failure!\n");
//		return IHU_FAILURE;
//	}
//	zIhuVmCtrTab.fsm.currentTaskId = task_id;
//void (*TaskFunction_t)( void * )
	
	// creation of the task
  ret = OS_TASK_CREATE(
        (const char *)zIhuVmCtrTab.task[task_id].taskName,  /* The text name assigned to the task, for debug only; not used by the kernel. */
        //(void *(*)(void *))(task_func),         /* The System Initialization task. */
        (TaskFunction_t)(task_func),
        arg,                                      /* The parameter passed to the task. */
        IHU_SYSDIM_TASK_STACK_SIZE_MAX * OS_STACK_WORD_SIZE, /* The number of bytes to allocate to the stack of the task. */
        prio,                                     /* The priority assigned to the task. */
        zIhuVmCtrTab.task[task_id].TaskHandle);        /* The task handle */
	if(ret != OS_TASK_CREATE_SUCCESS)
	{
	  zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuDebugPrint("VMFO: Create task nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
		return IHU_FAILURE;
	}

	//存储现场记录, zIhuVmCtrTab.task[task_id].TaskName to be added in another way, TBD
	zIhuVmCtrTab.task[task_id].TaskId = task_id;
	IhuDebugPrint("VMFO: pthread_create() OK ...\n");

	//返回
	return IHU_SUCCESS;
}

//kill task
OPSTAT ihu_task_delete(UINT8 task_id)
{
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}

	//只是清理掉FSM中的信息
	FsmRemove(task_id);

  //删除任务
	if ((zIhuVmCtrTab.task[task_id].TaskHandle == NULL) || (zIhuVmCtrTab.task[task_id].TaskHandle == 0)){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMUO: No exist or right task to be deleted! \n");
    return IHU_FAILURE;
	}
	OS_TASK_DELETE(zIhuVmCtrTab.task[task_id].TaskHandle);

	//返回
	return IHU_SUCCESS;
}

//聚合创建任务，消息队列，并直接让其开始运行
OPSTAT ihu_task_create_and_run(UINT8 task_id, IhuFsmStateItem_t* pIhuFsmStateItem)
{
	OPSTAT ret = 0;
	
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Input Error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}

	//Init Fsm table
	ret = FsmAddNew(task_id, pIhuFsmStateItem);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Init state machine FsmAddNew error, taskid = %d.\n", task_id);
		return IHU_FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_CRT_ON) != FALSE){
		IhuDebugPrint("VMFO: FsmAddNew Successful, taskId = 0x%x [%s].\n", task_id, zIhuVmCtrTab.task[task_id].taskName);
	}

	//Create Queid
	ret = ihu_message_queue_create(task_id);
	if (ret == IHU_FAILURE)
	{
	zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
	IhuErrorPrint("VMFO: Create queue unsuccessfully, taskId = %d.\n", task_id);
	return IHU_FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_CRT_ON) != FALSE){
		IhuDebugPrint("VMFO: Msgque create successful, taskId = 0x%x [%s].\n", task_id, zIhuVmCtrTab.task[task_id].taskName);
	}

	//Create task and make it running for the 1st time
	//让VMFO保持更高一级的优先级，以便VMFO及时处理守护程序的职责
	if (task_id == TASK_ID_VMFO) ret = ihu_task_create(task_id, /*CALLBACK*/(void *(*)(void *))FsmProcessingLaunch, (void *)&(zIhuVmCtrTab.task[task_id].TaskId), IHU_SYSMSG_TASK_THREAD_PRIO+1);
	else ret = ihu_task_create(task_id, /*CALLBACK*/(void *(*)(void *))FsmProcessingLaunch, (void *)&(zIhuVmCtrTab.task[task_id].TaskId), IHU_SYSMSG_TASK_THREAD_PRIO);
	if (ret == IHU_FAILURE)
	{
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Create task un-successfully, taskid = %d.\n", task_id);		
		return IHU_FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_CRT_ON) != FALSE){
	  IhuDebugPrint("VMFO: Task create Successful, taskId = 0x%x [%s].\n", task_id, zIhuVmCtrTab.task[task_id].taskName);
	}
	
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_IPT_ON) != FALSE){
		IhuDebugPrint("VMFO: Whole task environment setup successful, taskId = 0x%x [%s].\n", task_id, zIhuVmCtrTab.task[task_id].taskName);
	}
	
	//给该任务设置一个软的状态：省略了
	
	return IHU_SUCCESS;
}

//初始化任务并启动
OPSTAT ihu_system_task_init_call(UINT8 task_id, IhuFsmStateItem_t *p)
{
	int ret = 0;
	
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Input Error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}
	
	//入参检查
	if (p == NULL){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Input wrong IhuFsmStateItem pointer!\n");
		return IHU_FAILURE;
	}
	
	//任务控制启动标示检查
	if (zIhuVmCtrTab.task[task_id].pnpState != IHU_SYSCFG_TASK_PNP_ON){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: no need create this task [%s]!\n", zIhuVmCtrTab.task[task_id].taskName);	
		return IHU_FAILURE;
	}
	
	//打印信息
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_CRT_ON) != FALSE){
		IhuDebugPrint("VMFO: Staring to create task [%s] related environments...\n", zIhuVmCtrTab.task[task_id].taskName);
	}

	//任务创建并初始化所有状态机
	ret = ihu_task_create_and_run(task_id, p);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: create task env [%s] un-successfully, program exit.\n", zIhuVmCtrTab.task[task_id].taskName);
		return IHU_FAILURE;
	}else{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: create task successfully, taskid=%d[%s].\n", task_id, zIhuVmCtrTab.task[task_id].taskName);
		}
	}
	
	//将该任务执行时对应的状态转移机指针存入，以供以后扫描时使用
	zIhuVmCtrTab.task[task_id].fsmPtr = p;
	
	//正确返回
	return IHU_SUCCESS;
}

//创建所有任务，采用了InputConfiguration FuncHandler之后，这里的处理已经大大简化了
void ihu_task_create_all(void)
{
	IhuFsmStateItem_t *p;
	int task_id = 0;
	
	for (task_id = TASK_ID_MIN + 1; task_id < TASK_ID_MAX; task_id++){
		p = (IhuFsmStateItem_t *)zIhuVmCtrTab.task[task_id].taskFuncEntry;
		if ((p != NULL) && (zIhuVmCtrTab.task[task_id].pnpState == IHU_SYSCFG_TASK_PNP_ON)){
			ihu_system_task_init_call(task_id, p);
			ihu_vm_send_init_msg_to_app_task(task_id);
		}
	}
}

void ihu_task_delete_all_and_queue(void)
{
	int task_id=0;

	//这是的清除，只是为了进程在系统终止后，清除相应的上下文
	//Test purpose, remove all message queue, to be removed at formal situation
	//task_id => starting from TASK_NAME_ID_MIN to TASK_NAME_ID_MAX
	IhuDebugPrint("VMFO: Clean all running task and message queue for next time debug!\n");
	for (task_id=TASK_ID_MIN; task_id<TASK_ID_MAX; task_id++){
		ihu_task_delete(task_id);
		ihu_message_queue_delete(task_id);
	}
	return;
}

//公共API，不做任何事情
OPSTAT fsm_com_do_nothing(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	return IHU_SUCCESS;
}

OPSTAT fsm_com_heart_beat_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	int ret=0;

	//接收参数检查
	msg_struct_com_heart_beat_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_heart_beat_t));
	if ((param_ptr == NULL) || (param_len > sizeof(msg_struct_com_heart_beat_t))){
		IhuErrorPrint("%s: Receive heart beat message error!\n", zIhuVmCtrTab.task[dest_id].taskName);
		return IHU_FAILURE;
	}
	//优化的结果，可以不用拷贝，暂时没有意义
	//memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zIhuRunErrCnt[dest_id]是否超限
	if (zIhuSysStaPm.taskRunErrCnt[dest_id] > IHU_RUN_ERROR_LEVEL_4_DEAD){
		//减少重复RESTART的概率
		zIhuSysStaPm.taskRunErrCnt[dest_id] = zIhuSysStaPm.taskRunErrCnt[dest_id] - IHU_RUN_ERROR_LEVEL_4_DEAD;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = ihu_message_send(MSG_ID_COM_RESTART, dest_id, dest_id, &snd0, snd0.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_TASK(dest_id, "%s: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[dest_id].taskName, zIhuVmCtrTab.task[dest_id].taskName, zIhuVmCtrTab.task[dest_id].taskName);
	}

	//发送消息
	if ((src_id > TASK_ID_MIN) && (src_id < TASK_ID_MAX) && (dest_id > TASK_ID_MIN) && (dest_id < TASK_ID_MAX) ){
		//发送心跳回复消息给src_id
		msg_struct_com_heart_beat_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_heart_beat_fb_t));
		snd.length = sizeof(msg_struct_com_heart_beat_fb_t);
		ret = ihu_message_send(MSG_ID_COM_HEART_BEAT_FB, src_id, dest_id, &snd, snd.length);
		if (ret == IHU_FAILURE)
			IHU_ERROR_PRINT_TASK(dest_id, "%s: Send message error, TASK[%s] to TASK[%s]!\n", zIhuVmCtrTab.task[dest_id].taskName, zIhuVmCtrTab.task[dest_id].taskName, zIhuVmCtrTab.task[src_id].taskName);
	}
	//也可能是调用关系，故而直接采用SRC_ID=0的方式，这种情况原则上也允许

	return IHU_SUCCESS;
}


//Unix Time transfer to YMD time
struct tm ihu_clock_unix_to_ymd(time_t t_unix)
{
		return *localtime(&t_unix);
}


/*******************************************************************************
**
**  没有RTOS的情形
**
**********************************************************************************/
//这里将LaunchEntry和LaunchExecute是因为在VMDA环境中，创造环境和执行不在一起
//具有真正任务的操作系统环境中，没有这个必要了
OPSTAT FsmProcessingLaunchEntryBareRtos(UINT8 task_id)
{
  /*
  ** Check the task_id
  */
  if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
    return IHU_FAILURE;
  }

  /*
  ** Run each task entry API, give each task a chance to init any specific information
  */
  if (zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_ENTRY].stateFunc != NULL){
    (zIhuVmCtrTab.fsm.pIhuFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_ENTRY].stateFunc)(task_id, 0, NULL, 0);
  }else{
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Task (ID=%d) get no init entry fetched!\n", task_id);
  }

  return IHU_SUCCESS;
}

//IhuMsgSruct_t rcv;
OPSTAT FsmProcessingLaunchExecuteBareRtos(UINT8 task_id)
{
  OPSTAT ret;
  IhuMsgSruct_t rcv;

  /*
  ** Check the task_id
  */
  if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
    return IHU_FAILURE;
  }

  //任务中的消息轮循
  //从当前TASKID对应的消息队列读取消息
  //每一次循环，只从消息队列中读取一个消息并进行执行，这相当于所有任务的优先级都是均等的，不存在优先级高的问题
  //如果消息过多，可能会造成消息队列的溢出，这里没有考虑消息队列的共享问题，未来可以考虑继续优化
  //小喇叭响起来，这里是资源优化集中营！
  memset(&rcv, 0, sizeof(IhuMsgSruct_t));
  ret = ihu_message_rcv(task_id, &rcv);
  if (ret == IHU_SUCCESS)
  {
    ret = FsmRunEngine(rcv.msgType, rcv.dest_id, rcv.src_id, rcv.msgBody, rcv.msgLen);
    if (ret == IHU_FAILURE)
    {
      zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
      IhuErrorPrint("VMFO: Error execute FsmRun state machine!\n");
      return IHU_FAILURE;
    }
  }
  else
  {
    //失败了，啥都不干，说明读取出错，或者消息队列是空的
    //这意味着只是空转一下
    //真正失败的时候，需要返回FAILURE

    //这里是消息队列空，所以返回成功
    return IHU_SUCCESS;
  }

  return IHU_SUCCESS;
}

//Reveive message
//dest_id: in
//*msg: out
OPSTAT ihu_message_rcv_bare_rtos(UINT8 dest_id, IhuMsgSruct_t *msg)
{
  int i=0, j=0;
  bool Flag = FALSE;

  //Checking task_id range
  if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
    return IHU_FAILURE;
  }

  //循环查找一个任务的消息队列，看看有无有效消息
  for (i=0; i<IHU_SYSDIM_MSG_QUEUE_NBR_MAX; i++)
  {
    j = zIhuVmCtrTab.fsm.taskQue[dest_id].queIndex + i;
    j = j%IHU_SYSDIM_MSG_QUEUE_NBR_MAX;
    if (zIhuVmCtrTab.fsm.taskQue[dest_id].queList[j].useFlag == TRUE)
    {
      //找到了未消耗的消息
      memcpy(msg, zIhuVmCtrTab.fsm.taskQue[dest_id].queList[j].msgQue, IHU_SYSDIM_MSG_BODY_LEN_MAX);
      zIhuVmCtrTab.fsm.taskQue[dest_id].queList[j].useFlag = FALSE;
      //将INDEX指向下一个
      j = (j+1) % IHU_SYSDIM_MSG_QUEUE_NBR_MAX;
      zIhuVmCtrTab.fsm.taskQue[dest_id].queIndex = j;
      //判断消息的合法性
      if ((msg->dest_id <= TASK_ID_MIN) || (msg->dest_id >= TASK_ID_MAX)){
        zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
        IhuErrorPrint("VMFO: Receive message error on task_id, dest_id=%d!!!\n", msg->dest_id);
        return IHU_FAILURE;
      }
      if ((msg->src_id <= TASK_ID_MIN) || (msg->src_id >= TASK_ID_MAX)){
        zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
        IhuErrorPrint("VMFO: Receive message error on src_id, dest_id=%d!!!\n", msg->src_id);
        return IHU_FAILURE;
      }
      //不允许定义消息结构中长度为0的消息体，至少需要一个长度域
      if ((msg->msgLen <= 0) || (msg->msgLen > IHU_SYSMSG_COM_BODY_LEN_MAX)){
        zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
        IhuErrorPrint("VMFO: Receive message error on length, msgLen=%d!!!\n", msg->msgLen);
        return IHU_FAILURE;
      }
      if ((msg->msgType <= MSG_ID_COM_MIN) || (msg->msgType >= MSG_ID_COM_MAX)){
        zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
        IhuErrorPrint("VMFO: Receive message error on msgId, msgType=%d!!!\n", msg->msgType);
        return IHU_FAILURE;
      }
      //退出循环
      Flag = TRUE;
      break;
    }
  }
  //返回错误，并不一定表示真发生了错误，也有可能是没有任何消息供消耗，此时，上层调用者就啥都不做，确保自己安全
  if (Flag == TRUE){
    return IHU_SUCCESS;
  }else{
    return IHU_FAILURE;
  }
}

//message send basic processing
//All in parameters
IhuMsgSruct_t msg;
OPSTAT ihu_message_send_bare_rtos(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
{
  //IhuMsgSruct_t msg;
  int i=0, j=0;
  bool Flag = FALSE;
  char s1[IHU_SYSDIM_TASK_NAME_LEN_MAX+2]="", s2[IHU_SYSDIM_TASK_NAME_LEN_MAX+2]="", s3[IHU_SYSDIM_MSGID_NAME_LEN_MAX]="";

  //入参检查Checking task_id range
  if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
    return IHU_FAILURE;
  }
  if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Error on task_id, src_id=%d!!!\n", src_id);
    return IHU_FAILURE;
  }
  if (param_len>IHU_SYSDIM_MSG_BODY_LEN_MAX){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Too large message length than IHU set capability, param_len=%d!!!\n", param_len);
    return IHU_FAILURE;
  }
  if ((msg_id <= MSG_ID_COM_MIN) || (msg_id >= MSG_ID_COM_MAX)){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Receive message error on msgId, msgType=%d!!!\n", msg_id);
    return IHU_FAILURE;
  }

  //对来源数据进行初始化，是为了稳定可靠安全
  memset(&msg, 0, sizeof(IhuMsgSruct_t));
  msg.msgType = msg_id;
  msg.dest_id = dest_id;
  msg.src_id = src_id;
  msg.msgLen = param_len;
  memcpy(&(msg.msgBody[0]), param_ptr, param_len);


  //然后送到目的地，循环查找一个任务的消息队列，看看有无空闲位置
  for (i=0; i<IHU_SYSDIM_MSG_QUEUE_NBR_MAX; i++)
  {
    j = zIhuVmCtrTab.fsm.taskQue[dest_id].queIndex + i;
    j = j%IHU_SYSDIM_MSG_QUEUE_NBR_MAX;
    if (zIhuVmCtrTab.fsm.taskQue[dest_id].queList[j].useFlag == FALSE)
    {
      //找到了空闲的消息位置
      memcpy(zIhuVmCtrTab.fsm.taskQue[dest_id].queList[j].msgQue, &msg, IHU_SYSDIM_MSG_BODY_LEN_MAX);
      zIhuVmCtrTab.fsm.taskQue[dest_id].queList[j].useFlag = TRUE;
      //将INDEX指向下一个
      j = (j+1) % IHU_SYSDIM_MSG_QUEUE_NBR_MAX;
      zIhuVmCtrTab.fsm.taskQue[dest_id].queIndex = j;
      //退出循环
      Flag = TRUE;
      break;
    }
  }
  //错误，表示没有空闲队列
  if (Flag == FALSE){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Message queue full, can not send into task = %d!!!\n", dest_id);
    return IHU_FAILURE;
  }

  /*
   *  Message Trace processing
   *  注意字符串长度，太小会出现内存飞掉的情形，MessageTrace的数据配置来源于数据库，层次比这个还要高，虽然有点怪怪的，但不影响总体架构
   *  最开始采用IFDEF的形式，后期完善后改为更为直接的代码方式
   *  软件开发中，DEBUG和TRACE是两种最常用/最有用的调试模式，比单步还有用，这不仅是因为熟手不需要单步执行，而且也是因为多线程多进程单步执行环境制造的复杂性
   *
   *  有关MESSAGE TRACE的效率，是一个要注意的问题，当系统负载不高时，打开所有的TRACE是合适的，但一旦部署实际系统，TRACE需要减少到最低程度，这是原则，实际需要
   *  维护人员根据情况灵活把我
   *
   *  本TRACE功能，提供了多种工作模式
   *
   */
  switch (zIhuSysEngPar.traceMode)
  {
    case IHU_SYSCFG_TRACE_MSG_MODE_OFF:
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_INVALID:
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_ALL:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_ALL_BUT_TIME_OUT:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if (msg_id != MSG_ID_COM_TIME_OUT){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_ALL_BUT_HEART_BEAT:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_ALL_BUT_TIME_OUT_AND_HEART_BEAT:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((msg_id != MSG_ID_COM_TIME_OUT) && (msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_TO_ALLOW:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_TO_RESTRICT:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict!= TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_FROM_ALLOW:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_FROM_RESTRICT:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToRestrict!= TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_DOUBLE_ALLOW:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)
          && (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_MOUDLE_DOUBLE_RESTRICT:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToRestrict != TRUE)
          && (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_MSGID_ALLOW:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_MSGID_RESTRICT:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_TO_ALLOW:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
          && (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_TO_RESTRIC:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
          && (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_FROM_ALLOWC:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
          && (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_FROM_RESTRICT:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)
          && (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_DOUBLE_ALLOW:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
          && (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)
          && (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    case IHU_SYSCFG_TRACE_MSG_MODE_COMBINE_DOUBLE_RESTRICT:
      ihu_taskid_to_string(dest_id, s1);
      ihu_taskid_to_string(src_id, s2);
      ihu_msgid_to_string(msg_id, s3);
      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)
          && (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)
          && (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)){
        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
      }
      break;

    default:
      if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
        zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
        IhuErrorPrint("VMFO: System Engineering Parameter Trace Mode setting error! DebugMode=%d.\n", zIhuSysEngPar.debugMode);
      }
      break;
  }

  return IHU_SUCCESS;
}


//为了对付不存在OS的情形
OPSTAT ihu_system_task_execute_bare_rtos(UINT8 task_id, IhuFsmStateItem_t *p)
{
  int ret = 0;
  char strDebug[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX];

  //Checking task_id range
  if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    sprintf(strDebug, "VMFO: Input Error on task_id, task_id=%d!!!\n", task_id);
    IhuErrorPrint(strDebug);
    return IHU_FAILURE;
  }

  //入参检查
  if (p == NULL){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Input wrong IhuFsmStateItem pointer!\n");
    return IHU_FAILURE;
  }

  //任务控制启动标示检查
  if (zIhuVmCtrTab.task[task_id].pnpState != IHU_SYSCFG_TASK_PNP_ON){
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    sprintf(strDebug, "VMFO: no need execute this task [%s]!\n", zIhuVmCtrTab.task[task_id].taskName);
    IhuErrorPrint(strDebug);
    return IHU_FAILURE;
  }

  //轮询执行任务的消息消耗
  ret = FsmProcessingLaunchExecuteBareRtos(task_id);
  if (ret == IHU_FAILURE)
  {
    zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
    sprintf(strDebug, "VMFO: Execute task FSM un-successfully, taskid = %d", task_id);
    IhuErrorPrint(strDebug);
    return IHU_FAILURE;
  }

  //正确返回
  return IHU_SUCCESS;
}

//轮询所有任务的正常执行
void ihu_task_execute_all_bare_rtos(void)
{
  #ifdef IHU_COMPILE_TASK_VMFO
    //Execute task VMFO FSM /1
    if (zIhuVmCtrTab.task[TASK_ID_VMFO].pnpState == IHU_SYSCFG_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_VMFO, FsmVMFO);
  #endif //IHU_COMPILE_TASK_VMFO

  #ifdef IHU_COMPILE_TASK_TIMER
    //Execute task Timer FSM /2
    if (zIhuVmCtrTab.task[TASK_ID_TIMER].pnpState == IHU_SYSCFG_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_TIMER, FsmTimer);
  #endif //IHU_COMPILE_TASK_TIMER

  #ifdef IHU_COMPILE_TASK_ASYLIBRA
    //Execute task ASYLIBRA FSM /3
    if (zIhuVmCtrTab.task[TASK_ID_ASYLIBRA].pnpState == IHU_SYSCFG_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_ASYLIBRA, FsmAsylibra);
  #endif //IHU_COMPILE_TASK_ASYLIBRA

  #ifdef IHU_COMPILE_TASK_ADCARIES
    //Execute task ADCARIES FSM /4
    if (zIhuVmCtrTab.task[TASK_ID_ADCARIES].pnpState == IHU_SYSCFG_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_ADCARIES, FsmAdcaries);
  #endif //IHU_COMPILE_TASK_ADCARIES

  #ifdef IHU_COMPILE_TASK_EMC
    //Execute task EMC FSM /5
    if (zIhuVmCtrTab.task[TASK_ID_EMC].pnpState == IHU_SYSCFG_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_EMC, FsmEmc);
  #endif //IHU_COMPILE_TASK_EMC

  IhuDebugPrint("VMFO: Execute task once, for test!\n");
}

//安装ISR服务程序，目前暂时找不到合适的ucosIII对应的方式，待定
//采用ISR方式后，硬件的轮询将变得不重要，都采用ISR方式，将更加干净、高效，完全是最正常的方式
OPSTAT ihu_isr_install(UINT8 priority, void *my_routine)
{

        return IHU_SUCCESS;
}

/**********************************************************************************
 *
 *   MAIN函数入口，供应上层使用
 *
 **********************************************************************************/
int ihu_vm_main(void) 
{
	//硬件初始化
	ihu_vm_system_ctr_table_init();

	//从数据库或者系统缺省配置中，读取系统配置的工程参数
	//省略

	//启动所有任务
  ihu_task_create_all();

	//wait for ever，由于FreeRTOS需要vTaskStartScheduler()以后才能执行，所以这里不能阻塞
//	while (1){
//		ihu_sleep(60); //可以设置为5秒的定时，甚至更长
//		ihu_vm_check_task_que_status_and_action();
//	}

	//清理现场环境，永远到达不了，清掉以消除COMPILE WARNING
	//ihu_task_delete_all_and_queue();

	//永远到达不了，清掉以消除COMPILE WARNING
	return EXIT_SUCCESS;
}

//TBD
void ihu_vm_check_task_que_status_and_action(void)
{
	return;
}

OPSTAT ihu_vm_send_init_msg_to_app_task(UINT8 dest_id)
{
	int ret = 0;
	msg_struct_com_init_t snd;
	
	//入参检查
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return IHU_FAILURE;
	}
		
	//发送初始化消息给目标任务，以便初始化所有任务模块
	memset(&snd, 0, sizeof(msg_struct_com_init_t));
	snd.length = sizeof(msg_struct_com_init_t);
	ret = ihu_message_send(MSG_ID_COM_INIT, dest_id, TASK_ID_VMFO, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_VMFO].taskName, zIhuVmCtrTab.task[dest_id].taskName);
	}
	
	return ret;
}

