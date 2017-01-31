/**
 ****************************************************************************************
 *
 * @file l3bfsc_msg.c
 *
 * @brief L3 BFSC
 *
 * BXXH team
 * Created by MYC, 20170131
 *
 ****************************************************************************************
 */
 
#include "l3bfsc.h"
#include "l3bfsc_msg.h"

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

/* ====== PROTOTYPE ====== */
//OPSTAT fsm_bfsc_wmc_set_config_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)	//MYC
void msg_wmc_set_config_req_process(void *param_ptr, error_code_t *ec_ptr)
{
	
		IhuDebugPrint("L3BFSC: msg_wmc_set_config_req_process start ...\r\n");
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_set_config_req_process: NULL == param_ptr, return\r\n");
				if(NULL != ec_ptr) *ec_ptr = ERROR_CODE_CALLING_ERROR;
				return;
		}
		
		/* Check Input Parameter */
		if(NULL == ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_set_config_req_process: NULL == ec_ptr, return\r\n");
				return;
		}
		
		/* Check If it is the right/valid state to process the message */
		if( FSM_STATE_BFSC_COMBINATION == FsmGetState(TASK_ID_BFSC) )
		{
				IhuErrorPrint("L3BFSC: msg_wmc_set_config_req_process: FSM_STATE_BFSC_COMBINATION == FsmGetState(TASK_ID_BFSC), return\r\n");
				*ec_ptr = ERROR_CODE_WRONG_WMC_STATE;
				return;
		}
	
		/* Check Parameters */
		*ec_ptr = msg_wmc_set_config_req_check(param_ptr);
		if(ERROR_CODE_INPUT_PARAMETER_KO == *ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_set_config_req_process: parameters check nok, return\r\n");
				return;
		}
		
		/* Process the message */
		
		return;
}

error_code_t msg_wmc_set_config_req_check(void *param_ptr)
{
		IhuDebugPrint("L3BFSC: msg_wmc_set_config_req_check start ...\r\n");
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_set_config_req_check: NULL == param_ptr, return\r\n");
				return ERROR_CODE_CALLING_ERROR;
		}
		
		return ERROR_CODE_NO_ERROR;
}

void msg_wmc_set_config_resp(error_code_t ec)
{

		IhuDebugPrint("L3BFSC: msg_wmc_set_config_resp start ...\r\n");
	
		/* Check Input Parameter */
		if(ec > ERROR_CODE_MAX)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_set_config_resp: (ec > ERROR_CODE_MAX), return\r\n");
				return;
		}
		
		return;
}

/* ====== PROTOTYPE ====== */
//OPSTAT fsm_bfsc_wmc_get_config_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)	//MYC
void msg_wmc_get_config_req_process(void *param_ptr, error_code_t *ec_ptr)
{
	
		IhuDebugPrint("L3BFSC: msg_wmc_get_config_req_process start ...\r\n");
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_get_config_req_process: NULL == param_ptr, return\r\n");
				if(NULL != ec_ptr) *ec_ptr = ERROR_CODE_CALLING_ERROR;
				return;
		}
		
		/* Check Input Parameter */
		if(NULL == ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_get_config_req_process: NULL == ec_ptr, return\r\n");
				return;
		}
		
		/* Check If it is the right/valid state to process the message */
		if( FSM_STATE_BFSC_CONFIGURATION != FsmGetState(TASK_ID_BFSC) )
		{
				IhuErrorPrint("L3BFSC: msg_wmc_get_config_req_process: FSM_STATE_BFSC_CONFIGURATION != FsmGetState(TASK_ID_BFSC), return\r\n");
				*ec_ptr = ERROR_CODE_WRONG_WMC_STATE;
				return;
		}
		
		/* Check Parameters */
		*ec_ptr = msg_wmc_get_config_req_check(param_ptr);
		if(ERROR_CODE_INPUT_PARAMETER_KO == *ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_get_config_req_process: parameters check nok, return\r\n");
				return;
		}
		
		/* Process the message */
		
		return;
}

error_code_t msg_wmc_get_config_req_check(void *param_ptr)
{
		IhuDebugPrint("L3BFSC: msg_wmc_get_config_req_check start ...\r\n");
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_get_config_req_check: NULL == param_ptr, return\r\n");
				return ERROR_CODE_CALLING_ERROR;
		}
		
		return ERROR_CODE_NO_ERROR;
}

