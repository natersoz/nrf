/**
 * @file ble/gattc_event_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/att.h"
#include "ble/uuid.h"
#include "ble/gatt_declaration.h"
#include "ble/profile_connectable_accessor.h"

namespace ble
{
namespace gattc
{

/**
 * @class ble::gattc::event_observer
 * The Generic Attribute (GATT) Client observer.
 */
class event_observer: public ble::profile::connectable_accessor
{
public:
    virtual ~event_observer()                        = default;

    event_observer()                                 = default;
    event_observer(event_observer const&)            = delete;
    event_observer(event_observer &&)                = delete;
    event_observer& operator=(event_observer const&) = delete;
    event_observer& operator=(event_observer&&)      = delete;

    virtual void service_discovered(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    handle_start,
        uint16_t                    handle_stop,
        ble::att::uuid const&       uuid
        ) {}

    virtual void relationship_discovered(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    handle_start,
        uint16_t                    handle_stop,
        uint16_t                    service_handle,
        ble::att::uuid const&       uuid
        ) {}

    virtual void characteristic_discovered(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    handle_decl,
        uint16_t                    handle_value,
        ble::att::uuid const&       uuid,
        ble::gatt::properties       properties
        ) {}

    virtual void descriptor_discovered(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    desciptor_handle,
        ble::att::uuid const&       uuid
        ) {}

    virtual void attribute_discovered(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    handle,
        ble::att::uuid const&       uuid
        ) {}

    virtual void read_characteristic_by_uuid_response(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    characteristic_handle,
        void const*                 data,
        ble::att::length_t          length
        ) {}

    virtual void read_response(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    attribute_handle,
        void const*                 data,
        ble::att::length_t          offset,
        ble::att::length_t          length
        ) {}

    virtual void read_multi_response(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        void const*                 data,
        ble::att::length_t          length
        ) {}

    virtual void write_response(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        ble::att::op_code           write_op_code,
        uint16_t                    attribute_handle,
        void const*                 data,
        ble::att::length_t          offset,
        ble::att::length_t          length
        ) {}

    virtual void handle_notification(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    attribute_handle,
        void const*                 data,
        ble::att::length_t          length
        ) {}

    virtual void handle_indication(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    attribute_handle,
        void const*                 data,
        ble::att::length_t          length
        ) {}

    virtual void exchange_mtu_response(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    server_rx_mtu_size
        ) {}

    virtual void timeout(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle
        ) {}

    virtual void write_command_tx_completed(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint8_t                     count
        ) {}
};

} // namespace gattc
} // namespace ble

