/**
****************************************************************************************
*
* @file user_sps_device.c
*
* @brief Empty peripheral template project source code.
*
* Copyright (C) 2012. Dialog Semiconductor Ltd, unpublished work. This computer 
 * program includes Confidential, Proprietary Information and is a Trade Secret of 
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited 
 * unless authorized in writing. All Rights Reserved.
*
* <bluetooth.support@diasemi.com> and contributors.
*
****************************************************************************************
*/

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "arch_console.h"
#include "gattc.h"
#include "user_sps_device.h"
#include "user_sps_scheduler.h"

enum process_event_response user_spss_process_handler(ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id, 
                                         enum ke_msg_status_tag *msg_ret);


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
/**
 ****************************************************************************************
 * @brief Sets BLE Rx irq threshold. It should be called at app_init and in external wakeup irq callabck if deep sleep is used.
 * @param[in] Rx buffer irq threshold
 * @return void 
 ****************************************************************************************
 */
//static void app_set_rxirq_threshold(uint32_t rx_threshold)
//{
//    jump_table_struct[lld_rx_irq_thres] = (uint32_t) rx_threshold;
//}

/**
 ****************************************************************************************
 * @brief Sends a exchange MTU command
 * @param[in]   conidx Connection index
 * @return void
 ****************************************************************************************
 */
static void user_gattc_exc_mtu_cmd(uint8_t conidx)
{
  struct gattc_exc_mtu_cmd *cmd =  KE_MSG_ALLOC(GATTC_EXC_MTU_CMD,
            KE_BUILD_ID(TASK_GATTC, conidx), TASK_APP,
            gattc_exc_mtu_cmd);
    
    cmd->req_type = GATTC_MTU_EXCH;
    
    ke_msg_send(cmd);
}

/*
 * APP TASK HANDLERS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Handles SPS profiles events
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return void
 ****************************************************************************************
 */
void user_process_catch(ke_msg_id_t const msgid, void const *param,
                                         ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    int i=0;
    enum ke_msg_status_tag msg_ret;
    const process_event_func_t user_process_handlers[] = {
        (process_event_func_t) user_spss_process_handler,
    };
    
    while (i<sizeof(user_process_handlers)/sizeof(process_event_func_t))
    {
        ASSERT_ERR(app_process_handlers[i]);
         if (user_process_handlers[i](msgid,param,dest_id,src_id, &msg_ret)==PR_EVENT_HANDLED)
               return;
         i++;
    }
}

/*
 * APP CALLBACKS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Handles connection event
 * @param[in]   connection_idx Connection index
 * @param[in]   param Parameters of connection
 * @return void
 ****************************************************************************************
 */
void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
    user_gattc_exc_mtu_cmd(connection_idx);
    app_easy_gap_param_update_start(connection_idx);
    arch_printf("Device connected\r\n");
}

/**
 ****************************************************************************************
 * @brief Handles disconnection event
 * @param[in]   param Parameters of disconnect message
 * @return void
 ****************************************************************************************
 */
void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
    arch_printf("Device disconnected\r\n");
}

/**
 ****************************************************************************************
 * @brief Handles connection failure
 * @return void
 ****************************************************************************************
 */
void user_on_connect_failed (void)
{
    ASSERT_WARNING(0);
}

/*
 * MAIN LOOP CALLBACKS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Called on device initialization
 * @return void
 ****************************************************************************************
 */
void user_on_init(void)
{
    // SPS application initialisation
    user_scheduler_init();

    default_app_on_init();
}

/**
 ****************************************************************************************
 * @brief Called on system power up
 * @return true to force calling of schedule(), else false
 ****************************************************************************************
 */
enum arch_main_loop_callback_ret user_on_system_powered(void)
{
    user_sps_sleep_restore();
    user_ble_pull(true, NULL);

    return GOTO_SLEEP;
}

/**
 ****************************************************************************************
 * @brief Called before entering sleep
 * @return void
 ****************************************************************************************
 */
void user_before_sleep(void)
{
    user_sps_sleep_check();
}

/// @} APP
