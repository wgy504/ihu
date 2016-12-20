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
#include "da1458x_config_basic.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define USER_CONFIG                (1)
//default sleep mode. Possible values 	ARCH_SLEEP_OFF,	ARCH_EXT_SLEEP_ON, ARCH_DEEP_SLEEP_ON
const static sleep_state_t app_default_sleep_mode=ARCH_EXT_SLEEP_ON;


/*
 * LOCAL VARIABLES
 ****************************************************************************************
 */
/*
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
*/

/*
 ****************************************************************************************
 *
 * Security related configuration
 *
 ****************************************************************************************
 */
static const struct security_configuration user_security_conf = {
    /**************************************************************************************
     * IO capabilities (@see gap_io_cap)
     *
     * - GAP_IO_CAP_DISPLAY_ONLY          Display Only
     * - GAP_IO_CAP_DISPLAY_YES_NO        Display Yes No
     * - GAP_IO_CAP_KB_ONLY               Keyboard Only
     * - GAP_IO_CAP_NO_INPUT_NO_OUTPUT    No Input No Output
     * - GAP_IO_CAP_KB_DISPLAY            Keyboard Display
     *
     **************************************************************************************
     */
    .iocap          = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,

    /**************************************************************************************
     * OOB information (@see gap_oob)
     *
     * - GAP_OOB_AUTH_DATA_NOT_PRESENT    OOB Data not present
     * - GAP_OOB_AUTH_DATA_PRESENT        OOB data present
     *
     **************************************************************************************
     */
    .oob            = GAP_OOB_AUTH_DATA_NOT_PRESENT,

    /**************************************************************************************
     * Authentication (@see gap_auth)
     *
     * - GAP_AUTH_REQ_NO_MITM_NO_BOND     No MITM No Bonding
     * - GAP_AUTH_REQ_NO_MITM_BOND        No MITM Bonding
     * - GAP_AUTH_REQ_MITM_NO_BOND        MITM No Bonding
     * - GAP_AUTH_REQ_MITM_BOND           MITM and Bonding
     *
     **************************************************************************************
     */
    .auth           = GAP_AUTH_REQ_NO_MITM_BOND,

    /**************************************************************************************
     * Device security requirements (minimum security level). (@see gap_sec_req)
     *
     * - GAP_NO_SEC                       No security (no authentication and encryption)
     * - GAP_SEC1_NOAUTH_PAIR_ENC         Unauthenticated pairing with encryption
     * - GAP_SEC1_AUTH_PAIR_ENC           Authenticated pairing with encryption
     * - GAP_SEC2_NOAUTH_DATA_SGN         Unauthenticated pairing with data signing
     * - GAP_SEC2_AUTH_DATA_SGN           Authentication pairing with data signing
     * - GAP_SEC_UNDEFINED                Unrecognized security
     *
     **************************************************************************************
     */
    .sec_req        = GAP_NO_SEC,

     /// Encryption key size (7 to 16) - LTK Key Size
    .key_size       = KEY_LEN,

    /**************************************************************************************
     * Initiator key distribution (@see gap_kdist)
     *
     * - GAP_KDIST_NONE                   No Keys to distribute
     * - GAP_KDIST_ENCKEY                 LTK (Encryption key) in distribution
     * - GAP_KDIST_IDKEY                  IRK (ID key)in distribution
     * - GAP_KDIST_SIGNKEY                CSRK (Signature key) in distribution
     * - Any combination of the above
     *
     **************************************************************************************
     */
    .ikey_dist      = GAP_KDIST_SIGNKEY,

    /**************************************************************************************
     * Responder key distribution (@see gap_kdist)
     *
     * - GAP_KDIST_NONE                   No Keys to distribute
     * - GAP_KDIST_ENCKEY                 LTK (Encryption key) in distribution
     * - GAP_KDIST_IDKEY                  IRK (ID key)in distribution
     * - GAP_KDIST_SIGNKEY                CSRK (Signature key) in distribution
     * - Any combination of the above
     *
     **************************************************************************************
     */
    .rkey_dist      = GAP_KDIST_ENCKEY,
};

