/**
 ****************************************************************************************
 *
 * @file user_callback_config.h
 *
 * @brief Configuration file for the profiles used in the application.
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

#ifndef _USER_CALLBACK_CONFIG_H_
#define _USER_CALLBACK_CONFIG_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "arch_api.h"
#include "mainloop_callbacks.h"

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

static const struct arch_main_loop_callbacks user_app_main_loop_callbacks = {
    .app_on_init            = NULL,
    .app_on_ble_powered     = app_on_ble_powered,
    .app_on_sytem_powered      = app_on_full_power,
    .app_before_sleep       = NULL,
    .app_validate_sleep     = NULL,
    .app_going_to_sleep     = app_going_to_sleep,
    .app_resume_from_sleep  = app_resume_from_sleep,
};

#endif // _USER_CALLBACK_CONFIG_H_
