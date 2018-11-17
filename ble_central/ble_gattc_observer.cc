/**
 * @file ble_peripheral/ble_gatts_observer.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble_gattc_observer.h"
#include "logger.h"

ble_gattc_observer::~ble_gattc_observer()
{
    if (this->nordic_gattc_event_observer_.is_attached())
    {
        nordic::ble_observables &observables = nordic::ble_observables::instance();
        observables.gattc_event_observable.detach(this->nordic_gattc_event_observer_);
    }
}

ble_gattc_observer::ble_gattc_observer():
    super(),
    nordic_gattc_event_observer_(*this)
{
}

void ble_gattc_observer::init()
{
    nordic::ble_observables &observables = nordic::ble_observables::instance();
    observables.gattc_event_observable.attach(this->nordic_gattc_event_observer_);
}

void ble_gattc_observer::service_discovery_response(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    handle_start,
    uint16_t                    handle_stop,
    ble::att::uuid const&       uuid)
{
    super::service_discovery_response(conection_handle,
                                      error_code,
                                      error_handle,
                                      handle_start,
                                      handle_stop,
                                      uuid);
}

void ble_gattc_observer::relationship_discovery_response(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    handle_start,
    uint16_t                    handle_stop,
    uint16_t                    service_handle,
    ble::att::uuid const&       uuid)
{
    super::relationship_discovery_response(conection_handle,
                                           error_code,
                                           error_handle,
                                           handle_start,
                                           handle_stop,
                                           service_handle,
                                           uuid);
}

void ble_gattc_observer::characteristic_discovery_response(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    handle,
    uint16_t                    handle_stop,
    ble::att::uuid const&       uuid,
    ble::gatt::properties       properties)
{
    super::characteristic_discovery_response(conection_handle,
                                             error_code,
                                             error_handle,
                                             handle,
                                             handle_stop,
                                             uuid,
                                             properties);
}

void ble_gattc_observer::descriptor_discovery_response(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    desciptor_handle,
    ble::att::uuid const&       uuid)
{
    super::descriptor_discovery_response(conection_handle,
                                         error_code,
                                         error_handle,
                                         desciptor_handle,
                                         uuid);
}

void ble_gattc_observer::attribute_uuid_discovery_response(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    handle,
    ble::att::uuid const&       uuid)
{
    super::attribute_uuid_discovery_response(conection_handle,
                                             error_code,
                                             error_handle,
                                             handle,
                                             uuid);
}

void ble_gattc_observer::read_characteristic_by_uuid_response(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    characteristic_handle,
    void const*                 data,
    ble::att::length_t          length)
{
    super::read_characteristic_by_uuid_response(conection_handle,
                                                error_code,
                                                error_handle,
                                                characteristic_handle,
                                                data,
                                                length);
}

void ble_gattc_observer::read_response(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    attribute_handle,
    void const*                 data,
    ble::att::length_t          offset,
    ble::att::length_t          length)
{
    super::read_response(conection_handle,
                         error_code,
                         error_handle,
                         attribute_handle,
                         data,
                         offset,
                         length);
}

void ble_gattc_observer::read_multi_response(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    void const*                 data,
    ble::att::length_t          length)
{
    super::read_multi_response(conection_handle,
                               error_code,
                               error_handle,
                               data,
                               length);
}

void ble_gattc_observer::write_response(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    ble::att::op_code           write_op_code,
    uint16_t                    attribute_handle,
    void const*                 data,
    ble::att::length_t          offset,
    ble::att::length_t          length)
{
    super::write_response(conection_handle,
                          error_code,
                          error_handle,
                          write_op_code,
                          attribute_handle,
                          data,
                          offset,
                          length);
}

void ble_gattc_observer::handle_notification(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    attribute_handle,
    void const*                 data,
    ble::att::length_t          length)
{
    super::handle_notification(conection_handle,
                               error_code,
                               error_handle,
                               attribute_handle,
                               data,
                               length);
}

void ble_gattc_observer::handle_indication(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    attribute_handle,
    void const*                 data,
    ble::att::length_t          length)
{
    super::handle_indication(conection_handle,
                             error_code,
                             error_handle,
                             attribute_handle,
                             data,
                             length);
}

void ble_gattc_observer::mtu_rx_size(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    server_rx_mtu_size)
{
    super::mtu_rx_size(conection_handle,
                       error_code,
                       error_handle,
                       server_rx_mtu_size);
}

void ble_gattc_observer::timeout(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle)
{
    super::timeout(conection_handle,
                   error_code,
                   error_handle);
}

void ble_gattc_observer::write_command_tx_completed(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint8_t                     count)
{
    super::write_command_tx_completed(conection_handle,
                                      error_code,
                                      error_handle,
                                      count);
}
