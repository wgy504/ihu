/**
 ****************************************************************************************
 *
 * @file l2adcaries.h
 *
 * @brief L2 ADC ARIES
 *
 * BXXH team
 * Created by ZJL, 20160127
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2ADCARIES_H_
#define L2FRAME_L2ADCARIES_H_

#include "vmlayer.h"
#include "l1comdef.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_ADCARIES
{
	FSM_STATE_ADCARIES_INITED = 0x02,
	FSM_STATE_ADCARIES_AVTIVE,
	FSM_STATE_ADCARIES_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmAdcaries[];

//API
extern OPSTAT fsm_adcaries_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_adcaries_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_adcaries_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_adcaries_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);

//Local API
OPSTAT func_adcaries_hw_init(void);

#endif /* L2FRAME_L2ADCARIES_H_ */

