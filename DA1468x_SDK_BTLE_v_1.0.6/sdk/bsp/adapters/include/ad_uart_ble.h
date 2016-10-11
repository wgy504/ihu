/**
 * \addtogroup BSP
 * \{
 * \addtogroup ADAPTERS
 * \{
 * \addtogroup UART_BLE_ADAPTER
 *
 * \brief UART adapter for BLE
 *
 * \{
 */

/**
 ****************************************************************************************
 * @file ad_uart_ble.h
 *
 * @brief UART adapter for BLE API
 *
 * Copyright (C) 2015. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd. All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 * <black.orca.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */

#ifndef AD_UART_BLE_H
#define AD_UART_BLE_H

#if dg_configUART_BLE_ADAPTER

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "ad_ble.h"
#include "ble_mgr.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/** 
 * \brief Enumeration of External Interface status codes 
 */
enum ble_eif_status
{
    BLE_EIF_STATUS_OK,          /**< EIF status OK */
    BLE_EIF_STATUS_ERROR,       /**< EIF status NOK */

#if (BLE_EMB_PRESENT == 0)
    BLE_EIF_STATUS_DETACHED,    /**< External interface detached */
    BLE_EIF_STATUS_ATTACHED,    /**< External interface attached */
#endif // (BLE_EMB_PRESENT == 0)
};

/** 
 * \brief BLE states for RX 
 */
enum BLE_EIF_RX_STATES
{
        S_EIF_RX_START,         /**< GTL RX Start State - receive message type */
        S_EIF_RX_HEADER,        /**< GTL RX Header State - receive message header */
        S_EIF_RX_PAYLOAD,       /**< GTL RX Header State - receive (rest of) message payload */
        S_EIF_RX_OUT_OF_SYNC,   /**< GTL RX Out Of Sync state - receive sync pattern */
};

/** 
 * \brief GTL message header format 
 */
typedef struct ble_gtl_msg_hdr {
        uint16_t   msg_id;
        uint16_t   dest_id;
        uint16_t   src_id;
        uint16_t   param_len;
} ble_gtl_msg_hdr_t;

/** 
 * \brief GTL message format 
 */
typedef struct gtl_msg {
        uint16_t   msg_id;
        uint16_t   dest_id;
        uint16_t   src_id;
        uint16_t   param_len;
        uint32_t    param[0];
} gtl_msg_t;

/**
 *\brief Function called when packet transmission is finished.
 */
typedef void (*eif_tx_callback) (const uint8_t *data, uint16_t written);

/**
 *\brief Function called when packet reception is finished.
 */
typedef void (*eif_rx_callback) (uint8_t *data, uint16_t read);

/**
 * \brief BLE external interface out of synchronization recovery variables
 */
struct ble_eif_out_of_sync_tag
{
    uint8_t byte;               /**< Current received byte */
    uint8_t index;              /**< Index of the sync pattern */
};

/**
 * \brief BLE external interface environment context structure
 */
struct ble_eif_env_tag
{
        const struct eif_api* ext_if;                   /**< Pointer to External interface api */
        irb_ble_stack_msg_t *p_msg_rx;                  /**< Ongoing RX message */
        irb_ble_stack_msg_t *p_msg_tx;                  /**< Ongoing TX message */
        struct ble_eif_out_of_sync_tag out_of_sync;     /**< GTL synchronization error parameters */
        uint8_t  curr_hdr_buff[8];                      /**< Latest received message header, 8 bytes 
                                                             buffer */
        uint8_t  rx_state;                              /**< Rx state - can be receiving message type, 
                                                             header, payload or error */
        uint8_t  curr_msg_type;                         /**< Latest received message type: KE/.... */
};

/**
 * \brief Initialize UART adapter for BLE.
 */
void ad_uart_ble_init(void);

/**
 * \brief Notify UART BLE adapter task
 */
void ad_uart_ble_notify(void);

#endif  /* dg_configUART_BLE_ADAPTER */

#endif /* AD_UART_BLE_H */
/**
 \}
 \}
 \}
 */
