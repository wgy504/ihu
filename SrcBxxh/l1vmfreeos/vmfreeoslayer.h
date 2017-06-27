/*
 * vmfreeoslayer.h
 *
 *  Created on: 2016年9月18日
 *      Author: test
 */

#ifndef L1VMFREEOS_VMFREEOSLAYER_H_
#define L1VMFREEOS_VMFREEOSLAYER_H_

//自行标准配置
#include "comtype.h"
#include "sysversion.h"
#include "sysdim.h"
#include "sysconfig.h"
#include "sysengpar.h"
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	#include "commsgemc68.h"
  #include "osal.h"
  #include "resmgmt.h"
  #include "hw_cpm.h"
  #include "hw_gpio.h"
  #include "hw_watchdog.h"
  #include "sys_clock_mgr.h"
  #include "sys_power_mgr.h"
	
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
  #include "commsgccl.h"
  #include "FreeRTOS.h"
	#include "task.h"
  #include "queue.h"
	#include "timers.h"	
	#include "semphr.h"
	#include "bsp_adc.h"
	#include "bsp_usart.h"
	#include "bsp_led.h"
	#include "bsp_i2c.h"
	#include "bsp_rtc.h"
	#include "bsp_calendar.h"
	//#include "bsp_wdog.h"
	#include "bsp_cpuid.h"
	#include "bsp_stmflash.h"
	#include "bsp_camera.h"
	
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
  #include "commsgbfsc.h"
  #include "FreeRTOS.h"
	#include "task.h"
  #include "queue.h"
	#include "timers.h"	
	#include "semphr.h"
	#include "bsp_adc.h"
	#include "bsp_usart.h"	
	#include "bsp_led.h"
	#include "bsp_i2c.h"
	#include "bsp_can.h"
	//#include "bsp_wdog.h"
	#include "bsp_crc.h"
	#include "bsp_rtc.h"
	#include "bsp_calendar.h"
	#include "bsp_cpuid.h"
	#include "bsp_stmflash.h"

#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)
  #include "commsgiap.h"
  #include "FreeRTOS.h"
	#include "task.h"
  #include "queue.h"
	#include "timers.h"	
	#include "semphr.h"
	#include "bsp_usart.h"	
	#include "bsp_led.h"
	#include "bsp_can.h"
	//#include "bsp_wdog.h"
	#include "bsp_rtc.h"
	#include "bsp_calendar.h"
	#include "bsp_cpuid.h"
	#include "bsp_stmflash.h"

	
#else
	#error Un-correct constant definition
#endif

#define IHU_WORKING_FREE_RTOS_SELECTION_BARE 1
#define IHU_WORKING_FREE_RTOS_SELECTION_OSAL 2
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
  #define IHU_WORKING_FREE_RTOS_SELECTION IHU_WORKING_FREE_RTOS_SELECTION_OSAL
#elif ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)\
	|| (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID) \
	|| (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID))
  #define IHU_WORKING_FREE_RTOS_SELECTION IHU_WORKING_FREE_RTOS_SELECTION_BARE
#else
	#error Un-correct constant definition
#endif

#if (IHU_WORKING_FREE_RTOS_SELECTION == IHU_WORKING_FREE_RTOS_SELECTION_BARE)	
	#define OS_MUTEX                SemaphoreHandle_t
#endif

/*
 *	
 *  定义结构体
 *
 */

//定义TASK对应的名字
//!!!!!!!!!!!!!!!!ATTENTION!!!!!!!!!!!!!!!!
//不是任务的任务，比如TRACE/CONFIG/3G/GPIO等等，留待清理，简化任务列表
/*
 *
 *   系统全局常量定义区
 *
 *   【增加任务】，必须同时修改四个地方：
 *   - IHU_TASK_NAME_ID
 *   - IHU_TASK_QUEUE_ID
 *   - zIhuVmCtrTaskStaticCfg
 *   - 还要修改可能的本地配置文件，或者sysengpar.h的固定工参配置信息，#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID
 *	 - 继续修改初始化函数void ihu_vm_system_ctr_table_init(void)
 *	
 *	基础模块：TASK_ID_MIN, TASK_ID_VMFO, TASK_ID_TIMER, TASK_ID_ADCLIBRA, TASK_ID_SPILEO, TASK_ID_I2CARIES, TASK_ID_PWMTAURUS
 *	          TASK_ID_SPSVIRGO, TASK_ID_CANVELA, TASK_ID_DIDOCAP, TASK_ID_LEDPISCES, TASK_ID_ETHORION, TASK_ID_DCMIARIS, 
 *            TASK_ID_EMC68X, TASK_ID_CCL, TASK_ID_MAX, TASK_ID_INVALID
 */
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	enum IHU_TASK_NAME_ID
	{
		TASK_ID_MIN = 0,
		TASK_ID_VMFO,
		TASK_ID_TIMER,
		TASK_ID_EMC68X,
		TASK_ID_MAX,
		TASK_ID_INVALID = 0xFF,
	}; //end of IHU_TASK_NAME_ID
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	enum IHU_TASK_NAME_ID
	{
		TASK_ID_MIN = 0,
		TASK_ID_VMFO,
		TASK_ID_TIMER,
		TASK_ID_ADCLIBRA,
		//TASK_ID_SPILEO,
		TASK_ID_I2CARIES,
		//TASK_ID_PWMTAURUS,
		//TASK_ID_CANVELA,
		TASK_ID_SPSVIRGO,
		//TASK_ID_CANVELA,
		TASK_ID_DIDOCAP,
		TASK_ID_LEDPISCES,
		//TASK_ID_ETHORION,
		TASK_ID_DCMIARIS,	
		TASK_ID_CCL,
		TASK_ID_MAX,
		TASK_ID_INVALID = 0xFF,
	}; //end of IHU_TASK_NAME_ID
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	enum IHU_TASK_NAME_ID
	{
		TASK_ID_MIN = 0,
		TASK_ID_VMFO,
		TASK_ID_TIMER,
		TASK_ID_ADCLIBRA,
		TASK_ID_SPILEO,
		TASK_ID_I2CARIES,
		//TASK_ID_PWMTAURUS,
		TASK_ID_CANVELA,
		//TASK_ID_SPSVIRGO,
		//TASK_ID_DIDOCAP,
		TASK_ID_LEDPISCES,
		//TASK_ID_ETHORION,
		//TASK_ID_DCMIARIS,	
		TASK_ID_BFSC,
		TASK_ID_MAX,
		TASK_ID_INVALID = 0xFF,
	}; //end of IHU_TASK_NAME_ID
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)
	enum IHU_TASK_NAME_ID
	{
		TASK_ID_MIN = 0,
		TASK_ID_VMFO,
		TASK_ID_TIMER,
		//TASK_ID_ADCLIBRA,
		//TASK_ID_SPILEO,
		//TASK_ID_I2CARIES,
		//TASK_ID_PWMTAURUS,
		TASK_ID_CANVELA,
		TASK_ID_SPSVIRGO,
		//TASK_ID_DIDOCAP,
		TASK_ID_LEDPISCES,
		//TASK_ID_ETHORION,
		//TASK_ID_DCMIARIS,	
		//TASK_ID_BFSC,
		TASK_ID_IAP,
		TASK_ID_MAX,
		TASK_ID_INVALID = 0xFF,
	}; //end of IHU_TASK_NAME_ID	
#else  //为了提供完整表达
	enum IHU_TASK_NAME_ID
	{
		TASK_ID_MIN = 0,
		TASK_ID_VMFO,
		TASK_ID_TIMER,
		TASK_ID_ADCLIBRA,
		TASK_ID_SPILEO,
		TASK_ID_I2CARIES,
		TASK_ID_PWMTAURUS,
		TASK_ID_SPSVIRGO,
		TASK_ID_CANVELA,
		TASK_ID_DIDOCAP,
		TASK_ID_LEDPISCES,
		TASK_ID_ETHORION,
		TASK_ID_DCMIARIS,	
		TASK_ID_BFSC,
		TASK_ID_MAX,
		TASK_ID_INVALID = 0xFF,
	}; //end of IHU_TASK_NAME_ID
#endif


/*
 *
 *   三大表单之一： 系统总控表IhuVmCtrTab_t定义区
 *
 *
*/
	
/*
** Fsm INFORMATION structure.
*/
#define FSM_STATE_ENTRY  0x00
#define FSM_STATE_IDLE  0x01
#define FSM_STATE_COMMON  0x02
#define FSM_STATE_END   0xFE
#define FSM_STATE_INVALID 0xFF

//FSM的基础结构定义
typedef struct IhuFsmStateItem
{
	UINT16 msg_id;
	UINT8 state;
	OPSTAT (*stateFunc)(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len);
}IhuFsmStateItem_t;
//任务表
typedef struct IhuTaskTag
{
	UINT8  TaskId;
	UINT8  pnpState;
	QueueHandle_t  QueId;
	UINT8  state;
	char   taskName[IHU_SYSDIM_TASK_NAME_LEN_MAX];
	IhuFsmStateItem_t *fsmPtr;
	xTaskHandle TaskHandle;
	void*  taskFuncEntry;
}IhuTaskTag_t;
//状态机列表
typedef struct IhuFsmArrayElement
{
	OPSTAT (*stateFunc)(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len);
}IhuFsmArrayElement_t;
//状态机控制表
typedef struct IhuFsmCtrlTable
{
	UINT8 numOfIhuFsmArrayElement;  //每一个具体任务TASK中，定义了多少个STATE-MSGID映射表单
	UINT8 taskId;
	IhuFsmArrayElement_t pFsmArray[IHU_SYSDIM_TASK_STATE_NBR_MAX][IHU_SYSDIM_MSGID_NBR_MAX];
}IhuFsmCtrlTable_t;
//MsgQueue的基础结构定义
typedef struct IhuFsmQueueElement
{
	UINT8 msgQue[IHU_SYSDIM_MSG_BODY_LEN_MAX];
	bool useFlag;
}IhuFsmQueueElement_t;
//队列列表
typedef struct IhuFsmQueueListTable
{
	IhuFsmQueueElement_t queList[IHU_SYSDIM_MSG_QUEUE_NBR_MAX];
	UINT8 queIndex;
}IhuFsmQueueListTable_t;
//状态机总表
typedef struct IhuFsmTable
{
	UINT8 numOfIhuFsmCtrlTable;  //Number of running (Task + Instance)
	UINT8 currentTaskId;  //transfer task_id to launched FSM machine, then useless
	IhuFsmCtrlTable_t  pIhuFsmCtrlTable[IHU_SYSDIM_TASK_NBR_MAX];  //所有任务的状态机总控表
	IhuFsmQueueListTable_t taskQue[IHU_SYSDIM_TASK_NBR_MAX];  //所有任务的消息队列总控表
}IhuFsmTable_t;
//打印缓冲区
typedef struct IhuPrintBufferChar
{
	UINT8 localIndex;
	char  PrintHeader[IHU_SYSDIM_PRINT_FILE_LINE_NBR_MAX];
	char  PrintBuffer[IHU_SYSDIM_PRINT_CHAR_SIZE_MAX];
}IhuPrintBufferChar_t;
//打印控制表
typedef struct IhuPrintBufCtrlTab
{
	IhuPrintBufferChar_t 	prtBuf[IHU_SYSDIM_PRINT_BUF_NBR_MAX];
	unsigned int 					prtIndex;
#if (IHU_WORKING_FREE_RTOS_SELECTION == IHU_WORKING_FREE_RTOS_SELECTION_BARE)		
	OS_MUTEX 							prtMutex;	
#endif	
}IhuPrintBufCtrlTab_t;

