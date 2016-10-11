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
 * @file ble_gatts.h
 *
 * @brief BLE GATT Server API
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

#ifndef BLE_GATTS_H_
#define BLE_GATTS_H_

#include <stdint.h>
#include "ble_att.h"
#include "ble_common.h"
#include "ble_gatt.h"

/** GATT Server flags */
typedef enum {
        GATTS_FLAG_CHAR_READ_REQ = 0x01,        ///< enable ::BLE_EVT_GATTS_READ_REQ for attribute
} gatts_flag_t;

/** GATT Server events */
enum ble_evt_gatts {
        /** Read request from peer */
        BLE_EVT_GATTS_READ_REQ = BLE_EVT_CAT_FIRST(BLE_EVT_CAT_GATTS),
        /** Write request from peer */
        BLE_EVT_GATTS_WRITE_REQ,
        /** Prepare write request from peer */
        BLE_EVT_GATTS_PREPARE_WRITE_REQ,
        /** Event (notification or indication) sent */
        BLE_EVT_GATTS_EVENT_SENT,
};

/** Structure for ::BLE_EVT_GATTS_READ_REQ */
typedef struct {
        ble_evt_hdr_t   hdr;            ///< event header
        uint16_t        conn_idx;       ///< connection index
        uint16_t        handle;         ///< attribute handle
        uint16_t        offset;         ///< attribute value offset
} ble_evt_gatts_read_req_t;

/** Structure for ::BLE_EVT_GATTS_WRITE_REQ */
typedef struct {
        ble_evt_hdr_t   hdr;            ///< event header
        uint16_t        conn_idx;       ///< connection index
        uint16_t        handle;         ///< attribute handle
        uint16_t        offset;         ///< attribute value offset
        uint16_t        length;         ///< attribute value length
        uint8_t         value[];        ///< attribute value
} ble_evt_gatts_write_req_t;

/** Structure for ::BLE_EVT_GATTS_PREPARE_WRITE_REQ */
typedef struct {
        ble_evt_hdr_t   hdr;            ///< event header
        uint16_t        conn_idx;       ///< connection index
        uint16_t        handle;         ///< attribute handle
} ble_evt_gatts_prepare_write_req_t;

/** Structure for ::BLE_EVT_GATTS_EVENT_SENT */
typedef struct {
        ble_evt_hdr_t   hdr;            ///< event header
        uint16_t        conn_idx;       ///< connection index
        uint16_t        handle;         ///< attribute handle
        gatt_event_t    type;           ///< event type
        bool            status;         ///< event status
} ble_evt_gatts_event_sent_t;

/**
 * Add new GATT service
 *
 * This begins adding new service to ATT database. Subsequent calls to ble_gatts_add_include(),
 * ble_gatts_add_characteristic() and ble_gatts_add_descriptor() will add attributes to service
 * added by this call. All of these calls should be in the same order as attributes should be
 * added to database.
 *
 * Service is added to database as is enabled once ble_gatts_register_service()
 * is called after all other attributes were added.
 *
 * \param [in]  uuid       service UUID
 * \param [in]  type       service type
 * \param [in]  num_attrs  number of attributes to be added
 *
 * \return result code
 *
 * \sa ble_gatts_add_include
 * \sa ble_gatts_add_characteristic
 * \sa ble_gatts_add_descriptor
 * \sa ble_gatts_register_service
 * \sa ble_gatts_get_num_attr
 *
 */
ble_error_t ble_gatts_add_service(const att_uuid_t *uuid, const gatt_service_t type, uint16_t num_attrs);

/**
 * Add included service to GATT service
 *
 * This adds included service declaration to service started by ble_gatts_add_service().
 *
 * \param [in]  handle    included service handle
 * \param [out] h_offset  attribute offset (relative to service handle)
 *
 * \return result code
 *
 */
ble_error_t ble_gatts_add_include(uint16_t handle, uint16_t *h_offset);

/**
 * Add characteristic to GATT service
 *
 * This adds characteristic declaration to service started by ble_gatts_add_service().
 *
 * Application will receive ::BLE_EVT_GATTS_WRITE_REQ event every time value attribute is written by
 * peer. In order for application to received also ::BLE_EVT_GATTS_READ_REQ event every time value
 * attribute is read by peer, \p flags shall be set to ::GATTS_FLAG_CHAR_READ_REQ.
 *
 * \note
 * If Extended Characteristic Properties Descriptor will be added to this characteristic,
 * extended properties shall be added to \p prop. It will be used later to set correct
 * descriptor's value
 *
 * \param [in]  uuid          characteristic UUID
 * \param [in]  prop          characteristic properties
 * \param [in]  perm          characteristic value attribute permissions
 * \param [in]  max_len       maximum length of characteristic value
 * \param [in]  flags         additional settings for characteristic
 * \param [out] h_offset      attribute offset (relative to service handle)
 * \param [out] h_val_offset  value attribute offset (relative to service handle)
 *
 * \return result code
 *
 */
