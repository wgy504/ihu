/**
****************************************************************************************
*
* @file app_entry_point.c
*
* @brief Proximity project source code .
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

#include "rwip_config.h"             // SW configuration

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#include "app_api.h"
#include "user_callback_config.h"
#include "user_modules_config.h"


enum process_event_response app_gap_process_handler (ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id, 
                                         enum ke_msg_status_tag *msg_ret);
                                         
enum process_event_response app_sec_process_handler (ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id, 
                                         enum ke_msg_status_tag *msg_ret);
enum process_event_response app_dis_process_handler (ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id, 
                                         enum ke_msg_status_tag *msg_ret);
enum process_event_response app_proxr_process_handler (ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id, 
                                         enum ke_msg_status_tag *msg_ret);
enum process_event_response app_bass_process_handler(ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id, 
                                         enum ke_msg_status_tag *msg_ret);
enum process_event_response app_bass_findme_handler(ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id, 
                                         enum ke_msg_status_tag *msg_ret);
enum process_event_response app_findme_process_handler(ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id, 
                                         enum ke_msg_status_tag *msg_ret);                                         
enum process_event_response app_spota_process_handler(ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id, 
                                         enum ke_msg_status_tag *msg_ret);

enum process_event_response app_timer_api_process_handler (ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id, 
                                         enum ke_msg_status_tag *msg_ret);
enum process_event_response app_msg_utils_api_process_handler (ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id, 
                                         enum ke_msg_status_tag *msg_ret);
                                         
// CUSTOM1 SERVER process handler
enum process_event_response app_custs1_process_handler(ke_msg_id_t const msgid,
                                                       void const *param,
                                                       ke_task_id_t const dest_id,
                                                       ke_task_id_t const src_id, 
                                                       enum ke_msg_status_tag *msg_ret);
                                         
const process_event_func_t app_process_handlers[] = {

#if (!EXCLUDE_DLG_GAP)
     (process_event_func_t) app_gap_process_handler,
#endif

#if (!EXCLUDE_DLG_TIMER)
    (process_event_func_t) app_timer_api_process_handler,
#endif

#if (!EXCLUDE_DLG_MSG)
    (process_event_func_t) app_msg_utils_api_process_handler,
#endif

#if ((BLE_APP_SEC) && (!EXCLUDE_DLG_SEC))
    (process_event_func_t) app_sec_process_handler,
#endif

#if ((BLE_DIS_SERVER) && (!EXCLUDE_DLG_DISS))
    (process_event_func_t) app_dis_process_handler,
#endif

#if ((BLE_PROX_REPORTER) && (!EXCLUDE_DLG_PROXR))
    (process_event_func_t) app_proxr_process_handler,
#endif
#if ((BLE_BAS_SERVER) && (!EXCLUDE_DLG_BASS))
    (process_event_func_t) app_bass_process_handler,
#endif

#if (((BLE_FINDME_TARGET)&& (!EXCLUDE_DLG_FINDT)) || ((BLE_FINDME_LOCATOR)&& (!EXCLUDE_FINDL)))  
    (process_event_func_t) app_findme_process_handler,
#endif //BLE_FINDME_LOCATOR

#if ((BLE_SPOTA_RECEIVER) && (!EXCLUDE_DLG_SPOTAR))
    (process_event_func_t) app_spota_process_handler,
#endif

#if ((BLE_CUSTOM1_SERVER) && (!EXCLUDE_DLG_CUSTS1))
    (process_event_func_t) app_custs1_process_handler,
#endif

};

int app_entry_point_handler (ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
    int i=0;
    enum ke_msg_status_tag process_msg_handling_result;
    
    while (i<sizeof(app_process_handlers)/sizeof(process_event_func_t))
    {
        ASSERT_ERR(app_process_handlers[i]);
         if (app_process_handlers[i](msgid,param,dest_id,src_id, &process_msg_handling_result)==PR_EVENT_HANDLED)
               return (process_msg_handling_result);
         i++;
    }

    //user cannot do anything else than consume the message
    if (app_process_catch_rest_cb!=NULL)
    {
        app_process_catch_rest_cb(msgid,param,dest_id,src_id);
    }
    
    return (KE_MSG_CONSUMED);
    
};


static ke_msg_func_t handler_search(  ke_msg_id_t const msg_id, 
                                         const struct ke_msg_handler *handlers,
                                         const int handler_num)
{
    //table is empty
    if (handler_num==0) 
        return NULL;
    
    // Get the message handler function by parsing the message table
    for (int i = (handler_num-1); 0 <= i; i--)
    {
        if ((handlers[i].id == msg_id)
                || (handlers[i].id == KE_MSG_DEFAULT_HANDLER))
        {
            // If handler is NULL, message should not have been received in this state
            ASSERT_ERR(handlers[i].func);

            return handlers[i].func;
        }
    }

    // If we execute this line of code, it means that we did not find the handler
    return NULL;
}



enum process_event_response app_std_process_event (             
                                         ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const src_id,
                                         ke_task_id_t const dest_id,
                                         enum ke_msg_status_tag *msg_ret,
                                         const struct ke_msg_handler *handlers,
                                         const int handler_num)
{
    ke_msg_func_t func = NULL;
    func = handler_search(msgid, handlers, handler_num );
    
    if (func != NULL)
    {
        (*msg_ret) = (enum ke_msg_status_tag) func(msgid, param, dest_id, src_id);
        return (PR_EVENT_HANDLED);
    }
    else
        return (PR_EVENT_UNHANDLED);
        
};