//三表之一的系统总控表
typedef struct IhuVmCtrTab
{
	IhuFsmTable_t					fsm;
	IhuTaskTag_t					task[IHU_SYSDIM_TASK_NBR_MAX];
	IhuPrintBufCtrlTab_t	print;
}IhuVmCtrTab_t;
//系统总控表
extern IhuVmCtrTab_t zIhuVmCtrTab;

//统一定义，如果不存在不影响编译
extern IhuFsmStateItem_t IhuFsmVmfo[];                           	//状态机
extern IhuFsmStateItem_t IhuFsmTimer[];                           //状态机
extern IhuFsmStateItem_t IhuFsmAdclibra[];                        //状态机
extern IhuFsmStateItem_t IhuFsmSpileo[];                          //状态机
extern IhuFsmStateItem_t IhuFsmI2caries[];                        //状态机
extern IhuFsmStateItem_t IhuFsmPwmtaurus[];                       //状态机
extern IhuFsmStateItem_t IhuFsmSpsvirgo[];                        //状态机
extern IhuFsmStateItem_t IhuFsmCanvela[];                      	  //状态机
extern IhuFsmStateItem_t IhuFsmDidocap[];                         //状态机
extern IhuFsmStateItem_t IhuFsmLedpisces[];                       //状态机
extern IhuFsmStateItem_t IhuFsmEthorion[];                        //状态机
extern IhuFsmStateItem_t IhuFsmDcmiaris[];                        //状态机
extern IhuFsmStateItem_t IhuFsmEmc68x[];                          //状态机
extern IhuFsmStateItem_t IhuFsmCcl[];                          	  //状态机
extern IhuFsmStateItem_t IhuFsmBfsc[];                          	//状态机 
extern IhuFsmStateItem_t IhuFsmIap[];                            	//状态机 

//任务TASK初始化配置静态表
typedef struct IhuVmCtrTaskStaticCfg
{
	const UINT8 taskInputId;
	const char  taskInputName[IHU_SYSDIM_TASK_NAME_LEN_MAX];
	void* fsmFuncEntry;
	const UINT8 pnpFlag;
	const UINT8 traceCtrFlag;
	const UINT8 traceModToAllowFlag;
	const UINT8 traceModToRestrictFlag;
	const UINT8 traceModFromAllowFlag;
	const UINT8 traceModFromRestrictFlag;	
}IhuVmCtrTaskStaticCfg_t;

//消息MSG初始化配置静态表
typedef struct IhuVmCtrMsgStaticCfg
{
	const UINT16 msgId;
	const char  msgName[IHU_SYSDIM_MSGID_NAME_LEN_MAX];
	const UINT8 traceCtrFlag;
	const UINT8 traceMsgAllowFlag;
	const UINT8 traceMsgRestrictFlag;
}IhuVmCtrMsgStaticCfg_t;


/*
 *
 *   三大表单之二： 系统工参表IhuSysEngPar_t定义区
 *
 *
*/
//定时器精度定义
#define TIMER_RESOLUTION_NULL 0
#define TIMER_RESOLUTION_1S 1
#define TIMER_RESOLUTION_10MS 2
#define TIMER_RESOLUTION_1MS 3
#define TIMER_RESOLUTION_INVALID 0xFF

enum IHU_TIMER_ID_ALL
{
	TIMER_ID_MIN = 0,
	TIMER_ID_1S_MIN,
	TIMER_ID_1S_VMFO_INIT_FB,
	TIMER_ID_1S_VMFO_PERIOD_SCAN,
	TIMER_ID_1S_ADCLIBRA_PERIOD_SCAN,
	TIMER_ID_1S_DIDOCAP_PERIOD_SCAN,
	TIMER_ID_1S_ETHORION_PERIOD_SCAN,
	TIMER_ID_1S_CANVELA_PERIOD_SCAN,
	TIMER_ID_1S_I2CARIES_PERIOD_SCAN,
	TIMER_ID_1S_LEDPISCES_PERIOD_SCAN,
	TIMER_ID_1S_LEDPISCES_GALOWAG_SCAN,	
	TIMER_ID_1S_PWMTAURUS_PERIOD_SCAN,
	TIMER_ID_1S_SPILEO_PERIOD_SCAN,
	TIMER_ID_1S_SPSVIRGO_PERIOD_SCAN,
	TIMER_ID_1S_DCMIARIS_PERIOD_SCAN,
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	TIMER_ID_1S_EMC68X_PERIOD_SCAN,
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	TIMER_ID_1S_CCL_PERIOD_SCAN,
	//TIMER_ID_1S_CCL_EVENT_REPORT_PEROID_SCAN,
	TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE,    			//锁具最长的激活时间
	TIMER_ID_1S_CCL_LOCK_WORK_WAIT_TO_OPEN,   //锁被打开，等待开门的时间
	//TIMER_ID_1S_CCL_DIDO_TRIGGER_PERIOD_SCAN,  //SLEEP模式下触发工作的定时器：G502改版以后不再需要
	//TIMER_ID_1S_CCL_DIDO_WORKING_PERIOD_SCAN,  //WORKING模式下外部传感器的定时扫描：G502改版以后不再需要
	//TIMER_ID_1S_CCL_SPS_WORKING_PERIOD_SCAN,   //WORKING模式下外部传感器的定时扫描，并未启用
	//TIMER_ID_1S_CCL_I2C_WORKING_PERIOD_SCAN,   //WORKING模式下外部传感器的定时扫描，并未启用
	//TIMER_ID_1S_CCL_DCMI_WORKING_PERIOD_SCAN,	 //WORKING模式下外部传感器的定时扫描，并未启用
	//TIMER_ID_1S_CCL_ADC_WORKING_PERIOD_SCAN,	 //WORKING模式下外部传感器的定时扫描，并未启用
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
  TIMER_ID_1S_BFSC_STARTUP_IND,
	TIMER_ID_1S_BFSC_PERIOD_SCAN,
	TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER,
	TIMER_ID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER,
	TIMER_ID_1S_BFSC_L3BFSC_GIVE_UP_TIMER,
	TIMER_ID_1S_BFSC_HEART_BEAT,
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)	
  TIMER_ID_1S_IAP_PERIOD_SCAN,
#else
#endif	
	TIMER_ID_1S_MAX,
	TIMER_ID_10MS_MIN,
	TIMER_ID_10MS_VMFO_TEST,
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	TIMER_ID_10MS_BFSC_ADCLIBRA_SCAN_TIMER,
	TIMER_ID_10MS_BFSC_STARTUP_IND,
	TIMER_ID_10MS_BFSC_PERIOD_SCAN,
#else
#endif		
	TIMER_ID_10MS_MAX,
	TIMER_ID_1MS_MIN,
	TIMER_ID_1MS_VMFO_TEST,
	TIMER_ID_1MS_MAX,
	TIMER_ID_MAX,
};

//不同精度定时器是否激活的开关
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	#define IHU_TIMER_CONFIG_START_RESOLUTION_1S TRUE
	#define IHU_TIMER_CONFIG_START_RESOLUTION_10MS FALSE
	#define IHU_TIMER_CONFIG_START_RESOLUTION_1MS FALSE
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)	
	#define IHU_TIMER_CONFIG_START_RESOLUTION_1S TRUE
	#define IHU_TIMER_CONFIG_START_RESOLUTION_10MS FALSE
	#define IHU_TIMER_CONFIG_START_RESOLUTION_1MS FALSE
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	#define IHU_TIMER_CONFIG_START_RESOLUTION_1S TRUE
	#define IHU_TIMER_CONFIG_START_RESOLUTION_10MS TRUE
	#define IHU_TIMER_CONFIG_START_RESOLUTION_1MS FALSE
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)	
	#define IHU_TIMER_CONFIG_START_RESOLUTION_1S TRUE
	#define IHU_TIMER_CONFIG_START_RESOLUTION_10MS FALSE
	#define IHU_TIMER_CONFIG_START_RESOLUTION_1MS FALSE
#else
	#error Un-correct constant definition
#endif

//全局工参
extern IhuSysEngParTab_t zIhuSysEngPar;                   

//消息初始化的数据表单
typedef struct IhuSysEngTimerStaticCfg
{
	const UINT32 timerId;
	const char   timerName[IHU_SYSDIM_TIMERID_NAME_LEN_MAX];
	const INT32  timerDur;
	const UINT8  timerGranularity;
}IhuSysEngTimerStaticCfg_t;

/*
 *
 *   三大表单之三： 系统运行统计性能表IhuSysStaPm_t定义区
 *
 *
*/

//三大表之三：系统统计性能表


//任务模块RESTART的一些全局定义
#define IHU_RUN_ERROR_LEVEL_0_WARNING 10
#define IHU_RUN_ERROR_LEVEL_1_MINOR 100
#define IHU_RUN_ERROR_LEVEL_2_MAJOR 1000
#define IHU_RUN_ERROR_LEVEL_3_CRITICAL 10000
#define IHU_RUN_ERROR_LEVEL_4_DEAD 100000

//全局Counter，用于性能指标统计之用
typedef struct IhuGlobalCounter
{
	UINT32 errCnt[IHU_SYSDIM_TASK_NBR_MAX];  //以每个任务为单位
	UINT32 restartCnt;
}IhuGlobalCounter_t;


typedef struct IhuSysStaPm
{
	IhuGlobalCounter_t 	statisCnt;
	UINT32 							taskRunErrCnt[IHU_SYSDIM_TASK_NBR_MAX];
}IhuSysStaPm_t;
extern IhuSysStaPm_t	zIhuSysStaPm;



/*
 *
 *   通信机制核心API，对外提供的基础服务
 *
 *
*/

//全局API，不依赖具体操作系统
void IhuDebugPrintFo(UINT8 index, char *format, ...);
void IhuErrorPrintFo(UINT8 index, char *format, ...);
UINT8 IhuDebugPrintId(char *file, int line);
//Eclipse的编译器有些不一样，晕乎
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	void IhuDebugPrintFoEmc68x(char *format, ...);
	void IhuErrorPrintFoEmc68x(char *format, ...);
	#define IhuDebugPrint IhuDebugPrintFoEmc68x
	#define IhuErrorPrint IhuErrorPrintFoEmc68x
#else
	#define IhuDebugPrint(...) (((void (*)(UINT8, const char *, ...))IhuDebugPrintFo)(IhuDebugPrintId(__FILE__, __LINE__), __VA_ARGS__))
	#define IhuErrorPrint(...) (((void (*)(UINT8, const char *, ...))IhuErrorPrintFo)(IhuDebugPrintId(__FILE__, __LINE__), __VA_ARGS__))
#endif
//VM FSM related APIs，状态机核心部分，不依赖具体操作系统
extern OPSTAT FsmInit(void);
extern OPSTAT FsmAddNew(UINT8 task_id, IhuFsmStateItem_t* pIhuFsmStateItem);
extern OPSTAT FsmRemove(UINT8 task_id);
extern OPSTAT FsmRunEngine(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len);
extern OPSTAT FsmProcessingLaunch(void *task);
extern OPSTAT FsmSetState(UINT8 task_id, UINT8 newState);
extern UINT8  FsmGetState(UINT8 task_id);

/*
 *
 *   通信机制核心API，对外提供的基础服务
 *
 *
*/