//---------------------------------------------------------------------
//---------------------ADVERTIZE---RELATED-----------------------------


//---------------------------------------------------------------------
//-------------UNDIRECTED ADVERTIZE---RELATED--------------------------
static const struct advertise_configuration user_adv_conf = {
    /// Own BD address source of the device:
    /// - GAPM_PUBLIC_ADDR:         Public Address
    /// - GAPM_PROVIDED_RND_ADDR:   Provided random address
    /// - GAPM_GEN_STATIC_RND_ADDR: Generated static random address
    /// - GAPM_GEN_RSLV_ADDR:       Generated resolvable private random address
    /// - GAPM_GEN_NON_RSLV_ADDR:   Generated non-resolvable private random address
    /// - GAPM_PROVIDED_RECON_ADDR: Provided Reconnection address (only for GAPM_ADV_DIRECT)
    .addr_src = GAPM_PUBLIC_ADDR,

    /// Duration of resolvable address before regenerate it.
    .renew_dur = 0,

    /// Provided own static private random address (addr_src = GAPM_PROVIDED_RND_ADDR or GAPM_PROVIDED_RECON_ADDR)
    // .addr = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6},

    /// Minimum interval for advertising
    .intv_min = 160,                    // 6187.5 ms (9900*0.625ms)

    /// Maximum interval for advertising
    .intv_max = 160,                    // 8250ms (13200*0.625ms)

    /// Advertising channel map
    .channel_map = 0x7,

    /*************************
     * Advertising information
     *************************
     */

    /// Host information advertising data (GAPM_ADV_NON_CONN and GAPM_ADV_UNDIRECT)
    /// Advertising mode :
    /// - GAP_NON_DISCOVERABLE: Non discoverable mode
    /// - GAP_GEN_DISCOVERABLE: General discoverable mode
    /// - GAP_LIM_DISCOVERABLE: Limited discoverable mode
    /// - GAP_BROADCASTER_MODE: Broadcaster mode
    .mode = GAP_GEN_DISCOVERABLE,

    /// Host information advertising data (GAPM_ADV_NON_CONN and GAPM_ADV_UNDIRECT)
    /// Advertising filter policy:
    /// - ADV_ALLOW_SCAN_ANY_CON_ANY: Allow both scan and connection requests from anyone
    /// - ADV_ALLOW_SCAN_WLST_CON_ANY: Allow both scan req from White List devices only and
    ///   connection req from anyone
    /// - ADV_ALLOW_SCAN_ANY_CON_WLST: Allow both scan req from anyone and connection req
    ///   from White List devices only
    /// - ADV_ALLOW_SCAN_WLST_CON_WLST: Allow scan and connection requests from White List
    ///   devices only
    .adv_filt_policy = ADV_ALLOW_SCAN_ANY_CON_ANY,

    /// Direct address information (GAPM_ADV_DIRECT)
    /// (used only if reconnection address isn't set or privacy disabled)
    /// BD Address of device
    .peer_addr = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6},

    /// Direct address information (GAPM_ADV_DIRECT)
    /// (used only if reconnection address isn't set or privacy disabled)
    /// Address type of the device 0=public/1=private random
    .peer_addr_type = 0,
};

//---------------------------------------------------------------------
//-------------NON-CONNECTABLE & UNDIRECTED ADVERTISE RELATED COMMON --

/// Advertising service data
/// Advertising AD type flags, shall not be set in advertising data
#if (IHU_DEVELOPER_CURRENT_WORKING_ENV_NAME == IHU_DEVELOPER_ENV_NAME_NULL)
	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x12\x34\x56\x78\x9a\xbc\x03\x03\xE7\xFE"
#elif (IHU_DEVELOPER_CURRENT_WORKING_ENV_NAME == IHU_DEVELOPER_ENV_NAME_ZJL)
	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x27\xEF\xA5\x72\x39\xD0\x03\x03\xE7\xFE"
