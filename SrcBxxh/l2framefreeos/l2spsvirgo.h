/**
 ****************************************************************************************
 *
 * @file l2spsvirgo.h
 *
 * @brief L2 SPSVIRGO
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2SPSVIRGO_H_
#define L2FRAME_L2SPSVIRGO_H_

#include "sysdim.h"
#if ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_EMC68X_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID))
	#include "vmfreeoslayer.h"
	#include "l1comdef_freeos.h"
	#include "l1timer_freeos.h"		
#elif ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID))
	#include "vmucoslayer.h"
	#include "l1comdef_ucos.h"
	#include "l1timer_ucos.h"
	#include "l1hd_usart.h"
#else
#endif


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_SPSVIRGO
{
	FSM_STATE_SPSVIRGO_INITED = 0x02,
	FSM_STATE_SPSVIRGO_ACTIVED,
	FSM_STATE_SPSVIRGO_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmSpsvirgo[];

//API
extern OPSTAT fsm_spsvirgo_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_l2frame_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_ccl_ctrl_cmd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//Local API
OPSTAT func_spsvirgo_hw_init(void);
void func_spsvirgo_time_out_period_scan(void);

//Global API
extern void TIM_USART_GPRS_Init_Config(void);
extern void GPRS_UART_GSM_working_procedure_selection(UINT8 option, UINT8 sub_opt);
//extern u8  l1hd_f205_rtc_init (void);

#endif /* L2FRAME_L2SPSVIRGO_H_ */

