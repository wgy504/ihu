/**
 ****************************************************************************************
 *
 * @file l3ccl.h
 *
 * @brief L3 CCL
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L3APPL_L3CCL_H_
#define L3APPL_L3CCL_H_

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
#include "l1vmfo.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_CCL
{
	FSM_STATE_CCL_INITED = 0x02,
	FSM_STATE_CCL_ACTIVED,  			//IHU被唤醒，开始工作。开的原因可能是触发电源，也可能是定时触发
	FSM_STATE_CCL_CLOUD_INQUERY,
	FSM_STATE_CCL_TO_OPEN_DOOR,
	FSM_STATE_CCL_DOOR_OPEN,
	FSM_STATE_CCL_FATAL_FAULT,
	FSM_STATE_CCL_EVENT_REPORT,	
	FSM_STATE_CCL_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmCcl[];

//API
OPSTAT fsm_ccl_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_adc_ul_data_pull_bwd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_adc_ul_ctrl_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_spi_ul_data_pull_bwd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_spi_ul_ctrl_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_dido_sensor_status_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_sps_cloud_fb(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_sps_sensor_status_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_dido_lock_trigger_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_dido_door_ilg_open_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_dido_lock_ilg_open_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_dido_door_open_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_lock_and_door_close_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
OPSTAT fsm_ccl_dido_sensor_warning_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//Local API
OPSTAT func_ccl_hw_init(void);
void func_ccl_time_out_working_status_scan(void);
void func_ccl_time_out_period_event_report(void);



#endif /* L3APPL_L3CCL_H_ */

