/**
 ****************************************************************************************
 *
 * @file user_custs_config.h
 *
 * @brief Custom1/2 Server (CUSTS1/2) profile database initialization.
 *
 * Copyright (C) 2016. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */

#ifndef _USER_CUSTS_CONFIG_H_
#define _USER_CUSTS_CONFIG_H_

/**
 ****************************************************************************************
 * @defgroup USER_CONFIG
 * @ingroup USER
 * @brief Custom1/2 Server (CUSTS1/2) profile database initialization.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_prf_types.h"

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

extern const struct cust_prf_func_callbacks cust_prf_funcs[];

/// @} USER_CONFIG

#endif // _USER_CUSTS_CONFIG_H_
