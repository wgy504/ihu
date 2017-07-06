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
#include "l2adclibra.h"

extern WmcInventory_t										zWmcInvenory;
extern WeightSensorParamaters_t					zWeightSensorParam;
extern MotorControlParamaters_t 				zMotorControlParam;
extern BfscWmcState_t										zBfscWmcState;

extern strIhuBfscAdcWeightPar_t 				zIhuAdcBfscWs;

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
		/* */
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
		//blk230_set_lamp(WMC_LAMP_OUT2_GREEN, WMC_LAMP_OFF);
		blk230_led_send_cmd(WMC_LAMP_OUT2_GREEN, LED_COMMNAD_OFF);
		//blk230_set_lamp(WMC_LAMP_OUT3_YELLOW, WMC_LAMP_OFF);
		blk230_led_send_cmd(WMC_LAMP_OUT3_YELLOW, LED_COMMNAD_OFF);
		blk230_set_stop(1);

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
		/* IF NOT IN THE FOLLOW STATE, REPLY ERROR TO HCU/AWS */
		if( (FSM_STATE_BFSC_CONFIGURATION != FsmGetState(TASK_ID_BFSC)) && 
			  (FSM_STATE_BFSC_COMBINATION != FsmGetState(TASK_ID_BFSC)) && 
				(FSM_STATE_BFSC_SCAN != FsmGetState(TASK_ID_BFSC)) )
		{
				IhuErrorPrint("L3BFSC: msg_wmc_start_req_process: FSM_STATE_BFSC_CONFIGURATION/COMBINATION/SCAN != FsmGetState(TASK_ID_BFSC), return\r\n");
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
		/* IF ALREADY RECEIVED COMBIN_OUT */
		/* STOP ALL RUNNING PROCESS, START TO REPORT NEW WEIGHT EVENT */
		if( (FSM_STATE_BFSC_COMBINATION == FsmGetState(TASK_ID_BFSC)) )
		{
				IhuDebugPrint("L3BFSC: FSM_STATE_BFSC_COMBINATION == FsmGetState(TASK_ID_BFSC), STOP, THEN RESTART\n");
				/* Step 1: STOP SENSOR */
				weight_sensor_send_cmd(WIGHT_SENSOR_CMD_TYPE_STOP);
			
				/* Step 2: STOP MOTOR */
				blk230_set_stop(1);
			
				/* Step 3: Restore Light */
				//blk230_set_lamp(WMC_LAMP_OUT2_GREEN, WMC_LAMP_OFF);
				blk230_led_send_cmd(WMC_LAMP_OUT2_GREEN, LED_COMMNAD_OFF);
				//blk230_set_lamp(WMC_LAMP_OUT3_YELLOW, WMC_LAMP_OFF);
				blk230_led_send_cmd(WMC_LAMP_OUT3_YELLOW, LED_COMMNAD_OFF);
			
				/* Step 4: Make sure these two are exective in sequence */
				memset(&zIhuAdcBfscWs, 0, sizeof(strIhuBfscAdcWeightPar_t));
				InitWeightAdcBfscLocalParam(&zWeightSensorParam);
		}
		else if( (FSM_STATE_BFSC_CONFIGURATION == FsmGetState(TASK_ID_BFSC)) )
		{
				IhuDebugPrint("L3BFSC: FSM_STATE_BFSC_CONFIGURATION == FsmGetState(TASK_ID_BFSC), START NOW\n");
		}
		else
		{
				IhuDebugPrint("L3BFSC: FSM_STATE_BFSC_SCAN == FsmGetState(TASK_ID_BFSC), ALREADY START, DO NOTHING\n");
		}
			
		/*
		** Re-enter in FSM_STATE_BFSC_SCAN state
		**
		** FSM_STATE_BFSC_CONFIGURATION -> FSM_STATE_BFSC_SCAN
		** FSM_STATE_BFSC_COMBINATION -> FSM_STATE_BFSC_SCAN
		**
		*/
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
		if( FSM_STATE_BFSC_SCAN != FsmGetState(TASK_ID_BFSC) && FSM_STATE_BFSC_COMBINATION != FsmGetState(TASK_ID_BFSC))
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
		weight_sensor_send_cmd(WIGHT_SENSOR_CMD_TYPE_STOP);
		
		/* STOP MOTOR */
		blk230_set_stop(1);
		
		/* TURN OFF LIGHT */
		blk230_led_send_cmd(WMC_LAMP_OUT2_GREEN, LED_COMMNAD_OFF);
		blk230_led_send_cmd(WMC_LAMP_OUT3_YELLOW, LED_COMMNAD_OFF);

		
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
//		if( FSM_STATE_BFSC_CONFIGURATION != FsmGetState(TASK_ID_BFSC) )
//		{
//				IhuErrorPrint("L3BFSC: msg_wmc_command_req_process: FSM_STATE_BFSC_CONFIGURATION != FsmGetState(TASK_ID_BFSC), return\r\n");
//				*ec_ptr = ERROR_CODE_WRONG_WMC_STATE;
//				//return;
//		}
		
		/* Check Parameters */
		*ec_ptr = msg_wmc_command_req_check(param_ptr);
		if(ERROR_CODE_INPUT_PARAMETER_KO == *ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_command_req_process: parameters check nok, return\r\n");
				*ec_ptr = ERROR_CODE_CALLING_ERROR;
				return;
		}
		
		/* Process the message */
		switch(cmd_req->cmdid)
		{
				case CMDID_SENSOR_COMMAND_IGORE:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: CMDID_SENSOR_COMMAND_IGORE\n");	
						break;
				
				case CMDID_SENSOR_COMMAND_WEITGH_READ:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: CMDID_SENSOR_COMMAND_WEITGH_READ\n");	
						break;
								
				case CMDID_SENSOR_COMMAND_CALIBRATION_ZERO:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: CMDID_SENSOR_COMMAND_CALIBRATION_ZERO\n");	
						break;

				case CMDID_SENSOR_COMMAND_CALIBRATION_FULL:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: CMDID_SENSOR_COMMAND_CALIBRATION_FULL\n");	
						break;								
				
				case CMDID_MOTOR_COMMAND:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: CMDID_MOTOR_SPEED_READ\n");
					
						blk230_cmd_t motor_cmd = *(blk230_cmd_t *)(&cmd_req->cmdvalue);
						blk230_send_cmd(!motor_cmd.stop, motor_cmd.ccw, motor_cmd.speed, motor_cmd.time2stop);
						break;
				
				case CMDID_LED1_COMMNAD_ON:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: CMDID_LED1_COMMNAD_ON\n");
						blk230_led_send_cmd(WMC_LAMP_OUT2_GREEN, LED_COMMNAD_ON);
						break;
				
				case CMDID_LED1_COMMNAD_OFF:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: CMDID_LED1_COMMNAD_OFF\n");
						blk230_led_send_cmd(WMC_LAMP_OUT2_GREEN, LED_COMMNAD_OFF);
						break;
				
				case CMDID_LED1_COMMNAD_BINKING_HIGHSPEED:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: CMDID_LED1_COMMNAD_BINKING_HIGHSPEED\n");
						blk230_led_send_cmd(WMC_LAMP_OUT2_GREEN, LED_COMMNAD_BINKING_HIGHSPEED);
						break;
				
				case CMDID_LED1_COMMNAD_BINKING_LOWSPEED:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: CMDID_LED1_COMMNAD_BINKING_LOWSPEED\n");
						blk230_led_send_cmd(WMC_LAMP_OUT2_GREEN, LED_COMMNAD_BINKING_LOWSPEED);
				
				case CMDID_LED2_COMMNAD_ON:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: CMDID_LED2_COMMNAD_ON\n");
						blk230_led_send_cmd(WMC_LAMP_OUT3_YELLOW, LED_COMMNAD_ON);
						break;
				
				case CMDID_LED2_COMMNAD_OFF:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: CMDID_LED2_COMMNAD_OFF\n");
						blk230_led_send_cmd(WMC_LAMP_OUT3_YELLOW, LED_COMMNAD_OFF);
						break;
				
				case CMDID_LED2_COMMNAD_BINKING_HIGHSPEED:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: CMDID_LED2_COMMNAD_BINKING_HIGHSPEED\n");
						blk230_led_send_cmd(WMC_LAMP_OUT3_YELLOW, LED_COMMNAD_BINKING_HIGHSPEED);
						break;
				
				case CMDID_LED2_COMMNAD_BINKING_LOWSPEED:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: CMDID_LED2_COMMNAD_BINKING_LOWSPEED\n");
						blk230_led_send_cmd(WMC_LAMP_OUT3_YELLOW, LED_COMMNAD_BINKING_LOWSPEED);
						break;
				
				default:
						IhuDebugPrint("L3BFSC: msg_wmc_command_req_process: unknown command.\n");
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

void msg_wmc_command_resp(error_code_t ec, msg_struct_l3bfsc_wmc_command_req_t *p_msg)
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
		msg_wmc_command_resp.validFlag = TRUE;
		
		/* WHEN Calibration, this save the Full Weight Value, like 100000 */
		msg_wmc_command_resp.cmdvalue2 = p_msg->cmdvalue;
		msg_wmc_command_resp.cmdid = p_msg->cmdid;  /* write back to HCU */
		
		/* Build Message Content Body */
		msg_wmc_command_resp.cmdvalue1 = zMotorControlParam.MotorSpeed;
		
		/* Stop Reading ADC */
		if((FSM_STATE_BFSC_COMBINATION == FsmGetState(TASK_ID_BFSC) )||
			 (FSM_STATE_BFSC_SCAN == FsmGetState(TASK_ID_BFSC)) )
		{
				weight_sensor_send_cmd(WIGHT_SENSOR_CMD_TYPE_STOP);
		}
		
		/* Start to run weight sensor report */
		if( CMDID_SENSOR_COMMAND_WEITGH_READ == p_msg->cmdid)
		{
				msg_wmc_command_resp.cmdvalue1 = WeightSensorReadCurrent(&zWeightSensorParam);
				msg_wmc_command_resp.cmdvalue2 = p_msg->cmdvalue;
		}
		else if ( CMDID_SENSOR_COMMAND_CALIBRATION_ZERO == p_msg->cmdid )
		{
				
				//zWeightSensorParam.WeightSensorCalibrationZeroAdcValue = WeightSensorReadInstantAdc();
				msg_wmc_command_resp.cmdvalue1 = WeightSensorCalibrationZero(&zWeightSensorParam);
				msg_wmc_command_resp.cmdvalue2 = p_msg->cmdvalue;	
		}
		else if ( CMDID_SENSOR_COMMAND_CALIBRATION_FULL == p_msg->cmdid ) 
		{	
				//zWeightSensorParam.WeightSensorCalibrationFullAdcValue = WeightSensorReadInstantAdc();
				zWeightSensorParam.WeightSensorCalibrationFullWeight = p_msg->cmdvalue;
				msg_wmc_command_resp.cmdvalue1 = WeightSensorCalibrationFull(&zWeightSensorParam);
				msg_wmc_command_resp.cmdvalue2 = p_msg->cmdvalue;				
		}
		
		/* Restart Reading ADC */
		if((FSM_STATE_BFSC_COMBINATION == FsmGetState(TASK_ID_BFSC) )||
			 (FSM_STATE_BFSC_SCAN == FsmGetState(TASK_ID_BFSC)) )
		{
				weight_sensor_send_cmd(WIGHT_SENSOR_CMD_TYPE_START);
		}
		
		IhuDebugPrint("L3BFSC: msg_wmc_command_resp: msgid=0x%08X, cmdvalue1(adc)=%d, cmdvalue2(FullWeight)=%d, cmdid=%d[10(null),11(read weight),12(cal.zero),13(cal.full)]\r\n", \
										msg_wmc_command_resp.msgid, msg_wmc_command_resp.cmdvalue1, msg_wmc_command_resp.cmdvalue2, p_msg->cmdid);
		
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
		//blk230_set_lamp(WMC_LAMP_OUT2_GREEN, WMC_LAMP_ON);
		blk230_led_send_cmd(WMC_LAMP_OUT2_GREEN, LED_COMMNAD_ON);
		
		/* Save WeightCurrentTicks to WeightCombinOutReceivedTicks */
		zIhuAdcBfscWs.WeightCombinOutReceivedTicks = zIhuAdcBfscWs.WeightCurrentTicks;
		IhuDebugPrint("L3BFSC: msg_wmc_combin_req_process start: WeightCombinOutReceivedTicks(%d) <= WeightCurrentTicks(%d)\r\n", \
										zIhuAdcBfscWs.WeightCombinOutReceivedTicks, zIhuAdcBfscWs.WeightCurrentTicks);
		
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
	
		ret = ret;
	
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


void msg_wmc_err_inq_req_process(void *param_ptr, error_code_t *ec_ptr)
{
		IhuDebugPrint("L3BFSC: msg_wmc_err_inq_req_process start ...\r\n");
		//msg_struct_l3bfsc_wmc_err_inq_req_t *pCombinOut;
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_err_inq_req_process: NULL == param_ptr, return\r\n");
				if(NULL != ec_ptr) *ec_ptr = ERROR_CODE_CALLING_ERROR;
				return;
		}
		
		/* Check Input Parameter */
		if(NULL == ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_err_inq_req_process: NULL == ec_ptr, return\r\n");
				return;
		}
		
//		/* Check If it is the right/valid state to process the message */
//		if( FSM_STATE_BFSC_SCAN != FsmGetState(TASK_ID_BFSC) )
//		{
//				IhuErrorPrint("L3BFSC: msg_wmc_err_inq_req_process: FSM_STATE_BFSC_SCAN != FsmGetState(TASK_ID_BFSC), return\r\n");
//				*ec_ptr = ERROR_CODE_WRONG_WMC_STATE;
//				return;
//		}
		
		/* Check Parameters */
		*ec_ptr = msg_wmc_err_inq_req_check(param_ptr);
		if(ERROR_CODE_INPUT_PARAMETER_KO == *ec_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_err_inq_req_process: parameters check nok, return\r\n");
				return;
		}
		
		/* Process the message: start motor */
		//blk230_set_lamp(WMC_LAMP_OUT3_YELLOW, WMC_LAMP_ON);
		blk230_led_send_cmd(WMC_LAMP_OUT3_YELLOW, LED_COMMNAD_BINKING_HIGHSPEED);
		
		blk230_set_stop(1);
		IhuDebugPrint("L3BFSC: Set WMC_LAMP_OUT3_YELLOW to LED_COMMNAD_BINKING_HIGHSPEED, STOP MOTOR", zBfscWmcState.last_combin_type.WeightCombineType);	
		/* Other Action Like Reset to be added */
		
		//pCombinOut = (msg_struct_l3bfsc_wmc_err_inq_req_t *)param_ptr;
		zBfscWmcState.state = BFSC_MWC_STATE_ALREADY_COMBIN_OUT;
    zBfscWmcState.last_combin_type.WeightCombineType = HUITP_IEID_SUI_BFSC_COMINETYPE_NULL;
		IhuDebugPrint("L3BFSC: Set to FSM_STATE_BFSC_CONFIGURATION, WeightCombineType=%d", zBfscWmcState.last_combin_type.WeightCombineType);	
		
		if (FsmSetState(TASK_ID_BFSC, FSM_STATE_BFSC_CONFIGURATION) == IHU_FAILURE){
				IhuErrorPrint("L3BFSC: Error Set FSM State FSM_STATE_BFSC_COMBINATION");	
				return;
		}
		
		IhuDebugPrint("L3BFSC: Set to FSM_STATE_BFSC_CONFIGURATION, WeightCombineType=%d", zBfscWmcState.last_combin_type);	
		return;
}

error_code_t msg_wmc_err_inq_req_check(void *param_ptr)
{

		IhuDebugPrint("L3BFSC: msg_wmc_err_inq_req_check start ...\r\n");
	
		/* Check Input Parameter */
		if(NULL == param_ptr)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_err_inq_req_check: NULL == param_ptr, return\r\n");
				return ERROR_CODE_CALLING_ERROR;
		}

		return ERROR_CODE_NO_ERROR;
}

