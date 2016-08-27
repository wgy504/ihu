/**
 ****************************************************************************************
 *
 * @file user_custs_config.h
 *
 * @brief Custom1/2 Server (CUSTS1/2) profile database structure and initialization.
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

#ifndef _USER_CUSTS_CONFIG_H_
#define _USER_CUSTS_CONFIG_H_

/**
 ****************************************************************************************
 * @defgroup USER_CONFIG
 * @ingroup USER
 * @brief Custom1/2 Server (CUSTS1/2) profile database structure and initialization.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "prf_types.h"
#include "attm_db_128.h"
#include "app_prf_types.h"
#include "app_customs.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define DEF_CUST1_SVC_UUID_128            {0x2F, 0x2A, 0x93, 0xA6, 0xBD, 0xD8, 0x41, 0x52, 0xAC, 0x0B, 0x10, 0x99, 0x2E, 0xC6, 0xFE, 0xED}

#define DEF_CUST1_CTRL_POINT_UUID_128     {0x20, 0xEE, 0x8D, 0x0C, 0xE1, 0xF0, 0x4A, 0x0C, 0xB3, 0x25, 0xDC, 0x53, 0x6A, 0x68, 0x86, 0x2D}
#define DEF_CUST1_LED_STATE_UUID_128      {0x4F, 0x43, 0x31, 0x3C, 0x93, 0x92, 0x42, 0xE6, 0xA8, 0x76, 0xFA, 0x3B, 0xEF, 0xB4, 0x87, 0x5A}
#define DEF_CUST1_ADC_VAL_1_UUID_128      {0x17, 0xB9, 0x67, 0x98, 0x4C, 0x66, 0x4C, 0x01, 0x96, 0x33, 0x31, 0xB1, 0x91, 0x59, 0x00, 0x15}
#define DEF_CUST1_ADC_VAL_2_UUID_128      {0x23, 0x68, 0xEC, 0x52, 0x1E, 0x62, 0x44, 0x74, 0x9A, 0x1B, 0xD1, 0x8B, 0xAB, 0x75, 0xB6, 0x6E}
#define DEF_CUST1_BUTTON_STATE_UUID_128   {0x9E, 0xE7, 0xBA, 0x08, 0xB9, 0xA9, 0x48, 0xAB, 0xA1, 0xAC, 0x03, 0x1C, 0x2E, 0x0D, 0x29, 0x6C}
#define DEF_CUST1_INDICATEABLE_UUID_128   {0x28, 0xD5, 0xE1, 0xC1, 0xE1, 0xC5, 0x47, 0x29, 0xB5, 0x57, 0x65, 0xC3, 0xBA, 0x47, 0x15, 0x9E}
#define DEF_CUST1_LONG_VALUE_UUID_128     {0x8C, 0x09, 0xE0, 0xD1, 0x81, 0x54, 0x42, 0x40, 0x8E, 0x4F, 0xD2, 0xB3, 0x77, 0xE3, 0x2A, 0x77}

#define DEF_CUST1_CTRL_POINT_CHAR_LEN     1
#define DEF_CUST1_LED_STATE_CHAR_LEN      1
#define DEF_CUST1_ADC_VAL_1_CHAR_LEN      2
#define DEF_CUST1_ADC_VAL_2_CHAR_LEN      2
#define DEF_CUST1_BUTTON_STATE_CHAR_LEN   1
#define DEF_CUST1_INDICATEABLE_CHAR_LEN   20
#define DEF_CUST1_LONG_VALUE_CHAR_LEN     50

#define CUST1_CONTROL_POINT_USER_DESC     "Control Point"
#define CUST1_LED_STATE_USER_DESC         "LED State"
#define CUST1_ADC_VAL_1_USER_DESC         "ADC Value 1"
#define CUST1_ADC_VAL_2_USER_DESC         "ADC Value 2"
#define CUST1_BUTTON_STATE_USER_DESC      "Button State"
#define CUST1_INDICATEABLE_USER_DESC      "Indicateable"
#define CUST1_LONG_VALUE_CHAR_USER_DESC   "Long Value"

/// Custom1 Service Data Base Characteristic enum
enum
{
    CUST1_IDX_SVC = 0,
    
    CUST1_IDX_CONTROL_POINT_CHAR,
    CUST1_IDX_CONTROL_POINT_VAL,
    CUST1_IDX_CONTROL_POINT_USER_DESC,
    
    CUST1_IDX_LED_STATE_CHAR,
    CUST1_IDX_LED_STATE_VAL,
    CUST1_IDX_LED_STATE_USER_DESC,
    
    CUST1_IDX_ADC_VAL_1_CHAR,
    CUST1_IDX_ADC_VAL_1_VAL,
    CUST1_IDX_ADC_VAL_1_NTF_CFG,
    CUST1_IDX_ADC_VAL_1_USER_DESC,
    
    CUST1_IDX_ADC_VAL_2_CHAR,
    CUST1_IDX_ADC_VAL_2_VAL,
    CUST1_IDX_ADC_VAL_2_USER_DESC,
    
    CUST1_IDX_BUTTON_STATE_CHAR,
    CUST1_IDX_BUTTON_STATE_VAL,
    CUST1_IDX_BUTTON_STATE_NTF_CFG,
    CUST1_IDX_BUTTON_STATE_USER_DESC,
    
    CUST1_IDX_INDICATEABLE_CHAR,
    CUST1_IDX_INDICATEABLE_VAL,
    CUST1_IDX_INDICATEABLE_IND_CFG,
    CUST1_IDX_INDICATEABLE_USER_DESC,
    
    CUST1_IDX_LONG_VALUE_CHAR,
    CUST1_IDX_LONG_VALUE_VAL,
    CUST1_IDX_LONG_VALUE_NTF_CFG,
    CUST1_IDX_LONG_VALUE_USER_DESC,
    
    CUST1_IDX_NB
};

/*
 * LOCAL VARIABLES
 ****************************************************************************************
 */


