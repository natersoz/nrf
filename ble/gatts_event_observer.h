/**
 * @file ble/gatts_event_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Generic GATTS BLE event observer interface.
 */

#pragma once

#include <cstdint>
#include "ble/att.h"

namespace ble
{
namespace gatts
{

class event_observer
{
public:
    virtual ~event_observer()                        = default;

    event_observer()                                 = default;
    event_observer(event_observer const&)            = delete;
    event_observer(event_observer &&)                = delete;
    event_observer& operator=(event_observer const&) = delete;
    event_observer& operator=(event_observer&&)      = delete;

    /*
       BLE_GATTS_OPS:
       BLE_GATTS_OP_INVALID                0x00  Invalid Operation.
       BLE_GATTS_OP_WRITE_REQ              0x01  Write Request.
       BLE_GATTS_OP_WRITE_CMD              0x02  Write Command.
       BLE_GATTS_OP_SIGN_WRITE_CMD         0x03  Signed Write Command.
       BLE_GATTS_OP_PREP_WRITE_REQ         0x04  Prepare Write Request.
       BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL  0x05  Execute Write Request: Cancel all prepared writes.
       BLE_GATTS_OP_EXEC_WRITE_REQ_NOW     0x06  Execute Write Request: Immediately execute all prepared writes.
    */
    virtual void write(
        uint16_t            conection_handle,
        uint16_t            attribute_handle,
        uint8_t             write_type,     /// @todo @see BLE_GATTS_OPS below and fix
        bool                authorization_required,
        att::length_t       offset,
        att::length_t       length,
        void const*         data            /// @todo @ref sd_ble_evt_get for more information
                                            /// on how to use event structures with variable
                                            /// length array members.
        ) {} // BLE_GATTS_EVT_WRITE

    virtual void read_authorization_request(
        uint16_t            conection_handle,
        uint16_t            attribute_handle,
        att::length_t       offset
        ) {} // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

    virtual void write_authorization_request(
        uint16_t            conection_handle,
        uint16_t            attribute_handle,
        uint8_t             write_type,     /// @todo @see BLE_GATTS_OPS below and fix
        bool                authorization_required,
        att::length_t       offset,
        att::length_t       length,
        void const*         data            /// @todo @ref sd_ble_evt_get for more information
                                            /// on how to use event structures with variable
                                            /// length array members.
        ) {} // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

    virtual void system_attribute_missing(
        uint16_t            conection_handle,
        uint8_t             hint
        );   // BLE_GATTS_EVT_SYS_ATTR_MISSING

    virtual void service_change_confirmation(
        uint16_t            conection_handle
        ) {} // BLE_GATTS_EVT_SC_CONFIRM

    virtual void handle_value_confirmation(
        uint16_t            conection_handle,
        uint16_t            attribute_handle
        ) {} // BLE_GATTS_EVT_HVC

    virtual void mtu_rx_size(
        uint16_t            conection_handle,
        uint16_t            client_rx_mtu_size
        ) {} // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST

    virtual void timeout(
        uint16_t            conection_handle,
        uint8_t             timeout_source      // always BLE_GATT_TIMEOUT_SRC_PROTOCOL (0)
        ) {} // BLE_GATTS_EVT_TIMEOUT

    virtual void handle_value_notifications_tx_completed(
        uint16_t            conection_handle,
        uint8_t             count
        ) {} // BLE_GATTS_EVT_HVN_TX_COMPLETE
};

} // namespace gatts
} // namespace ble

