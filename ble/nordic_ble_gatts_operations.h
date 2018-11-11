/**
 * @file ble/nordic_ble_gatts_operations.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Implement GATTS BLE commands, response, indication, notification methods
 * using the Nordic Softdevice.
 */

#pragma once

#include "ble/gatts_operations.h"

namespace nordic
{

class ble_gatts_operations: public ble::gatts::operations
{
public:
    virtual ~ble_gatts_operations()                                 = default;

    ble_gatts_operations()                                          = default;
    ble_gatts_operations(ble_gatts_operations const&)               = delete;
    ble_gatts_operations(ble_gatts_operations &&)                   = delete;
    ble_gatts_operations& operator=(ble_gatts_operations const&)    = delete;
    ble_gatts_operations& operator=(ble_gatts_operations&&)         = delete;

    /**
     * Nordic specific:
     * BLE_GATTS_EVT_HVN_TX_COMPLETE    Transmission complete.
     * BLE_GATTS_EVT_HVC                Notification received from peer.
     */
    virtual ble::att::length_t notify(
        uint16_t                connection_handle,
        uint16_t                attribute_handle,
        ble::att::length_t      offset,
        ble::att::length_t      length,
        void const*             data) override;

    virtual ble::att::length_t indicate(
        uint16_t                connection_handle,
        uint16_t                attribute_handle,
        ble::att::length_t      offset,
        ble::att::length_t      length,
        void const*             data) override;

    virtual ble::gatts::error_code read_authorize_reply(
        uint16_t                connection_handle,
        uint16_t                attribute_handle,
        ble::att::error_code    error_code,
        bool                    atttribute_update,
        ble::att::length_t      offset,
        ble::att::length_t      length,
        void const*             data) override;

    virtual ble::gatts::error_code write_authorize_reply(
        uint16_t                connection_handle,
        uint16_t                attribute_handle,
        ble::att::error_code    error_code,
        bool                    atttribute_update,
        ble::att::length_t      offset,
        ble::att::length_t      length,
        void const*             data) override;

    virtual ble::gatts::error_code exchange_mtu_reply(
        uint16_t                connection_handle,
        ble::att::length_t      att_mtu_length) override;

    virtual ble::gatts::error_code service_add(
        ble::gatt::service& service) override;
};

} // namespace nordic

