/**
 * @file ble_peripheral/ble_gattc_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gattc_event_observer.h"

class ble_gattc_observer: public ble::gattc::event_observer
{
private:
    using super = ble::gattc::event_observer;

public:
    virtual ~ble_gattc_observer() override;

    ble_gattc_observer(ble_gattc_observer const&)               = delete;
    ble_gattc_observer(ble_gattc_observer &&)                   = delete;
    ble_gattc_observer& operator=(ble_gattc_observer const&)    = delete;
    ble_gattc_observer& operator=(ble_gattc_observer&&)         = delete;

    ble_gattc_observer();

protected:
    virtual void read_characteristic_by_uuid_response(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    characteristic_handle,
        void const*                 data,
        ble::att::length_t          length) override;

    virtual void read_response(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    attribute_handle,
        void const*                 data,
        ble::att::length_t          offset,
        ble::att::length_t          length) override;

    virtual void read_multi_response(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        void const*                 data,
        ble::att::length_t          length) override;

    virtual void write_response(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        ble::att::op_code           write_op_code,
        uint16_t                    attribute_handle,
        void const*                 data,
        ble::att::length_t          offset,
        ble::att::length_t          length) override;

    virtual void handle_notification(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    attribute_handle,
        void const*                 data,
        ble::att::length_t          length) override;

    virtual void handle_indication(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    attribute_handle,
        void const*                 data,
        ble::att::length_t          length) override;

    virtual void exchange_mtu_response(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint16_t                    server_rx_mtu_size) override;

    virtual void timeout(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle) override;

    virtual void write_command_tx_completed(
        uint16_t                    conection_handle,
        ble::att::error_code        error_code,
        uint16_t                    error_handle,
        uint8_t                     count) override;
};
