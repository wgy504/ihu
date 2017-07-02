/**
 ****************************************************************************************
 *
 * @file l2canvela.h
 *
 * @brief L2 CANVELA
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2CANVELA_H_
#define L2FRAME_L2CANVELA_H_

#include "sysdim.h"

#if ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID))
	#include "vmfreeoslayer.h"
	#include "l1comdef_freeos.h"
	#include "l1timer_freeos.h"		
	#include "l2packet.h"
	#include "huixmlcodec.h"
	
#elif ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID))
	#include "vmucoslayer.h"
	#include "l1comdef_ucos.h"
	#include "l1timer_ucos.h"
#else
	#error Un-correct constant definition
#endif


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  0x02
enum FSM_STATE_CANVELA
{
	FSM_STATE_CANVELA_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_CANVELA_ACTIVED,
	FSM_STATE_CANVELA_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//控制定时器定时扫描是否启动
#define IHU_CANVELA_PERIOD_TIMER_ACTIVE 1
#define IHU_CANVELA_PERIOD_TIMER_DEACTIVE 2
#define IHU_CANVELA_PERIOD_TIMER_SET IHU_CANVELA_PERIOD_TIMER_ACTIVE


//Global variables
extern IhuFsmStateItem_t IhuFsmCanvela[];

//Task init
extern OPSTAT fsm_canvela_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//Old Design: to be obsoleted
extern OPSTAT fsm_canvela_l3bfsc_init_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_l3bfsc_new_ws_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_l3bfsc_roll_out_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_l3bfsc_give_up_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_l3bfsc_error_status_report(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_l3bfsc_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//BFSC Send
extern OPSTAT fsm_canvela_bfsc_l2frame_startup_ind(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_bfsc_l2frame_set_config_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_bfsc_l2frame_start_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_bfsc_l2frame_stop_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_bfsc_l2frame_new_ws_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_bfsc_l2frame_repeat_ws_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_bfsc_l2frame_combin_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_bfsc_l2frame_fault_ind(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_bfsc_l2frame_command_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_bfsc_l2frame_err_inq_cmd_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_bfsc_l2frame_heart_beat_report(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_bfsc_l2frame_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_bfsc_l2frame_snd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//IAP Send
extern OPSTAT fsm_canvela_l3iap_inventory_report(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_canvela_l3iap_sw_package_report(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//Common Receive
extern OPSTAT fsm_canvela_l2frame_rcv_handler(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len);


//Local API
OPSTAT func_canvela_hw_init(void);
void func_canvela_time_out_period_scan(void);
void func_canvela_frame_send(IHU_HUITP_L2FRAME_STD_frame_header_t *pframe);
OPSTAT WmcAwsMsgCheck(void * param_ptr, UINT16 msg_len);
OPSTAT func_canitfleo_l2frame_msg_bfsc_set_config_req_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_set_config_req_t *rcv);
OPSTAT func_canitfleo_l2frame_msg_bfsc_start_req_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_start_req_t *rcv);
OPSTAT func_canitfleo_l2frame_msg_bfsc_stop_req_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_stop_req_t *rcv);
OPSTAT func_canitfleo_l2frame_msg_bfsc_ws_comb_out_req_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_req_t *rcv);
OPSTAT func_canitfleo_l2frame_msg_bfsc_command_req_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_command_req_t *rcv);
OPSTAT func_canitfleo_l2frame_msg_bfsc_err_inq_cmd_req_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_req_t *rcv);
OPSTAT func_canitfleo_l2frame_msg_bfsc_heart_beat_confirm_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_confirm_t *rcv);
OPSTAT func_canitfleo_l2frame_msg_inventory_confirm_received_handle(StrMsg_HUITP_MSGID_sui_inventory_confirm_t *rcv);
OPSTAT func_canitfleo_l2frame_msg_sw_package_confirm_received_handle(StrMsg_HUITP_MSGID_sui_sw_package_confirm_t *rcv);



//高级定义，简化程序的可读性
#define IHU_ERROR_PRINT_CANVELA(...)	do{zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;  IhuErrorPrint(__VA_ARGS__);  return IHU_FAILURE;}while(0)	

#endif /* L2FRAME_L2CANVELA_H_ */

