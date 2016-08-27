/**
****************************************************************************************
*
* @file user_proxm_proj.h
*
* @brief Proximity monitor user header file.
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

#ifndef USER_PROXM_PROJ_H_
#define USER_PROXM_PROJ_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Accelerometer Application entry point.
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwble_config.h"
#include "gapc_task.h"                  // gap functions and messages
#include "gapm_task.h"                  // gap functions and messages
#include "co_error.h"                   // error code definitions
#include "smpc_task.h"                  // error code definitions
#include "arch_api.h"                  // error code definitions
                  
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

/**
 ****************************************************************************************
 * @brief User code initiliazation function.
 *
 * @void 
 *
 * @return void.
 ****************************************************************************************
*/

void user_on_init(void);

/// @} APP

#endif //APP_PROXR_PROJ_H_
