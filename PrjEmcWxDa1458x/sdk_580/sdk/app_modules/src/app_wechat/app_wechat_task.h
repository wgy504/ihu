/**
 ****************************************************************************************
 *
 * @file app_wechat_task.h
 *
* @brief Wechat task header. Wechat application task Handlers declaration.
*
* Copyright (C) 2012. Dialog Semiconductor Ltd, unpublished work. This computer 
* program includes Confidential, Proprietary Information and is a Trade Secret of 
* Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited 
* unless authorized in writing. All Rights Reserved.
*
* <bluetooth.support@diasemi.com> and contributors.
*
****************************************************************************************
*/

#ifndef APP_WECHAT_TASK_H_
#define APP_WECHAT_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup APPDISTASK Task
 * @ingroup APPDIS
 * @brief Wechat Application Task
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_WECHAT)

#include "wechat_task.h"
#include "ke_task.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles wechat profile database creation confirmation.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int app_wechat_create_db_cfm_handler(ke_msg_id_t const msgid,
                                        struct wechat_create_db_cfm const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Handles disable indication from the wechat profile.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int app_wechat_disable_ind_handler(ke_msg_id_t const msgid,
                                      struct wechat_disable_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id);

int app_wechat_send_indication_cfm_handler(ke_msg_id_t const msgid,
                                  struct wechat_indication_cfm const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id);
                                  
int app_wechat_enable_ind_handler(ke_msg_id_t const msgid,
                                  struct wechat_enable_indication_req const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id);                                      
/**
 ****************************************************************************************
 * @brief Handles write characteristic event indication from wechat profile
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int app_wechat_write_val_ind_handler(ke_msg_id_t const msgid,
                                  struct wechat_write_val_ind const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id);
                                  
int app_wechat_send_data_handler(ke_msg_id_t const msgid,
                                  struct wechat_send_data_req const *req,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id);
																	
int app_wechat_period_report_time_out_handler(ke_msg_id_t const msgid,
                                  struct wechat_period_report_time_out const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id);     
                                  

extern int arch_printf(const char *fmt, ...);
extern void mpbledemo2_airsync_link_setup_period_report(void);
extern int32_t mpbledemo2_sendData(uint8_t* ptrData, uint32_t lengthInByte);
																	
#endif //(BLE_WECHAT)

/// @} APPDISTASK

#endif //APP_DIS_TASK_H_
