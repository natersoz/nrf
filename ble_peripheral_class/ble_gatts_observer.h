/**
 * @file ble_peripheral_class/ble_gap_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gatts_event_observer.h"
#include "ble/gatts_event_observer.h"

class ble_gatts_observer: public ble::gatts::event_observer
{
    void write(uint16_t conection_handle,
               uint16_t attribute_handle,
               uint8_t write_type,
               bool authorization_required,
               ble::att::length_t offset,
               ble::att::length_t length,
               void const *data) override
    {
    }

    void read_authorization_request(uint16_t conection_handle,
                                    uint16_t attribute_handle,
                                    ble::att::length_t offset) override
    {
    }

    void write_authorization_request(uint16_t conection_handle,
                                     uint16_t attribute_handle,
                                     uint8_t write_type,
                                     bool authorization_required,
                                     ble::att::length_t offset,
                                     ble::att::length_t length,
                                     void const *data) override
    {
    }

    void system_attribute_missing(uint16_t conection_handle,
                                  uint8_t hint) override
    {
    }

    void service_change_confirmation(uint16_t conection_handle) override
    {
    }

    void handle_value_confirmation(uint16_t conection_handle,
                                   uint16_t attribute_handle) override
    {

    };

    void mtu_rx_size(uint16_t conection_handle,
                     uint16_t client_rx_mtu_size) override
    {
    }

    void timeout(uint16_t conection_handle, uint8_t timeout_source) override
    {
    }

    void handle_value_notifications_tx_completed(uint16_t conection_handle,
                                                 uint8_t count) override
    {
    }
};
