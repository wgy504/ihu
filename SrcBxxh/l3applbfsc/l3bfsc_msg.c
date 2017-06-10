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
#include "blk230.h"
#include "l2adc_cs5532.h"

extern WmcInventory_t										zWmcInvenory;
extern WeightSensorParamaters_t					zWeightSensorParam;
extern MotorControlParamaters_t 				zMotorControlParam;
extern BfscWmcState_t										zBfscWmcState;

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
	  msg_struct_l3bfsc_wmc_set_config_req_t *config_req = (msg_struct_l3bfsc_wmc_set_config_req_t *)param_ptr;
    
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
		/* 
//		if( FSM_STATE_BFSC_COMBINATION == FsmGetState(TASK_ID_BFSC) )
//		{
//				IhuErrorPrint("L3BFSC: msg_wmc_set_config_req_process: FSM_STATE_BFSC_COMBINATION == FsmGetState(TASK_ID_BFSC), return\r\n");
//				*ec_ptr = ERROR_CODE_WRONG_WMC_STATE;
//				return;
//		}
	
		/* Check Parameters */
		*ec_ptr = msg_wmc_set_config_req_check(param_ptr);
		if(ERROR_CODE_INPUT_PARAMETER_KO == *ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_set_config_req_process: parameters check nok, return\r\n");
				return;
		}
		
		/* Process the message */
    // TODO: check each parameters...
    weightSensorConfig(&config_req->weight_sensor_param);
    zMotorControlParam = config_req->motor_control_param;
		
		/* Force to STOP */
		weight_sensor_send_cmd(WIGHT_SENSOR_CMD_TYPE_STOP);

		/* Force to BACK TO FSM_STATE_BFSC_CONFIGURATION */
		if( FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_CONFIGURATION) == IHU_FAILURE)
		{
				IhuErrorPrint("L3BFSC: Error Set FSM State FSM_STATE_BFSC_CONFIGURATION");	
				return;
		}
    
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
		msg_wmc_set_config_resp.msgid = (MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP);
    msg_wmc_set_config_resp.length = sizeof(msg_struct_l3bfsc_wmc_resp_t);
    //msg_wmc_set_config_resp.wmc_id = zWmcInvenory.wmc_id;
		msg_wmc_set_config_resp.errCode = ec;
    msg_wmc_set_config_resp.spare1 = 0;
    msg_wmc_set_config_resp.validFlag = TRUE;
		
		IhuDebugPrint("L3BFSC: msg_wmc_set_config_resp: msgid = 0x%08X error_code=%d", \
										msg_wmc_set_config_resp.msgid, ec);
		
		/* Send Message to CAN Task */
		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
														&msg_wmc_set_config_resp, MSG_SIZE_L3BFSC_WMC_SET_CONFIG_RESP);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_CANVELA]);
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

//void msg_wmc_get_config_resp(error_code_t ec)
//{

//		OPSTAT ret = IHU_SUCCESS;
//		msg_struct_l3bfsc_wmc_get_config_resp_t msg_wmc_get_config_resp;
//	
//		IhuDebugPrint("L3BFSC: msg_wmc_get_config_resp start ...\r\n");
//	
//		/* Check Input Parameter */
//		if(ec > ERROR_CODE_MAX)
//		{
//				IhuErrorPrint("L3BFSC: msg_wmc_get_config_resp: (ec > ERROR_CODE_MAX), return\r\n");
//				return;
//		}
//		
//		/* Build Message Content Header */
//		msg_wmc_get_config_resp.msgid = MSG_ID_L3BFSC_WMC_GET_CONFIG_RESP;
//		msg_wmc_get_config_resp.wmc_state = FsmGetState(TASK_ID_BFSC);
//		msg_wmc_get_config_resp.error_code = ec;		
//		if(ERROR_CODE_NO_ERROR == ec)
//		{
//				msg_wmc_get_config_resp.result = IHU_SUCCESS;
//		}
//		else
//		{
//				msg_wmc_get_config_resp.result = IHU_FAILURE;
//		}
//		
//		/* Build Message Content Body*/
//		/* TODO */
//		
//		IhuDebugPrint("L3BFSC: msg_wmc_get_config_resp: msgid = 0x%08X, result = %d, wmc_state = %d, error_code = %d\r\n", \
//										msg_wmc_get_config_resp.msgid, msg_wmc_get_config_resp.result, \
//										msg_wmc_get_config_resp.wmc_state, msg_wmc_get_config_resp.error_code);
//		
//		/* Send Message to CAN Task */
//		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_GET_CONFIG_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
//														&msg_wmc_get_config_resp, MSG_SIZE_L3BFSC_WMC_GET_CONFIG_RESP);
//		if (ret == IHU_FAILURE){
//			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_CANVELA]);
//			return;
//		}