//Global VM layer basic API and functions，跟具体操作系统相关的API部分
extern OPSTAT ihu_message_queue_create(UINT8 task_id);
extern OPSTAT ihu_message_queue_delete(UINT8 task_id);
extern QueueHandle_t ihu_message_queue_inquery(UINT8 task_id);
extern OPSTAT ihu_message_queue_resync(void);
extern OPSTAT ihu_message_queue_clean(UINT8 dest_id);
extern OPSTAT ihu_message_send(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len); //message send
extern OPSTAT ihu_message_send_isr(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len); //message send //MYC
extern OPSTAT ihu_message_rcv(UINT8 dest_id, IhuMsgSruct_t *msg);
extern OPSTAT ihu_task_create(UINT8 task_id, void *(*task_func)(void *), void *arg, int prio);
extern OPSTAT ihu_task_delete(UINT8 task_id);
extern OPSTAT ihu_task_create_and_run(UINT8 task_id, IhuFsmStateItem_t* pIhuFsmStateItem);
extern OPSTAT ihu_system_task_init_call(UINT8 task_id, IhuFsmStateItem_t *p);
extern void   ihu_task_create_all(void);
extern OPSTAT fsm_com_do_nothing(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_com_heart_beat_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern void ihu_sw_restart(void);
extern struct tm ihu_clock_unix_to_ymd(time_t t_unix);
extern OPSTAT ihu_isr_install(UINT8 priority, void *my_routine);
//对于没有RTOS的情形，为了兼容VMDA，暂时没用到
extern OPSTAT FsmProcessingLaunchEntryBareRtos(UINT8 task_id);   //当创建和启动分离时使用
extern OPSTAT FsmProcessingLaunchExecuteBareRtos(UINT8 task_id); //当创建和启动分离时使用
extern OPSTAT ihu_message_send_bare_rtos(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len); //message send
extern OPSTAT ihu_message_rcv_bare_rtos(UINT8 dest_id, IhuMsgSruct_t *msg);
extern OPSTAT ihu_system_task_execute_bare_rtos(UINT8 task_id, IhuFsmStateItem_t *p);
extern void   ihu_task_execute_all_bare_rtos(void);

/*
 *
 *   HCU初始化核心API，本地私有使用
 *
 *
*/
//系统级别的初始化
extern void ihu_vm_system_ctr_table_init(void);  
extern void ihu_sleep(UINT32 second);
extern void ihu_usleep(UINT32 usecond);  //resulution 10^(-3)s = 1 microsecond
extern OPSTAT ihu_taskid_to_string(UINT8 id, char *string);
extern OPSTAT ihu_msgid_to_string(UINT8 id, char *string);
extern OPSTAT ihu_timerid_to_string(UINT8 id, char *string);
extern int  ihu_vm_main(void);
extern void ihu_vm_check_task_que_status_and_action(void);
extern OPSTAT ihu_vm_send_init_msg_to_app_task(UINT8 dest_id);

/*
 *
 *  外部参考API
 *
 *
*/

//外部引用API，来自于TIMER任务模块。TIMER任务模块的机制是，必须将VM启动起来，然后TIMER上层任务模块才能被激活，并产生自定义的TIME_OUT消息
extern OPSTAT ihu_timer_start(UINT8 task_id, UINT8 timer_id, UINT32 t_dur, UINT8 t_type, UINT8 t_res);
extern OPSTAT ihu_timer_stop(UINT8 task_id, UINT8 timer_id, UINT8 t_res);
extern void ihu_timer_routine_handler_1s(void);
extern void ihu_timer_routine_handler_10ms(void);


/*
 *
 *  提高效率的宏定义API部分
 *
 *
*/

//错误打印宏定义高级定义技巧
//第一种方式，具备编译通过的能力，但显示错误
//#define IHU_ERROR_PRINT_TASK(taskid, arg...)	do{zIhuSysStaPm.taskRunErrCnt[taskid]++;  IhuErrorPrint(##arg);  return IHU_FAILURE;}while(0)	
//第二种方式，貌似好看些
#define IHU_ERROR_PRINT_TASK(taskid, ...)	do{zIhuSysStaPm.taskRunErrCnt[taskid]++;  IhuErrorPrint(__VA_ARGS__);  return IHU_FAILURE;}while(0)	
//都保留着，都是极为有参考意义的宏编写方式

//高级定义，简化程序的可读性
#define IHU_DEBUG_PRINT_INF		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint
#define IHU_DEBUG_PRINT_NOR		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE) IhuDebugPrint
#define IHU_DEBUG_PRINT_IPT		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_IPT_ON) != FALSE) IhuDebugPrint
#define IHU_DEBUG_PRINT_CRT		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_CRT_ON) != FALSE) IhuDebugPrint
#define IHU_DEBUG_PRINT_FAT		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE) IhuDebugPrint




/*
 *	
 *	
 *	
 *  底层L0BSP的函数映射，以便上层在未来更改BSP的时候，可以保持上层应用的不变性
 *  映射到的目标函数命名为L1HD，意味着L1_Hardware_Driver抽象函数
 *	
 *	
 *
 */

//L0BSP=>L1HD: ADC
//ADC的IT工作模式必须先启动，读数，然后停止。ADC的初始化已经在MAIN初始化中完成了
#define ihu_l1hd_adc_cpu_temp_start 						ihu_bsp_stm32_adc_cpu_temp_start
#define ihu_l1hd_adc_cpu_temp_stop 							ihu_bsp_stm32_adc_cpu_temp_stop
#define ihu_l1hd_adc1_start											ihu_bsp_stm32_adc1_start
#define ihu_l1hd_adc1_get_sample_value 					ihu_bsp_stm32_adc1_get_sample_value
#define ihu_l1hd_adc1_stop				 							ihu_bsp_stm32_adc1_stop
#define ihu_l1hd_adc2_start 										ihu_bsp_stm32_adc2_start
#define ihu_l1hd_adc2_get_sample_value 					ihu_bsp_stm32_adc2_get_sample_value
#define ihu_l1hd_adc2_stop 											ihu_bsp_stm32_adc2_stop
#define ihu_l1hd_adc3_start				 							ihu_bsp_stm32_adc3_start
#define ihu_l1hd_adc3_get_sample_value 					ihu_bsp_stm32_adc3_get_sample_value
#define ihu_l1hd_adc3_stop				 							ihu_bsp_stm32_adc3_stop
#define ihu_l1hd_adc1_start_in_DMA_mode 				ihu_bsp_stm32_adc1_start_in_DMA_mode
#define ihu_l1hd_adc2_start_in_DMA_mode					ihu_bsp_stm32_adc2_start_in_DMA_mode
#define ihu_l1hd_adc3_start_in_DMA_mode					ihu_bsp_stm32_adc3_start_in_DMA_mode       
#define ihu_l1hd_adc1_ccl_get_battery_value			ihu_bsp_stm32_adc1_ccl_get_battery_value

//L0BSP=>L1HD: LED 
#define ihu_l1hd_led_power_f2board_on 					ihu_bsp_stm32_led_power_f2board_on
#define ihu_l1hd_led_power_f2board_off					ihu_bsp_stm32_led_power_f2board_off
#define ihu_l1hd_led_power_f2board_toggle 			ihu_bsp_stm32_led_power_f2board_toggle
#define ihu_l1hd_led_commu_f2board_on 					ihu_bsp_stm32_led_commu_f2board_on
#define ihu_l1hd_led_commu_f2board_off 					ihu_bsp_stm32_led_commu_f2board_off
#define ihu_l1hd_led_commu_f2board_toggle 			ihu_bsp_stm32_led_commu_f2board_toggle
#define ihu_l1hd_led_work_state_f2board_on	 		ihu_bsp_stm32_led_work_state_f2board_on
#define ihu_l1hd_led_work_state_f2board_off 		ihu_bsp_stm32_led_work_state_f2board_off
#define ihu_l1hd_led_work_state_f2board_toggle 	ihu_bsp_stm32_led_work_state_f2board_toggle
#define ihu_l1hd_beep_f2board_on	 							ihu_bsp_stm32_beep_f2board_on
#define ihu_l1hd_beep_f2board_off 							ihu_bsp_stm32_beep_f2board_off
#define ihu_l1hd_beep_f2board_toggle 						ihu_bsp_stm32_beep_f2board_toggle
#define ihu_l1hd_beep_f2board_pattern_05s	 			ihu_bsp_stm32_beep_f2board_pattern_05s
#define ihu_l1hd_beep_f2board_pattern_1s	 			ihu_bsp_stm32_beep_f2board_pattern_1s
#define ihu_l1hd_beep_f2board_pattern_2s	 			ihu_bsp_stm32_beep_f2board_pattern_2s

//LED->BFSC独特业务部分
#define ihu_l1hd_led_serv1_f2board_on 					ihu_bsp_stm32_led_serv1_f2board_on
#define ihu_l1hd_led_serv1_f2board_off					ihu_bsp_stm32_led_serv1_f2board_off
#define ihu_l1hd_led_serv1_f2board_toggle 			ihu_bsp_stm32_led_serv1_f2board_toggle
#define ihu_l1hd_led_serv2_f2board_on 					ihu_bsp_stm32_led_serv2_f2board_on
#define ihu_l1hd_led_serv2_f2board_off					ihu_bsp_stm32_led_serv2_f2board_off
#define ihu_l1hd_led_serv2_f2board_toggle 			ihu_bsp_stm32_led_serv2_f2board_toggle
#define ihu_l1hd_led_serv3_f2board_on 					ihu_bsp_stm32_led_serv3_f2board_on
#define ihu_l1hd_led_serv3_f2board_off					ihu_bsp_stm32_led_serv3_f2board_off
#define ihu_l1hd_led_serv3_f2board_toggle 			ihu_bsp_stm32_led_serv3_f2board_toggle


//待去掉部分，纯粹用于207VC板子的测试
#define ihu_l1hd_led_f2board_on 								ihu_bsp_stm32_led_f2board_on
#define ihu_l1hd_led_f2board_off 								ihu_bsp_stm32_led_f2board_off
#define ihu_l1hd_led_f2board_negation 					ihu_bsp_stm32_led_f2board_negation
#define ihu_l1hd_led_f2board_test_flag 					ihu_bsp_stm32_led_f2board_test_flag
#define ihu_l1hd_led_f2board_timer_ind_on 			ihu_bsp_stm32_led_f2board_timer_ind_on
#define ihu_l1hd_led_f2board_timer_ind_off 			ihu_bsp_stm32_led_f2board_timer_ind_off
#define ihu_l1hd_led_f2board_timer_ind_negation ihu_bsp_stm32_led_f2board_timer_ind_negation

//L0BSP=>L1HD: USART
#define ihu_l1hd_sps_slave_hw_init							ihu_bsp_stm32_sps_slave_hw_init
#define ihu_l1hd_sps_gprs_send_data 		  			ihu_bsp_stm32_sps_gprs_send_data
#define ihu_l1hd_sps_gprs_rcv_data 		  				ihu_bsp_stm32_sps_gprs_rcv_data
#define ihu_l1hd_sps_gprs_rcv_data_timeout 		  ihu_bsp_stm32_sps_gprs_rcv_data_timeout
#define ihu_l1hd_sps_rfid_send_data 						ihu_bsp_stm32_sps_rfid_send_data
#define ihu_l1hd_sps_rfid_rcv_data 		  				ihu_bsp_stm32_sps_rfid_rcv_data
#define ihu_l1hd_sps_rfid_rcv_data_timeout 		  ihu_bsp_stm32_sps_rfid_rcv_data_timeout
#define ihu_l1hd_sps_ble_send_data 							ihu_bsp_stm32_sps_ble_send_data
#define ihu_l1hd_sps_ble_rcv_data 		  				ihu_bsp_stm32_sps_ble_rcv_data
#define ihu_l1hd_sps_ble_rcv_data_timeout 		  ihu_bsp_stm32_sps_ble_rcv_data_timeout
#define ihu_l1hd_sps_cam_send_data 							ihu_bsp_stm32_sps_cam_send_data
#define ihu_l1hd_sps_cam_rcv_data 		  				ihu_bsp_stm32_sps_cam_rcv_data
#define ihu_l1hd_sps_cam_rcv_data_timeout 			ihu_bsp_stm32_sps_cam_rcv_data_timeout
#define ihu_l1hd_sps_print_send_data 						ihu_bsp_stm32_sps_print_send_data
#define ihu_l1hd_sps_print_rcv_data 		  			ihu_bsp_stm32_sps_print_rcv_data
#define ihu_l1hd_sps_print_rcv_data_timeout 		ihu_bsp_stm32_sps_print_rcv_data_timeout
#define ihu_l1hd_sps_spare1_send_data 					ihu_bsp_stm32_sps_spare1_send_data
#define ihu_l1hd_sps_spare1_rcv_data 		  			ihu_bsp_stm32_sps_spare1_rcv_data
#define ihu_l1hd_sps_spare1_rcv_data_timeout 		ihu_bsp_stm32_sps_spare1_rcv_data_timeout
#define ihu_l1hd_sps_spare2_send_data 					ihu_bsp_stm32_sps_spare2_send_data
#define ihu_l1hd_sps_spare2_rcv_data 		  			ihu_bsp_stm32_sps_spare2_rcv_data
#define ihu_l1hd_sps_spare2_rcv_data_timeout 		ihu_bsp_stm32_sps_spare2_rcv_data_timeout

//L0BSP=>L1HD: SPI
//IAU
#define ihu_l1hd_spi_slave_hw_init		  				ihu_bsp_stm32_spi_slave_hw_init
#define ihu_l1hd_spi_iau_send_data 							ihu_bsp_stm32_spi_iau_send_data
#define ihu_l1hd_spi_iau_receive_data 					ihu_bsp_stm32_spi_iau_rcv_data
#define ihu_l1hd_spi_spare1_send_data 					ihu_bsp_stm32_spi_spare1_send_data
#define ihu_l1hd_spi_spare1_receive_data 				ihu_bsp_stm32_spi_spare1_rcv_data
//BFSC AD Scale
#define ihu_l1hd_spi_ad_scale_send_data 				ihu_bsp_stm32_spi_ad_scale_send_data
#define ihu_l1hd_spi_ad_scale_receive_data 			ihu_bsp_stm32_spi_ad_scale_rcv_data
//MFRC522
#define ihu_l1hd_spi_rfid522_send_data 					ihu_bsp_stm32_spi_rfid522_send_data
#define ihu_l1hd_spi_rfid522_receive_data 			ihu_bsp_stm32_spi_rfid522_rcv_data
#define ihu_l1hd_spi_flash_read_byte           	ihu_bsp_stm32_spi_flash_read_byte
#define ihu_l1hd_spi_flash_send_byte 	          ihu_bsp_stm32_spi_flash_send_byte
#define ihu_l1hd_spi_mf522_cs_enable   		    	ihu_bsp_stm32_spi_mf522_cs_enable
#define ihu_l1hd_spi_mf522_cs_disable   		  	ihu_bsp_stm32_spi_mf522_cs_disable
#define ihu_l1hd_spi_mf522_reset_enable   		  ihu_bsp_stm32_spi_mf522_reset_enable
#define ihu_l1hd_spi_mf522_reset_disable   		  ihu_bsp_stm32_spi_mf522_reset_disable


//NRF24L01
#define ihu_l1hd_spi_nrf24l01_cs_enable   			ihu_bsp_stm32_spi_nrf24l01_cs_enable
#define ihu_l1hd_spi_nrf24l01_cs_disable   			ihu_bsp_stm32_spi_nrf24l01_cs_disable
#define ihu_l1hd_spi_nrf24l01_ce_low   		    	ihu_bsp_stm32_spi_nrf24l01_ce_low
#define ihu_l1hd_spi_nrf24l01_ce_high     			ihu_bsp_stm32_spi_nrf24l01_ce_high
#define ihu_l1hd_spi_nrf24l01_irq_read     			ihu_bsp_stm32_spi_nrf24l01_irq_read
#define ihu_l1hd_spi_nrf24l01_read_write_byte   ihu_bsp_stm32_spi_nrf24l01_read_write_byte


//L0BSP=>L1HD: I2C
#define ihu_l1hd_i2c_slave_hw_init							ihu_bsp_stm32_i2c_slave_hw_init
#define ihu_l1hd_i2c_iau_send_data 							ihu_bsp_stm32_i2c_iau_send_data
#define ihu_l1hd_i2c_iau_receive_data 					ihu_bsp_stm32_i2c_iau_rcv_data
#define ihu_l1hd_i2c_spare1_send_data 					ihu_bsp_stm32_i2c_spare1_send_data
#define ihu_l1hd_i2c_spare1_receive_data 				ihu_bsp_stm32_i2c_spare1_rcv_data
//三轴传感器MPU6050读取必须先初始化，然后再读取
#define ihu_l1hd_i2c_mpu6050_write_data 				ihu_bsp_stm32_i2c_mpu6050_write_data
#define ihu_l1hd_i2c_mpu6050_write_buffer 			ihu_bsp_stm32_i2c_mpu6050_write_buffer
#define ihu_l1hd_i2c_mpu6050_read_data 					ihu_bsp_stm32_i2c_mpu6050_read_data
#define ihu_l1hd_i2c_mpu6050_read_buffer 				ihu_bsp_stm32_i2c_mpu6050_read_buffer
#define ihu_l1hd_i2c_mpu6050_is_device_ready 		ihu_bsp_stm32_i2c_mpu6050_is_device_ready
//CCL SENSOR传感器
#define ihu_l1hd_i2c_ccl_send_data 							ihu_bsp_stm32_i2c_ccl_send_data
#define ihu_l1hd_i2c_ccl_sensor_rcv_data 				ihu_bsp_stm32_i2c_ccl_sensor_rcv_data
//RTC时钟芯片PCF8563
#define ihu_l1hd_i2c_pcf8563_write_data 				ihu_bsp_stm32_i2c_pcf8563_write_data
#define ihu_l1hd_i2c_pcf8563_write_buffer 			ihu_bsp_stm32_i2c_pcf8563_write_buffer
#define ihu_l1hd_i2c_pcf8563_read_data 					ihu_bsp_stm32_i2c_pcf8563_read_data
#define ihu_l1hd_i2c_pcf8563_read_buffer 				ihu_bsp_stm32_i2c_pcf8563_read_buffer
#define ihu_l1hd_i2c_pcf8563_is_device_ready 		ihu_bsp_stm32_i2c_pcf8563_is_device_ready


//L0BSP=>L1HD: CAN
#define ihu_l1hd_can_slave_hw_init							ihu_bsp_stm32_can_slave_hw_init
#define ihu_l1hd_can_iau_send_data 							ihu_bsp_stm32_can_send_data
#define ihu_l1hd_can_iau_receive_data 					ihu_bsp_stm32_can_rcv_data
#define ihu_l1hd_can_spare1_send_data 					ihu_bsp_stm32_can_spare1_send_data
#define ihu_l1hd_can_spare1_receive_data 				ihu_bsp_stm32_can_spare1_rcv_data

//L0BSP=>L1HD: KEY

//L0BSP=>L1HD: RS485

//L0BSP=>L1HD: TIM

//L0BSP=>L1HD: DIDO
//温湿度DHT11的使用：必须先单独初始化，然后才能使用
#define ihu_l1hd_dido_f2board_dht11_init               ihu_bsp_stm32_dido_f2board_dht11_init
#define ihu_l1hd_dido_f2board_dht11_temp_read          ihu_bsp_stm32_dido_f2board_dht11_temp_read   
#define ihu_l1hd_dido_f2board_dht11_humid_read         ihu_bsp_stm32_dido_f2board_dht11_humid_read   
//#define ihu_l1hd_dido_f2board_fall_read                ihu_bsp_stm32_dido_f2board_fall_read
#define ihu_l1hd_dido_f2board_shake_read               ihu_bsp_stm32_dido_f2board_shake_read
#define ihu_l1hd_dido_f2board_smoke_read               ihu_bsp_stm32_dido_f2board_smoke_read            
#define ihu_l1hd_dido_f2board_water_read               ihu_bsp_stm32_dido_f2board_water_read     
#define ihu_l1hd_dido_f2board_lock1_do1_on             ihu_bsp_stm32_dido_f2board_lock1_do1_on          
#define ihu_l1hd_dido_f2board_lock1_do1_off            ihu_bsp_stm32_dido_f2board_lock1_do1_off         
#define ihu_l1hd_dido_f2board_door1_restriction_read   ihu_bsp_stm32_dido_f2board_door1_restriction_read
#define ihu_l1hd_dido_f2board_lock2_do1_on             ihu_bsp_stm32_dido_f2board_lock2_do1_on          
#define ihu_l1hd_dido_f2board_lock2_do1_off            ihu_bsp_stm32_dido_f2board_lock2_do1_off         
#define ihu_l1hd_dido_f2board_door2_restriction_read   ihu_bsp_stm32_dido_f2board_door2_restriction_read
#define ihu_l1hd_dido_f2board_lock3_do1_on             ihu_bsp_stm32_dido_f2board_lock3_do1_on          
#define ihu_l1hd_dido_f2board_lock3_do1_off            ihu_bsp_stm32_dido_f2board_lock3_do1_off         
#define ihu_l1hd_dido_f2board_door3_restriction_read   ihu_bsp_stm32_dido_f2board_door3_restriction_read
#define ihu_l1hd_dido_f2board_lock4_do1_on             ihu_bsp_stm32_dido_f2board_lock4_do1_on          
#define ihu_l1hd_dido_f2board_lock4_do1_off            ihu_bsp_stm32_dido_f2board_lock4_do1_off         
#define ihu_l1hd_dido_f2board_door4_restriction_read   ihu_bsp_stm32_dido_f2board_door4_restriction_read
#define ihu_l1hd_dido_f2board_cpu_power_ctrl_on        ihu_bsp_stm32_dido_f2board_cpu_power_ctrl_on  
#define ihu_l1hd_dido_f2board_cpu_power_ctrl_off       ihu_bsp_stm32_dido_f2board_cpu_power_ctrl_off 
#define ihu_l1hd_dido_f2board_mq2_cam_power_ctrl_on    ihu_bsp_stm32_dido_f2board_mq2_cam_power_ctrl_on  
#define ihu_l1hd_dido_f2board_mq2_cam_power_ctrl_off   ihu_bsp_stm32_dido_f2board_mq2_cam_power_ctrl_off 
#define ihu_l1hd_dido_f2board_gprsmod_power_supply_on  ihu_bsp_stm32_dido_f2board_gprsmod_power_supply_on 
#define ihu_l1hd_dido_f2board_gprsmod_power_supply_off ihu_bsp_stm32_dido_f2board_gprsmod_power_supply_off
#define ihu_l1hd_dido_f2board_gprsmod_power_key_on     ihu_bsp_stm32_dido_f2board_gprsmod_power_key_on 
#define ihu_l1hd_dido_f2board_gprsmod_power_key_off    ihu_bsp_stm32_dido_f2board_gprsmod_power_key_off
#define ihu_l1hd_dido_f2board_sensor_power_ctrl_on     ihu_bsp_stm32_dido_f2board_sensor_power_ctrl_on  
#define ihu_l1hd_dido_f2board_sensor_power_ctrl_off    ihu_bsp_stm32_dido_f2board_sensor_power_ctrl_off 
#define ihu_l1hd_dido_f2board_ble_atcmd_mode_ctrl_on   ihu_bsp_stm32_dido_f2board_ble_atcmd_mode_ctrl_on     
#define ihu_l1hd_dido_f2board_ble_atcmd_mode_ctrl_off  ihu_bsp_stm32_dido_f2board_ble_atcmd_mode_ctrl_off
#define ihu_l1hd_dido_f2board_ds18b20_init    					ihu_bsp_stm32_dido_ds18b20_init
#define ihu_l1hd_dido_f2board_ds18b20_temp_read     		ihu_bsp_stm32_dido_f2board_ds18b20_temp_read 
#define ihu_l1hd_dido_f2board_lock_act_flag_read     		ihu_bsp_stm32_dido_f2board_lock_act_flag_read 


//L0BSP=>L1HD: WDOG
#define ihu_l1hd_watch_dog_refresh      				ihu_bsp_stm32_watch_dog_refresh

//L0BSP=>L1HD: CRC
#define ihu_l1hd_crc_caculate			      				ihu_bsp_stm32_crc_caculate

//L0BSP=>L1HD: RTC
#define ihu_l1hd_rtc_get_current_unix_time		  ihu_bsp_stm32_rtc_get_current_unix_time
#define ihu_l1hd_rtc_get_current_ymdhms_time		ihu_bsp_stm32_rtc_get_current_ymdhms_time
#define ihu_l1hd_rtc_get_current_ymd_time				ihu_bsp_stm32_rtc_get_current_ymd_time
#define ihu_l1hd_rtc_get_current_hms_time			  ihu_bsp_stm32_rtc_get_current_hms_time
#define ihu_l1hd_rtc_get_moonday		      			ihu_bsp_stm32_rtc_get_moonday
#define ihu_l1hd_rtc_get_china_calendar 				ihu_bsp_stm32_rtc_get_china_calendar
#define ihu_l1hd_rtc_get_sky_earth							ihu_bsp_stm32_rtc_get_sky_earth
#define ihu_l1hd_rtc_get_jieqi			      			ihu_bsp_stm32_rtc_get_jieqi

//L0BSP=>L1HD: CPUID
#define ihu_l1hd_get_cpuid_f1board				      ihu_bsp_stm32_get_cpuid_f1board
#define ihu_l1hd_get_cpuid_f2board				      ihu_bsp_stm32_get_cpuid_f2board

//L0BSP=>L1HD: STMFLASH/IAP
#define ihu_l1hd_f2board_equid_get				      ihu_bsp_stm32_f2board_equid_get

//L0BSP=>L1HD: Power Saving feature
#define ihu_l1hd_pwr_sav_enter_into_sleep_mode			ihu_bsp_stm32_enter_into_sleep_mode
#define ihu_l1hd_pwr_sav_enter_into_stop_mode				ihu_bsp_stm32_enter_into_stop_mode
#define ihu_l1hd_pwr_sav_enter_into_standby_mode		ihu_bsp_stm32_enter_into_standby_mode


//BFSC独有的映射
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
#define ihu_l1hd_i2c_bfsc_send_data 						ihu_bsp_stm32_i2c_iau_send_data
#define ihu_l1hd_can_bfsc_send_data 						ihu_bsp_stm32_can_send_data
#define ihu_l1hd_adc_bfsc_get_value 						ihu_bsp_stm32_adc1_get_sample_value
#endif









/*
 *	
 *	
 *	
 *	
 *  移植FREERTOS所必须需要的环境
 *	
 *	
 *	
 *	
 *
 */

//来自于DA1468x - Portmacro.h
#if (IHU_WORKING_FREE_RTOS_SELECTION == IHU_WORKING_FREE_RTOS_SELECTION_BARE)
	#define portTICK_PERIOD_MS                      ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
	/*-----------------------------------------------------------
	 * Port specific definitions.
	 *
	 * The settings in this file configure FreeRTOS correctly for the
	 * given hardware and compiler.
	 *
	 * These settings should not be altered.
	 *-----------------------------------------------------------
	 */

	/* Type definitions. */
	#define portCHAR                char
	#define portFLOAT               float
	#define portDOUBLE              double
	#define portLONG                long
	#define portSHORT               short
	#define portSTACK_TYPE          uint32_t
	#define portBASE_TYPE           long

	typedef portSTACK_TYPE StackType_t;
	typedef long BaseType_t;
	typedef unsigned long UBaseType_t;

	#if( configUSE_16_BIT_TICKS == 1 )
		typedef uint16_t TickType_t;
		#define portMAX_DELAY ( TickType_t )    0xffff
	#else
		typedef uint32_t TickType_t;
		#define portMAX_DELAY ( TickType_t )    0xffffffffUL

		/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
		not need to be guarded with a critical section. */
		#define portTICK_TYPE_IS_ATOMIC         1
	#endif

	/* Architecture specifics. */
	#define portSTACK_GROWTH                        ( -1 )
	#define portTICK_PERIOD_MS                      ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
	#define portBYTE_ALIGNMENT                      8
	#define portCONVERT_MS_2_TICKS( x )             ( ( (x) * configTICK_RATE_HZ ) / 1000 )
	#define portCONVERT_TICKS_2_MS( x )             ( ( (x) * 1000 ) / configTICK_RATE_HZ )

	extern void prvSystemSleep( TickType_t xExpectedIdleTime );
	/*-----------------------------------------------------------*/

	/* Scheduler utilities. */
	extern void vPortYield( void );
	#define portNVIC_INT_CTRL_REG                   ( * ( ( volatile uint32_t * ) 0xe000ed04 ) )
	#define portNVIC_PENDSVSET_BIT                  ( 1UL << 28UL )
	#define portYIELD()                             vPortYield()

	#define portEND_SWITCHING_ISR( xSwitchRequired ) if( xSwitchRequired ) portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT

	#define portYIELD_FROM_ISR( x )                 portEND_SWITCHING_ISR( x )
	/*-----------------------------------------------------------*/

	/* Critical section management. */
	extern void vPortEnterCritical( void );
	extern void vPortExitCritical( void );
	#define portENTER_CRITICAL()                    vPortEnterCritical()
	#define portEXIT_CRITICAL()                     vPortExitCritical()

	/*-----------------------------------------------------------*/
	/* Task function macros as described on the FreeRTOS.org WEB site. */
	#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
	#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )
	#define portREMOVE_STATIC_QUALIFIER             1
	#define portNOP()
#else
#endif  //#if (IHU_WORKING_FREE_RTOS_SELECTION == IHU_WORKING_FREE_RTOS_SELECTION_BARE)
	

//来自于DA1468x - osal.h，在移植核心函数的过程中，大量使用了<osal>函数，以简化api调用过程
#if (IHU_WORKING_FREE_RTOS_SELECTION == IHU_WORKING_FREE_RTOS_SELECTION_BARE)	

	#define OS_FREERTOS   				/* Define this to use FreeRTOS */
	#if defined(OS_FREERTOS)

	#define OS_STACK_WORD_SIZE      (sizeof(StackType_t))

	#define OS_TASK                 TaskHandle_t
	#define OS_TASK_CREATE_SUCCESS  pdPASS
	#define OS_TASK_NOTIFY_SUCCESS  pdPASS
	#define OS_TASK_NOTIFY_FAIL     pdFALSE
	#define OS_TASK_NOTIFY_NO_WAIT  0
	#define OS_TASK_NOTIFY_FOREVER  portMAX_DELAY
	#define OS_TASK_NOTIFY_ALL_BITS 0xFFFFFFFF

	#define OS_TASK_PRIORITY_LOWEST    tskIDLE_PRIORITY
	#define OS_TASK_PRIORITY_NORMAL    tskIDLE_PRIORITY + 1
	#define OS_TASK_PRIORITY_HIGHEST   configMAX_PRIORITIES - 1

	//#define OS_MUTEX                SemaphoreHandle_t
	#define OS_MUTEX_TAKEN          pdTRUE
	#define OS_MUTEX_CREATE_SUCCESS 1
	#define OS_MUTEX_CREATE_FAILED  0
	#define OS_MUTEX_NO_WAIT        0
	#define OS_MUTEX_FOREVER        portMAX_DELAY

	#define OS_EVENT                SemaphoreHandle_t
	#define OS_EVENT_SIGNALED       pdTRUE
	#define OS_EVENT_NO_WAIT        0
	#define OS_EVENT_FOREVER        portMAX_DELAY

	#define OS_EVENT_GROUP          EventGroupHandle_t
	#define OS_EVENT_GROUP_OK       pdTRUE
	#define OS_EVENT_GROUP_FAIL     pdFALSE
	#define OS_EVENT_GROUP_FOREVER  portMAX_DELAY

	#define OS_QUEUE                QueueHandle_t
	#define OS_QUEUE_FULL           errQUEUE_FULL
	#define OS_QUEUE_EMPTY          pdFALSE
	#define OS_QUEUE_OK             pdTRUE
	#define OS_QUEUE_NO_WAIT        0
	#define OS_QUEUE_FOREVER        portMAX_DELAY

	#define OS_TIMER                TimerHandle_t
	#define OS_TIMER_SUCCESS        pdPASS
	#define OS_TIMER_FAIL           pdFAIL
	#define OS_TIMER_FOREVER        portMAX_DELAY

	#define OS_BASE_TYPE            BaseType_t

	#define OS_OK                   pdPASS
	#define OS_FAIL                 pdFAIL

	#define OS_IRB                  IRB_t

	#define OS_TICK_TIME            TickType_t
	#define OS_PERIOD_MS            portTICK_PERIOD_MS

	#define OS_ASSERT               configASSERT

	/**
	 * \brief OS Notification action
	 */
	typedef enum {
					OS_NOTIFY_NO_ACTION = eNoAction,                                /**< subject task receives event, but its notification value is not updated */
					OS_NOTIFY_SET_BITS = eSetBits,                                  /**< notification value of subject task will be bitwise ORed with task value */
					OS_NOTIFY_INCREMENT = eIncrement,                               /**< notification value of subject task will be incremented by one */
					OS_NOTIFY_VAL_WITH_OVERWRITE = eSetValueWithOverwrite,          /**< notification value of subject task is unconditionally set to task value */
					OS_NOTIFY_VAL_WITHOUT_OVERWRITE = eSetValueWithoutOverwrite,    /**< if subject task has a notification pending then notification value
																																							 will be set to task value otherwise task value is not updated */
	} OS_NOTIFY_ACTION;

	/**
	 * \brief Converts a time in milliseconds to a time in ticks
	 *
	 * \param [in] time_in_ms time in milliseconds
	 *
	 * \return time in ticks
	 *
	 */
	#define OS_TIME_TO_TICKS(time_in_ms) pdMS_TO_TICKS(time_in_ms)

	/**
	 * \brief Return current OS task handle
	 *
	 * \return current task handle of type OS_TASK
	 *
	 */
	#define OS_GET_CURRENT_TASK() xTaskGetCurrentTaskHandle()

	/**
	 * \brief Create OS task
	 *
	 * Function creates OS task. Task is added to ready list.
	 *
	 * \param [in] name task name
	 * \param [in] task_func starting point of task
	 * \param [in] arg parameter past to \p task_func on task start
	 * \param [in] stack_size stack size allocated for task in bytes
	 * \param [in] priority number specifying task priority
	 * \param [in,out] task OS specific task handle
	 *
	 * \return OS_TASK_CREATE_SUCCESS if task was created successfully
	 *
	 */
	#define OS_TASK_CREATE(name, task_func, arg, stack_size, priority, task) \
													xTaskCreate((task_func), (name), \
																									(((stack_size) - 1) / sizeof(StackType_t) + 1), \
																																					(arg), (priority), &(task))

	/**
	 * \brief Delete OS task
	 *
	 * Function deletes OS task.
	 *
	 * \param [in,out] task OS specific task handle
	 *
	 */
	#define OS_TASK_DELETE(task) vTaskDelete(task)

	/**
	 * \brief Send an event directly to task
	 *
	 * Must not be called from ISR!
	 *
	 * \param [in] task id of task to notify
	 * \param [in] value used to update the notification value of the subject \p task
	 * \param [in] action action that performs when notify is occurred
	 *
	 * \return OS_TASK_NOTIFY_FAIL if \p action is set as OS_NOTIFY_VAL_WITHOUT_OVERWRITE and \p task
	 *         already had a notification pending, OS_TASK_NOTIFY_SUCCESS otherwise
	 *
	 */
	#define OS_TASK_NOTIFY(task, value, action) xTaskNotify((task), (value), (action))

	/**
	 * \brief Notify OS task sending an event and return previous notification value
	 *
	 * Must not be called from ISR!
	 *
	 * \param [in] task id of task to notify
	 * \param [in] value used to update the notification value of the subject \p task
	 * \param [in] action action that performs when notify is occurred
	 * \param [out] prev_value pointer to previous notification value - optional - can be set to NULL
	 *
	 * \return OS_TASK_NOTIFY_FAIL if \p action is set as OS_NOTIFY_VAL_WITHOUT_OVERWRITE and \p task
	 *         already had a notification pending, OS_TASK_NOTIFY_SUCCESS otherwise
	 *
	 */
	#define OS_TASK_NOTIFY_AND_QUERY(task, value, action, prev_value) xTaskNotifyAndQuery((task), \
																																	(value), (action), (prev_value))

	/**
	 * \brief Send an event and unblock OS task with updating notification value
	 *
	 * A version of OS_TASK_NOTIFY that can be called from ISR.
	 *
	 * \param [in] task id of task to notify
	 * \param [in] value used to update the notification value of the subject \p task
	 * \param [in] action action that performs when notify is occurred
	 *
	 * \return OS_TASK_NOTIFY_FAIL if \p action is set as OS_NOTIFY_VAL_WITHOUT_OVERWRITE and \p task
	 *         already had a notification pending, OS_TASK_NOTIFY_SUCCESS otherwise
	 *
	 */
	#define OS_TASK_NOTIFY_FROM_ISR(task, value, action) \
					({ \
									BaseType_t need_switch, ret; \
									ret = xTaskNotifyFromISR(task, value, action, &need_switch); \
									portEND_SWITCHING_ISR(need_switch); \
									ret; \
					})

	/**
	 * \brief Notify task and receive OS task notification value
	 *
	 * Must not called from ISR! Use OS_TASK_NOTIFY_GIVE_FROM_ISR instead.
	 *
	 * \param [in] task id of task to notify
	 *
	 * \return OS_TASK_NOTIFY_GIVE calls OS_TASK_NOTIFY with action set to OS_NOTIFY_INCREMENT resulting
	 *         in all calls returning OS_TASK_NOTIFY_SUCCESS
	 *
	 */
	#define OS_TASK_NOTIFY_GIVE(task) xTaskNotifyGive(task)

	/**
	 * \brief Notify task from ISR
	 *
	 * Send notification from interrupt service routine (ISR) to \p task that can unblock the receiving
	 * task and optionally update the receiving task's notification value.
	 * This function is safe to call from ISR.
	 *
	 * \param [in] task id of task to notify
	 *
	 */
	#define OS_TASK_NOTIFY_GIVE_FROM_ISR(task) \
					({ \
									BaseType_t need_switch; \
									vTaskNotifyGiveFromISR(task, &need_switch); \
									portEND_SWITCHING_ISR(need_switch); \
					})

	/**
	 * \brief Clear to zero or decrement task notification value on exit
	 *
	 * \param [in] clear_on_exit = pdFASLE: RTOS task's notification value is decremented before
	 *                             OS_TASK_NOTIFY_TAKE() exits,
	 *                             pdTRUE: the RTOS task's notification value is reset to 0 before
	 *                             OS_TASK_NOTIFY_TAKE() exits.
	 * \param [in] time_to_wait maximum time to wait in the blocked state for a notification to be
	 *                          received if a notification is not already pending when
	 *                          OS_TASK_NOTIFY_TAKE() is called
	 *
	 * \return the value of the task's notification value before it is decremented or cleared
	 *
	 */
	#define OS_TASK_NOTIFY_TAKE(clear_on_exit, time_to_wait) ulTaskNotifyTake((clear_on_exit), \
																																									(time_to_wait))

	/**
	 * \brief Wait for the calling task to receive a notification
	 *
	 * \param [in] entry_bits any bits set in entry_bits will be cleared in calling notification value
	 *                        before enters to OS_TASK_NOTIFY_WAIT
	 * \param [in] exit_bits any bits set in exit_bits will be cleared in calling notification value
	 *                       before OS_TASK_NOTIFY_WAIT function exits if a notification was received
	 * \param [out] value pointer to task's notification value, if not required can be set to NULL
	 * \param [in] ticks_to_wait maximum time to wait in the blocked state for a notification to be
	 *                           received if a notification is not already pending when
	 *                           OS_TASK_NOTIFY_WAIT is called
	 *
	 * \return OS_TASK_NOTIFY_SUCCESS if notification was received or was already pending when
	 *         OS_TASK_NOTIFY_WAIT was called
	 *         OS_TASK_NOTIFY_FAIL if the call OS_TASK_NOTIFY_WAIT timed out before notification was
	 *         received
	 *
	 */
	#define OS_TASK_NOTIFY_WAIT(entry_bits, exit_bits, value, ticks_to_wait) \
																	xTaskNotifyWait((entry_bits), (exit_bits), (value), \
																																									(ticks_to_wait))

	/**
	 * \brief Resume task
	 *
	 * Make \p task ready to run.
	 *
	 * \param [in] task id of task to resume
	 *
	 */
	#define OS_TASK_RESUME(task) vTaskResume(task)

	/**
	 * \brief Resume task
	 *
	 * Make \p task ready to run. This function is safe to call from ISR.
	 *
	 * \param [in] task id of task to resume
	 *
	 */
	#define OS_TASK_RESUME_FROM_ISR(task) xTaskResumeFromISR(task)

	/**
	 * \brief Suspend task
	 *
	 * Remove \p task from execution queue. Task will not be run
	 * until OS_TASK_RESUME or OS_TASK_RESUME_FROM_ISR is called.
	 *
	 * \param [in] task id of task to suspend
	 *
	 */
	#define OS_TASK_SUSPEND(task) vTaskSuspend(task)

	/**
	 * \brief Suspend task
	 *
	 * Remove \p task from execution queue. Task will not be run
	 * until OS_TASK_RESUME or OS_TASK_RESUME_FROM_ISR is called.
	 * This function is safe to call from ISR.
	 *
	 * \param [in] task id of task to suspend
	 *
	 */
	#define OS_TASK_SUSPEND_FROM_ISR(task) vTaskSuspendFromISR(task)

	/**
	 * \brief Create OS mutex
	 *
	 * Function creates OS mutex.
	 *
	 * \param [in,out] mutex
	 *
	 * \return OS_MUTEX_CREATE_SUCCESS when mutex was created successfully, OS_MUTEX_CREATE_FAILED
	 *         otherwise
	 */
	#define OS_MUTEX_CREATE(mutex) \
        ({ \
                (mutex) = xSemaphoreCreateRecursiveMutex(); \
                mutex != NULL ? OS_MUTEX_CREATE_SUCCESS : OS_MUTEX_CREATE_FAILED; \
        })

	/**
	 * \brief Delete OS mutex
	 *
	 * Function deletes OS mutex.
	 *
	 * \param [in] mutex
	 *
	 */
	#define OS_MUTEX_DELETE(mutex) vSemaphoreDelete(mutex)

	/**
	 * \brief Relase mutex
	 *
	 * Decrease mutex count, when numer of calls to OS_MUTEX_GET equals number of calls to
	 * OS_MUTEX_PUT, mutex can be acquired by other task.
	 *
	 * \param [in] mutex id of mutex to realse
	 *
	 */
	#define OS_MUTEX_PUT(mutex) xSemaphoreGiveRecursive(mutex)

	/**
	 * \brief Acquire mutex
	 *
	 * Access to shared resource can be guarded by mutex. When task wants to get access
	 * to this resource call OS_MUTEX_GET. If mutex was not taken by any task yet it, call
	 * will succeed and mutex will be assigned to calling task. Next call to already acquired
	 * mutex from same task will succeed. If mutex is already taken by other task calling
	 * task will wait for specified time before failing.
	 * For non-blocking acquire \p timeout can be OS_MUTEX_NO_WAIT, for infinite wait till
	 * mutex is released OS_MUTEX_FOREVER should be used.
	 *
	 * \param [in] mutex id of mutex to acquire
	 * \param [in] timeout number of ticks that to acquire mutex
	 *
	 */
	#define OS_MUTEX_GET(mutex, timeout) xSemaphoreTakeRecursive((mutex), (timeout))

	/**
	 * \brief Create OS event
	 *
	 * Function creates OS event that can be used to synchronize.
	 *
	 * \param [in,out] event
	 *
	 */
	#define OS_EVENT_CREATE(event) do { (event) = xSemaphoreCreateBinary(); } while(0)

	/**
	 * \brief Delete OS event
	 *
	 * Function destroys OS event.
	 *
	 * \param [in] event item to delete
	 *
	 */
	#define OS_EVENT_DELETE(event) vSemaphoreDelete(event)

	/**
	 * \brief Set event in signaled state
	 *
	 * Set event in signaled stated so OS_EVENT_WAIT will release waiting task if any.
	 * Event will remain in signaled stated till call to OS_EVENT_WAIT releases one task.
	 * This function should not be called from ISR.
	 *
	 * \param [in] event
	 *
	 */
	#define OS_EVENT_SIGNAL(event) xSemaphoreGive(event)

	/**
	 * \brief Set event in signaled state
	 *
	 * Set event in signaled stated so OS_EVENT_WAIT will release waiting task if any.
	 * Event will remain in signaled stated till call to OS_EVENT_WAIT releases one task.
	 * This function is safe to call from ISR.
	 *
	 * \param [in] event
	 *
	 */
	#define OS_EVENT_SIGNAL_FROM_ISR(event) \
					({ \
									BaseType_t need_switch, ret; \
									ret = xSemaphoreGiveFromISR(event, &need_switch); \
									portEND_SWITCHING_ISR(need_switch); \
									ret; \
					})
	/**
	 * \brief Wait for event
	 *
	 * This function wait for \p event to be in signaled stated.
	 * If event was already in signaled state or become signaled in specified time
	 * function will return OS_EVENT_SIGNALED.
	 * To check if event is already signaled use OS_EVENT_NO_WAIT as timeout.
	 * To wait till event is signaled use OS_EVENT_FOREVERE.
	 * This function can't be used in ISR.
	 *
	 * \param [in] event if of event to wait on
	 * \param [in] timeout number of ticks to wait
	 *
	 * \return OS_EVENT_SIGNALED if event was signaled other value if not
	 *
	 */
	#define OS_EVENT_WAIT(event, timeout) xSemaphoreTake((event), (timeout))


	/**
	 * \brief Check if event is signaled without waiting
	 *
	 * This function will return immediately with OS_EVENT_SIGNALED if event
	 * was in signaled state already.
	 *
	 * \param [in] event if of event to wait on
	 *
	 * \return OS_EVENT_SIGNALED if event was signaled other value if not
	 *
	 */
	#define OS_EVENT_CHECK(event) xSemaphoreTake((event), OS_EVENT_NO_WAIT)

	#define OS_EVENT_YIELD(higherPriorityTaskWoken) portYIELD_FROM_ISR(higherPriorityTaskWoken)

	/**
	 * \brief Create OS event group
	 *
	 * Function creates OS event group
	 *
	 * \return event group handle if successful, otherwise NULL
	 *
	 */
	#define OS_EVENT_GROUP_CREATE() xEventGroupCreate()

	/**
	 * \brief Event group wait bits
	 *
	 * Function reads bits within event group optionally entering the Blocked state (with a timeout)
	 * to wait for a bit or group of bits to become set.
	 *
	 * \param [in] event_group the event group in which the bits are being tested
	 * \param [in] bits_to_wait a bitwise value to test inside the event group
	 * \param [in] clear_on_exit = OS_OK: any bits set in the value passed as the bits_to_wait
	 *                             parameter will be cleared in the event group before
	 *                             xEventGroupWaitBits returns
	 *                             OS_FAIL: bits in the event group are not altered when the call
	 *                             to xEventGroupWaitBits() returns
	 * \param [in] wait_for_all = OS_OK: xEventGroupWaitBits() will return when either *all* bits
	 *                            set in the value passed as the bits_to_wait parameter are set in
	 *                            the event group
	 *                            OS_FAIL: xEventGroupWaitBits() will return when *any* of the bits set
	 *                            in the value passed as the bits_to_wait parameter are set in the event
	 *                            group
	 * \param [in] timeout maximum amount of time to wait for one/all of the bits specified
	 *                     by bits_to_wait to become set
	 *
	 * \return the value of the event group at the time either the event bits being waited for
	 *         became set, or the timeout expired
	 *
	 */
	#define OS_EVENT_GROUP_WAIT_BITS(event_group, bits_to_wait, clear_on_exit, wait_for_all, timeout) \
																			xEventGroupWaitBits((event_group), (bits_to_wait), \
																													 (clear_on_exit), (wait_for_all), (timeout))

	/**
	 * \brief Event group set bits
	 *
	 * Set bits (flags) within an event group
	 *
	 * \param [in] event_group the event group in which the bits are to be set
	 * \param [in] bits_to_set a bitwise value that indicates the bit or bits to set in the event group
	 *
	 * \return the value of the event group at the time the call to xEventGroupSetBits() returns
	 *
	 */
	#define OS_EVENT_GROUP_SET_BITS(event_group, bits_to_set) xEventGroupSetBits((event_group), \
																																												(bits_to_set))

	/**
	 * \brief Set event group bits from ISR
	 *
	 * Set bits (flags) within an RTOS event group that can be called from an ISR.
	 *
	 * \param [in] event_group the event group in which the bits are to be set
	 * \param [in] bits_to_set a bitwise value that indicates the bit or bits to set in the event group
	 *
	 * \return if the message was sent to the RTOS daemon task then OS_OK is returned,
	 *         otherwise OS_FAIL is returned
	 *
	 */
	#define OS_EVENT_GROUP_SET_BITS_FROM_ISR(event_group, bits_to_set) \
	({ \
					BaseType_t need_switch, ret; \
					ret = xEventGroupSetBitsFromISR((event_group), (bits_to_set), &(need_switch)); \
					portEND_SWITCHING_ISR(need_switch); \
					ret; \
	})

	/**
	 * \brief Clear event group bits
	 *
	 * Function clear bits (flags) within an event group.
	 *
	 * \param [in] event_group the event group in which the bits are to be cleared
	 * \param [in] bits_to_clear a bitwise value that indicates the bit or bits to clear
	 *                           in the event group
	 *
	 * \return value of the event group before the specified bits were cleared
	 *
	 */
	#define OS_EVENT_GROUP_CLEAR_BITS(event_group, bits_to_clear) xEventGroupClearBits((event_group), \
																																											(bits_to_clear))

	/**
	 * \brief Clear event group bits from an interrupt
	 *
	 * Function clear bits (flags) within an event group from an interrupt.
	 *
	 * \param [in] event_group the event group in which the bits are to be cleared
	 * \param [in] bits_to_clear a bitwise value that indicates the bit or bits to clear
	 *                           in the event group
	 *
	 * \return value of the event group before the specified bits were cleared
	 *
	 */
	#define OS_EVENT_GROUP_CLEAR_BITS_FROM_ISR(event_group, bits_to_clear) \
																					 xEventGroupClearBitsFromISR((event_group), (bits_to_clear))

	/**
	 * \brief Get event group bits
	 *
	 * Function returns the current value of the event bits (event flags) in an event group.
	 *
	 * \param [in] event_group the event group being queried
	 *
	 * \return value of the event bits in the event group at the time
	 *         OS_EVENT_GROUP_GET_BITS() was called
	 *
	 */
	#define OS_EVENT_GROUP_GET_BITS(event_group) xEventGroupGetBits(event_group)

	/**
	 * \brief Get event group bits from an interrupt
	 *
	 * Function returns the current value of the event bits (event flags) in an event group from an
	 * interrupt
	 *
	 * \param [in] event_group the event group being queried
	 *
	 * \return value of the event bits in the event group at the time OS_EVENT_GROUP_GET_BITS_FROM_ISR()
	 *         was called
	 *
	 */
	#define OS_EVENT_GROUP_GET_BITS_FROM_ISR(event_group) xEventGroupGetBitsFromISR(event_group)

	/**
	 * \brief Synchronize event group bits
	 *
	 * Atomically set bits (flags) within an event group, then wait for a combination of bits to be
	 * set within the same event group.
	 *
	 * \param [in] event_group event group in which the bits are being set and tested
	 * \param [in] bits_to_set bit or bits to set in the event group before determining if all
	 *                         the bits specified by the bits_to_wait parameter are set
	 * \param [in] bits_to_wait a bitwise value that indicates the bit or bits to test inside
	 *                          the event group
	 * \param [in] timeout maximum amount of time (specified in 'ticks') to wait for all the bits
	 *                     specified by the bits_to_wait parameter value to become set
	 *
	 * \return value of the event group at the time either the bits being waited for became set,
	 *         or the block time expired
	 *
	 */
	#define OS_EVENT_GROUP_SYNC(event_group, bits_to_set, bits_to_wait, timeout) \
																			xEventGroupSync((event_group), (bits_to_set), \
																																						(bits_to_wait), (timeout))

	/**
	 * \brief Delete event group
	 *
	 * Function deletes an event group.
	 *
	 * \param [in] event_group the event group being deleted.
	 *
	 */
	#define OS_EVENT_GROUP_DELETE(event_group) xEventGroupDelete(event_group)

	/**
	 * \brief Create OS queue
	 *
	 * Function creates OS queue that can contain \p max_items of specified size.
	 *
	 * \param [in,out] queue queue to initialize
	 * \param [in] item_size queue element size
	 * \param [in] max_items max number of items that queue can store
	 *
	 */
	#define OS_QUEUE_CREATE(queue, item_size, max_items) \
																	do { (queue) = xQueueCreate((max_items), (item_size)); } while(0)

	/**
	 * \brief Deletes OS queue
	 *
	 * Function deletes OS.
	 *
	 * \param [in] queue queue to delete
	 *
	 */
	#define OS_QUEUE_DELETE(queue) vQueueDelete(queue)

	/**
	 * \brief Put element in queue
	 *
	 * Function adds element into queue if there is enough room for it.
	 * If there is no room in queue for \p timeout ticks element is not
	 * put in queue and error is returned.
	 *
	 * \param [in] queue id of queue to put item to
	 * \param [in] item pointer to element to enqueue
	 * \param [in] timeout max time in ticks to wait for space in queue
	 *
	 * \return OS_QUEUE_FULL if there was no place in queue
	 *         OS_QUEUE_OK if message was put in queue
	 *
	 */
	#define OS_QUEUE_PUT(queue, item, timeout) xQueueSend((queue), (item), (timeout))

	/**
	 * \brief Put element in queue
	 *
	 * Function adds element into queue if there is enough room for it.
	 * If there is no room in queue error is returned immediately.
	 *
	 * This is safe to call from ISR.
	 *
	 * \param [in] queue id of queue to put item to
	 * \param [in] item pointer to element to enqueue
	 *
	 * \return OS_QUEUE_FULL if there was no place in queue
	 *         OS_QUEUE_OK if message was put in queue
	 *
	 */
	#define OS_QUEUE_PUT_FROM_ISR(queue, item) \
					({ \
									BaseType_t need_switch, ret; \
									ret = xQueueSendToBackFromISR((queue), (item), &need_switch); \
									portEND_SWITCHING_ISR(need_switch); \
									ret; \
					})

	/**
	 * \brief Get element from queue
	 *
	 * Function adds element into queue if there is enough room for it.
	 * If there is nothing in queue for \p timeout ticks error is returned.
	 * Use OS_QUEUE_NO_WAIT for \p timeout to get message without waiting.
	 * Use OS_QUEUE_FOREVER to wait till message arrives.
	 *
	 * \param [in] queue id of queue to get item from
	 * \param [out] item pointer to buffer that will receive element from queue
	 * \param [in] timeout max time in ticks to wait for element in queue
	 *
	 * \return OS_QUEUE_EMPTY if there was nothing in queue
	 *         OS_QUEUE_OK if message was get from queue
	 *
	 */
	#define OS_QUEUE_GET(queue, item, timeout) xQueueReceive((queue), (item), (timeout))

	/**
	 * \brief Peek element on queue
	 *
	 * Function gets element from queue without removing it.
	 * If there is nothing in queue for \p timeout ticks error is returned.
	 * Use OS_QUEUE_NO_WAIT for \p timeout to get message without waiting.
	 * Use OS_QUEUE_FOREVER to wait till message arrives.
	 *
	 * \param [in] queue id of queue to get item from
	 * \param [out] item pointer to buffer that will receive element from queue
	 * \param [in] timeout max time in ticks to wait for element in queue
	 *
	 * \return OS_QUEUE_EMPTY if there was nothing in queue
	 *         OS_QUEUE_OK if message was get from queue
	 *
	 */
	#define OS_QUEUE_PEEK(queue, item, timeout) xQueuePeek((queue), (item), (timeout))

	/**
	 * \brief Create software timer
	 *
	 * Function creates software timer with given timeout
	 *
	 * \param [in] name     timer name
	 * \param [in] period   timer period in ticks
	 * \param [in] reload   indicates if callback will be called once or multiple times
	 * \param [in] timer_id identifier which can be used to identify timer in callback function
	 * \param [in] callback callback called when timer expires
	 *
	 * \return OS_TIMER if timer created successfully, otherwise null
	 */
	#define OS_TIMER_CREATE(name, period, reload, timer_id, callback) \
													xTimerCreate((name), (period), ((reload) ? pdTRUE : pdFALSE), \
																													((void *) (timer_id)), (callback))

	/**
	 * \brief Get timer ID
	 *
	 * Function returns timer_id assigned in OS_TIMER_CREATE
	 *
	 * \param [in] timer timer handle
	 *
	 * \return timer id
	 */
	#define OS_TIMER_GET_TIMER_ID(timer) pvTimerGetTimerID(timer)

	/**
	 * \brief Check if timer is active
	 *
	 * Function checks timer status
	 *
	 * \param [in] timer timer handle
	 *
	 * \return true if timer is active, otherwise false
	 */
	#define OS_TIMER_IS_ACTIVE(timer) xTimerIsTimerActive(timer)

	/**
	 * \brief Start timer
	 *
	 * Function starts timer
	 *
	 * \param [in]  timer timer handle returned in OS_TIMER_CREATE
	 * \param [in]  timeout max time in ticks to wait until command is sent
	 *
	 * \return OS_TIMER_SUCCESS if command has been sent successfully,
	 * or OS_TIMER_FAIL if timeout occur
	 */
	#define OS_TIMER_START(timer, timeout) xTimerStart((timer), (timeout))

	/**
	 * \brief Stop timer
	 *
	 * Function stops timer
	 *
	 * \param [in]  timer timer handle returned in OS_TIMER_CREATE
	 * \param [in]  timeout max time in ticks to wait until command is sent
	 *
	 * \return OS_TIMER_SUCCESS if command has been sent successfully,
	 * or OS_TIMER_FAIL if timeout occur
	 */
	#define OS_TIMER_STOP(timer, timeout) xTimerStop((timer), (timeout))

	/**
	 * \brief Change timer's period
	 *
	 * Functions updates timer's period
	 *
	 * \param [in] timer timer handle
	 * \param [in] period new timer's period
	 * \param [in] timeout max time in ticks to wait until command is sent
	 *
	 * \return OS_TIMER_SUCCESS if command has been sent successfully,
	 * or OS_TIMER_FAIL if timeout occur
	 */
	#define OS_TIMER_CHANGE_PERIOD(timer, period, timeout) \
																	xTimerChangePeriod((timer), (period), (timeout))

	/**
	 * \brief Delete timer
	 *
	 * Function deletes previously created timer
	 *
	 * \param [in] timer timer handle
	 * \param [in] timeout max time in ticks to wait until command is sent
	 *
	 * \return OS_TIMER_SUCCESS if command has been sent successfully,
	 * or OS_TIMER_FAIL if timeout occur
	 */
	#define OS_TIMER_DELETE(timer, timeout) xTimerDelete((timer), (timeout))

	/**
	 * \brief Reset timer
	 *
	 * Function restarts previously created timer
	 *
	 * \param [in] timer timer handle
	 * \param [in] timeout max time in ticks to wait until command is sent
	 *
	 * \return OS_TIMER_SUCCESS if command has been sent successfully,
	 * or OS_TIMER_FAIL if timeout occur
	 */
	#define OS_TIMER_RESET(timer, timeout) xTimerReset((timer), (timeout))

	/**
	 * \brief Start timer from ISR
	 *
	 * Version of OS_TIMER_START that can be called from an interrupt service
	 * routine
	 *
	 * \param [in] timer timer handle
	 *
	 * \return OS_TIMER_SUCCESS if command has been sent successfully,
	 * otherwise OS_TIMER_FAIL
	 *
	 * \sa OS_TIMER_START()
	 */
	#define OS_TIMER_START_FROM_ISR(timer) \
	({ \
					BaseType_t need_switch, ret; \
					ret = xTimerStartFromISR((timer), &(need_switch)); \
					portEND_SWITCHING_ISR(need_switch); \
					ret; \
	})

	/**
	 * \brief Stop timer from ISR
	 *
	 * Version of OS_TIMER_STOP that can be called from an interrupt service
	 * routine
	 *
	 * \param [in] timer timer handle
	 *
	 * \return OS_TIMER_SUCCESS if command has been sent successfully,
	 * otherwise OS_TIMER_FAIL
	 *
	 * \sa OS_TIMER_STOP()
	 */
	#define OS_TIMER_STOP_FROM_ISR(timer) \
	({ \
					BaseType_t need_switch, ret; \
					ret = xTimerStopFromISR((timer), &(need_switch)); \
					portEND_SWITCHING_ISR(need_switch); \
					ret; \
	})

	/**
	 * \brief Change timer period from ISR
	 *
	 * Version of OS_TIMER_CHANGE_PERIOD that can be called from an interrupt service
	 * routine
	 *
	 * \param [in] timer timer handle
	 * \param [in] period new timer period
	 *
	 * \return OS_TIMER_SUCCESS if command has been sent successfully,
	 * otherwise OS_TIMER_FAIL
	 *
	 * \sa OS_TIMER_CHANGE_PERIOD
	 */
	#define OS_TIMER_CHANGE_PERIOD_FROM_ISR(timer, period) \
	({ \
					BaseType_t need_switch, ret; \
					ret = xTimerChangePeriodFromISR((timer), (period), &(need_switch)); \
					portEND_SWITCHING_ISR(need_switch); \
					ret; \
	})

	/**
	 * \brief Reset timer from ISR
	 *
	 * Version of OS_TIMER_RESET that can be called from an interrupt service
	 * routine
	 *
	 * \param [in] timer timer handle
	 *
	 * \return OS_TIMER_SUCCESS if command has been sent successfully,
	 * otherwise OS_TIMER_FAIL
	 *
	 * \sa OS_TIMER_RESET()
	 */
	#define OS_TIMER_RESET_FROM_ISR(timer) \
	({ \
					BaseType_t need_switch, ret; \
					ret = xTimerResetFromISR((timer), &(need_switch)); \
					portEND_SWITCHING_ISR(need_switch); \
					ret; \
	})

	/**
	 * \brief Delay execution of task for specified time
	 *
	 * This function delays in OS specific way execution of current task.
	 *
	 * \param [in] ticks number of ticks to wait
	 *
	 */
	#define OS_DELAY(ticks) vTaskDelay(ticks)

	/**
	 * \brief Get current tick count
	 *
	 * \return current tick count
	 *
	 */
	#define OS_GET_TICK_COUNT() xTaskGetTickCount()

	/**
	 * \brief Convert from OS ticks to ms
	 *
	 * \param [in] ticks tick count to convert
	 *
	 * \return value in ms
	 *
	 */
	#define OS_TICKS_2_MS(ticks) portCONVERT_TICKS_2_MS(ticks)

	/**
	 * \brief Convert from ms to OS ticks
	 *
	 * \param [in] ms milliseconds to convert
	 *
	 * \return value in OS ticks
	 *
	 */
	#define OS_MS_2_TICKS(ms) portCONVERT_MS_2_TICKS(ms)

	/**
	 * \brief Delay execution of task for specified time
	 *
	 * This function delays in OS specific way execution of current task.
	 *
	 * \param [in] ms number of ms to wait
	 *
	 */
	#define OS_DELAY_MS(ms) OS_DELAY(OS_MS_2_TICKS(ms))

	/**
	 * \brief Enter critical section
	 *
	 * This allows to enter critical section.
	 * Implementation will disable interrupts with nesting counter.
	 * This function can be called several time by task but requires same
	 * number of OS_LEAVE_CRITICAL_SECTION calls to allow task switching and interrupts
	 * again.
	 *
	 * \sa OS_LEAVE_CRITICAL_SECTION
	 *
	 */
	#define OS_ENTER_CRITICAL_SECTION() portENTER_CRITICAL()

	/**
	 * \brief Leave critical section
	 *
	 * Function restores interrupts and task switching.
	 * Number of calls to this function must match number of calls to OS_ENTER_CRITICAL_SECTION.
	 *
	 * \sa OS_ENTER_CRITICAL_SECTION
	 *
	 */
	#define OS_LEAVE_CRITICAL_SECTION() portEXIT_CRITICAL()

	/**
	 * \brief Name for OS memory allocation function
	 *
	 * \sa OS_MALLOC
	 *
	 */
	#define OS_MALLOC_FUNC pvPortMalloc

	/**
	 * \brief Name for non-retain memory allocation function
	 *
	 * \sa OS_MALLOC_FUNC
	 *
	 */
	#define OS_MALLOC_NORET_FUNC pvPortMalloc

	/**
	 * \brief Allocate memory from OS provided heap
	 *
	 * \sa OS_FREE
	 *
	 */
	#define OS_MALLOC(size) OS_MALLOC_FUNC(size)

	/**
	 * \brief Allocate memory from non-retain heap
	 *
	 * \sa OS_FREE
	 *
	 */
	#define OS_MALLOC_NORET(size) OS_MALLOC_NORET_FUNC(size)

	/**
	 * \brief Name for OS free memory function
	 *
	 * \sa OS_FREE
	 *
	 */
	#define OS_FREE_FUNC vPortFree

	/**
	 * \brief Name for non-retain memory free function
	 *
	 * \sa OS_FREE_NORET
	 * \sa OS_MALLOC_NORET
	 *
	 */
	#define OS_FREE_NORET_FUNC vPortFree

	/**
	 * \brief Free memory allocated by OS_MALLOC()
	 *
	 * \sa OS_MALLOC
	 *
	 */
	#define OS_FREE(addr) OS_FREE_FUNC(addr)

	/**
	 * \brief Free memory allocated by OS_MALLOC_NORET()
	 *
	 * \sa OS_MALLOC_NORET
	 *
	 */
	#define OS_FREE_NORET(addr) OS_FREE_NORET_FUNC(addr)

	#endif

	#undef OS_BAREMETAL
	#if defined OS_BAREMETAL
	/*
	 * Basic set of macros that can be used in non OS environment.
	 */
	# define PRIVILEGED_DATA
	# define OS_MALLOC malloc
	# define OS_FREE free
	# ifndef RELEASE_BUILD
	#  define OS_ASSERT(a) do { if (!a) {__BKPT(0);} } while (0)
	# else
	#  define OS_ASSERT(a) ((void) (a))
	# endif

	#endif

	/**
	 * \brief Cast any pointer to unsigned int value
	 */
	#define OS_PTR_TO_UINT(p) ((unsigned) (void *) (p))

	/**
	 * \brief Cast any pointer to signed int value
	 */
	#define OS_PTR_TO_INT(p) ((int) (void *) (p))

	/**
	 * \brief Cast any unsigned int value to pointer
	 */
	#define OS_UINT_TO_PTR(u) ((void *) (unsigned) (u))

	/**
	 * \brief Cast any signed int value to pointer
	 */
	#define OS_INT_TO_PTR(i) ((void *) (int) (i))


#else
#endif  //(IHU_WORKING_FREE_RTOS_SELECTION == IHU_WORKING_FREE_RTOS_SELECTION_BARE)	

#endif /* L1VMFREEOS_VMFREEOSLAYER_H_ */
