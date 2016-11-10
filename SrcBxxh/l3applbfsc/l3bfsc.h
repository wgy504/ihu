/**
 ****************************************************************************************
 *
 * @file l3bfsc.h
 *
 * @brief L3 BFSC
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L3APPL_L3BFSC_H_
#define L3APPL_L3BFSC_H_

#include "l1comdef_freeos.h"
#include "vmfreeoslayer.h"
#include "l1timer_freeos.h"
#include "l2adclibra.h"
#include "l2spileo.h"
#include "l2i2caries.h"
#include "l2pwmtaurus.h"
#include "l2spsvirgo.h"
#include "l2canvela.h"
#include "l2didocap.h"
#include "l2ledpisces.h"
#include "l2ethorion.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_BFSC
{
	FSM_STATE_BFSC_INITED = 0x02,
	FSM_STATE_BFSC_ACTIVED,
	FSM_STATE_BFSC_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmBfsc[];

//API
extern OPSTAT fsm_bfsc_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_adc_ul_data_pull_bwd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_adc_ul_ctrl_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_spi_ul_data_pull_bwd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_spi_ul_ctrl_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//Local API
OPSTAT func_bfsc_hw_init(void);
void func_bfsc_time_out_period_scan(void);

#endif /* L3APPL_L3BFSC_H_ */

