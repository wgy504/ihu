/**
****************************************************************************************
*
* @file app_proxr_task.h
*
* @brief Proximity Reporter task header. Proximity Reporter Handlers declaration.
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

#include "rwip_config.h"

#ifndef APP_PROXR_TASK_H_
#define APP_PROXR_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief 
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */


#if BLE_PROX_REPORTER

#include "proxr_task.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

enum process_event_response app_proxr_process_handler(ke_msg_id_t const msgid,
                                                      void const *param,
                                                      ke_task_id_t const dest_id,
                                                      ke_task_id_t const src_id,
                                                      enum ke_msg_status_tag *msg_ret);


#endif //BLE_PROX_REPORTER

/// @} APP

#endif // APP_PROXR_TASK_H_
