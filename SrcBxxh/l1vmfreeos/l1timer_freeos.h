/**
 ****************************************************************************************
 *
 * @file l1timer_freeos.h
 *
 * @brief L1 TIMER FreeRTOS
 *
 * BXXH team
 * Created by ZJL, 20160127
 *
 ****************************************************************************************
 */

#ifndef L1VMFREEOS_L1TIMER_FREEOS_H_
#define L1VMFREEOS_L1TIMER_FREEOS_H_

#include "vmfreeoslayer.h"
#include "l1comdef_freeos.h"
	
//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_TIMER
{
	FSM_STATE_TIMER_INITED = 0x02,
	FSM_STATE_TIMER_AVTIVE,
	FSM_STATE_TIMER_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF


/*
 *	
 *  全局TIMER常量定义
 *
 */
//当前就是1ms的定时时钟！！！
//Granularity of 1second and 10ms timer set
#define IHU_TIMER_TICK_1_SEC 1000      //in second
#define IHU_TIMER_TICK_10_MS 10  //in ms second
#define IHU_TIMER_TICK_1_MS 1  //in ms second
#define IHU_TIMER_TICK_10_MS_IN_NS 10000000  //in n second
#define IHU_TIMER_TICK_1_MS_IN_NS 1000000  //in n second
#define IHU_TIMER_TICK_100_MS_IN_NS 100000000  //in n second

//Timer Id definition
//#define MAX_TIMER_NUM_IN_ONE_IHU_1S 40  //Normal situation
//#define MAX_TIMER_NUM_IN_ONE_IHU_10MS 4 //Frame usage
//#define MAX_TIMER_NUM_IN_ONE_IHU_1MS 4  //Real-time usage
#define MAX_TIMER_SET_DURATION 100000
enum IHU_TIMER_ID_1S_ALL
{
	TIMER_ID_1S_MIN = 0,
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
	TIMER_ID_1S_CCL_EVENT_REPORT_PEROID_SCAN,
	TIMER_ID_1S_CCL_LOCK_WORK_ACTIVE,    			//锁具最长的激活时间
	TIMER_ID_1S_CCL_LOCK_WORK_WAIT_TO_OPEN,   //锁被打开，等待开门的时间
	TIMER_ID_1S_CCL_DIDO_TRIGGER_PERIOD_SCAN,  //SLEEP模式下触发工作的定时器
	TIMER_ID_1S_CCL_DIDO_WORKING_PERIOD_SCAN,  //WORKING模式下外部传感器的定时扫描
	TIMER_ID_1S_CCL_SPS_WORKING_PERIOD_SCAN,   //WORKING模式下外部传感器的定时扫描
	TIMER_ID_1S_CCL_I2C_WORKING_PERIOD_SCAN,   //WORKING模式下外部传感器的定时扫描
	TIMER_ID_1S_CCL_DCMI_WORKING_PERIOD_SCAN,	 //WORKING模式下外部传感器的定时扫描
	TIMER_ID_1S_CCL_ADC_WORKING_PERIOD_SCAN,	 //WORKING模式下外部传感器的定时扫描
	
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	TIMER_ID_1S_BFSC_PERIOD_SCAN,
	TIMER_ID_1S_BFSC_L3BFSC_WAIT_WEIGHT_TIMER,
	TIMER_ID_1S_BFSC_L3BFSC_ROLL_OUT_TIMER,
	TIMER_ID_1S_BFSC_L3BFSC_GIVE_UP_TIMER,
	
#else
#endif	
	TIMER_ID_1S_MAX,
};
enum IHU_TIMER_ID_10MS_ALL
{
	TIMER_ID_10MS_MIN = 0,
	TIMER_ID_10MS_VMFO_TEST,
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	TIMER_ID_10MS_BFSC_ADCLIBRA_SCAN_TIMER,
#else
#endif		
	TIMER_ID_10MS_MAX,
};
enum IHU_TIMER_ID_1MS_ALL
{
	TIMER_ID_1MS_MIN = 0,
	TIMER_ID_1MS_VMFO_TEST,
	TIMER_ID_1MS_MAX,
};
#define MAX_TIMER_NUM_IN_ONE_IHU_1S TIMER_ID_1S_MAX + 1  //Normal situation
#define MAX_TIMER_NUM_IN_ONE_IHU_10MS TIMER_ID_10MS_MAX + 1  //Frame usage
#define MAX_TIMER_NUM_IN_ONE_IHU_1MS TIMER_ID_1MS_MAX + 1  //Real-time usage


//System level timer global variables
typedef struct IhuTimerElement
{
	UINT32 timerId;
	UINT32 taskId;
	UINT32 tDuration;
	UINT8  timerType;
	UINT8  timerRes; //Timer Resolution
	UINT32 elapse;
	UINT8  status;
}IhuTimerElement_t;
typedef struct IhuTimerTable
{
	IhuTimerElement_t timer1s[MAX_TIMER_NUM_IN_ONE_IHU_1S];
	IhuTimerElement_t timer10ms[MAX_TIMER_NUM_IN_ONE_IHU_10MS];
	IhuTimerElement_t timer1ms[MAX_TIMER_NUM_IN_ONE_IHU_1MS];
}IhuTimerTable_t;

#define TIMER_STATUS_DEACTIVE 1
#define TIMER_STATUS_ACTIVE 2
#define TIMER_STATUS_START 0xFD
#define TIMER_STATUS_STOP 0xFE
#define TIMER_STATUS_INVALID 0xFF

#define TIMER_TYPE_ONE_TIME 1
#define TIMER_TYPE_PERIOD 2
#define TIMER_TYPE_INVALID 0xFF

#define TIMER_RESOLUTION_1S 1
#define TIMER_RESOLUTION_10MS 2
#define TIMER_RESOLUTION_1MS 3
#define TIMER_RESOLUTION_INVALID 0xFF

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
#else
#endif

//Global variables
extern FsmStateItem_t FsmTimer[];
extern IhuTimerTable_t zIhuTimerTable;

//API
extern OPSTAT fsm_timer_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_timer_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_timer_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_timer_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
//用法中，TIMER_DUR就是多少秒为单位
extern OPSTAT ihu_timer_start(UINT8 task_id, UINT8 timer_id, UINT32 t_dur, UINT8 t_type, UINT8 t_res);
extern OPSTAT ihu_timer_stop(UINT8 task_id, UINT8 timer_id, UINT8 t_res);

//Local API
OPSTAT func_timer_hw_init(void);
void func_timer_routine_handler_1s(OS_TIMER timerid);
void func_timer_routine_handler_10ms(OS_TIMER timerid);
void func_timer_routine_handler_1ms(OS_TIMER timerid);

#endif /* L1VMFREEOS_L1TIMER_FREEOS_H_ */