#elif (IHU_DEVELOPER_CURRENT_WORKING_ENV_NAME == IHU_DEVELOPER_ENV_NAME_MYC)		
	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x29\xEF\xA5\x72\x39\xD0\x03\x03\xE7\xFE"
#elif (IHU_DEVELOPER_CURRENT_WORKING_ENV_NAME == IHU_DEVELOPER_ENV_NAME_XX)		
	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x30\xEF\xA5\x72\x39\xD0\x03\x03\xE7\xFE"
#elif (IHU_DEVELOPER_CURRENT_WORKING_ENV_NAME == IHU_DEVELOPER_ENV_NAME_CZ)		
	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x2A\xEF\xA5\x72\x39\xD0\x03\x03\xE7\xFE"
#elif (IHU_DEVELOPER_CURRENT_WORKING_ENV_NAME == IHU_DEVELOPER_ENV_NAME_LZH)		
	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x2B\xEF\xA5\x72\x39\xD0\x03\x03\xE7\xFE"
#elif (IHU_DEVELOPER_CURRENT_WORKING_ENV_NAME == IHU_DEVELOPEr_ENV_NAME_JT)		
	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x2C\xEF\xA5\x72\x39\xD0\x03\x03\xE7\xFE"
#else
	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x27\xEF\xA5\x72\x39\xD0\x03\x03\xE7\xFE"
#endif

//#ifdef IHU_DEVELOPER_ENV_NAME_NULL
//	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x12\x34\x56\x78\x9a\xbc\x03\x03\xE7\xFE"
//#endif
//#ifdef IHU_DEVELOPER_ENV_NAME_ZJL
//	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x27\xEF\xA5\x72\x39\xD0\x03\x03\xE7\xFE"
//#endif
//#ifdef IHU_DEVELOPER_ENV_NAME_MYC
//	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x28\xEF\xA5\x72\x39\xD0\x03\x03\xE7\xFE"
//#endif
//#ifdef IHU_DEVELOPER_ENV_NAME_XX
//	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x29\xEF\xA5\x72\x39\xD0\x03\x03\xE7\xFE"
//#endif
//#ifdef IHU_DEVELOPER_ENV_NAME_CZ
//	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x2A\xEF\xA5\x72\x39\xD0\x03\x03\xE7\xFE"
//#endif
//#ifdef IHU_DEVELOPER_ENV_NAME_LZH
//	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x2B\xEF\xA5\x72\x39\xD0\x03\x03\xE7\xFE"
//#endif
//#ifdef IHU_DEVELOPEr_ENV_NAME_JT
//	#define USER_ADVERTISE_DATA    "\x09\xff\x60\x00\x2C\xEF\xA5\x72\x39\xD0\x03\x03\xE7\xFE"
//#endif

/// Advertising data length - maximum 28 bytes, 3 bytes are reserved to set
#define USER_ADVERTISE_DATA_LEN (sizeof(USER_ADVERTISE_DATA)-1)
/// Advertising name
#define USER_DEVICE_NAME    "BAXIAN_BLE"
/// Advertising name Length
#define USER_DEVICE_NAME_LEN (sizeof(USER_DEVICE_NAME)-1)
/// Scan response data
#define USER_ADVERTISE_SCAN_RESPONSE_DATA ""
/// Scan response data length- maximum 31 bytes
#define USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN (sizeof(USER_ADVERTISE_SCAN_RESPONSE_DATA)-1)


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

static const struct gapm_configuration user_gapm_conf = {
    .role = GAP_PERIPHERAL_SLV,
    .irk = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    /// Device Appearance (0x0000 - Unknown appearance)
    //Fill in according to https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
    .appearance = 0,
    .appearance_write_perm = GAPM_WRITE_DISABLE,
    .name_write_perm = GAPM_WRITE_DISABLE,
    /// Maximal MTU
    .max_mtu = 23,
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
    .flags = 0
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

/*
 ****************************************************************************************
 *
 * Central configuration (not used by current example)
 *
 ****************************************************************************************
 */
static const struct central_configuration user_central_conf;

#endif // _USER_CONFIG_H_
