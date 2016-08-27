/**
 ****************************************************************************************
 *
 * @file user_profiles_config.h
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

#ifndef _USER_PROFILES_CONFIG_H_
#define _USER_PROFILES_CONFIG_H_

/**
 ****************************************************************************************
 * @defgroup APP_CONFIG
 * @ingroup APP
 * @brief  Application configuration file
 *
 * This file contains the configuaration of the profiles used by the application.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
/// Add below the profiles that the application wishes to use by including the <profile_name>.h file.

/*
 * PROFILE CONFIGUARTION
 ****************************************************************************************
 */

/// Add profile specific configurations

/* SPOTAR configurations
 * -----------------------------------------------------------------------------------------
 */
 
#define SPOTAR_PATCH_AREA   1   // Placed in the RetRAM when SPOTAR_PATCH_AREA is 0 and in SYSRAM when 1
 
/* DISS profile configurations
 * -----------------------------------------------------------------------------------------
 */
#if defined(CFG_APP)

/// Manufacturer Name (up to 18 chars)
#define APP_DIS_MANUFACTURER_NAME       ("Dialog Semi")
#define APP_DIS_MANUFACTURER_NAME_LEN   (11)
/// Model Number String (up to 18 chars)
#define APP_DIS_MODEL_NB_STR            ("DA1458x")
#define APP_DIS_MODEL_NB_STR_LEN        (7)
/// System ID - LSB -> MSB (FIXME)
#define APP_DIS_SYSTEM_ID               ("\x12\x34\x56\xFF\xFE\x9A\xBC\xDE")
#define APP_DIS_SYSTEM_ID_LEN           (8)
#define APP_DIS_SW_REV                  DA14580_REFDES_SW_VERSION
#define APP_DIS_FIRM_REV                DA14580_SW_VERSION
#define APP_DIS_FEATURES                (DIS_MANUFACTURER_NAME_CHAR_SUP | DIS_MODEL_NB_STR_CHAR_SUP | DIS_SYSTEM_ID_CHAR_SUP | DIS_SW_REV_STR_CHAR_SUP | DIS_FIRM_REV_STR_CHAR_SUP | DIS_PNP_ID_CHAR_SUP)
/// PNP ID
#define APP_DISS_PNP_COMPANY_ID_TYPE    (0x01)
#define APP_DISS_PNP_VENDOR_ID          (0x00D2)
#define APP_DISS_PNP_PRODUCT_ID         (0x0580)
#define APP_DISS_PNP_PRODUCT_VERSION    (0x0100)

#else

#define APP_DIS_FEATURES                (0)

#endif //(CFG_APP)


/* BASS profile configurations
 * -----------------------------------------------------------------------------------------
 */
 //Measured in timer units (10ms)
 #define APP_BASS_POLL_INTERVAL   (6000)  //  (6000*10ms)/60sec = Every 1 minutes
 
/// @} APP_CONFIG

#endif // _USER_PROFILES_CONFIG_H_
