/**
 ****************************************************************************************
 *
 * @file l1vmfo.h
 *
 * @brief L1 VMFO
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2VMFO_H_
#define L2FRAME_L2VMFO_H_

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
	#include "l1hd_led.h"
#else
	#error Un-correct constant definition
#endif


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_VMFO
{
	FSM_STATE_VMFO_INITED = 0x02,
	FSM_STATE_VMFO_ACTIVED,
	FSM_STATE_VMFO_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t IhuFsmVmfo[];

//Local variables
//structure
typedef struct IhuVmfoTaskInitCtrlInfo
{
	UINT8 active;
	UINT8 state;
	UINT8 heart;
}IhuVmfoTaskInitCtrlInfo_t;
#define IHU_VMFO_TASK_INIT_NULL 0x00
#define IHU_VMFO_TASK_INIT_SEND 0x01
#define IHU_VMFO_TASK_INIT_WAIT_FOR_BACK 0x02
#define IHU_VMFO_TASK_INIT_FEEDBACK 0x03
#define IHU_VMFO_TASK_INIT_INVALID 0xFF
#define IHU_VMFO_TASK_NULL 0
#define IHU_VMFO_TASK_ACTIVE 1
#define IHU_VMFO_TASK_DEACTIVE 2
#define IHU_VMFO_TASK_HEART_NULL 0
#define IHU_VMFO_TASK_HEART_RECEIVED 1
#define IHU_VMFO_TASK_HEART_NOREC 2

#define IHU_VMFO_TASK_HEART_BEAT_ALL 1
#define IHU_VMFO_TASK_HEART_BEAT_L3_ONLY 2
#define IHU_VMFO_TASK_HEART_BEAT_CHOICE IHU_VMFO_TASK_HEART_BEAT_L3_ONLY


//API
extern OPSTAT fsm_vmfo_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_vmfo_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_vmfo_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_vmfo_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_vmfo_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_vmfo_init_fb(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_vmfo_heart_beat(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);


//Local API
OPSTAT func_vmfo_hw_init(void);
void func_vmfo_time_out_period_scan(void);
BOOL func_vmfo_init_caculate_all_fb(void);
BOOL func_vmfo_heart_caculate_all_received(void);

#endif /* L2FRAME_L2VMFO_H_ */

