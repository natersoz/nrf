/**
 * @file ble/peripheral.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/peripheral_connection.h"
#include "ble/gatts_event_observer.h"
#include "ble/gattc_event_observer.h"
#include "ble/gatt_service.h"
#include "ble/stack.h"
#include "ble/uuid.h"
#include "ble/hci_error_codes.h"

namespace ble
{

/**
 * @class peripheral
 * Aggregate the specific classes which compose a BLE peripheral
 */
class peripheral
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
               ble::gattc::event_observer       &ble_gattc_event_observer)
    : ble_stack_(ble_stack),
      gap_connection_(ble_gap_connection),
      gatts_event_observer_(ble_gatts_event_observer),
      gattc_event_observer_(ble_gattc_event_observer)
    {
    }

    /// ctor: A peripheral with a GATT server only; no client.
    peripheral(ble::stack                       &ble_stack,
               ble::gap::peripheral_connection  &ble_gap_connection,
               ble::gatts::event_observer       &ble_gatts_event_observer)
    : ble_stack_(ble_stack),
      gap_connection_(ble_gap_connection),
      gatts_event_observer_(ble_gatts_event_observer),
      gattc_event_observer_(ble_gattc_event_observer_trivial)
    {
    }

    /// ctor: A peripheral with a GATT client only; no server.
    peripheral(ble::stack                       &ble_stack,
               ble::gap::peripheral_connection  &ble_gap_connection,
               ble::gattc::event_observer       &ble_gattc_event_observer)
    : ble_stack_(ble_stack),
      gap_connection_(ble_gap_connection),
      gatts_event_observer_(ble_gatts_event_observer_trivial),
      gattc_event_observer_(ble_gattc_event_observer)
    {
    }

    ble::stack&             ble_stack()         { return this->ble_stack_; }
    ble::stack const &      ble_stack() const   { return this->ble_stack_; }

    ble::gap::peripheral_connection const& connection() const { return this->gap_connection_; }
    ble::gap::peripheral_connection&       connection()       { return this->gap_connection_; }

    void service_add(ble::gatt::service const& service);
    ble::gatt::service*       service_get(ble::att::uuid uuid);
    ble::gatt::service const* service_get(ble::att::uuid uuid) const;
    ble::gatt::service*       service_get(ble::gatt::services uuid);
    ble::gatt::service const* service_get(ble::gatt::services uuid) const;

private:
    ble::stack                      &ble_stack_;
    ble::gap::peripheral_connection &gap_connection_;
    ble::gatts::event_observer      &gatts_event_observer_;
    ble::gattc::event_observer      &gattc_event_observer_;

    ble::gatt::service_container    service_container_;
};

} // namespace ble
