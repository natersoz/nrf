/**
 * @file ble_peripheral/ble_gatts_observer.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/profile_connectable.h"
#include "ble/gap_connection.h"
#include "ble_gattc_observer.h"
#include "logger.h"

ble_gattc_observer::~ble_gattc_observer()
{
}

ble_gattc_observer::ble_gattc_observer() :
    super()
{
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

void ble_gattc_observer::exchange_mtu_response(
    uint16_t                    conection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    server_rx_mtu_size)
{
    super::exchange_mtu_response(conection_handle,
                                 error_code,
                                 error_handle,
                                 server_rx_mtu_size);

    ble::gap::connection &gap_connection = this->get_connecteable()->connection();
    gap_connection.get_negotiation_state().set_gatt_mtu_exchange_pending(false);

    if (not gap_connection.get_negotiation_state().is_any_update_pending())
    {
        logger::instance().debug("--- pending updates complete ---");
    }
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