/// Custom Server Attributes Values Definition

static const att_svc_desc128_t custs1_svc                        = DEF_CUST1_SVC_UUID_128;

static uint8_t CUST1_CTRL_POINT_UUID_128[ATT_UUID_128_LEN]       = DEF_CUST1_CTRL_POINT_UUID_128;
static uint8_t CUST1_LED_STATE_UUID_128[ATT_UUID_128_LEN]        = DEF_CUST1_LED_STATE_UUID_128;
static uint8_t CUST1_ADC_VAL_1_UUID_128[ATT_UUID_128_LEN]        = DEF_CUST1_ADC_VAL_1_UUID_128;
static uint8_t CUST1_ADC_VAL_2_UUID_128[ATT_UUID_128_LEN]        = DEF_CUST1_ADC_VAL_2_UUID_128;
static uint8_t CUST1_BUTTON_STATE_UUID_128[ATT_UUID_128_LEN]     = DEF_CUST1_BUTTON_STATE_UUID_128;
static uint8_t CUST1_INDICATEABLE_UUID_128[ATT_UUID_128_LEN]     = DEF_CUST1_INDICATEABLE_UUID_128;
static uint8_t CUST1_LONG_VALUE_UUID_128[ATT_UUID_128_LEN]       = DEF_CUST1_LONG_VALUE_UUID_128;

static const struct att_char128_desc custs1_ctrl_point_char       = {ATT_CHAR_PROP_WR,
                                                                     {0, 0},
                                                                     DEF_CUST1_CTRL_POINT_UUID_128};

static const struct att_char128_desc custs1_led_state_char        = {ATT_CHAR_PROP_WR_NO_RESP,
                                                                     {0, 0},
                                                                     DEF_CUST1_LED_STATE_UUID_128};

static const struct att_char128_desc custs1_adc_val_1_char        = {ATT_CHAR_PROP_RD | ATT_CHAR_PROP_NTF,
                                                                     {0, 0},
                                                                     DEF_CUST1_ADC_VAL_1_UUID_128};

static const struct att_char128_desc custs1_adc_val_2_char        = {ATT_CHAR_PROP_RD,
                                                                     {0, 0},
                                                                     DEF_CUST1_ADC_VAL_2_UUID_128};

static const struct att_char128_desc custs1_button_state_char     = {ATT_CHAR_PROP_RD | ATT_CHAR_PROP_NTF,
                                                                     {0, 0},
                                                                     DEF_CUST1_BUTTON_STATE_UUID_128};

static const struct att_char128_desc custs1_indicateable_char     = {ATT_CHAR_PROP_RD | ATT_CHAR_PROP_IND,
                                                                     {0, 0},
                                                                     DEF_CUST1_INDICATEABLE_UUID_128};

static const struct att_char128_desc custs1_long_value_char       = {ATT_CHAR_PROP_RD | ATT_CHAR_PROP_WR | ATT_CHAR_PROP_NTF,
                                                                     {0, 0},
                                                                     DEF_CUST1_LONG_VALUE_UUID_128};

