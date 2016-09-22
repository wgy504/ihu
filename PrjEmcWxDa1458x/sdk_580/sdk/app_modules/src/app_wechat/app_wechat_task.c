/**
 ****************************************************************************************
 *
 * @file app_wechat_task.c
 *
 * @brief Wechat Service Application Task
 *
 * Copyright (C) RivieraWaves 2009-2013
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

#include "rwip_config.h"        // SW Configuration
#include <string.h>             // srtlen()

#if (BLE_WECHAT)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "wechat_task.h"            // Wechat Service Task API
#include "wechat.h"                   // Wechat Service Definitions
#include "app_wechat.h"               // Wechat Service Application Definitions
#include "app_wechat_task.h"          // Wechat Service Application Task API
#include "app_task.h"               // Application Task API

#include "user_callback_config.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
#include "app_entry_point.h"

extern void mpbledemo2_reset(void);
extern void mpbledemo2_indication_state(bool isEnable);

static const struct ke_msg_handler app_wechat_process_handlers[]=
{
    {WECHAT_CREATE_DB_CFM,               (ke_msg_func_t)app_wechat_create_db_cfm_handler},
    {WECHAT_DISABLE_IND,                 (ke_msg_func_t)app_wechat_disable_ind_handler},
    {WECHAT_SEND_INDICATION_CFM,         (ke_msg_func_t)app_wechat_send_indication_cfm_handler},
    {WECHAT_WRITE_VAL_IND,               (ke_msg_func_t)app_wechat_write_val_ind_handler},
    {WECHAT_ENABLE_IND_REQ,              (ke_msg_func_t)app_wechat_enable_ind_handler},
    {WECHAT_SEND_DATA_TO_MASTER,         (ke_msg_func_t)app_wechat_send_data_handler},
    //{WECHAT_PERIOD_REPORT_TIME_OUT,      (ke_msg_func_t)app_wechat_task_period_report_time_out_handler},		
};

enum process_event_response app_wechat_process_handler (ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id, 
                                         enum ke_msg_status_tag *msg_ret)
{
    return (app_std_process_event(msgid, param,src_id,dest_id,msg_ret, app_wechat_process_handlers,
                                         sizeof(app_wechat_process_handlers)/sizeof(struct ke_msg_handler)));
} 

/**
 ****************************************************************************************
 * @brief Handles wechat profile database creation confirmation.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance .
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int app_wechat_create_db_cfm_handler(ke_msg_id_t const msgid,
                                        struct wechat_create_db_cfm const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    // If state is not idle, ignore the message
    if (ke_state_get(dest_id) == APP_DB_INIT)
    {
        // Inform the Application Manager
        struct app_module_init_cmp_evt *cfm = KE_MSG_ALLOC(APP_MODULE_INIT_CMP_EVT,
                                                           TASK_APP, TASK_APP,
                                                           app_module_init_cmp_evt);

        cfm->status = param->status;

        ke_msg_send(cfm);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles disable indication from wechat profile.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int app_wechat_disable_ind_handler(ke_msg_id_t const msgid,
                                      struct wechat_disable_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    mpbledemo2_reset();
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles write characteristic event indication from wechat profile
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int app_wechat_write_val_ind_handler(ke_msg_id_t const msgid,
                                  struct wechat_write_val_ind const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    ble_wechat_process_received_data((uint8_t *)(param->value), param->length);    
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles enable indication request from wechat profile
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int app_wechat_enable_ind_handler(ke_msg_id_t const msgid,
                                  struct wechat_enable_indication_req const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    mpbledemo2_indication_state(param->isEnable);    
    app_wechat_datahandler()->m_data_main_process_func();
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles sending indication completion conformation from wechat profile
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int app_wechat_send_indication_cfm_handler(ke_msg_id_t const msgid,
                                  struct wechat_indication_cfm const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    ble_wechat_indicate_data_chunk();
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles sending data to bonded wechat server
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int app_wechat_send_data_handler(ke_msg_id_t const msgid,
                                  struct wechat_send_data_req const *req,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
	#ifdef CATCH_LOG
		arch_printf("\r\n button 1");
	#endif
  mpbledemo2_sendData(req->pDataBuf, req->dataLen);
  return (KE_MSG_CONSUMED);
}

//int app_wechat_task_period_report_time_out_handler(ke_msg_id_t const msgid,
//                                  void const *param,
//                                  ke_task_id_t const dest_id,
//                                  ke_task_id_t const src_id)																		
//{
//	arch_printf("\r\n App Wechat Task Time Out received No 2. ");
//  mpbledemo2_airsync_link_setup_period_report();
//  return (KE_MSG_CONSUMED);
//}

#endif //(BLE_DIS_SERVER)

/// @} APP
