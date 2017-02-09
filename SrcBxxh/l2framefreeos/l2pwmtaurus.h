/**
 ****************************************************************************************
 *
 * @file l2pwmtaurus.h
 *
 * @brief L2 PWMTAURUS
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2PWMTAURUS_H_
#define L2FRAME_L2PWMTAURUS_H_

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
//#define FSM_STATE_COMMON  0x02
enum FSM_STATE_PWMTAURUS
{
	FSM_STATE_PWMTAURUS_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_PWMTAURUS_ACTIVED,
	FSM_STATE_PWMTAURUS_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern IhuFsmStateItem_t IhuFsmPwmtaurus[];

//API
extern OPSTAT fsm_pwmtaurus_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_pwmtaurus_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_pwmtaurus_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_pwmtaurus_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_pwmtaurus_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);


//Local API
OPSTAT func_pwmtaurus_hw_init(void);
void func_pwmtaurus_time_out_period_scan(void);

#endif /* L2FRAME_L2PWMTAURUS_H_ */