ble_error_t ble_gatts_add_characteristic(const att_uuid_t *uuid, gatt_prop_t prop, att_perm_t perm,
                                                        uint16_t max_len, gatts_flag_t flags,
                                                        uint16_t *h_offset, uint16_t *h_val_offset);

/**
 * Add descriptor to GATT service
 *
 * This adds characteristic declaration to service started by ble_gatts_add_service().
 *
 * Application will receive ::BLE_EVT_GATTS_WRITE_REQ event every time attribute is written by peer.
 * In order for application to received also ::BLE_EVT_GATTS_READ_REQ event every time attribute is
 * read by peer, \p flags shall be set to ::GATTS_FLAG_CHAR_READ_REQ.
 *
 * \note
 * for some descriptor ::BLE_EVT_GATTS_READ_REQ will be sent regardless of \p flags value, this
 * applies e.g. to Client Characteristic Configuration.
 *
 * \note
 * In case of Extended Characteristic Properties Descriptor, descriptor's value will be updated
 * based on props passed to ble_gatts_add_characteristic.
 *
 * \param [in]  uuid          descriptor UUID
 * \param [in]  perm          descriptor attribute permissions
 * \param [in]  max_len       maxumum length of descriptor value
 * \param [in]  flags         additional settings for descriptor
 * \param [out] h_offset      attribute offset (relative to service handle)
 *
 * \return result code
 *
 */
ble_error_t ble_gatts_add_descriptor(const att_uuid_t *uuid, att_perm_t perm, uint16_t max_len,
                                                        gatts_flag_t flags, uint16_t *h_offset);

/**
 * Register service in database
 *
 * This adds to ATT database all attributes previously added to service.
 *
 * \p handle can be used to calculate actual handle values for offset values returned in \p h_offset
 * and \p h_val_offset parameters. A list of pointers to offset values can be specified as arguments
 * in order for this call to update this automatically.
 *
 * \param [out] handle  service handle
 * \param [out] ...     attributes offsets to be translated (shall end with 0)
 *
 * \return result code
 *
 */
ble_error_t ble_gatts_register_service(uint16_t *handle, ...);

/**
 * Enable service in database
 *
 * This makes service visible to clients. Once registered, services are always enabled by default.
 * Since this call may change database structure, application shall ensure that Service Changed
 * characteristic is supported in database and indicated if necessary.
 *
 * \param [in] handle   service start handle
 *
 * \return result code
 *
 * \sa ble_gatts_service_changed_ind
 *
 */
ble_error_t ble_gatts_enable_service(uint16_t handle);

/**
 * Disable service in database
 *
 * This makes service invisible to clients. Once registered, services are always enabled by default.
 * Since this call may change database structure, application shall ensure that Service Changed
 * characteristic is supported in database and indicated if necessary.
 *
 * \param [in] handle   service start handle
 *
 * \return result code
 *
 * \sa ble_gatts_service_changed_ind
 *
 */
ble_error_t ble_gatts_disable_service(uint16_t handle);

/**
 * Read current characteristic properties and permissions
 *
 * \param [in]  handle  characteristic handle
 * \param [out] prop    characteristic properties
 * \param [out] perm    characteristic permissions
 *
 * \return result code
 *
 * \sa ble_gatts_set_characteristic_prop
 *
 */
ble_error_t ble_gatts_get_characteristic_prop(uint16_t handle, gatt_prop_t *prop, att_perm_t *perm);

/**
 * Set characteristic properties and permissions
 *
 * As mandated by Core Specification version 4.1, characteristic declaration shall not change while
 * bond with any device exists on the server. Application shall ensure it does not use this API
 * to change characteristic properties while such bond exists.
 *
 * \param [in] handle   characteristic handle
 * \param [in] prop     characteristic properties
 * \param [in] perm     characteristic permissions
 *
 * \return result code
 *
 * \sa ble_gatts_get_characteristic_prop
 *
 */
ble_error_t ble_gatts_set_characteristic_prop(uint16_t handle, gatt_prop_t prop, att_perm_t perm);

