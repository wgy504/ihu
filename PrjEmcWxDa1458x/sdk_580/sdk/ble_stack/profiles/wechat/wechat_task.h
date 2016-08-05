/**
 ****************************************************************************************
 *
 * @file wechat_task.h
 *
 * @brief Header file - wechat_task.
 * @brief 128 UUID service. sample code
 *
 * Copyright (C) 2013 Dialog Semiconductor GmbH and its Affiliates, unpublished work
 * This computer program includes Confidential, Proprietary Information and is a Trade Secret 
 * of Dialog Semiconductor GmbH and its Affiliates. All use, disclosure, and/or 
 * reproduction is prohibited unless authorized in writing. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef WECHAT_TASK_H_
#define WECHAT_TASK_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
*/

#if (BLE_WECHAT)

#include <stdint.h>
#include "ke_task.h"
#include "wechat.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of WeChat task instances
#define WECHAT_IDX_MAX                 (1)

/**** NOTIFICATION BITS ****/
extern uint8_t FirstNotificationBit;
extern uint8_t SecondNotificationBit;


/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Possible states of the WECHAT task
enum
{
    /// Disabled State
    WECHAT_DISABLED,
    /// Idle state
    WECHAT_IDLE,
    /// Connected state
    WECHAT_CONNECTED,

    /// Number of defined states.
    WECHAT_STATE_MAX
};

/// Messages for Sample128
enum
{
    /// Start wechat. Device connection
    WECHAT_ENABLE_REQ = KE_FIRST_MSG(TASK_WECHAT),

    /// Disable confirm.
    WECHAT_DISABLE_IND,

    /// Att Value change indication
    WECHAT_WRITE_VAL_IND,
    
    /// enable indication
    WECHAT_ENABLE_IND_REQ,

    ///Create DataBase
    WECHAT_CREATE_DB_REQ,
    ///Inform APP of database creation status
    WECHAT_CREATE_DB_CFM,

    ///Send indication to peer master
    WECHAT_SEND_INDICATION_REQ,
    ///Inform the handling result of indication result
    WECHAT_SEND_INDICATION_CFM,
    
    WECHAT_SEND_DATA_TO_MASTER,
    
    /// Error Indication
    WECHAT_ERROR_IND,
		
		WECHAT_PERIOD_REPORT_TIME_OUT,
};


/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the @ref WECHAT_ENABLE_REQ message
struct wechat_enable_req
{
    /// Connection Handle
    uint16_t conhdl;
    /// Security level
    uint8_t sec_lvl;
    
    /// WECHAT_IDX_IND_VAL property status
    uint8_t feature;
};

/// Parameters of the @ref WECHAT_DISABLE_IND message
struct wechat_disable_ind
{
    /// Connection Handle
    uint16_t conhdl;
};

/// Parameters of the @ref WECHAT_WRITE_VAL_IND message
struct wechat_write_val_ind
{
    /// Connection handle
    uint16_t conhdl;
    /// data length
    uint16_t length;
    /// Value
    uint8_t value[BLE_WECHAT_MAX_DATA_LEN];
    
};

/// Parameters of the @ref WECHAT_ENABLE_IND_REQ message
struct wechat_enable_indication_req
{
    /// Connection handle
    uint16_t conhdl;
    /// true if enable, false if disable
    uint16_t isEnable;
    
};

/// Parameters of the @ref WECHAT_SEND_INDICATION_REQ message
struct wechat_indication_req
{
    /// Connection handle
    uint16_t conhdl;
    /// Indicated data length
    uint16_t length;
    /// Characteristic Value
    uint8_t value[BLE_WECHAT_MAX_DATA_LEN];
    
};

/// Parameters of the @ref WECHAT_SEND_INDICATION_CFM message
struct wechat_indication_cfm
{
    /// Status
    uint8_t status;
};

/// Parameters of the @ref WECHAT_CREATE_DB_REQ message
struct wechat_create_db_req
{
    /// Indicate if TXPS is supported or not
    uint8_t features;
};

/// Parameters of the @ref WECHAT_CREATE_DB_CFM message
struct wechat_create_db_cfm
{
    /// Status
    uint8_t status;
};

struct wechat_send_data_req
{
    uint32_t dataLen;
    uint8_t* pDataBuf;
};

struct wechat_period_report_time_out
{
    uint8_t status;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler wechat_state_handler[WECHAT_STATE_MAX];
extern const struct ke_state_handler wechat_default_handler;
extern ke_state_t wechat_state[WECHAT_IDX_MAX];

#endif //BLE_WECHAT

#endif // WECHAT_TASK_H_