/*
 * CUSTS1 ATTRIBUTES
 ****************************************************************************************
 */

static const uint16_t att_decl_svc       = ATT_DECL_PRIMARY_SERVICE;
static const uint16_t att_decl_char      = ATT_DECL_CHARACTERISTIC;
static const uint16_t att_decl_cfg       = ATT_DESC_CLIENT_CHAR_CFG;
static const uint16_t att_decl_user_desc = ATT_DESC_CHAR_USER_DESCRIPTION;

/// Full CUSTOM1 Database Description - Used to add attributes into the database
static const struct attm_desc_128 custs1_att_db[CUST1_IDX_NB] =
{
    // CUSTOM1 Service Declaration
    [CUST1_IDX_SVC]                     = {(uint8_t*)&att_decl_svc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_svc), sizeof(custs1_svc), (uint8_t*)&custs1_svc},

    // Control Point Characteristic Declaration
    [CUST1_IDX_CONTROL_POINT_CHAR]      = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_ctrl_point_char), sizeof(custs1_ctrl_point_char), (uint8_t*)&custs1_ctrl_point_char},
    // Control Point Characteristic Value
    [CUST1_IDX_CONTROL_POINT_VAL]       = {CUST1_CTRL_POINT_UUID_128, ATT_UUID_128_LEN, PERM(WR, ENABLE),
                                            DEF_CUST1_CTRL_POINT_CHAR_LEN, 0, NULL},
    // Control Point Characteristic User Description
    [CUST1_IDX_CONTROL_POINT_USER_DESC] = {(uint8_t*)&att_decl_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(CUST1_CONTROL_POINT_USER_DESC) - 1, sizeof(CUST1_CONTROL_POINT_USER_DESC) - 1, CUST1_CONTROL_POINT_USER_DESC},

    // LED State Characteristic Declaration
    [CUST1_IDX_LED_STATE_CHAR]          = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_led_state_char), sizeof(custs1_led_state_char), (uint8_t*)&custs1_led_state_char},
    // LED State Characteristic Value
    [CUST1_IDX_LED_STATE_VAL]           = {CUST1_LED_STATE_UUID_128, ATT_UUID_128_LEN, PERM(WR, ENABLE),
                                            DEF_CUST1_LED_STATE_CHAR_LEN, 0, NULL},
    // LED State Characteristic User Description
    [CUST1_IDX_LED_STATE_USER_DESC]     = {(uint8_t*)&att_decl_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(CUST1_LED_STATE_USER_DESC) - 1, sizeof(CUST1_LED_STATE_USER_DESC) - 1, CUST1_LED_STATE_USER_DESC},

    // ADC Value 1 Characteristic Declaration
    [CUST1_IDX_ADC_VAL_1_CHAR]          = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_adc_val_1_char), sizeof(custs1_adc_val_1_char), (uint8_t*)&custs1_adc_val_1_char},
    // ADC Value 1 Characteristic Value
    [CUST1_IDX_ADC_VAL_1_VAL]           = {CUST1_ADC_VAL_1_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(NTF, ENABLE),
                                            DEF_CUST1_ADC_VAL_1_CHAR_LEN, 0, NULL},
    // ADC Value 1 Client Characteristic Configuration Descriptor
    [CUST1_IDX_ADC_VAL_1_NTF_CFG]       = {(uint8_t*)&att_decl_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE),
                                            sizeof(uint16_t), 0, NULL},
    // ADC Value 1 Characteristic User Description
    [CUST1_IDX_ADC_VAL_1_USER_DESC]     = {(uint8_t*)&att_decl_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(CUST1_ADC_VAL_1_USER_DESC) - 1, sizeof(CUST1_ADC_VAL_1_USER_DESC) - 1, CUST1_ADC_VAL_1_USER_DESC},

    // ADC Value 2 Characteristic Declaration
    [CUST1_IDX_ADC_VAL_2_CHAR]          = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_adc_val_2_char), sizeof(custs1_adc_val_2_char), (uint8_t*)&custs1_adc_val_2_char},
    // ADC Value 2 Characteristic Value
    [CUST1_IDX_ADC_VAL_2_VAL]           = {CUST1_ADC_VAL_2_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE),
                                            DEF_CUST1_ADC_VAL_2_CHAR_LEN, 0, NULL},
    // ADC Value 2 Characteristic User Description
    [CUST1_IDX_ADC_VAL_2_USER_DESC]     = {(uint8_t*)&att_decl_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(CUST1_ADC_VAL_2_USER_DESC) - 1, sizeof(CUST1_ADC_VAL_2_USER_DESC) - 1, CUST1_ADC_VAL_2_USER_DESC},

    // Button State Characteristic Declaration
    [CUST1_IDX_BUTTON_STATE_CHAR]       = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_button_state_char), sizeof(custs1_button_state_char), (uint8_t*)&custs1_button_state_char},
    // Button State Characteristic Value
    [CUST1_IDX_BUTTON_STATE_VAL]        = {CUST1_BUTTON_STATE_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(NTF, ENABLE),
                                            DEF_CUST1_BUTTON_STATE_CHAR_LEN, 0, NULL},
    // Button State Client Characteristic Configuration Descriptor
    [CUST1_IDX_BUTTON_STATE_NTF_CFG]    = {(uint8_t*)&att_decl_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE),
                                            sizeof(uint16_t), 0, NULL},
    // Button State Characteristic User Description
    [CUST1_IDX_BUTTON_STATE_USER_DESC]  = {(uint8_t*)&att_decl_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(CUST1_BUTTON_STATE_USER_DESC) - 1, sizeof(CUST1_BUTTON_STATE_USER_DESC) - 1, CUST1_BUTTON_STATE_USER_DESC},

    // Indicateable Characteristic Declaration
    [CUST1_IDX_INDICATEABLE_CHAR]       = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_indicateable_char), sizeof(custs1_indicateable_char), (uint8_t*)&custs1_indicateable_char},
    // Indicateable Characteristic Value
    [CUST1_IDX_INDICATEABLE_VAL]        = {CUST1_INDICATEABLE_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(IND, ENABLE),
                                            DEF_CUST1_INDICATEABLE_CHAR_LEN, 0, NULL},
    // Indicateable Client Characteristic Configuration Descriptor
    [CUST1_IDX_INDICATEABLE_IND_CFG]    = {(uint8_t*)&att_decl_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE),
                                            sizeof(uint16_t), 0, NULL},
    // Indicateable Characteristic User Description
    [CUST1_IDX_INDICATEABLE_USER_DESC]  = {(uint8_t*)&att_decl_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(CUST1_INDICATEABLE_USER_DESC) - 1, sizeof(CUST1_INDICATEABLE_USER_DESC) - 1, CUST1_INDICATEABLE_USER_DESC},

    // Long Value Characteristic Declaration
    [CUST1_IDX_LONG_VALUE_CHAR]         = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_long_value_char), sizeof(custs1_long_value_char), (uint8_t*)&custs1_long_value_char},
    // Long Value Characteristic Value
    [CUST1_IDX_LONG_VALUE_VAL]          = {CUST1_LONG_VALUE_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(NTF, ENABLE),
                                            DEF_CUST1_LONG_VALUE_CHAR_LEN, 0, NULL},
    // Long Value Client Characteristic Configuration Descriptor
    [CUST1_IDX_LONG_VALUE_NTF_CFG]      = {(uint8_t*)&att_decl_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE),
                                            sizeof(uint16_t), 0, NULL},
    // Long Value Characteristic User Description
    [CUST1_IDX_LONG_VALUE_USER_DESC]    = { (uint8_t*)&att_decl_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(CUST1_LONG_VALUE_CHAR_USER_DESC) - 1, sizeof(CUST1_LONG_VALUE_CHAR_USER_DESC) - 1, CUST1_LONG_VALUE_CHAR_USER_DESC},
};

/// Custom1/2 server function callback table
static const struct cust_prf_func_callbacks cust_prf_funcs[] =
{
#if (BLE_CUSTOM1_SERVER)
    {   TASK_CUSTS1,
        custs1_att_db,
        CUST1_IDX_NB,
        #if (BLE_APP_PRESENT)
        app_custs1_create_db, app_custs1_enable,
        #else
        NULL, NULL,
        #endif
        custs1_init, NULL
    },
#endif
#if (BLE_CUSTOM2_SERVER)
    {   TASK_CUSTS2,
        NULL,
        0,
        #if (BLE_APP_PRESENT)
        app_custs2_create_db, app_custs2_enable,
        #else
        NULL, NULL,
        #endif
        custs2_init, NULL
    },
#endif
    {TASK_NONE, NULL, 0, NULL, NULL, NULL, NULL},   // DO NOT MOVE. Must always be last
};

/// @} USER_CONFIG

#endif // _USER_CUSTS_CONFIG_H_