/**
 * Get attribute value
 *
 * This retrieves attribute value from database. Up to \p size bytes are copied to \p val buffer.
 * Value returned in \p size is total attribute value length and may be larger than supplied size.
 * This information can be used by application to allocate larger buffer to fit complete value data.
 *
 * \param [in]     handle attribute handle
 * \param [in,out] length input buffer size or attribute value length
 * \param [out]    value  buffer to store attribute value
 *
 * \return result code
 *
 */
ble_error_t ble_gatts_get_value(uint16_t handle, uint16_t *length, void *value);

/**
 * Set attribute value
 *
 * This sets attribute value in internal database. Any read request from any peer will have this
 * value returned. In order for attribute to have different values for each peer, application should
 * store them locally and use ::BLE_EVT_GATTS_READ_REQ to handle read requests for given attribute.
 *
 * \param [in] handle attribute handle
 * \param [in] length attribute value length
 * \param [in] value  attribute value
 *
 * \return result code
 *
 */
ble_error_t ble_gatts_set_value(uint16_t handle, uint16_t length, const void *value);

/**
 * Send response for ::BLE_EVT_GATTS_READ_REQ
 *
 * This should be called in response to ::BLE_EVT_GATTS_READ_REQ event.
 *
 * \param [in] conn_idx connection index
 * \param [in] handle   attribute handle
 * \param [in] status   operation status
 * \param [in] length   attribute value length
 * \param [in] value    attribute value
 *
 * \return result code
 *
 */
ble_error_t ble_gatts_read_cfm(uint16_t conn_idx, uint16_t handle, att_error_t status, uint16_t length, const void *value);


/**
 * Send response for ::BLE_EVT_GATTS_WRITE_REQ
 *
 * This should be called in response to ::BLE_EVT_GATTS_WRITE_REQ event.
 *
 * \note
 * Application shall also use this to confirm write requests for characteristics with "Write Without
 * Response" property set.
 *
 * \param [in] conn_idx connection index
 * \param [in] handle   attribute handle
 * \param [in] status   operation status
 *
 * \return result code
 *
 */
ble_error_t ble_gatts_write_cfm(uint16_t conn_idx, uint16_t handle, att_error_t status);

/**
 * Send response for ::BLE_EVT_GATTS_PREPARE_WRITE_REQ
 *
 * This should be called in response to ::BLE_EVT_GATTS_PREPARE_WRITE_REQ event.
 *
 * \param [in] conn_idx connection index
 * \param [in] handle   attribute handle
 * \param [in] length   attribute value length
 * \param [in] status   operation status
 *
 * \return result code
 *
 */
ble_error_t ble_gatts_prepare_write_cfm(uint16_t conn_idx, uint16_t handle, uint16_t length, att_error_t status);

/**
 * Send characteristic value notification or indication
 *
 * Send indication or notification to connected peer.
 *
 * Application will receive ::BLE_EVT_GATTS_EVENT_SENT when the notification is successfully sent
 * over the air.
 *
 * \note
 * Characteristic must declare GATT_PROP_NOTIFY or GATT_PROP_INDICATE property.
 *
 * \note
 * If a disconnection happens after calling this function or the notification cannot be successfully
 * sent over the air due to e.g. a bad connection, ::BLE_EVT_GATTS_EVENT_SENT will not be received.
 *
 * \param [in] conn_idx connection index
 * \param [in] handle   characteristic value handle
 * \param [in] type     indication or notification
 * \param [in] length   characteristic value length
 * \param [in] value    characteristic value
 *
 * \return result code
 *
 */
ble_error_t ble_gatts_send_event(uint16_t conn_idx, uint16_t handle, gatt_event_t type,
                                                                uint16_t length, const void *value);

/**
 * Send indication of Service Changed Characteristic indication to remote
 *
 * This should be called only if Service Changed Characteristic is enabled in configuration.
 * Make sure bit 0x20 is set in #defaultBLE_ATT_DB_CONFIGURATION.
 *
 * \param [in] conn_idx         connection index
 * \param [in] start_handle     start handle of affected database region
 * \param [in] end_handle       end handle of affected database region
 *
 * \return result code
 *
 */
ble_error_t ble_gatts_service_changed_ind(uint16_t conn_idx, uint16_t start_handle,
                                                                        uint16_t end_handle);

/**
 * Calculate number of attributes required for service
 *
 * \param [in] include         number of included services
 * \param [in] characteristic  number of characteristics
 * \param [in] descriptor      number of descriptors
 *
 * \return number of attributes required
 *
 */
static inline uint16_t ble_gatts_get_num_attr(uint16_t include, uint16_t characteristic,
                                                                                uint16_t descriptor)
{
        return 1 * include + 2 * characteristic + 1 * descriptor;
}

#endif /* BLE_GATTS_H_ */
/**
 \}
 \}
 \}
 */
