/**
 * @file ble/nordic_ble_peripheral.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Implementation of the ble/peripheral class for the Nordic nrf5x device.
 */

#pragma once

#include "ble/peripheral.h"
#include "ble/nordic_ble_event_observable.h"

namespace nordic
{

class ble_peripheral: public ble::peripheral
{
public:
    virtual ~ble_peripheral() override {}

    ble_peripheral(ble::stack                   &ble_stack,
                   ble::gap::advertising        &ble_advertising,
                   ble::gap::connection         &ble_gap_connection,
                   ble::gatts::event_observer   &ble_gatts_event_observer,
                   ble::gattc::event_observer   &ble_gattc_event_observer)
    : ble::peripheral(ble_stack,
                      ble_advertising,
                      ble_gap_connection,
                      ble_gatts_event_observer,
                      ble_gattc_event_observer),
      nordic_gatts_event_observer_(ble_gatts_event_observer)
    {
    }

    ble_peripheral(ble::stack                   &ble_stack,
                   ble::gap::advertising        &ble_advertising,
                   ble::gap::connection         &ble_gap_connection,
                   ble::gatts::event_observer   &ble_gatts_event_observer)
    : ble::peripheral(ble_stack,
                      ble_advertising,
                      ble_gap_connection,
                      ble_gatts_event_observer),
      nordic_gatts_event_observer_(ble_gatts_event_observer)
    {
    }

    /// @todo Nordic GATT client oberserver not yet implemented.

private:
    ble_gatts_event_observer    nordic_gatts_event_observer_;
};

} // namespace nordic
