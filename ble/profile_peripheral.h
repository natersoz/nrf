/**
 * @file ble/profile_peripheral.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/profile_connectable.h"
#include "ble/peripheral_connection.h"
#include "ble/gatts_event_observer.h"
#include "ble/gattc_event_observer.h"
#include "ble/gatt_service.h"
#include "ble/gatt_service_container.h"
#include "ble/stack.h"
#include "ble/uuid.h"
#include "ble/hci_error_codes.h"

namespace ble
{
namespace profile
{
/**
 * @class peripheral
 * Aggregate the specific classes which compose a BLE peripheral.
 */
class peripheral: public connectable
{
public:
    // These trival observers do nothing.
    // They stand in for the GATTS or GATTC references when none
    // is supplied in the ctor.
    static ble::gattc::event_observer ble_gattc_event_observer_trivial;
    static ble::gatts::event_observer ble_gatts_event_observer_trivial;

    virtual ~peripheral()                     = default;

    peripheral()                              = delete;
    peripheral(peripheral const&)             = delete;
    peripheral(peripheral &&)                 = delete;
    peripheral& operator=(peripheral const&)  = delete;
    peripheral& operator=(peripheral&&)       = delete;

    /// ctor: A peripheral with both GATT server and client.
    peripheral(ble::stack                       &ble_stack,
               ble::gap::peripheral_connection  &ble_gap_connection,
               ble::gatts::event_observer       &ble_gatts_event_observer,
               ble::gatts::operations           &ble_gatts_operations,
               ble::gattc::event_observer       &ble_gattc_event_observer)
    : connectable(ble_stack,
                  ble_gap_connection,
                  ble_gatts_event_observer,
                  ble_gatts_operations,
                  ble_gattc_event_observer),
      advertising_(ble_gap_connection.advertising())
    {
    }

    /// ctor: A peripheral with a GATT server only; no client.
    peripheral(ble::stack                       &ble_stack,
               ble::gap::peripheral_connection  &ble_gap_connection,
               ble::gatts::event_observer       &ble_gatts_event_observer,
               ble::gatts::operations           &ble_gatts_operations)
    : connectable(ble_stack,
                  ble_gap_connection,
                  ble_gatts_event_observer,
                  ble_gatts_operations),
      advertising_(ble_gap_connection.advertising())
    {
    }

    /// ctor: A peripheral with a GATT client only; no server.
    peripheral(ble::stack                       &ble_stack,
               ble::gap::peripheral_connection  &ble_gap_connection,
               ble::gattc::event_observer       &ble_gattc_event_observer)
    : connectable(ble_stack,
                  ble_gap_connection,
                  ble_gattc_event_observer),
      advertising_(ble_gap_connection.advertising())
    {
    }

    ble::gap::advertising const& advertising() const { return this->advertising_; }
    ble::gap::advertising&       advertising()       { return this->advertising_; }

private:
    using super = ble::profile::connectable;

    ble::gap::advertising& advertising_;
};

} // namespace profile
} // namespace ble