//		return;
//}

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
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_SCAN) == IHU_FAILURE){
  		IhuErrorPrint("L3BFSC: Error Set FSM State FSM_STATE_BFSC_SCAN");	
  		return;
  	}
		
		/* NEED TO RE-INIT zBfscWmcState, every time received a START_REQ */
		zBfscWmcState.state = BFSC_MWC_STATE_WAIT_FOR_COMBIN_OUT;
		
		/* Active the Weight Sensor */
		weight_sensor_send_cmd(WIGHT_SENSOR_CMD_TYPE_START);
    
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
		msg_wmc_start_resp.msgid = (MSG_ID_L3BFSC_WMC_START_RESP);
		msg_wmc_start_resp.length = MSG_SIZE_L3BFSC_WMC_START_RESP;
    //msg_wmc_start_resp.wmc_id = zWmcInvenory.wmc_id;
		msg_wmc_start_resp.errCode = ec;
    msg_wmc_start_resp.validFlag = TRUE;
    msg_wmc_start_resp.spare1 = 0;
		
		IhuDebugPrint("L3BFSC: msg_wmc_start_resp: msgid = 0x%08X\r\n", \
										msg_wmc_start_resp.msgid);
		
		/* Send Message to CAN Task */
		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_START_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
														&msg_wmc_start_resp, MSG_SIZE_L3BFSC_WMC_START_RESP);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_CANVELA]);
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
		if( FSM_STATE_BFSC_SCAN != FsmGetState(TASK_ID_BFSC) || FSM_STATE_BFSC_COMBINATION != FsmGetState(TASK_ID_BFSC))
		{
				IhuErrorPrint("L3BFSC: msg_wmc_stop_req_process: FSM_STATE_BFSC_SCAN != FsmGetState(TASK_ID_BFSC), return\r\n");
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
		msg_wmc_stop_resp.msgid = (MSG_ID_L3BFSC_WMC_STOP_RESP);
		//msg_wmc_stop_resp.wmc_id = zWmcInvenory.wmc_id;
		msg_wmc_stop_resp.errCode = ec;
    msg_wmc_stop_resp.validFlag = TRUE;
    msg_wmc_stop_resp.spare1 = 0;
		
		IhuDebugPrint("L3BFSC: msg_wmc_stop_resp: msgid = 0x%08X\r\n", \
										msg_wmc_stop_resp.msgid);
		
		/* Send Message to CAN Task */
		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_STOP_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
														&msg_wmc_stop_resp, MSG_SIZE_L3BFSC_WMC_STOP_RESP);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_CANVELA]);
			return;
		}
		
		return;
}

