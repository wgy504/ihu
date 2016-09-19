/**
 ****************************************************************************************
 *
 * @file l2gpiocancer.h
 *
 * @brief L2 GPIOCANCER
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2GPIOCANCER_H_
#define L2FRAME_L2GPIOCANCER_H_

#include "sysdim.h"
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_EMC68X_ID)
	#include "vmfreeoslayer.h"
	#include "l1comdef_freeos.h"
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID)
	#include "vmucoslayer.h"
	#include "l1comdef_ucos.h"
#else
#endif


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_GPIOCANCER
{
	FSM_STATE_GPIOCANCER_INITED = 0x02,
	FSM_STATE_GPIOCANCER_AVTIVE,
	FSM_STATE_GPIOCANCER_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmGpiocancer[];

//API
extern OPSTAT fsm_gpiocancer_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_gpiocancer_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_gpiocancer_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_gpiocancer_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//Local API
OPSTAT func_gpiocancer_hw_init(void);

#endif /* L2FRAME_L2GPIOCANCER_H_ */

