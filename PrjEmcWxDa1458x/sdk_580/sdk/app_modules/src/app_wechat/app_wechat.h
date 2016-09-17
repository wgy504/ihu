/**
 ****************************************************************************************
 *
 * @file app_wechat.h
 *
* @brief Wechat application.
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

#ifndef APP_WECHAT_H_
#define APP_WECHAT_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 *
 * @brief Wechat Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW Configuration

#if (BLE_WECHAT)

#include <stdint.h>          // Standard Integer Definition
#include <co_bt.h>
#include "ble_580_sw_version.h"
#include "user_config_sw_ver.h"
#include "ble_wechat_util.h"


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
 *
 * Wechat Application Functions
 *
 ****************************************************************************************
 */
void app_wechat_init(void);

/**
 ****************************************************************************************
 * @brief Enable the wechat profile
 *
 ****************************************************************************************
 */
void app_wechat_enable(uint16_t conhdl);

/**
 ****************************************************************************************
 * @brief Create Wechat Database
 *
 ****************************************************************************************
 */
void app_wechat_create_db(void);

void app_wechat_SetDatahandler(data_handler* pHandler);
int ble_wechat_indicate_data(uint8_t *data, int len);
int ble_wechat_indicate_data_chunk(void);
void ble_wechat_process_received_data(uint8_t* pData, uint32_t length);
data_handler* app_wechat_datahandler(void);
bool ble_wechat_is_last_data_sent_out(void);

#endif //BLE_WECHAT

/// @} APP

#endif //APP_DIS_H_
