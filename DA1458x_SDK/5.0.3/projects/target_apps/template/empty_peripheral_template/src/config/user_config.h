/**
 ****************************************************************************************
 *
 * @file user_config.h
 *
 * @brief User configuration file.
 *
 * Copyright (C) 2015. Dialog Semiconductor Ltd, unpublished work. This computer 
 * program includes Confidential, Proprietary Information and is a Trade Secret of 
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited 
 * unless authorized in writing. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */
 
#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_user_config.h"
#include "arch_api.h"
#include "app_default_handlers.h"
#include "app_adv_data.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define USER_CONFIG                (1)
//default sleep mode. Possible values 	ARCH_SLEEP_OFF,	ARCH_EXT_SLEEP_ON, ARCH_DEEP_SLEEP_ON
const static sleep_state_t app_default_sleep_mode=ARCH_SLEEP_OFF;


/*
 * LOCAL VARIABLES
 ****************************************************************************************
 */

static const struct security_configuration user_security_configuration = {
    .oob            = GAP_OOB_AUTH_DATA_NOT_PRESENT,
    .key_size       = KEY_LEN,
    .iocap          = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
    .auth           = GAP_AUTH_REQ_NO_MITM_BOND,
    .sec_req        = GAP_NO_SEC,
    .ikey_dist      = GAP_KDIST_SIGNKEY,
    .rkey_dist      = GAP_KDIST_ENCKEY,
    .tk={ 
            .key={0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
        },
    .csrk={
            .key={0xAB,0xAB,0x45,0x55,0x23,0x01,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
        },
};

//---------------------------------------------------------------------
//---------------------ADVERTIZE---RELATED-----------------------------

//---------------------------------------------------------------------
//-------------UNDIRECTED ADVERTIZE---RELATED--------------------------

static const struct advertise_configuration user_undirected_advertise_conf ={
    /// Advertise operation type.
    .advertise_operation=ADV_UNDIRECT,      
    /// Own BD address source of the device:
    .address_src=GAPM_PUBLIC_ADDR,          
    /// Advertise interval
    .intv = 1100, 
    ///Advertising channel map
    .channel_map = 0x7,
};

static const enum gap_adv_mode user_undirected_advertise_mode = GAP_GEN_DISCOVERABLE;

//---------------------------------------------------------------------
//-------------NON-CONNECTABLE ADVERTISE---RELATED---------------------

static const struct advertise_configuration user_non_connectable_advertise_conf ={
    /// Advertise operation type.
    .advertise_operation=ADV_NON_CONN,      
    /// Own BD address source of the device:
    .address_src=GAPM_PUBLIC_ADDR,          
    /// Advertise interval
    .intv = 1100, 
    ///Advertising channel map
    .channel_map = 0x7,
};

static const enum gap_adv_mode user_non_connectable_advertise_mode = GAP_GEN_DISCOVERABLE;

//---------------------------------------------------------------------
//-------------NON-CONNECTABLE & UNDIRECTED ADVERTISE RELATED COMMON --

/// Advertising service data
/// Advertising AD type flags, shall not be set in advertising data
#define USER_ADVERTISE_DATA    ""
/// Advertising data length - maximum 28 bytes, 3 bytes are reserved to set
#define USER_ADVERTISE_DATA_LEN (sizeof(USER_ADVERTISE_DATA)-1)
/// Advertising name
#define USER_DEVICE_NAME    "DIALOG-TMPL"
/// Advertising name Length
#define USER_DEVICE_NAME_LEN (sizeof(USER_DEVICE_NAME)-1)
/// Scan response data
#define USER_ADVERTISE_SCAN_RESPONSE_DATA ""
/// Scan response data length- maximum 31 bytes
#define USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN (sizeof(USER_ADVERTISE_SCAN_RESPONSE_DATA)-1)

//---------------------------------------------------------------------
//-------------DIRECTED ADVERTIZE---RELATED----------------------------

static const struct advertise_configuration user_directed_advertise_conf ={
    /// Advertise operation type.
    .advertise_operation=ADV_DIRECT,      
    /// Own BD address source of the device:
    .address_src=GAPM_PUBLIC_ADDR,          
    ///Advertising channel map
    .channel_map = 0x7,
};
static const struct directed_advertise_configuration user_directed_advertise_target_address_conf={
    /// BD Address of device
    .addr = {0x1,0x2,0x3,0x4,0x5,0x6},
    /// Address type of the device 0=public/1=private random
    .addr_type=0
};

//---------------------------------------------------------------------
//---------------------PARAM UPDATE---RELATED--------------------------

static const struct connection_param_configuration user_connection_param_conf = {
    /// Connection interval minimum measured in ble double slots (1.25ms) 
    /// use the macro MS_TO_DOUBLESLOTS to convert from milliseconds (ms) to double slots 
    .intv_min = MS_TO_DOUBLESLOTS(10),
    /// Connection interval maximum measured in ble double slots (1.25ms) 
    /// use the macro MS_TO_DOUBLESLOTS to convert from milliseconds (ms) to double slots 
    .intv_max = MS_TO_DOUBLESLOTS(20),
    /// Latency measured in connection events
    .latency = 0,
    /// Supervision timeout measured in timer units (10 ms) 
    /// use the macro MS_TO_TIMERUNITS to convert from milliseconds (ms) to timer units 
    .time_out = MS_TO_TIMERUNITS(1250),
    /// Minimum Connection Event Duration measured in ble double slots (1.25ms) 
    /// use the macro MS_TO_DOUBLESLOTS to convert from milliseconds (ms) to double slots
    .ce_len_min = MS_TO_DOUBLESLOTS(0),
    /// Maximum Connection Event Duration measured in ble double slots (1.25ms) 
    /// use the macro MS_TO_DOUBLESLOTS to convert from milliseconds (ms) to double slots
    .ce_len_max = MS_TO_DOUBLESLOTS(0),
};

//---------------------------------------------------------------------
//---------------------GAPM---RELATED----------------------------------

static const struct gapm_configuration user_gapm_conf={
    .role = GAP_PERIPHERAL_SLV,
    .irk={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    /// Device Appearance (0x0000 - Unknown appearance)
    //Fill in according to https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
    .appearance=0,
    .appearance_write_perm=GAPM_WRITE_DISABLE,
    .name_write_perm=GAPM_WRITE_DISABLE,
    /// Maximal MTU
    .max_mtu= 23,
    /// Peripheral only: *****************************************************************
    /// Slave preferred Minimum of connection interval  measured in ble double slots (1.25ms) 
    /// use the macro MS_TO_DOUBLESLOTS to convert from milliseconds (ms) to double slots
    .con_intv_min = MS_TO_DOUBLESLOTS(10),
    /// Slave preferred Maximum of connection interval  measured in ble double slots (1.25ms) 
    /// use the macro MS_TO_DOUBLESLOTS to convert from milliseconds (ms) to double slots
    .con_intv_max = MS_TO_DOUBLESLOTS(20),
    /// Slave preferred Connection latency. It is measured in connection events skipped
    .con_latency = 0,
    /// Slave preferred Link supervision timeout measured in timer units (10 ms) 
    /// use the macro MS_TO_TIMERUNITS to convert from milliseconds (ms) to timer units
    .superv_to = MS_TO_TIMERUNITS(1000), 
    /// Privacy settings bit field (0b1 = enabled, 0b0 = disabled)
    ///  - [bit 0]: Privacy Support
    ///  - [bit 1]: Multiple Bond Support (Peripheral only); If enabled, privacy flag is
    ///             read only.
    ///  - [bit 2]: Reconnection address visible.
    .flags=0
};

//---------------------------------------------------------------------
//---------------------Default Handlers Configuration------------------
static const struct default_handlers_configuration  user_default_hnd_conf= {
    //Configure the advertise operation used by the default handlers
    //Possible values:
    //  - DEF_ADV_FOREVER
    //  - DEF_ADV_WITH_TIMEOUT
    .adv_scenario=DEF_ADV_FOREVER,

    //Configure the advertise period in case of DEF_ADV_WITH_TIMEOUT.
    //It is measured in timer units (10ms). Use MS_TO_TIMERUNITS macro to convert
    //from milliseconds (ms) to timer units.
    .advertise_period=MS_TO_TIMERUNITS(10000),

    //Configure the security start operation of the default handlers
    //if the security is enabled (CFG_APP_SECURITY)
    .security_request_scenario=DEF_SEC_REQ_NEVER
};

#endif // _USER_CONFIG_H_
