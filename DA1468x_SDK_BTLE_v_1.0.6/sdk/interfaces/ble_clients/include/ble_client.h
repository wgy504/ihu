/**
 ****************************************************************************************
 *
 * @file ble_client.h
 *
 * @brief GATT Client handling routines API
 *
 * Copyright (C) 2015. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 * <black.orca.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */

#ifndef BLE_CLIENT_H_
#define BLE_CLIENT_H_

#include <stdbool.h>
#include "ble_gattc.h"
#include "ble_gap.h"

typedef struct ble_client ble_client_t;

typedef void (* read_completed_evt_t) (ble_client_t *client,
                                                        const ble_evt_gattc_read_completed_t *evt);

typedef void (* write_completed_evt_t) (ble_client_t *client,
                                                const ble_evt_gattc_write_completed_t *evt);

typedef void (* notification_evt_t) (ble_client_t *client,
                                                        const ble_evt_gattc_notification_t *evt);

typedef void (* indication_evt_t) (ble_client_t *client, const ble_evt_gattc_indication_t *evt);

typedef void (* disconnect_evt_t) (ble_client_t *client, const ble_evt_gap_disconnected_t *evt);

typedef void (* attach_cb_t) (ble_client_t *client);

typedef void (* cleanup_cb_t) (ble_client_t *client);

typedef struct ble_client {
        uint16_t conn_idx;

        read_completed_evt_t    read_completed_evt;
        write_completed_evt_t   write_completed_evt;
        notification_evt_t      notification_evt;
        indication_evt_t        indication_evt;
        disconnect_evt_t        disconnected_evt;

        attach_cb_t             attach;
        cleanup_cb_t            cleanup;
} ble_client_t;

void ble_client_add(ble_client_t *client);

void ble_client_remove(ble_client_t *client);

void ble_client_cleanup(ble_client_t *client);

void ble_client_attach(ble_client_t *client, uint16_t conn_idx);

void ble_client_handle_event(const ble_evt_hdr_t *evt);

#endif /* BLE_CLIENT_H_ */
