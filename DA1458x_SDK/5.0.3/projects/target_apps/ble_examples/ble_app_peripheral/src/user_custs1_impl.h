/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.h
 *
 * @brief Peripheral project Custom1 Server implementation header file.
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

#ifndef _USER_CUSTS1_IMPL_H_
#define _USER_CUSTS1_IMPL_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief
 *
 * @{
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

enum
{
    CUSTS1_CP_ADC_VAL1_DISABLE = 0,
    CUSTS1_CP_ADC_VAL1_ENABLE,
};

enum
{
    CUSTS1_LED_OFF = 0,
    CUSTS1_LED_ON,
};

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gapc_task.h"                 // gap functions and messages
#include "gapm_task.h"                 // gap functions and messages
#include "custs1_task.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Control point write indication handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_custs1_ctrl_wr_ind_handler(ke_msg_id_t const msgid,
                                      struct custs1_val_write_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Led state value write indication handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_custs1_led_wr_ind_handler(ke_msg_id_t const msgid,
                                     struct custs1_val_write_ind const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Long value configuration indication handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_custs1_long_val_cfg_ind_handler(ke_msg_id_t const msgid,
                                           struct custs1_val_write_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Long value write indication handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_custs1_long_val_wr_ind_handler(ke_msg_id_t const msgid,
                                          struct custs1_val_write_ind const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Long value notification confirmation handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_custs1_long_val_ntf_cfm_handler(ke_msg_id_t const msgid,
                                           struct custs1_val_write_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief ADC value 1 configuration indication handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_custs1_adc_val_1_cfg_ind_handler(ke_msg_id_t const msgid,
                                            struct custs1_val_write_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief ADC value 1 notification confirmation handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_custs1_adc_val_1_ntf_cfm_handler(ke_msg_id_t const msgid,
                                            struct custs1_val_write_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Button configuration indication handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_custs1_button_cfg_ind_handler(ke_msg_id_t const msgid,
                                         struct custs1_val_write_ind const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Button notification confirmation handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_custs1_button_ntf_cfm_handler(ke_msg_id_t const msgid,
                                         struct custs1_val_write_ind const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Indicateable character configuration indication handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_custs1_indicateable_cfg_ind_handler(ke_msg_id_t const msgid,
                                               struct custs1_val_write_ind const *param,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Indicateable character indicate confirmation handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_custs1_indicateable_ind_cfm_handler(ke_msg_id_t const msgid,
                                               struct custs1_val_write_ind const *param,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief ADC sampling timer callback handler.
 * @return void
 ****************************************************************************************
 */
void app_adcval1_timer_cb_handler(void);

/// @} APP

#endif // _USER_CUSTS1_IMPL_H_
