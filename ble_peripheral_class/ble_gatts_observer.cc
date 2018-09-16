/**
 * @file ble_peripheral_class/ble_gatts_observer.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble_gatts_observer.h"
#include "logger.h"

#include "ble_gatts.h"      // Nordic Softdevice

ble_gatts_observer::~ble_gatts_observer()
{
    if (this->nordic_gatts_event_observer_.is_attached())
    {
        nordic::ble_observables &observables = nordic::ble_observables::instance();
        observables.gatts_event_observable.detach(this->nordic_gatts_event_observer_);
    }
}

ble_gatts_observer::ble_gatts_observer():
    super(),
    nordic_gatts_event_observer_(*this)
{
    nordic::ble_observables &observables = nordic::ble_observables::instance();
    observables.gatts_event_observable.attach(this->nordic_gatts_event_observer_);
}

void ble_gatts_observer::write(uint16_t             conection_handle,
                               uint16_t             attribute_handle,
                               ble::att::op_code    write_type,
                               bool                 authorization_required,
                               ble::att::length_t   offset,
                               ble::att::length_t   length,
                               void const*          data)
{
}

void ble_gatts_observer::write_cancel(uint16_t             conection_handle,
                                      uint16_t             attribute_handle,
                                      ble::att::op_code    write_type,
                                      bool                 authorization_required,
                                      ble::att::length_t   offset,
                                      ble::att::length_t   length,
                                      void const*          data)
{
}

void ble_gatts_observer::read_authorization_request(uint16_t            conection_handle,
                                                    uint16_t            attribute_handle,
                                                    ble::att::length_t  offset)
{
    // reply with sd_ble_gatts_rw_authorize_reply
}

void ble_gatts_observer::write_authorization_request(uint16_t           conection_handle,
                                                     uint16_t           attribute_handle,
                                                     ble::att::op_code  write_type,
                                                     bool               authorization_required,
                                                     ble::att::length_t offset,
                                                     ble::att::length_t length,
                                                     void const*        data)
{
}

void ble_gatts_observer::system_attribute_missing(uint16_t  conection_handle,
                                                  uint8_t   hint)
{
}

void ble_gatts_observer::service_change_confirmation(uint16_t conection_handle)
{
}

void ble_gatts_observer::handle_value_confirmation(uint16_t conection_handle,
                                                   uint16_t attribute_handle)
{

};

void ble_gatts_observer::exchange_mtu_request(uint16_t conection_handle,
                                              uint16_t client_rx_mtu_size)
{
    uint32_t error_code = sd_ble_gatts_exchange_mtu_reply(conection_handle, 23u);
    logger::instance().debug("sd_ble_gatts_exchange_mtu_reply(): 0x%04x", error_code);

    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("sd_ble_gatts_exchange_mtu_reply() failed: 0x%04x", error_code);
    }
}

void ble_gatts_observer::timeout(uint16_t conection_handle,
                                 uint8_t timeout_source)
{
}

void ble_gatts_observer::handle_value_notifications_tx_completed(uint16_t   conection_handle,
                                                                 uint8_t    count)
{
}