/* ====== PROTOTYPE ====== */
//OPSTAT fsm_bfsc_wmc_command_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)			//MYC
void msg_wmc_command_req_process(void *param_ptr, error_code_t *ec_ptr)
{
		msg_struct_l3bfsc_wmc_command_req_t *cmd_req = (msg_struct_l3bfsc_wmc_command_req_t *)param_ptr;
		*ec_ptr = ERROR_CODE_NO_ERROR;
	
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
				*ec_ptr = ERROR_CODE_CALLING_ERROR;
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
				*ec_ptr = ERROR_CODE_CALLING_ERROR;
				return;
		}
		
		/* Process the message */
		if(cmd_req->comand_flags & SENSOR_COMMAND_ID_WEITGH_READ)
		{
				IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: SENSOR_COMMAND_ID_WEITGH_READ\r\n");
		}
		
		if(cmd_req->comand_flags & MOTOR_COMMAND_ID)
		{
				IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: MOTOR_COMMAND_ID 0x%x\r\n", cmd_req->motor_command);
			
				blk230_cmd_t motor_cmd = *(blk230_cmd_t *)(&cmd_req->motor_command);
				blk230_send_cmd(!motor_cmd.stop, motor_cmd.ccw, motor_cmd.speed, motor_cmd.time2stop);
		}
		
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
		msg_wmc_command_resp.msgid = (MSG_ID_L3BFSC_WMC_COMMAND_RESP);
    msg_wmc_command_resp.length = sizeof(msg_struct_l3bfsc_wmc_command_resp_t);
		msg_wmc_command_resp.wmc_id = zWmcInvenory.wmc_id;
		msg_wmc_command_resp.result.error_code = ec;
		
		/* Build Message Content Body */
		msg_wmc_command_resp.motor_speed = zMotorControlParam.MotorSpeed;
    msg_wmc_command_resp.sensor_weight = WeightSensorReadCurrent(&zWeightSensorParam);
		
		IhuDebugPrint("L3BFSC: msg_wmc_command_resp: msgid = 0x%08X\r\n", \
										msg_wmc_command_resp.msgid);
		
		/* Send Message to CAN Task */
		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_COMMAND_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
														&msg_wmc_command_resp, MSG_SIZE_L3BFSC_WMC_COMMAND_RESP);
		if (ret == IHU_FAILURE){
			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_CANVELA]);
			return;
		}
		
		return;
}

/* ====== PROTOTYPE ====== */
//OPSTAT fsm_bfsc_wmc_combin_req(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)			//MYC
void msg_wmc_combin_req_process(void *param_ptr, error_code_t *ec_ptr)
{
		IhuDebugPrint("L3BFSC: msg_wmc_combin_req_process start ...\r\n");
		msg_struct_l3bfsc_wmc_combin_out_req_t *pCombinOut;
	
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
		if( FSM_STATE_BFSC_SCAN != FsmGetState(TASK_ID_BFSC) )
		{
				IhuErrorPrint("L3BFSC: msg_wmc_combin_req_process: FSM_STATE_BFSC_SCAN != FsmGetState(TASK_ID_BFSC), return\r\n");
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
		
		/* Process the message: start motor */
    blk230_send_cmd(1, zMotorControlParam.MotorDirection, zMotorControlParam.MotorSpeed, zMotorControlParam.MotorRollingStartMs);
		blk230_set_lamp(WMC_LAMP_OUT1, WMC_LAMP_ON);
		blk230_set_lamp(WMC_LAMP_OUT2, WMC_LAMP_ON);
		blk230_set_lamp(WMC_LAMP_OUT3, WMC_LAMP_ON);
		
		pCombinOut = (msg_struct_l3bfsc_wmc_combin_out_req_t *)param_ptr;
		
		zBfscWmcState.state = BFSC_MWC_STATE_ALREADY_COMBIN_OUT;
    zBfscWmcState.last_combin_type.WeightCombineType = pCombinOut->weight_combin_type.WeightCombineType;
		IhuDebugPrint("L3BFSC: Set to FSM_STATE_BFSC_COMBINATION, WeightCombineType=%d", zBfscWmcState.last_combin_type.WeightCombineType);	
		
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_COMBINATION) == IHU_FAILURE){
				IhuErrorPrint("L3BFSC: Error Set FSM State FSM_STATE_BFSC_COMBINATION");	
				return;
		}
		
		IhuDebugPrint("L3BFSC: Set to FSM_STATE_BFSC_COMBINATION, WeightCombineType=%d", zBfscWmcState.last_combin_type);	
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
		msg_wmc_combin_resp.msgid = (MSG_ID_L3BFSC_WMC_COMBIN_RESP);
		//msg_wmc_combin_resp.wmc_id = zWmcInvenory.wmc_id;
		msg_wmc_combin_resp.errCode = ec;
    msg_wmc_combin_resp.validFlag = TRUE;
    msg_wmc_combin_resp.spare1 = 0;
		
		IhuDebugPrint("L3BFSC: msg_wmc_combin_resp: msgid = 0x%08X, will be wait for wait goes to 0.\r\n", \
										msg_wmc_combin_resp.msgid);
		
//		/* Send Message to CAN Task */
//		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_COMBIN_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
//														&msg_wmc_combin_resp, MSG_SIZE_L3BFSC_WMC_COMBIN_RESP);
//		if (ret == IHU_FAILURE){
//			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_CANVELA]);
//			return;
//		}

		return;
}
