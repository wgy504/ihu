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

#include "arch_api.h"
#include "app_adv_data.h"
/*
 * DEFINES
 ****************************************************************************************
 */

//default sleep mode. Possible values 	ARCH_SLEEP_OFF,	ARCH_EXT_SLEEP_ON, ARCH_DEEP_SLEEP_ON
const static sleep_state_t app_default_sleep_mode=ARCH_SLEEP_OFF;


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

#endif // _USER_CONFIG_H_

