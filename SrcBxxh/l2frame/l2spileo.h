/**
 ****************************************************************************************
 *
 * @file l2spileo.h
 *
 * @brief L2 SPILEO
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2SPILEO_H_
#define L2FRAME_L2SPILEO_H_

#include "vmfreeoslayer.h"
#include "l1comdef_freeos.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_SPILEO
{
	FSM_STATE_SPILEO_INITED = 0x02,
	FSM_STATE_SPILEO_AVTIVE,
	FSM_STATE_SPILEO_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmSpileo[];

//API
extern OPSTAT fsm_spileo_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spileo_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spileo_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spileo_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//Local API
OPSTAT func_spileo_hw_init(void);

#endif /* L2FRAME_L2SPILEO_H_ */

