/**
 ****************************************************************************************
 *
 * @file l2i2caries.h
 *
 * @brief L2 I2CARIES
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2I2CARIES_H_
#define L2FRAME_L2I2CARIES_H_

#include "vmfreeoslayer.h"
#include "l1comdef_freeos.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_I2CARIES
{
	FSM_STATE_I2CARIES_INITED = 0x02,
	FSM_STATE_I2CARIES_AVTIVE,
	FSM_STATE_I2CARIES_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmI2caries[];

//API
extern OPSTAT fsm_i2caries_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_i2caries_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_i2caries_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_i2caries_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//Local API
OPSTAT func_i2caries_hw_init(void);

#endif /* L2FRAME_L2I2CARIES_H_ */

