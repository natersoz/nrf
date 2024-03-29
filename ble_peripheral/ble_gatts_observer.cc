/**
 * @file ble_peripheral/ble_gatts_observer.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble_gatts_observer.h"
#include "logger.h"

ble_gatts_observer::~ble_gatts_observer()
{
}

ble_gatts_observer::ble_gatts_observer(): super()
{
}

void ble_gatts_observer::write(uint16_t             connection_handle,
                               uint16_t             attribute_handle,
                               ble::att::op_code    write_type,
                               bool                 authorization_required,
                               ble::att::length_t   offset,
                               ble::att::length_t   length,
                               void const*          data)
{
    super::write(connection_handle,
                 attribute_handle,
                 write_type,
                 authorization_required,
                 offset,
                 length,
                 data);
}

void ble_gatts_observer::write_cancel(uint16_t             connection_handle,
                                      uint16_t             attribute_handle,
                                      ble::att::op_code    write_type,
                                      bool                 authorization_required,
                                      ble::att::length_t   offset,
                                      ble::att::length_t   length,
                                      void const*          data)
{
    super::write_cancel(connection_handle,
                        attribute_handle,
                        write_type,
                        authorization_required,
                        offset,
                        length,
                        data);
}

void ble_gatts_observer::read_authorization_request(uint16_t            connection_handle,
                                                    uint16_t            attribute_handle,
                                                    ble::att::length_t  offset)
{
    // reply with sd_ble_gatts_rw_authorize_reply
}

void ble_gatts_observer::write_authorization_request(uint16_t           connection_handle,
                                                     uint16_t           attribute_handle,
                                                     ble::att::op_code  write_type,
                                                     bool               authorization_required,
                                                     ble::att::length_t offset,
                                                     ble::att::length_t length,
                                                     void const*        data)
{
}

void ble_gatts_observer::service_change_confirmation(uint16_t connection_handle)
{
    super::service_change_confirmation(connection_handle);
}

void ble_gatts_observer::handle_value_confirmation(uint16_t connection_handle,
                                                   uint16_t attribute_handle)
{
    super::handle_value_confirmation(connection_handle, attribute_handle);

};

void ble_gatts_observer::exchange_mtu_request(uint16_t connection_handle,
                                              uint16_t client_rx_mtu_size)
{
    super::exchange_mtu_request(connection_handle, client_rx_mtu_size);
}

void ble_gatts_observer::timeout(uint16_t connection_handle, uint8_t timeout_source)
{
    super::timeout(connection_handle, timeout_source);
}

void ble_gatts_observer::handle_value_notifications_tx_completed(uint16_t   connection_handle,
                                                                 uint8_t    count)
{
    super::handle_value_notifications_tx_completed(connection_handle, count);
}
