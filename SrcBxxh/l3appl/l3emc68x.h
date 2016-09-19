/**
 ****************************************************************************************
 *
 * @file l2emc68x.h
 *
 * @brief L2 EMC68X
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2EMC68X_H_
#define L2FRAME_L2EMC68X_H_

#include "vmfreeoslayer.h"
#include "l1comdef_freeos.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_EMC68X
{
	FSM_STATE_EMC68X_INITED = 0x02,
	FSM_STATE_EMC68X_AVTIVE,
	FSM_STATE_EMC68X_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmEmc68x[];

//API
extern OPSTAT fsm_emc68x_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_emc68x_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_emc68x_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_emc68x_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//Local API
OPSTAT func_emc68x_hw_init(void);

#endif /* L2FRAME_L2EMC68X_H_ */

