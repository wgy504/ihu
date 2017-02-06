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
#include "l1vmfo.h"
#include "l2adclibra.h"
#include "l2spileo.h"
#include "l2i2caries.h"
//#include "l2pwmtaurus.h"
//#include "l2spsvirgo.h"
#include "l2canvela.h"
//#include "l2didocap.h"
#include "l2ledpisces.h"
//#include "l2ethorion.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_BFSC
{
	FSM_STATE_BFSC_INITED = 0x02,  	//MYC  	// 20170128: ALTOGETHER FOUR STATES, OTHERS RESERVED 
	FSM_STATE_BFSC_ACTIVED,					//MYC		// 20170128: ALTOGETHER FOUR STATES, OTHERS RESERVED
	FSM_STATE_BFSC_SCAN,
	FSM_STATE_BFSC_WEIGHT_REPORT,
	FSM_STATE_BFSC_ROLL_OUT,
	FSM_STATE_BFSC_GIVE_UP,
	FSM_STATE_BFSC_ERROR_TRAP,	
	
	//MYC ADDED
	FSM_STATE_BFSC_CONFIGURATION,		//MYC		// 20170128: ALTOGETHER FOUR STATES, OTHERS RESERVED
	FSM_STATE_BFSC_COMBINATION,			//MYC		// 20170128: ALTOGETHER FOUR STATES, OTHERS RESERVED
	
	FSM_STATE_BFSC_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t IhuFsmBfsc[];

//本地定义的常亮
#define IHU_L3BFSC_MOTO_HW_ERROR_RECOVER_TIMES_MAX 10

//API
extern OPSTAT fsm_bfsc_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_canvela_cmd_ctrl(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_adc_meas_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_i2c_moto_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_canvela_init_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_adc_new_material_ws(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_adc_material_drop(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_canvela_roll_out_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_bfsc_canvela_give_up_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//MYC ADDED 20170131
extern OPSTAT fsm_bfsc_wmc_set_config_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len);	//MYC
extern OPSTAT fsm_bfsc_wmc_get_config_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len);	//MYC
extern OPSTAT fsm_bfsc_wmc_start_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len);				//MYC
extern OPSTAT fsm_bfsc_wmc_command_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len);			//MYC
extern OPSTAT fsm_bfsc_wmc_stop_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len);				//MYC
extern OPSTAT fsm_bfsc_wmc_combin_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len);			//MYC

//Local API
OPSTAT func_bfsc_hw_init(WmcInventory_t *pwi);
OPSTAT func_bfsc_time_out_period_scan(void);
OPSTAT func_bfsc_time_out_wait_weight_command_process(void);
OPSTAT func_bfsc_time_out_roll_out_process(void);
OPSTAT func_bfsc_time_out_give_up_process(void);
void 	 func_bfsc_stm_main_recovery_from_fault(void);  //提供了一种比RESTART更低层次的状态恢复方式

//高级定义，简化程序的可读性
#define IHU_ERROR_PRINT_BFSC_WO_RETURN zIhuRunErrCnt[TASK_ID_BFSC]++; func_bfsc_stm_main_recovery_from_fault(); IhuErrorPrint
#define IHU_ERROR_PRINT_BFSC_RECOVERY(...)	do{zIhuRunErrCnt[TASK_ID_BFSC]++;  func_bfsc_stm_main_recovery_from_fault(); IhuErrorPrint(__VA_ARGS__);  return IHU_FAILURE;}while(0)	


#endif /* L3APPL_L3BFSC_H_ */

