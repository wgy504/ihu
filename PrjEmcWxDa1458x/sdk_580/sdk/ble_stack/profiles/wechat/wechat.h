/**
 ****************************************************************************************
 *
 * @file wechat.h
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

#ifndef WECHAT_H_
#define WECHAT_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_WECHAT)

#include "ke_task.h"
#include "atts.h"
#include "prf_types.h"

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

// should following wechat airsync protocol
#define	COMPANY_IDENTIFIER						    0x0056 
 
#define BLE_UUID_WECHAT_SERVICE						0xFEE7
#define BLE_UUID_WECHAT_WRITE_CHARACTERISTICS 		0xFEC7
#define BLE_UUID_WECHAT_INDICATE_CHARACTERISTICS 	0xFEC8
#define BLE_UUID_WECHAT_READ_CHARACTERISTICS 		0xFEC9

#define BLE_WECHAT_MAX_DATA_LEN            (ATT_DEFAULT_MTU - 3) 	

/// Handles offsets
enum
{
    WECHAT_IDX_SVC,

    WECHAT_IDX_WRITE_CHAR,
    WECHAT_IDX_WRITE_VAL,

    WECHAT_IDX_IND_CHAR,
    WECHAT_IDX_IND_VAL,
    WECHAT_IDX_IND_CFG,
 
    WECHAT_IDX_READ_CHAR,
    WECHAT_IDX_READ_VAL,
    
    WECHAT_IDX_NB,
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// wechat environment variable
struct wechat_env_tag
{
    /// Connection Information
    struct prf_con_info con_info;

    /// Sample128  svc Start Handle
    uint16_t wechat_shdl;
    
    //Notification property status
    uint8_t feature;

};

/*
 * WECHAT PROFILE ATTRIBUTES DECLARATION
 ****************************************************************************************
 */

/// WECHAT Database Description
extern const struct attm_desc wechat_att_db[WECHAT_IDX_NB];


/*
 *  WECHAT PROFILE ATTRIBUTES VALUES DECLARATION
 ****************************************************************************************
 */

/// WECHAT Service
extern const att_svc_desc_t wechat_svc;
/// WECHAT Service - write Characteristic
extern const struct att_char_desc wechat_write_char;
/// WECHAT Service - read Characteristic
extern const struct att_char_desc wechat_read_char;
/// WECHAT Service - indication Characteristic
extern const struct att_char_desc wechat_ind_char;

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

extern struct wechat_env_tag wechat_env;
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the WECHAT module.
 * This function performs all the initializations of the WECHAT module.
 ****************************************************************************************
 */
void wechat_init(void);

void wechat_send_write_val(uint8_t* val, uint32_t length);

void wechat_enable_indication(bool isEnable);

void wechat_indication_cfm_send(uint8_t status);

/**
 ****************************************************************************************
 * @brief Disable role.
 ****************************************************************************************
 */
void wechat_disable(void);

#endif //BLE_WECHAT

#endif // WECHAT_H_
