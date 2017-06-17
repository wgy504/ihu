/**
 ****************************************************************************************
 *
 * @file l3bfsc_msg.h
 *
 * @brief L3 BFSC
 *
 * BXXH team
 * Created by MYC, 20170131
 *
 ****************************************************************************************
 */

#ifndef L3APPL_L3BFSC_MSG_H_
#define L3APPL_L3BFSC_MSG_H_

#include "l1comdef_freeos.h"
#include "vmfreeoslayer.h"
#include "l1timer_freeos.h"
#include "l1vmfo.h"
#include "l2adclibra.h"
#include "l2spileo.h"
#include "l2i2caries.h"
#include "l2canvela.h"
#include "l2ledpisces.h"

//#define 	MSG_SIZE_L3BFSC_WMC_STARTUP_IND					(sizeof(msg_struct_l3bfsc_wmc_startup_ind_t))
//#define 	MSG_SIZE_L3BFSC_WMC_SET_CONFIG_REQ			(sizeof(msg_struct_l3bfsc_wmc_set_config_req_t))
//#define 	MSG_SIZE_L3BFSC_WMC_SET_CONFIG_RESP			(sizeof(msg_struct_l3bfsc_wmc_resp_t))
//#define 	MSG_SIZE_L3BFSC_WMC_GET_CONFIG_REQ			(sizeof(msg_struct_l3bfsc_wmc_get_config_req_t))
//#define 	MSG_SIZE_L3BFSC_WMC_GET_CONFIG_RESP			(sizeof(msg_struct_l3bfsc_wmc_get_config_resp_t))
//#define 	MSG_SIZE_L3BFSC_WMC_START_REQ						(sizeof(msg_struct_l3bfsc_wmc_start_req_t))
//#define 	MSG_SIZE_L3BFSC_WMC_START_RESP					(sizeof(msg_struct_l3bfsc_wmc_resp_t))
//#define 	MSG_SIZE_L3BFSC_WMC_STOP_REQ						(sizeof(msg_struct_l3bfsc_wmc_stop_req_t))
//#define 	MSG_SIZE_L3BFSC_WMC_STOP_RESP						(sizeof(msg_struct_l3bfsc_wmc_resp_t))
//#define 	MSG_SIZE_L3BFSC_WMC_WEIGHT_IND					(sizeof(msg_struct_l3bfsc_wmc_weight_ind_t))
//#define 	MSG_SIZE_L3BFSC_WMC_COMBIN_REQ					(sizeof(msg_struct_l3bfsc_wmc_combin_req_t))
//#define 	MSG_SIZE_L3BFSC_WMC_COMBIN_RESP					(sizeof(msg_struct_l3bfsc_wmc_resp_t))
//#define 	MSG_SIZE_L3BFSC_WMC_FAULT_IND						(sizeof(msg_struct_l3bfsc_wmc_fault_ind_t))
//#define 	MSG_SIZE_L3BFSC_WMC_COMMNAD_REQ					(sizeof(msg_struct_l3bfsc_wmc_command_req_t))
//#define 	MSG_SIZE_L3BFSC_WMC_COMMNAD_RESP				(sizeof(msg_struct_l3bfsc_wmc_command_resp_t))

//OPSTAT fsm_bfsc_wmc_set_config_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)	//MYC
//OPSTAT fsm_bfsc_wmc_get_config_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)	//MYC
//OPSTAT fsm_bfsc_wmc_start_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)				//MYC
//OPSTAT fsm_bfsc_wmc_command_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)			//MYC
//OPSTAT fsm_bfsc_wmc_stop_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)				//MYC
//OPSTAT fsm_bfsc_wmc_combin_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)			//MYC

void msg_wmc_set_config_req_process(void *param_ptr, error_code_t *ec_ptr);
error_code_t msg_wmc_set_config_req_check(void *param_ptr);
void msg_wmc_set_config_resp(error_code_t ec);

void msg_wmc_get_config_req_process(void *param_ptr, error_code_t *ec_ptr);
error_code_t msg_wmc_get_config_req_check(void *param_ptr);
void msg_wmc_get_config_resp(error_code_t ec);

void msg_wmc_start_req_process(void *param_ptr, error_code_t *ec_ptr);
error_code_t msg_wmc_start_req_check(void *param_ptr);
void msg_wmc_start_resp(error_code_t ec);

void msg_wmc_stop_req_process(void *param_ptr, error_code_t *ec_ptr);
error_code_t msg_wmc_stop_req_check(void *param_ptr);
void msg_wmc_stop_resp(error_code_t ec);

void msg_wmc_command_req_process(void *param_ptr, error_code_t *ec_ptr);
error_code_t msg_wmc_command_req_check(void *param_ptr);
void msg_wmc_command_resp(error_code_t ec, msg_struct_l3bfsc_wmc_command_req_t *p_msg);

void msg_wmc_combin_req_process(void *param_ptr, error_code_t *ec_ptr);
error_code_t msg_wmc_combin_req_check(void *param_ptr);
void msg_wmc_combin_resp(error_code_t ec);

void msg_wmc_err_inq_req_process(void *param_ptr, error_code_t *ec_ptr);
error_code_t msg_wmc_err_inq_req_check(void *param_ptr);
void msg_wmc_err_inq_resp(error_code_t ec);


#endif /* L3APPL_L3BFSC_MSG_H_ */

