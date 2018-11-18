/**
 * @file ble/profile_central.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/profile_connectable.h"
#include "ble/central_connection.h"
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
 * @class central
 * Aggregate the specific classes which compose a BLE central.
 */
class central: public connectable
{
private:
    using super = ble::profile::connectable;

public:
    virtual ~central()                  = default;

    central()                           = delete;
    central(central const&)             = delete;
    central(central &&)                 = delete;
    central& operator=(central const&)  = delete;
    central& operator=(central&&)       = delete;

    /// ctor: A central with both GATT server and client.
    central(ble::stack&                     ble_stack,
            ble::gap::central_connection&   ble_gap_connection,
            ble::gatts::event_observer&     ble_gatts_event_observer,
            ble::gatts::operations&         ble_gatts_operations,
            ble::gattc::event_observer&     ble_gattc_event_observer)
    : connectable(ble_stack,
                  ble_gap_connection,
                  ble_gatts_event_observer,
                  ble_gatts_operations,
                  ble_gattc_event_observer),
      scanning_(ble_gap_connection.scanning())
    {
    }

    /// ctor: A central with a GATT server only; no client.
    central(ble::stack&                     ble_stack,
            ble::gap::central_connection&   ble_gap_connection,
            ble::gatts::event_observer&     ble_gatts_event_observer,
            ble::gatts::operations&         ble_gatts_operations)
    : connectable(ble_stack,
                  ble_gap_connection,
                  ble_gatts_event_observer,
                  ble_gatts_operations),
      scanning_(ble_gap_connection.scanning())
    {
    }

    /// ctor: A central with a GATT client only; no server.
    central(ble::stack&                         ble_stack,
               ble::gap::central_connection&    ble_gap_connection,
               ble::gattc::event_observer&      ble_gattc_event_observer)
    : connectable(ble_stack,
                  ble_gap_connection,
                  ble_gattc_event_observer),
      scanning_(ble_gap_connection.scanning())
    {
    }

    ble::gap::scanning const& scanning() const { return this->scanning_; }
    ble::gap::scanning&       scanning()       { return this->scanning_; }

private:
    ble::gap::scanning& scanning_;
};

} // namespace profile
} // namespace ble
