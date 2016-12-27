/**
 ****************************************************************************************
 *
 * @file l2didocap.h
 *
 * @brief L2 DIDOCAP
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2DIDOCAP_H_
#define L2FRAME_L2DIDOCAP_H_

#include "sysdim.h"
#if ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID))
	#include "vmfreeoslayer.h"
	#include "l1comdef_freeos.h"
	#include "l1timer_freeos.h"
	#include "l2spsvirgo.h"	
	
#elif ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID))
	#include "vmucoslayer.h"
	#include "l1comdef_ucos.h"
	#include "l1timer_ucos.h"
#else
#endif

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#include "l3ccl.h"
#else
#endif

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_DIDOCAP
{
	FSM_STATE_DIDOCAP_INITED = 0x02,
	FSM_STATE_DIDOCAP_ACTIVED,
	FSM_STATE_DIDOCAP_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmDidocap[];

//本地需要用到的核心参数
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	typedef struct strIhuCclDidoPar
	{	
		UINT8 cclDidoWorkingMode;
		com_sensor_status_t sensor;
		bool flagSensorLastTimeScanFault;
		bool flagSensorThisTimeScanFault;
		bool flagDoorLockLastTimeScanFault;  //单独为门锁建立的故障跟踪，以便防止关门锁信号与故障恢复报告之间的冲突
		bool flagDoorLockThisTimeScanFault;
	}strIhuCclDidoPar_t;
	#define IHU_CCL_DIDO_SENSOR_INDEX_LOCK 0
	#define IHU_CCL_DIDO_SENSOR_INDEX_DOOR 1
	#define IHU_CCL_DIDO_SENSOR_INDEX_SMOKE 2
	#define IHU_CCL_DIDO_SENSOR_INDEX_WATER 3
	#define IHU_CCL_DIDO_SENSOR_INDEX_FALL 4
	#define IHU_CCL_DIDO_SENSOR_INDEX_BAT 5
#endif

//DIDO工作状态
#define IHU_CCL_DIDO_WORKING_MODE_NONE 0
#define IHU_CCL_DIDO_WORKING_MODE_SLEEP 1
#define IHU_CCL_DIDO_WORKING_MODE_ACTIVE 2
#define IHU_CCL_DIDO_WORKING_MODE_FAULT 3
#define IHU_CCL_DIDO_WORKING_MODE_INVALID 0xFF


//API
extern OPSTAT fsm_didocap_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_didocap_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_didocap_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_didocap_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_didocap_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_didocap_ccl_sensor_status_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_didocap_ccl_ctrl_cmd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
	

//Local API
OPSTAT func_didocap_hw_init(void);
void func_didocap_time_out_period_scan(void);
void func_didocap_time_out_external_trigger_period_scan(void);
void func_didocap_time_out_work_mode_period_scan(void);
void func_didocap_ccl_work_mode_dl_cmd_open_lock(UINT8 lockid);
void func_didocap_ccl_work_mode_dl_cmd_close_lock(UINT8 lockid);
void func_didocap_ccl_work_mode_dl_cmd_enable_lock(UINT8 lockid);
bool func_didocap_ccl_work_mode_ul_scan_any_door_open(void);
bool func_didocap_ccl_work_mode_ul_scan_all_door_and_lock_close(void);
bool func_didocap_ccl_work_mode_ul_scan_enable_lock_trigger(void);
bool func_didocap_ccl_work_mode_ul_scan_door_and_lock_status_change(void);
bool func_didocap_ccl_sleep_mode_ul_scan_lock_trigger(void);
bool func_didocap_ccl_sleep_mode_ul_scan_shake_trigger(void);
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_status(void);
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_door_open(UINT8 doorid);
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_lock_open(UINT8 lockid);
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_water(void);
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_smoke(void);
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_fall(void);
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_battery(void);
bool func_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_recover(void);

#endif /* L2FRAME_L2DIDOCAP_H_ */