void msg_wmc_get_config_resp(error_code_t ec)
{

		IhuDebugPrint("L3BFSC: msg_wmc_get_config_resp start ...\r\n");
	
		/* Check Input Parameter */
		if(ec > ERROR_CODE_MAX)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_get_config_resp: (ec > ERROR_CODE_MAX), return\r\n");
				return;
		}

		return;
}

/* ====== PROTOTYPE ====== */
//OPSTAT fsm_bfsc_wmc_start_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)				//MYC
void msg_wmc_start_req_process(void *param_ptr, error_code_t *ec_ptr)
{

		IhuDebugPrint("L3BFSC: msg_wmc_start_req_process start ...\r\n");
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_start_req_process: NULL == param_ptr, return\r\n");
				if(NULL != ec_ptr) *ec_ptr = ERROR_CODE_CALLING_ERROR;
				return;
		}
		
		/* Check Input Parameter */
		if(NULL == ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_start_req_process: NULL == ec_ptr, return\r\n");
				return;
		}
		
		/* Check If it is the right/valid state to process the message */
		if( FSM_STATE_BFSC_CONFIGURATION != FsmGetState(TASK_ID_BFSC) )
		{
				IhuErrorPrint("L3BFSC: msg_wmc_start_req_process: FSM_STATE_BFSC_CONFIGURATION != FsmGetState(TASK_ID_BFSC), return\r\n");
				*ec_ptr = ERROR_CODE_WRONG_WMC_STATE;
				return;
		}		

		/* Check Parameters */
		*ec_ptr = msg_wmc_start_req_check(param_ptr);
		if(ERROR_CODE_INPUT_PARAMETER_KO == *ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_start_req_process: parameters check nok, return\r\n");
				return;
		}
		
		/* Process the message */
		
		return;
}

error_code_t msg_wmc_start_req_check(void *param_ptr)
{
		IhuDebugPrint("L3BFSC: msg_wmc_start_req_check start ...\r\n");
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_start_req_check: NULL == param_ptr, return\r\n");
				return ERROR_CODE_CALLING_ERROR;
		}
		
		return ERROR_CODE_NO_ERROR;
}

void msg_wmc_start_resp(error_code_t ec)
{

		IhuDebugPrint("L3BFSC: msg_wmc_start_resp start ...\r\n");
		
		/* Check Input Parameter */
		if(ec > ERROR_CODE_MAX)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_start_resp: (ec > ERROR_CODE_MAX), return\r\n");
				return;
		}

		return;
}

/* ====== PROTOTYPE ====== */
//OPSTAT fsm_bfsc_wmc_stop_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)				//MYC
void msg_wmc_stop_req_process(void *param_ptr, error_code_t *ec_ptr)
{
		IhuDebugPrint("L3BFSC: msg_wmc_stop_req_process start ...\r\n");
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_stop_req_process: NULL == param_ptr, return\r\n");
				if(NULL != ec_ptr) *ec_ptr = ERROR_CODE_CALLING_ERROR;
				return;
		}
		
		/* Check Input Parameter */
		if(NULL == ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_stop_req_process: NULL == ec_ptr, return\r\n");
				return;
		}
		
		/* Check If it is the right/valid state to process the message */
		if( FSM_STATE_BFSC_COMBINATION != FsmGetState(TASK_ID_BFSC) )
		{
				IhuErrorPrint("L3BFSC: msg_wmc_stop_req_process: FSM_STATE_BFSC_COMBINATION != FsmGetState(TASK_ID_BFSC), return\r\n");
				*ec_ptr = ERROR_CODE_WRONG_WMC_STATE;
				return;
		}
		
		/* Check Parameters */
		*ec_ptr = msg_wmc_stop_req_check(param_ptr);
		if(ERROR_CODE_INPUT_PARAMETER_KO == *ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_stop_req_process: parameters check nok, return\r\n");
				return;
		}
		
		/* Process the message */
		
		return;
}

error_code_t msg_wmc_stop_req_check(void *param_ptr)
{

		IhuDebugPrint("L3BFSC: msg_wmc_stop_req_check start ...\r\n");
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_stop_req_check: NULL == param_ptr, return\r\n");
				return ERROR_CODE_CALLING_ERROR;
		}

		return ERROR_CODE_NO_ERROR;
}

void msg_wmc_stop_resp(error_code_t ec)
{

		IhuDebugPrint("L3BFSC: msg_wmc_stop_resp start ...\r\n");
	
		/* Check Input Parameter */
		if(ec > ERROR_CODE_MAX)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_stop_resp: (ec > ERROR_CODE_MAX), return\r\n");
				return;
		}

		return;
}