void msg_wmc_err_inq_resp(error_code_t ec)
{
		OPSTAT ret = IHU_SUCCESS;
		msg_struct_l3bfsc_wmc_err_inq_resp_t msg_wmc_err_inq_resp;
	
		IhuDebugPrint("L3BFSC: msg_wmc_err_inq_resp start ...\r\n");
	
		ret = ret;
	
		/* Check Input Parameter */
		if(ec > ERROR_CODE_MAX)
		{
				IhuErrorPrint("L3BFSC: msg_wmc_err_inq_resp: (ec > ERROR_CODE_MAX), return\r\n");
				return;
		}
		
		/* Build Message Content Header */
		msg_wmc_err_inq_resp.msgid = (MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_RESP);
		msg_wmc_err_inq_resp.wmc_id.wmc_id = zWmcInvenory.wmc_id.wmc_id;
		msg_wmc_err_inq_resp.average_weight = zIhuAdcBfscWs.WeightCurrValue; /// READ DIRECTLY FROM LOCAL ///
		msg_wmc_err_inq_resp.error_code = ec;
    msg_wmc_err_inq_resp.validFlag = TRUE;
    msg_wmc_err_inq_resp.spare1 = 0;
		msg_wmc_err_inq_resp.spare2 = 1;
		
		
		IhuDebugPrint("L3BFSC: msg_wmc_err_inq_resp: msgid = 0x%08X, will be wait for wait goes to 0.\r\n", \
										msg_wmc_err_inq_resp.msgid);
		
//		/* Send Message to CAN Task */
//		ret = ihu_message_send(MSG_ID_L3BFSC_WMC_COMBIN_RESP, TASK_ID_CANVELA, TASK_ID_BFSC, \
//														&msg_wmc_combin_resp, MSG_SIZE_L3BFSC_WMC_COMBIN_RESP);
//		if (ret == IHU_FAILURE){
//			IhuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_BFSC], zIhuVmCtrTab.task[TASK_ID_CANVELA]);
//			return;
//		}

		return;
}
