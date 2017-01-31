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
//#define 	MSG_SIZE_L3BFSC_WMC_COMMAND_REQ					(sizeof(msg_struct_l3bfsc_wmc_command_req_t))
//#define 	MSG_SIZE_L3BFSC_WMC_COMMAND_RESP				(sizeof(msg_struct_l3bfsc_wmc_command_resp_t))

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
		OPSTAT ret = IHU_SUCCESS;
		msg_struct_l3bfsc_wmc_resp_t msg_wmc_set_config_resp;
	
		IhuDebugPrint("L3BFSC: msg_wmc_set_config_resp start ...\r\n");
	
		/* Check Input Parameter */
		if(ec > ERROR_CODE_MAX)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_set_config_resp: (ec > ERROR_CODE_MAX), return\r\n");
				return;
		}
		
		/* Build Message Content */
		msg_wmc_set_config_resp.msgid = MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP;
		msg_wmc_set_config_resp.wmc_state = FsmGetState(TASK_ID_BFSC);
		msg_wmc_set_config_resp.error_code = ec;		
		if(ERROR_CODE_NO_ERROR == ec)
		{
				msg_wmc_set_config_resp.result = IHU_SUCCESS;
		}
		else
		{
				msg_wmc_set_config_resp.result = IHU_FAILURE;
		}
		
		IhuDebugPrint("L3BFSC: msg_wmc_set_config_resp: msgid = 0x%08X, result = %d, wmc_state = %d, error_code = %d\r\n", \
										msg_wmc_set_config_resp.msgid, msg_wmc_set_config_resp.result, \
										msg_wmc_set_config_resp.wmc_state, msg_wmc_set_config_resp.error_code);
		
		/* Send Message to CAN Task */
		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
														&msg_wmc_set_config_resp, MSG_SIZE_L3BFSC_WMC_SET_CONFIG_RESP);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC], zIhuTaskInfo[TASK_ID_CANVELA]);
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

		OPSTAT ret = IHU_SUCCESS;
		msg_struct_l3bfsc_wmc_get_config_resp_t msg_wmc_get_config_resp;
	
		IhuDebugPrint("L3BFSC: msg_wmc_get_config_resp start ...\r\n");
	
		/* Check Input Parameter */
		if(ec > ERROR_CODE_MAX)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_get_config_resp: (ec > ERROR_CODE_MAX), return\r\n");
				return;
		}
		
		/* Build Message Content Header */
		msg_wmc_get_config_resp.msgid = MSG_ID_L3BFSC_WMC_GET_CONFIG_RESP;
		msg_wmc_get_config_resp.wmc_state = FsmGetState(TASK_ID_BFSC);
		msg_wmc_get_config_resp.error_code = ec;		
		if(ERROR_CODE_NO_ERROR == ec)
		{
				msg_wmc_get_config_resp.result = IHU_SUCCESS;
		}
		else
		{
				msg_wmc_get_config_resp.result = IHU_FAILURE;
		}
		
		/* Build Message Content Body*/
		/* TODO */
		
		IhuDebugPrint("L3BFSC: msg_wmc_get_config_resp: msgid = 0x%08X, result = %d, wmc_state = %d, error_code = %d\r\n", \
										msg_wmc_get_config_resp.msgid, msg_wmc_get_config_resp.result, \
										msg_wmc_get_config_resp.wmc_state, msg_wmc_get_config_resp.error_code);
		
		/* Send Message to CAN Task */
		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_GET_CONFIG_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
														&msg_wmc_get_config_resp, MSG_SIZE_L3BFSC_WMC_GET_CONFIG_RESP);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC], zIhuTaskInfo[TASK_ID_CANVELA]);
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

		OPSTAT ret = IHU_SUCCESS;
		msg_struct_l3bfsc_wmc_resp_t msg_wmc_start_resp;
	
		IhuDebugPrint("L3BFSC: msg_wmc_start_resp start ...\r\n");
		
		/* Check Input Parameter */
		if(ec > ERROR_CODE_MAX)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_start_resp: (ec > ERROR_CODE_MAX), return\r\n");
				return;
		}

		/* Build Message Content */
		msg_wmc_start_resp.msgid = MSG_ID_L3BFSC_WMC_START_RESP;
		msg_wmc_start_resp.wmc_state = FsmGetState(TASK_ID_BFSC);
		msg_wmc_start_resp.error_code = ec;		
		if(ERROR_CODE_NO_ERROR == ec)
		{
				msg_wmc_start_resp.result = IHU_SUCCESS;
		}
		else
		{
				msg_wmc_start_resp.result = IHU_FAILURE;
		}
		
		IhuDebugPrint("L3BFSC: msg_wmc_start_resp: msgid = 0x%08X, result = %d, wmc_state = %d, error_code = %d\r\n", \
										msg_wmc_start_resp.msgid, msg_wmc_start_resp.result, \
										msg_wmc_start_resp.wmc_state, msg_wmc_start_resp.error_code);
		
		/* Send Message to CAN Task */
		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_START_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
														&msg_wmc_start_resp, MSG_SIZE_L3BFSC_WMC_START_RESP);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC], zIhuTaskInfo[TASK_ID_CANVELA]);
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
		OPSTAT ret = IHU_SUCCESS;
		msg_struct_l3bfsc_wmc_resp_t msg_wmc_stop_resp;
	
		IhuDebugPrint("L3BFSC: msg_wmc_stop_resp start ...\r\n");
	
		/* Check Input Parameter */
		if(ec > ERROR_CODE_MAX)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_stop_resp: (ec > ERROR_CODE_MAX), return\r\n");
				return;
		}

		/* Build Message Content */
		msg_wmc_stop_resp.msgid = MSG_ID_L3BFSC_WMC_STOP_RESP;
		msg_wmc_stop_resp.wmc_state = FsmGetState(TASK_ID_BFSC);
		msg_wmc_stop_resp.error_code = ec;		
		if(ERROR_CODE_NO_ERROR == ec)
		{
				msg_wmc_stop_resp.result = IHU_SUCCESS;
		}
		else
		{
				msg_wmc_stop_resp.result = IHU_FAILURE;
		}
		
		IhuDebugPrint("L3BFSC: msg_wmc_stop_resp: msgid = 0x%08X, result = %d, wmc_state = %d, error_code = %d\r\n", \
										msg_wmc_stop_resp.msgid, msg_wmc_stop_resp.result, \
										msg_wmc_stop_resp.wmc_state, msg_wmc_stop_resp.error_code);
		
		/* Send Message to CAN Task */
		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_STOP_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
														&msg_wmc_stop_resp, MSG_SIZE_L3BFSC_WMC_STOP_RESP);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC], zIhuTaskInfo[TASK_ID_CANVELA]);
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
	
		OPSTAT ret = IHU_SUCCESS;
		msg_struct_l3bfsc_wmc_command_resp_t msg_wmc_command_resp;

		IhuDebugPrint("L3BFSC: msg_wmc_command_resp start ...\r\n");
	
		/* Check Input Parameter */
		if(ec > ERROR_CODE_MAX)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_command_resp: (ec > ERROR_CODE_MAX), return\r\n");
				return;
		}

		/* Build Message Content Header */
		msg_wmc_command_resp.msgid = MSG_ID_L3BFSC_WMC_COMMAND_RESP;
		msg_wmc_command_resp.wmc_state = FsmGetState(TASK_ID_BFSC);
		msg_wmc_command_resp.error_code = ec;		
		if(ERROR_CODE_NO_ERROR == ec)
		{
				msg_wmc_command_resp.result = IHU_SUCCESS;
		}
		else
		{
				msg_wmc_command_resp.result = IHU_FAILURE;
		}
		
		/* Build Message Content Body */
		/* TODO */
		
		IhuDebugPrint("L3BFSC: msg_wmc_command_resp: msgid = 0x%08X, result = %d, wmc_state = %d, error_code = %d\r\n", \
										msg_wmc_command_resp.msgid, msg_wmc_command_resp.result, \
										msg_wmc_command_resp.wmc_state, msg_wmc_command_resp.error_code);
		
		/* Send Message to CAN Task */
		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_COMMAND_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
														&msg_wmc_command_resp, MSG_SIZE_L3BFSC_WMC_COMMAND_RESP);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC], zIhuTaskInfo[TASK_ID_CANVELA]);
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
		OPSTAT ret = IHU_SUCCESS;
		msg_struct_l3bfsc_wmc_resp_t msg_wmc_combin_resp;
	
		IhuDebugPrint("L3BFSC: msg_wmc_combin_resp start ...\r\n");
	
		/* Check Input Parameter */
		if(ec > ERROR_CODE_MAX)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_combin_resp: (ec > ERROR_CODE_MAX), return\r\n");
				return;
		}
		
		/* Build Message Content Header */
		msg_wmc_combin_resp.msgid = MSG_ID_L3BFSC_WMC_COMBIN_RESP;
		msg_wmc_combin_resp.wmc_state = FsmGetState(TASK_ID_BFSC);
		msg_wmc_combin_resp.error_code = ec;		
		if(ERROR_CODE_NO_ERROR == ec)
		{
				msg_wmc_combin_resp.result = IHU_SUCCESS;
		}
		else
		{
				msg_wmc_combin_resp.result = IHU_FAILURE;
		}
		
		IhuDebugPrint("L3BFSC: msg_wmc_combin_resp: msgid = 0x%08X, result = %d, wmc_state = %d, error_code = %d\r\n", \
										msg_wmc_combin_resp.msgid, msg_wmc_combin_resp.result, \
										msg_wmc_combin_resp.wmc_state, msg_wmc_combin_resp.error_code);
		
		/* Send Message to CAN Task */
		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_COMBIN_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
														&msg_wmc_combin_resp, MSG_SIZE_L3BFSC_WMC_COMBIN_RESP);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskInfo[TASK_ID_BFSC], zIhuTaskInfo[TASK_ID_CANVELA]);
			return;
		}

		return;
}