/* ====== PROTOTYPE ====== */
//OPSTAT fsm_bfsc_wmc_command_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)			//MYC
void msg_wmc_command_req_process(void *param_ptr, error_code_t *ec_ptr)
{

		IhuDebugPrint("L3BFSC: msg_wmc_command_req_process start ...\r\n");
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_command_req_process: NULL == param_ptr, return\r\n");
				if(NULL != ec_ptr) *ec_ptr = ERROR_CODE_CALLING_ERROR;
				return;
		}
		
		/* Check Input Parameter */
		if(NULL == ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_command_req_process: NULL == ec_ptr, return\r\n");
				return;
		}
		
		/* Check If it is the right/valid state to process the message */
		if( FSM_STATE_BFSC_CONFIGURATION != FsmGetState(TASK_ID_BFSC) )
		{
				IhuErrorPrint("L3BFSC: msg_wmc_command_req_process: FSM_STATE_BFSC_CONFIGURATION != FsmGetState(TASK_ID_BFSC), return\r\n");
				*ec_ptr = ERROR_CODE_WRONG_WMC_STATE;
				return;
		}
		
		/* Check Parameters */
		*ec_ptr = msg_wmc_command_req_check(param_ptr);
		if(ERROR_CODE_INPUT_PARAMETER_KO == *ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_command_req_process: parameters check nok, return\r\n");
				return;
		}
		
		/* Process the message */
		
		return;
}

error_code_t msg_wmc_command_req_check(void *param_ptr)
{
		IhuDebugPrint("L3BFSC: msg_wmc_command_req_check start ...\r\n");
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_command_req_check: NULL == param_ptr, return\r\n");
				return ERROR_CODE_CALLING_ERROR;
		}
		
		return ERROR_CODE_NO_ERROR;
}

void msg_wmc_command_resp(error_code_t ec)
{

		IhuDebugPrint("L3BFSC: msg_wmc_command_resp start ...\r\n");
	
		/* Check Input Parameter */
		if(ec > ERROR_CODE_MAX)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_command_resp: (ec > ERROR_CODE_MAX), return\r\n");
				return;
		}

		return;
}

/* ====== PROTOTYPE ====== */
//OPSTAT fsm_bfsc_wmc_combin_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)			//MYC
void msg_wmc_combin_req_process(void *param_ptr, error_code_t *ec_ptr)
{
		IhuDebugPrint("L3BFSC: msg_wmc_combin_req_process start ...\r\n");
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_combin_req_process: NULL == param_ptr, return\r\n");
				if(NULL != ec_ptr) *ec_ptr = ERROR_CODE_CALLING_ERROR;
				return;
		}
		
		/* Check Input Parameter */
		if(NULL == ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_combin_req_process: NULL == ec_ptr, return\r\n");
				return;
		}
		
		/* Check If it is the right/valid state to process the message */
		if( FSM_STATE_BFSC_COMBINATION != FsmGetState(TASK_ID_BFSC) )
		{
				IhuErrorPrint("L3BFSC: msg_wmc_combin_req_process: FSM_STATE_BFSC_CONFIGURATION != FsmGetState(TASK_ID_BFSC), return\r\n");
				*ec_ptr = ERROR_CODE_WRONG_WMC_STATE;
				return;
		}
		
		/* Check Parameters */
		*ec_ptr = msg_wmc_combin_req_check(param_ptr);
		if(ERROR_CODE_INPUT_PARAMETER_KO == *ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_combin_req_process: parameters check nok, return\r\n");
				return;
		}
		
		/* Process the message */
		
		return;
}

error_code_t msg_wmc_combin_req_check(void *param_ptr)
{

		IhuDebugPrint("L3BFSC: msg_wmc_combin_req_check start ...\r\n");
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_combin_req_check: NULL == param_ptr, return\r\n");
				return ERROR_CODE_CALLING_ERROR;
		}

		return ERROR_CODE_NO_ERROR;
}

void msg_wmc_combin_resp(error_code_t ec)
{

		IhuDebugPrint("L3BFSC: msg_wmc_combin_resp start ...\r\n");
	
		/* Check Input Parameter */
		if(ec > ERROR_CODE_MAX)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_combin_resp: (ec > ERROR_CODE_MAX), return\r\n");
				return;
		}

		return;
}
