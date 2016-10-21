/**
 ****************************************************************************************
 *
 * @file l2ethorion.h
 *
 * @brief L2 ETHORION
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2ETHORION_H_
#define L2FRAME_L2ETHORION_H_

#include "sysdim.h"
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_EMC68X_ID)
	#include "vmfreeoslayer.h"
	#include "l1comdef_freeos.h"
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
enum FSM_STATE_ETHORION
{
	FSM_STATE_ETHORION_INITED = 0x02,
	FSM_STATE_ETHORION_ACTIVED,
	FSM_STATE_ETHORION_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmEthorion[];

//API
extern OPSTAT fsm_ethorion_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ethorion_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ethorion_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ethorion_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ethorion_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//Local API
OPSTAT func_ethorion_hw_init(void);
void func_ethorion_time_out_period_scan(void);

#endif /* L2FRAME_L2ETHORION_H_ */

