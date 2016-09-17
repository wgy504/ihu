/**
 ****************************************************************************************
 *
 * @file wechat.c
 *
 * @brief 128 UUID service. Sample code
 *
 * Copyright (C) 2013 Dialog Semiconductor GmbH and its Affiliates, unpublished work
 * This computer program includes Confidential, Proprietary Information and is a Trade Secret 
 * of Dialog Semiconductor GmbH and its Affiliates. All use, disclosure, and/or 
 * reproduction is prohibited unless authorized in writing. All Rights Reserved.
 *
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwble_config.h"

#if (BLE_WECHAT)

#include "wechat.h"
#include "wechat_task.h"
#include "attm_db.h"
#include "gapc.h"
/*
 *  WECHAT PROFILE ATTRIBUTES VALUES DEFINTION
 ****************************************************************************************
 */
/// Full WECHAT Database Description - Used to add attributes into the database
const struct attm_desc wechat_att_db[WECHAT_IDX_NB] =
{
    // Wechat Service Declaration
    [WECHAT_IDX_SVC]                  =   {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), sizeof(wechat_svc),
                                        sizeof(wechat_svc), (uint8_t *)&wechat_svc},

    // Wechat Write Characteristic Declaration
    [WECHAT_IDX_WRITE_CHAR]        =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), sizeof(wechat_write_char),
                                         sizeof(wechat_write_char), (uint8_t *)&wechat_write_char},
    // Wechat Write Characteristic Value
    [WECHAT_IDX_WRITE_VAL]         =   {BLE_UUID_WECHAT_WRITE_CHARACTERISTICS, PERM(WR, ENABLE), BLE_WECHAT_MAX_DATA_LEN,
                                         0, NULL},
  
    // Wechat Read Characteristic Declaration
    [WECHAT_IDX_READ_CHAR]        =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), sizeof(wechat_read_char),
                                         sizeof(wechat_read_char), (uint8_t *)&wechat_read_char},
    // Wechat Read Characteristic Value
    [WECHAT_IDX_READ_VAL]         =   {BLE_UUID_WECHAT_READ_CHARACTERISTICS, PERM(RD, ENABLE), BLE_WECHAT_MAX_DATA_LEN,
                                         0, NULL},
    
    // Wechat IND Characteristic Declaration
    [WECHAT_IDX_IND_CHAR]        =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), sizeof(wechat_ind_char),
                                        sizeof(wechat_ind_char), (uint8_t *)&wechat_ind_char},
    // Wechat IND Characteristic Value
    [WECHAT_IDX_IND_VAL]         =   {BLE_UUID_WECHAT_INDICATE_CHARACTERISTICS, PERM(IND, ENABLE), BLE_WECHAT_MAX_DATA_LEN,
                                        0, NULL},
    // Wechat IND Characteristic - Client Characteristic Configuration Descriptor
    [WECHAT_IDX_IND_CFG]     =   {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE)|PERM(WR, ENABLE), sizeof(uint16_t),
                                        0, NULL},

};

/// Wechat Service
const att_svc_desc_t wechat_svc = BLE_UUID_WECHAT_SERVICE;

/// Wechat write characteristic
const struct att_char_desc wechat_write_char = ATT_CHAR(ATT_CHAR_PROP_WR,
                                                            0,
                                                            BLE_UUID_WECHAT_WRITE_CHARACTERISTICS); 

/// Wechat read characteristic
const struct att_char_desc wechat_read_char = ATT_CHAR(ATT_CHAR_PROP_RD,
                                                            0,
                                                            BLE_UUID_WECHAT_READ_CHARACTERISTICS); 
/// Wechat indication characteristic
const struct att_char_desc wechat_ind_char = ATT_CHAR(ATT_CHAR_PROP_IND,
                                                            0,
                                                            BLE_UUID_WECHAT_INDICATE_CHARACTERISTICS); 

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

struct wechat_env_tag wechat_env __attribute__((section("retention_mem_area0"),zero_init));

static const struct ke_task_desc TASK_DESC_WECHAT = {wechat_state_handler, &wechat_default_handler, wechat_state, WECHAT_STATE_MAX, WECHAT_IDX_MAX};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void wechat_init(void)
{
    // Reset Environment
    memset(&wechat_env, 0, sizeof(wechat_env));
    
    // Create WECHAT task
    ke_task_create(TASK_WECHAT, &TASK_DESC_WECHAT);

    ke_state_set(TASK_WECHAT, WECHAT_DISABLED);
}

void wechat_send_write_val(uint8_t* val, uint32_t length)
{
    // Allocate the write value indication message
   struct wechat_write_val_ind *ind = KE_MSG_ALLOC(WECHAT_WRITE_VAL_IND,
                                              wechat_env.con_info.appid, TASK_WECHAT,
                                              wechat_write_val_ind);
   // Fill in the parameter structure
   ind->conhdl = gapc_get_conhdl(wechat_env.con_info.conidx);
   memcpy(ind->value, val, length);
   ind->length = length;
   
   // Send the message
   ke_msg_send(ind);
}

void wechat_enable_indication(bool isEnable)
{
    // Allocate the enable indication request message
   struct wechat_enable_indication_req *ind = KE_MSG_ALLOC(WECHAT_ENABLE_IND_REQ,
                                              wechat_env.con_info.appid, TASK_WECHAT,
                                              wechat_enable_indication_req);
   // Fill in the parameter structure
   ind->conhdl = gapc_get_conhdl(wechat_env.con_info.conidx);
   ind->isEnable = isEnable;
   
   // Send the message
   ke_msg_send(ind);
}

void wechat_indication_cfm_send(uint8_t status)
{
	// allocate indication confirmation message to tell application layer the host has received the indication sent from device
    struct wechat_indication_cfm *cfm = KE_MSG_ALLOC(WECHAT_SEND_INDICATION_CFM,
                                                 wechat_env.con_info.appid, TASK_WECHAT,
                                                 wechat_indication_cfm);
    cfm->status = status;
    
    // Send the message
    ke_msg_send(cfm);
}

void wechat_disable(void)
{
    // Disable service in database
    attmdb_svc_set_permission(wechat_env.wechat_shdl, PERM_RIGHT_DISABLE);

    struct wechat_disable_ind *ind = KE_MSG_ALLOC(WECHAT_DISABLE_IND,
                                                 wechat_env.con_info.appid, TASK_WECHAT,
                                                 wechat_disable_ind);

    // Fill in the parameter structure
    ind->conhdl     = gapc_get_conhdl(wechat_env.con_info.conidx);

    // Send the message
    ke_msg_send(ind);

    // Go to idle state
    ke_state_set(TASK_WECHAT, WECHAT_IDLE);
}
#endif //BLE_WECHAT
