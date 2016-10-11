/**
 ****************************************************************************************
 *
 * @file ble_service.h
 *
 * @brief Services handling routines API
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

#ifndef BLE_SERVICE_H_
#define BLE_SERVICE_H_

#include <stdbool.h>
#include "ble_gap.h"
#include "ble_gatts.h"

typedef struct ble_service ble_service_t;

typedef void (* connected_evt_t) (ble_service_t *svc, const ble_evt_gap_connected_t *evt);

typedef void (* disconnected_evt_t) (ble_service_t *svc, const ble_evt_gap_disconnected_t *evt);

typedef void (* read_req_t) (ble_service_t *svc, const ble_evt_gatts_read_req_t *evt);

typedef void (* write_req_t) (ble_service_t *svc, const ble_evt_gatts_write_req_t *evt);

typedef void (* prepare_write_req_t) (ble_service_t *svc,
                                                const ble_evt_gatts_prepare_write_req_t *evt);

typedef void (* event_sent_t) (ble_service_t *svc, const ble_evt_gatts_event_sent_t *evt);

typedef struct ble_service {
        uint16_t        start_h;
        uint16_t        end_h;

        connected_evt_t     connected_evt;
        disconnected_evt_t  disconnected_evt;
        read_req_t          read_req;
        write_req_t         write_req;
        prepare_write_req_t prepare_write_req;
        event_sent_t        event_sent;
} ble_service_t;

typedef struct {
        gatt_service_t service_type;
        gap_sec_level_t sec_level;

        uint8_t         num_includes;
        ble_service_t   **includes;
} ble_service_config_t;

void ble_service_add(ble_service_t *svc);

bool ble_service_handle_event(const ble_evt_hdr_t *evt);

att_perm_t ble_service_config_elevate_perm(att_perm_t perm, const ble_service_config_t *config);

static inline uint8_t ble_service_get_num_attr(const ble_service_config_t *config, uint16_t chars,
                                                                                uint16_t descs)
{
        return ble_gatts_get_num_attr(config ? config->num_includes : 0, chars, descs);
}

void ble_service_config_add_includes(const ble_service_config_t *config);

#endif /* BLE_SERVICE_H_ */
