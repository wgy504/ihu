/**
 ****************************************************************************************
 *
 * @file app_user_config.h
 *
 * @brief Compile configuration file.
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

#ifndef _APP_USER_CONFIG_H_
#define _APP_USER_CONFIG_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup
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

#include "co_bt.h"
#include "gap.h"
#include "gapm.h"
#include "gapm_task.h"
#include "gapc_task.h"
#include <stdint.h>

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct security_configuration
{
	/// IO capabilities (@see gap_io_cap)
    enum gap_io_cap iocap;
    /// OOB information (@see gap_oob)
    enum gap_oob oob;
    /// Authentication (@see gap_auth)
    enum gap_auth auth;
    /// Encryption key size (7 to 16)
    uint8_t key_size;
    ///Initiator key distribution (@see gap_kdist)
    enum gap_kdist ikey_dist;
    ///Responder key distribution (@see gap_kdist)
    enum gap_kdist rkey_dist;
    /// Device security requirements (minimum security level). (@see gap_sec_req)
    enum gap_sec_req sec_req;
	struct gap_sec_key tk;
	struct gap_sec_key csrk;
};

//---------------------------------------------------------------------
//---------------------ADVERTIZE---RELATED-----------------------------

enum default_adv_operation
{
    /// Start non connectable advertising
    ADV_NON_CONN = GAPM_ADV_NON_CONN,
    /// Start undirected connectable advertising
    ADV_UNDIRECT,
    /// Start directed connectable advertising
    ADV_DIRECT,
};


struct advertise_configuration {
	/// Advertise operation type.
	enum default_adv_operation advertise_operation;
	/// Own BD address source of the device:
    enum gapm_own_addr_src address_src;
	/// Advertising Interval
    uint16_t intv;
    ///Advertising channel map
    uint8_t	channel_map;
};

struct directed_advertise_configuration {
    /// BD Address of device
    uint8_t  addr[BD_ADDR_LEN];
	/// Address type of the device 0=public/1=private random
    uint8_t addr_type;
};

//---------------------------------------------------------------------
//---------------------PARAM UPDATE---RELATED--------------------------

struct connection_param_configuration{
    /// Connection interval minimum
    uint16_t intv_min;
    /// Connection interval maximum
    uint16_t intv_max;
    /// Latency
    uint16_t latency;
    /// Supervision timeout
    uint16_t time_out;
    /// Minimum Connection Event Duration
    uint16_t ce_len_min;
    /// Maximum Connection Event Duration
    uint16_t ce_len_max;
};

#define MS_TO_BLESLOTS(x) ((int)(x/0.625))
#define MS_TO_DOUBLESLOTS(x) ((int)(x/1.25))
#define MS_TO_TIMERUNITS(x) ((int)(x/10))

#define US_TO_BLESLOTS(x) ((int)(x/625))
#define US_TO_DOUBLESLOTS(x) ((int)(x/1250))
#define US_TO_TIMERUNITS(x) ((int)(x/10000))


//---------------------------------------------------------------------
//---------------------PARAM UPDATE---RELATED--------------------------

struct gapm_configuration {
	enum gap_role role;
	uint8_t irk[KEY_LEN];
	/// Device Appearance (0x0000 - Unknown appearance)
	//Fill in according to https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
    uint16_t appearance;
	enum gapm_write_att_perm appearance_write_perm;
	enum gapm_write_att_perm name_write_perm;
	/// Maximal MTU
    uint16_t max_mtu;
    /// Peripheral only: *****************************************************************
    /// Slave preferred Minimum of connection interval
    uint16_t con_intv_min;
    /// Slave preferred Maximum of connection interval
    uint16_t con_intv_max;
    /// Slave preferred Connection latency
    uint16_t con_latency;
    /// Slave preferred Link supervision timeout
    uint16_t superv_to;

    /// Privacy settings bit field (0b1 = enabled, 0b0 = disabled)
    ///  - [bit 0]: Privacy Support
    ///  - [bit 1]: Multiple Bond Support (Peripheral only); If enabled, privacy flag is
    ///             read only.
    ///  - [bit 2]: Reconnection address visible.
    uint8_t flags;
};

/// @} APP

#endif // _APP_USER_CONFIG_H_
