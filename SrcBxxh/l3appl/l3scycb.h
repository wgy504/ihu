/**
 ****************************************************************************************
 *
 * @file l3scycb.h
 *
 * @brief L3 SCYCB
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2SCYCB_H_
#define L2FRAME_L2SCYCB_H_

#include "l1comdef_ucos.h"
#include "vmucoslayer.h"
#include "l1timer_ucos.h"
#include "l2adclibra.h"
#include "l2spileo.h"
#include "l2i2caries.h"
#include "l2pwmtaurus.h"
#include "l2spsvirgo.h"
#include "l2gpiocancer.h"
#include "l2didocap.h"
#include "l2ledpisces.h"
#include "l2ethorion.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_SCYCB
{
	FSM_STATE_SCYCB_INITED = 0x02,
	FSM_STATE_SCYCB_ACTIVED,
	FSM_STATE_SCYCB_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmScycb[];

//API
extern OPSTAT fsm_scycb_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_scycb_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_scycb_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_scycb_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_scycb_adc_ul_data_pull_bwd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_scycb_adc_ul_ctrl_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_scycb_spi_ul_data_pull_bwd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_scycb_spi_ul_ctrl_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_scycb_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//Local API
OPSTAT func_scycb_hw_init(void);

#endif /* L2FRAME_L2SCYCB_H_ */

