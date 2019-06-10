/**
 * @file nordic_ble_gattc_event_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Provide attachment and notifications for the ble::gattc::event_observer class
 * into the Nordic BLE observables.
 */

#include "ble/att.h"
#include "ble/nordic_ble_att.h"
#include "ble/nordic_ble_event_observer.h"
#include "ble/profile_connectable.h"
#include "ble/gattc_event_observer.h"
#include "ble/gattc_operations.h"
#include "logger.h"
#include "project_assert.h"
#include "nordic_error.h"

#include <ble_gatt.h>
#include <cmsis_gcc.h>

#include <cstring>

namespace nordic
{

struct uuid128_read_pending_t
{
    uint16_t connection_handle;
    uint16_t gattc_handle;
};

static uuid128_read_pending_t uuid128_read_pending = {
    .connection_handle  = ble::gap::handle_invalid,
    .gattc_handle       = ble::att::handle_invalid
};

/**
 * @todo This will work fine for single connection GATT clients.
 * The struct uuid128_read_pending_t instance needs to be included somewhere in
 * the ble::gattc::connectable heirarchy so that there is one instance per
 * connection. Otherwise 2 connections attempting to acquire 128-bit UUIDs are
 * going to collide.
 */
uint32_t gattc_uuid128_acquire(uint16_t connection_handle, uint16_t gatt_handle)
{
    logger& logger = logger::instance();

    logger.debug("gattc_uuid128_acquire(c: 0x%04x, h: 0x%04x)",
                 connection_handle, gatt_handle);

    ASSERT(uuid128_read_pending.connection_handle == ble::gap::handle_invalid);
    ASSERT(uuid128_read_pending.gattc_handle      == ble::att::handle_invalid);

    uuid128_read_pending.connection_handle = connection_handle;
    uuid128_read_pending.gattc_handle      = gatt_handle;

    uint32_t const error_code = sd_ble_gattc_read(
        uuid128_read_pending.connection_handle,
        uuid128_read_pending.gattc_handle,
        0u);

    return error_code;
}

void ble_gattc_event_notify(ble::gattc::event_observer&     observer,
                            enum BLE_GATTC_EVTS             event_type,
                            ble_gattc_evt_t const&          event_data)
{
    logger &logger = logger::instance();

    switch (event_type)
    {
    case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:
        // Read By UUID Response event.
        // See ble_gattc_evt_char_val_by_uuid_read_rsp_t.
        {
            ble_gattc_evt_char_val_by_uuid_read_rsp_t const &read_rsp =
                event_data.params.char_val_by_uuid_read_rsp;

            // Data is held in (handle, value) pairs. The length in bytes
            // of the value is value_length.
            uint32_t const  value_length = read_rsp.value_len;
            uint8_t  const* hv_pair_ptr  = read_rsp.handle_value;

            for (uint16_t iter = 0u;
                 iter < event_data.params.char_val_by_uuid_read_rsp.count; ++iter)
            {
                uint16_t handle = *hv_pair_ptr++;
                handle <<= 8u;
                handle |= *hv_pair_ptr++;
                handle = __REV16(handle);   // 16-bit endian swap.

                observer.read_characteristic_by_uuid_response(
                    event_data.conn_handle,
                    nordic::to_att_error_code(event_data.gatt_status),
                    event_data.error_handle,
                    handle,
                    &hv_pair_ptr,
                    value_length);

                hv_pair_ptr += value_length;
            }
        }
        break;

    case BLE_GATTC_EVT_READ_RSP:
        // Read Response event.
        // See ble_gattc_evt_read_rsp_t.
        if ((uuid128_read_pending.connection_handle == event_data.conn_handle) &&
            (uuid128_read_pending.gattc_handle      == event_data.params.read_rsp.handle))
        {
            ble_uuid128_t uuid_128;
            ASSERT(event_data.params.read_rsp.len >= sizeof(uuid_128.uuid128));
            memcpy(uuid_128.uuid128,
                   event_data.params.read_rsp.data,
                   sizeof(uuid_128.uuid128));

            uint8_t uuid_type = BLE_UUID_TYPE_VENDOR_BEGIN;
            uint32_t error_code = sd_ble_uuid_vs_add(&uuid_128, &uuid_type);

            if (error_code == NRF_SUCCESS)
            {
                uint16_t const connection_handle = uuid128_read_pending.connection_handle;
                uint16_t const gattc_handle      = uuid128_read_pending.gattc_handle;

                uuid128_read_pending.connection_handle = ble::gap::handle_invalid;
                uuid128_read_pending.gattc_handle      = ble::att::handle_invalid;

                /// @todo Hardcoded to primary services discovery.
                /// This needs to be made generic by adding a functor to the
                /// state storage struct uuid128_read_pending_t.
                error_code = sd_ble_gattc_primary_services_discover(
                    connection_handle, gattc_handle, nullptr);

                if (error_code != NRF_SUCCESS)
                {
                    logger::instance().error(
                        "sd_ble_gattc_primary_services_discover() failed: 0x%04x '%s'",
                        error_code, nordic_error_string(error_code));
                }
            }
            else
            {
                ASSERT(uuid128_read_pending.connection_handle == ble::gap::handle_invalid);
                ASSERT(uuid128_read_pending.gattc_handle      == ble::att::handle_invalid);

                ble::att::uuid uuid = nordic::to_att_uuid(uuid_128);
                char uuid_char_buffer[ble::att::uuid::conversion_length];
                uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

                logger::instance().error(
                    "sd_ble_uuid_vs_add(%s) failed: 0x%04x '%s'",
                    uuid_char_buffer, error_code, nordic_error_string(error_code));
            }
        }
        else
        {
            observer.read_response(
                event_data.conn_handle,
                nordic::to_att_error_code(event_data.gatt_status),
                event_data.error_handle,
                event_data.params.read_rsp.handle,
                event_data.params.read_rsp.data,
                event_data.params.read_rsp.offset,
                event_data.params.read_rsp.len);
        }
        break;

    case BLE_GATTC_EVT_CHAR_VALS_READ_RSP:
        // Read multiple Response event.
        // See ble_gattc_evt_char_vals_read_rsp_t.
        observer.read_multi_response(
            event_data.conn_handle,
            nordic::to_att_error_code(event_data.gatt_status),
            event_data.error_handle,
            event_data.params.char_vals_read_rsp.values,
            event_data.params.char_vals_read_rsp.len);
        break;

    case BLE_GATTC_EVT_WRITE_RSP:
        // Write Response event.
        // See ble_gattc_evt_write_rsp_t.
        observer.write_response(
            event_data.conn_handle,
            nordic::to_att_error_code(event_data.gatt_status),
            event_data.error_handle,
            nordic::to_att_write_op_code(event_data.params.write_rsp.write_op),
            event_data.params.write_rsp.handle,
            event_data.params.write_rsp.data,
            event_data.params.write_rsp.offset,
            event_data.params.write_rsp.len);
        break;

    case BLE_GATTC_EVT_HVX:
        // Handle Value Notification or Indication event.
        // Confirm indication with sd_ble_gattc_hv_confirm.
        // See ble_gattc_evt_hvx_t.
        switch (event_data.params.hvx.type)
        {
        case BLE_GATT_HVX_NOTIFICATION:
            observer.handle_notification(
                event_data.conn_handle,
                nordic::to_att_error_code(event_data.gatt_status),
                event_data.error_handle,
                event_data.params.hvx.handle,
                event_data.params.hvx.data,
                event_data.params.hvx.len);
            break;

        case BLE_GATT_HVX_INDICATION:
            observer.handle_indication(
                event_data.conn_handle,
                nordic::to_att_error_code(event_data.gatt_status),
                event_data.error_handle,
                event_data.params.hvx.handle,
                event_data.params.hvx.data,
                event_data.params.hvx.len);
            break;

        default:
            logger.error("BLE_GATTC_EVT_HVX: unknown type: %u",
                         event_data.params.hvx.type);
            break;
        }
        break;

    case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:
        // Exchange MTU Response event.
        // See ble_gattc_evt_exchange_mtu_rsp_t.
        observer.exchange_mtu_response(
            event_data.conn_handle,
            nordic::to_att_error_code(event_data.gatt_status),
            event_data.error_handle,
            event_data.params.exchange_mtu_rsp.server_rx_mtu);
        break;

    case BLE_GATTC_EVT_TIMEOUT:
        // Timeout event.
        // See ble_gattc_evt_timeout_t.
        observer.timeout(
            event_data.conn_handle,
            nordic::to_att_error_code(event_data.gatt_status),
            event_data.error_handle);
        break;

    case BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE:
        // Write without Response transmission complete.
        // See ble_gattc_evt_write_cmd_tx_complete_t.
        observer.write_command_tx_completed(
            event_data.conn_handle,
            nordic::to_att_error_code(event_data.gatt_status),
            event_data.error_handle,
            event_data.params.write_cmd_tx_complete.count);
        break;

    default:
        break;
    }
}

} // namespace nordic
