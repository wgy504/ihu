/**
 ****************************************************************************************
 *
 * @file wechat_task.c
 *
 * @brief Wechat Task implementation.
 *
 * @brief 128 UUID service. sample code
 *
 * Copyright (C) 2013 Dialog Semiconductor GmbH and its Affiliates, unpublished work
 * This computer program includes Confidential, Proprietary Information and is a Trade Secret 
 * of Dialog Semiconductor GmbH and its Affiliates. All use, disclosure, and/or 
 * reproduction is prohibited unless authorized in writing. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
*/

#include "rwble_config.h"

#if (BLE_WECHAT)

#include "gap.h"
#include "gapc.h"
#include "gattc_task.h"
#include "atts_util.h"
#include "wechat.h"
#include "wechat_task.h"
#include "attm_cfg.h"
#include "attm_db.h"
#include "prf_utils.h"
#include "attm_util.h"


/**** NOTIFICATION BITS ***********/
uint8_t FirstNotificationBit __attribute__((section("retention_mem_area0"), zero_init));
uint8_t SecondNotificationBit __attribute__((section("retention_mem_area0"), zero_init));

extern void mpbledemo2_reset(void);
extern void mpbledemo2_indication_state(bool isEnable);

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref WECHAT_CREATE_DB_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int wechat_create_db_req_handler(ke_msg_id_t const msgid,
                                       struct wechat_create_db_req const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    //Database Creation Status
    uint8_t status;

    //Save Profile ID
    wechat_env.con_info.prf_id = TASK_WECHAT;

    /*---------------------------------------------------*
     * Wechat Service Creation
     *---------------------------------------------------*/

    //Add Service Into Database
    status = attm_svc_create_db(&wechat_env.wechat_shdl, NULL, WECHAT_IDX_NB, NULL,
                               dest_id, &wechat_att_db[0]);
    //Disable LLS
    attmdb_svc_set_permission(wechat_env.wechat_shdl, PERM(SVC, DISABLE));
    
    //If we are here, database has been fulfilled with success, go to idle state
    ke_state_set(TASK_WECHAT, WECHAT_IDLE);

    //Send CFM to application
    struct wechat_create_db_cfm * cfm = KE_MSG_ALLOC(WECHAT_CREATE_DB_CFM, src_id,
                                                TASK_WECHAT, wechat_create_db_cfm);
    cfm->status = status;
    ke_msg_send(cfm);
    
		//初始化定时器以及标志位
		vmda1458x_timer_set(WECHAT_PERIOD_BAT_STATUS_TIME_OUT, TASK_WECHAT, WECHAT_TIME_OUT_DURATION_BAT_STATUS);
		
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Enable the Wechat role, used after connection.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int wechat_enable_req_handler(ke_msg_id_t const msgid,
                                    struct wechat_enable_req const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    
    uint16_t temp = 1;
    
    // Keep source of message, to respond to it further on
    wechat_env.con_info.appid = src_id;
    // Store the connection handle for which this profile is enabled
    wechat_env.con_info.conidx = gapc_get_conidx(param->conhdl);

    // Check if the provided connection exist
    if (wechat_env.con_info.conidx == GAP_INVALID_CONIDX)
    {
        // The connection doesn't exist, request disallowed
        prf_server_error_ind_send((prf_env_struct *)&wechat_env, PRF_ERR_REQ_DISALLOWED,
                 WECHAT_ERROR_IND, WECHAT_ENABLE_REQ);
    }
    else
    {
        // Wechat service permissions
        attmdb_svc_set_permission(wechat_env.wechat_shdl, param->sec_lvl);

        wechat_env.feature = param->feature; 
        
        if (!wechat_env.feature)
        {
               temp = 0;
        }
        
        attmdb_att_set_value(wechat_env.wechat_shdl + WECHAT_IDX_IND_CFG,
                             sizeof(uint16_t), (uint8_t *)&temp);
        
        // Go to Connected state
        ke_state_set(TASK_WECHAT, WECHAT_CONNECTED);
    }

    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Send indication to peer device
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int wechat_send_indication_req_handler(ke_msg_id_t const msgid,
                                    struct wechat_indication_req const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    uint8_t status = PRF_ERR_OK;

    // Check provided values
    if(param->conhdl == gapc_get_conhdl(wechat_env.con_info.conidx))
    {
        if((wechat_env.feature & PRF_CLI_START_IND))
        {
            attmdb_att_set_value(wechat_env.wechat_shdl + WECHAT_IDX_IND_VAL, 
                                        param->length,
                                        (uint8_t *)(param->value));
                    
            // Send notification through GATT
            prf_server_send_event((prf_env_struct *)&wechat_env, true,
                    wechat_env.wechat_shdl + WECHAT_IDX_IND_VAL);
        }        
    }
    else
    {
        status = PRF_ERR_INVALID_PARAM;
    }

    if (status != PRF_ERR_OK)
    {
        wechat_indication_cfm_send(status);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_CMP_EVT message.
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,
                                 struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{
    switch (param->req_type)
    {
        case GATTC_INDICATE:
        {
				
            if (FirstNotificationBit == 0)
            SecondNotificationBit = 0;
            FirstNotificationBit = 0;
            wechat_indication_cfm_send(param->status);
            break;
        }
        default:
            break;
    }

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATT_WRITE_CMD_IND message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_write_cmd_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_write_cmd_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    uint8_t status = PRF_APP_ERROR;

    if (KE_IDX_GET(src_id) == wechat_env.con_info.conidx)
    {
        switch (param->handle - wechat_env.wechat_shdl)
        {
            case WECHAT_IDX_WRITE_VAL:
            {
                //Save value in DB
                if(param->last)
                {
                    attmdb_att_set_value(param->handle, param->length, (uint8_t *)&param->value[0]);
                    wechat_send_write_val((uint8_t *)(param->value), param->length);
                }
                
                status = PRF_ERR_OK; 
                break;
            }
            case WECHAT_IDX_IND_CFG:
            {
                // Written value
                uint16_t ntf_cfg;
                uint16_t isEnable = true;
        
                // Extract value before check
                ntf_cfg = co_read16p(&param->value[0]);
                
                // Only update configuration if value for stop or notification enable
                if ((ntf_cfg == PRF_CLI_STOP_NTFIND) || (ntf_cfg == PRF_CLI_START_IND))
                {
                    //Save value in DB
                    attmdb_att_set_value(param->handle, sizeof(uint16_t), (uint8_t *)&param->value[0]);
                    
                    // Conserve information in environment
                    if (ntf_cfg == PRF_CLI_START_IND)
                    {
                        // Ntf cfg bit set to 1
                        wechat_env.feature |= PRF_CLI_START_IND;
                    }
                    else
                    {
                        isEnable = false;
                        // Ntf cfg bit set to 0
                        wechat_env.feature &= ~PRF_CLI_START_IND;
                    }                
                    
                    wechat_enable_indication(isEnable);
                    
                    status = PRF_ERR_OK; 
                    
                }                
                break;
            }
            default:
                break;
        }
    }
  
    if (true == param->response)
    {
        // Send Write Response
        atts_write_rsp_send(wechat_env.con_info.conidx, param->handle, status);
    }
    
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Disconnection indication to wechat.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gap_disconnnect_ind_handler(ke_msg_id_t const msgid,
                                        struct gapc_disconnect_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{

    // Check Connection Handle
    if (KE_IDX_GET(src_id) == wechat_env.con_info.conidx)
    {
        
        // In any case, inform APP about disconnection
        wechat_disable();
    }

    return (KE_MSG_CONSUMED);
}

int app_wechat_period_ble_status_time_out_handler(ke_msg_id_t const msgid,
                                  void const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)																		
{
	//arch_printf("\r\n Wechat Task Time Out received No 1. ");
  mpbledemo2_airsync_link_setup_period_report();
  return (KE_MSG_CONSUMED);
}

int app_wechat_period_bat_status_time_out_handler(ke_msg_id_t const msgid,
                                  void const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)																		
{
	//arch_printf("\r\n Wechat Task Time Out received No 1. ");
	//Restart timer
  vmda1458x_timer_set(WECHAT_PERIOD_BAT_STATUS_TIME_OUT, TASK_WECHAT, WECHAT_TIME_OUT_DURATION_BAT_STATUS);
	
	//Set BAT working status, to check if BAT_VIN_OK status is TRUE or not
	if (ihu_get_vin_ok_status() == false) vmda1458x_led_set(LED_ID_6, LED_MODE_BLINK_HIGH_SPEED);
	
  return (KE_MSG_CONSUMED);
}


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Disabled State handler definition.
const struct ke_msg_handler wechat_disabled[] =
{
    {WECHAT_CREATE_DB_REQ,   (ke_msg_func_t) wechat_create_db_req_handler },
};

/// Idle State handler definition.
const struct ke_msg_handler wechat_idle[] =
{
    {WECHAT_ENABLE_REQ,              (ke_msg_func_t) wechat_enable_req_handler },
};

/// Connected State handler definition.
const struct ke_msg_handler wechat_connected[] =
{
    {GATTC_WRITE_CMD_IND,               (ke_msg_func_t) gattc_write_cmd_ind_handler},
    {WECHAT_SEND_INDICATION_REQ,        (ke_msg_func_t) wechat_send_indication_req_handler},
    {GATTC_CMP_EVT,                     (ke_msg_func_t) gattc_cmp_evt_handler},
		{WECHAT_PERIOD_BLE_STATUS_TIME_OUT, (ke_msg_func_t) app_wechat_period_ble_status_time_out_handler},
};

/// Default State handlers definition
const struct ke_msg_handler wechat_default_state[] =
{
    {GAPC_DISCONNECT_IND,    (ke_msg_func_t) gap_disconnnect_ind_handler},
		{WECHAT_PERIOD_BAT_STATUS_TIME_OUT, (ke_msg_func_t)app_wechat_period_bat_status_time_out_handler},				
};

/// Specifies the message handler structure for every input state.
const struct ke_state_handler wechat_state_handler[WECHAT_STATE_MAX] =
{
    [WECHAT_DISABLED]    = KE_STATE_HANDLER(wechat_disabled),
    [WECHAT_IDLE]        = KE_STATE_HANDLER(wechat_idle),
    [WECHAT_CONNECTED]   = KE_STATE_HANDLER(wechat_connected),
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler wechat_default_handler = KE_STATE_HANDLER(wechat_default_state);

/// Defines the place holder for the states of all the task instances.
ke_state_t wechat_state[WECHAT_IDX_MAX] __attribute__((section("retention_mem_area0"),zero_init));

#endif //BLE_WECHAT

