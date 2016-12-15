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
#elif ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID))
	#include "vmucoslayer.h"
	#include "l1comdef_ucos.h"
	#include "l1timer_ucos.h"
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
typedef struct strIhuCclDidoPar
{	
	UINT8 cclDidoWorkingMode;
	com_sensor_status_t sensor;
}strIhuCclDidoPar_t;

#define IHU_CCL_DIDO_SENSOR_INDEX_LOCK 0
#define IHU_CCL_DIDO_SENSOR_INDEX_DOOR 1
#define IHU_CCL_DIDO_SENSOR_INDEX_SMOKE 2
#define IHU_CCL_DIDO_SENSOR_INDEX_WATER 3
#define IHU_CCL_DIDO_SENSOR_INDEX_FALL 4
#define IHU_CCL_DIDO_SENSOR_INDEX_BAT 5

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
void func_didocap_ccl_open_lock(UINT8 lockid);
void func_didocap_ccl_close_lock(UINT8 lockid);
void func_didocap_ccl_enable_lock(UINT8 lockid);



#endif /* L2FRAME_L2DIDOCAP_H_ */

