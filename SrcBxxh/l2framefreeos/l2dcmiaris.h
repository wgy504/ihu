/**
 ****************************************************************************************
 *
 * @file l2dcmiaris.h
 *
 * @brief L2 DCMIARIS
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2DCMIARIS_H_
#define L2FRAME_L2DCMIARIS_H_

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
	#error Un-correct constant definition
#endif


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  0x02
enum FSM_STATE_DCMIARIS
{
	FSM_STATE_DCMIARIS_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_DCMIARIS_ACTIVED,
	FSM_STATE_DCMIARIS_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern IhuFsmStateItem_t IhuFsmDcmiaris[];

//本地需要用到的核心参数
typedef struct strIhuCclDcmiPar
{	
	UINT8 cclDcmiWorkingMode;
	com_sensor_status_t sensor;
}strIhuCclDcmiPar_t;


//DCMI工作状态
//#define IHU_CCL_DCMI_WORKING_MODE_NONE 0
//#define IHU_CCL_DCMI_WORKING_MODE_SLEEP 1
//#define IHU_CCL_DCMI_WORKING_MODE_ACTIVE 2
//#define IHU_CCL_DCMI_WORKING_MODE_FAULT 3
//#define IHU_CCL_DCMI_WORKING_MODE_INVALID 0xFF

//控制定时器定时扫描是否启动
#define IHU_DCMIARIS_PERIOD_TIMER_ACTIVE 1
#define IHU_DCMIARIS_PERIOD_TIMER_DEACTIVE 2
#define IHU_DCMIARIS_PERIOD_TIMER_SET IHU_DCMIARIS_PERIOD_TIMER_DEACTIVE


//API
extern OPSTAT fsm_dcmiaris_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_dcmiaris_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_dcmiaris_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_dcmiaris_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_dcmiaris_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_dcmiaris_ccl_sensor_status_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
//extern OPSTAT fsm_dcmiaris_ccl_ctrl_cmd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//全局函数
extern INT16 ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_dcmi_value(void);


//Local API
OPSTAT func_dcmiaris_hw_init(void);
void func_dcmiaris_time_out_period_scan(void);

#endif /* L2FRAME_L2DCMIARIS_H_ */

