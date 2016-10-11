/**
 \addtogroup INTERFACES
 \{
 \addtogroup BLE
 \{
 \addtogroup API
 \{
 */

/**
 ****************************************************************************************
 *
 * @file ble_gattc_util.h
 *
 * @brief BLE GATT Client Utilities API
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

#ifndef BLE_GATTC_UTIL_H_
#define BLE_GATTC_UTIL_H_

#include "ble_gattc.h"
#include "ble_uuid.h"

/**
 * Initialize browse event iterators
 *
 * This call will initialize internal structures to iterate over ::BLE_EVT_GATTC_BROWSE_SVC. After
 * this call application can use ble_gattc_util_find_characteristic() to get characteristic from
 * event. \p evt instance should be valid as long as iterator is used since only weak reference is
 * stored internally.
 *
 * \param [in] evt  instance of event
 *
 */
void ble_gattc_util_find_init(const ble_evt_gattc_browse_svc_t *evt);

/**
 * Find characteristic in browse event
 *
 * This call will return first characteristic from event which matches given UUID (in case \p uuid
 * is \p NULL, first characteristic is returned). On subsequent calls with the same \p uuid given
 * it will return subsequent characteristics matching the same criteria or \p NULL if no more
 * matching characteristics are found.
 *
 * If ble_gattc_util_find_init() was not called prior to calling this function, its behavior is
 * undefined.
 *
 * Subsequent calls with different \p uuid will restart searching from first characteristic.
 *
 * Returned item will always have ::GATTC_ITEM_TYPE_CHARACTERISTIC type.
 *
 * \param [in] uuid  optional UUID of characteristic
 *
 * \return found item
 *
 * \sa ble_gattc_util_find_init
 *
 */
const gattc_item_t *ble_gattc_util_find_characteristic(const att_uuid_t *uuid);

/**
 * Find descriptor in browse event
 *
 * This call will return first descriptor from event which matches given UUID (in case \p uuid
 * is \p NULL, first descriptor is returned). On subsequent calls with the same \p uuid given
 * it will return subsequent descriptors matching the same criteria or \p NULL if no more
 * matching descriptors are found.
 *
 * Only descriptors for chracteristic previously found using ble_gattc_util_find_characteristic()
 * are returned. If characteristic was not found prior to calling this function, its behavior is
 * undefined.
 *
 * Subsequent calls with different \p uuid will restart searching from first descriptor.
 *
 * Returned item will always have ::GATTC_ITEM_TYPE_DESCRIPTOR type.
 *
 * \param [in] uuid  optional UUID of descriptor
 *
 * \return found item
 *
 * \sa ble_gattc_util_find_characteristic
 *
 */
const gattc_item_t *ble_gattc_util_find_descriptor(const att_uuid_t *uuid);

/**
 * Write value to CCC descriptor
 *
 * This function writes CCC value to given handle.
 *
 * \param [in]  conn_idx        connection index
 * \param [in]  handle          CCC descriptor handle
 * \param [in]  ccc             value to be written
 *
 * \return status of GATT write operation
 */

static inline ble_error_t ble_gattc_util_write_ccc(uint16_t conn_idx, uint16_t handle,
                                                                                gatt_ccc_t ccc)
{
        uint16_t value = ccc;

        return ble_gattc_write(conn_idx, handle, 0, sizeof(value), (uint8_t *) &value);
}

#endif /* BLE_GATTC_UTIL_H_ */
/**
 \}
 \}
 \}
 */
