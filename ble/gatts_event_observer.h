/**
 * @file ble/gatts_event_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Generic GATTS BLE event observer interface.
 */

#pragma once

#include <cstdint>
#include "ble/att.h"
#include "ble/profile_connectable_accessor.h"

namespace ble
{
namespace gatts
{

/**
 * @class ble::gatts::event_observer
 * The Generic Attribute (GATT) Server observer.
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

    virtual void write(
        uint16_t            connection_handle,
        uint16_t            attribute_handle,
        att::op_code        write_operation_type,
        bool                authorization_required,
        att::length_t       offset,
        att::length_t       length,
        void const*         data);

    virtual void write_cancel(
        uint16_t            connection_handle,
        uint16_t            attribute_handle,
        att::op_code        write_operation_type,
        bool                authorization_required,
        att::length_t       offset,
        att::length_t       length,
        void const*         data);

    virtual void read_authorization_request(
        uint16_t            connection_handle,
        uint16_t            attribute_handle,
        att::length_t       offset);

    virtual void write_authorization_request(
        uint16_t            connection_handle,
        uint16_t            attribute_handle,
        att::op_code        write_operation_type,
        bool                authorization_required,
        att::length_t       offset,
        att::length_t       length,
        void const*         data);

    virtual void service_change_confirmation(
        uint16_t            connection_handle);

    virtual void handle_value_confirmation(
        uint16_t            connection_handle,
        uint16_t            attribute_handle);

    /**
     * BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part F
     * Section 3.4.2.1 Exchange MTU Request
     * Table 3.4: Format of Exchange MTU Request
     */
    virtual void exchange_mtu_request(
        uint16_t            connection_handle,
        att::length_t       client_rx_mtu_size);

    virtual void timeout(
        uint16_t            connection_handle,
        uint8_t             timeout_source);

    virtual void handle_value_notifications_tx_completed(
        uint16_t            connection_handle,
        uint8_t             count);
};

} // namespace gatts
} // namespace ble

