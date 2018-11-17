/**
 * @file ble_peripheral/ble_gatts_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gatts_event_observer.h"
#include "ble/nordic_ble_event_observable.h"

class ble_gatts_observer: public ble::gatts::event_observer
{
private:
    using super = ble::gatts::event_observer;

public:
    virtual ~ble_gatts_observer() override;

    ble_gatts_observer(ble_gatts_observer const&)            = delete;
    ble_gatts_observer(ble_gatts_observer &&)                = delete;
    ble_gatts_observer& operator=(ble_gatts_observer const&) = delete;
    ble_gatts_observer& operator=(ble_gatts_observer&&)      = delete;

    ble_gatts_observer();

    /**
     * Post constructor initialization.
     * Attach the this observer to the Noridc BLE GATT server observable.
     *
     * This is required since C++ does not provide ordering for statically
     * allocated modules across classes. The Nordic BLE GATT server may not
     * be initialized prior to this ctor being called.
     */
    void init();

protected:
    void write(uint16_t             conection_handle,
               uint16_t             attribute_handle,
               ble::att::op_code    write_type,
               bool                 authorization_required,
               ble::att::length_t   offset,
               ble::att::length_t   length,
               void const*          data) override;

    void write_cancel(uint16_t             conection_handle,
                      uint16_t             attribute_handle,
                      ble::att::op_code    write_type,
                      bool                 authorization_required,
                      ble::att::length_t   offset,
                      ble::att::length_t   length,
                      void const*          data) override;

    void read_authorization_request(uint16_t            conection_handle,
                                    uint16_t            attribute_handle,
                                    ble::att::length_t  offset) override;

    void write_authorization_request(uint16_t           conection_handle,
                                     uint16_t           attribute_handle,
                                     ble::att::op_code  write_type,
                                     bool               authorization_required,
                                     ble::att::length_t offset,
                                     ble::att::length_t length,
                                     void const*        data) override;

    void service_change_confirmation(uint16_t conection_handle) override;

    void handle_value_confirmation(uint16_t conection_handle,
                                   uint16_t attribute_handle) override;

    void exchange_mtu_request(uint16_t conection_handle,
                              uint16_t client_rx_mtu_size) override;

    void timeout(uint16_t conection_handle, uint8_t timeout_source) override;

    void handle_value_notifications_tx_completed(uint16_t   conection_handle,
                                                 uint8_t    count) override;

private:
    // This is where/how this generic interface ties in with the Nordic BLE
    // GATT server events.
    nordic::ble_gatts_event_observer nordic_gatts_event_observer_;
};
