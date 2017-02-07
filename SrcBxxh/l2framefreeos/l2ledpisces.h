/**
 ****************************************************************************************
 *
 * @file l2ledpisces.h
 *
 * @brief L2 LEDPISCES
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2LEDPISCES_H_
#define L2FRAME_L2LEDPISCES_H_

#include "sysdim.h"
#if ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID))
	#include "vmfreeoslayer.h"
	#include "l1comdef_freeos.h"
	#include "l1timer_freeos.h"
	#include "bsp_led.h"
	#include "bsp_dido.h"
	
#elif ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID))
	#include "vmucoslayer.h"
	#include "l1comdef_ucos.h"
	#include "l1timer_ucos.h"
	#include "l1hd_led.h"
#else
	#error Un-correct constant definition
#endif

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_LEDPISCES
{
	FSM_STATE_LEDPISCES_INITED = 0x02,
	FSM_STATE_LEDPISCES_ACTIVED,
	FSM_STATE_LEDPISCES_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern IhuFsmStateItem_t IhuFsmLedpisces[];

//GPIO Based LED and Output Wave Gennerator, 简称GALOWAG / Galowag
//总控表结构 //暂时只能支持秒级计数器
typedef struct GlobalGalowagCtrlTable
{
	UINT8 galId;
	UINT8 galState;
	UINT16 galPattenOn;
	UINT16 galPattenOff;
	UINT16 galPattenCycle;
	UINT32 galTotalDuration;
	UINT16 galPeriodWorkCounter;
	UINT32 galTotalWorkCounter;
	void (*galFuncOn)(void);
	void (*galFuncOff)(void);
}GlobalGalowagCtrlTable_t;
enum GLOBAL_GALOWAG_CTRL_TABLE_ID
{
	GALOWAG_CTRL_ID_MIN = 0x00,
	GALOWAG_CTRL_ID_GLOBAL_POWER,
	GALOWAG_CTRL_ID_GLOBAL_COMMU,
	GALOWAG_CTRL_ID_GLOBAL_WORK_STATE,
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)	
	GALOWAG_CTRL_ID_CCL_LOCK1,
	GALOWAG_CTRL_ID_CCL_LOCK2,
	GALOWAG_CTRL_ID_CCL_LOCK3,
	GALOWAG_CTRL_ID_CCL_LOCK4,
#endif
	GALOWAG_CTRL_ID_MAX,
};
enum GLOBAL_LED_LIGHT_CTRL_TABLE_STATE
{
	GALOWAG_CTRL_STATE_MIN = 0x00,
	GALOWAG_CTRL_STATE_ON,
	GALOWAG_CTRL_STATE_OFF,
	GALOWAG_CTRL_STATE_MAX,
};

//控制LED输出频率闪烁功能是否启动
#define IHU_LEDPISCES_GALOWAG_FUNC_ACTIVE 1
#define IHU_LEDPISCES_GALOWAG_FUNC_DEACTIVE 2
#define IHU_LEDPISCES_GALOWAG_FUNC_SET IHU_LEDPISCES_GALOWAG_FUNC_ACTIVE

//控制定时器定时扫描是否启动
#define IHU_LEDPISCES_PERIOD_TIMER_ACTIVE 1
#define IHU_LEDPISCES_PERIOD_TIMER_DEACTIVE 2
#define IHU_LEDPISCES_PERIOD_TIMER_SET IHU_LEDPISCES_PERIOD_TIMER_DEACTIVE


//API
extern OPSTAT fsm_ledpisces_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ledpisces_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ledpisces_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ledpisces_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ledpisces_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT ihu_ledpisces_galowag_start(UINT8 galId, UINT32 galDur);
extern OPSTAT ihu_ledpisces_galowag_stop(UINT8 galId);

//Local API
OPSTAT func_ledpisces_hw_init(void);
void func_ledpisces_time_out_period_scan(void);
void func_ledpisces_time_out_galowag_scan(void);

#endif /* L2FRAME_L2LEDPISCES_H_ */

