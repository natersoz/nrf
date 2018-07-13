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
    using ble::peripheral::peripheral;

    virtual ~ble_peripheral() override {}

    ble_peripheral(ble::stack                   &ble_stack,
                   ble::advertising             &ble_advertising,
                   ble::gap::event_observer     &ble_gap_event_observer,
                   ble::gatts::event_observer   &ble_gatts_event_observer,
                   ble::gattc::event_observer   &ble_gattc_event_observer)
    : ble::peripheral(ble_stack,
                      ble_advertising,
                      ble_gap_event_observer,
                      ble_gatts_event_observer,
                      ble_gattc_event_observer),
        nordic_gap_event_observer_(ble_gap_event_observer),
        nordic_gatts_event_observer_(ble_gatts_event_observer)
    {
        ble_observables& observables = nordic::ble_observables::instance();

        observables.gap_event_observable.attach(this->nordic_gap_event_observer_);
        observables.gatts_event_observable.attach(this->nordic_gatts_event_observer_);
    }

    ble_peripheral(ble::stack                   &ble_stack,
                   ble::advertising             &ble_advertising,
                   ble::gap::event_observer     &ble_gap_event_observer,
                   ble::gatts::event_observer   &ble_gatts_event_observer)
    : ble::peripheral(ble_stack,
                      ble_advertising,
                      ble_gap_event_observer,
                      ble_gatts_event_observer),
        nordic_gap_event_observer_(ble_gap_event_observer),
        nordic_gatts_event_observer_(ble_gatts_event_observer)
    {
        ble_observables& observables = nordic::ble_observables::instance();

        observables.gap_event_observable.attach(this->nordic_gap_event_observer_);
        observables.gatts_event_observable.attach(this->nordic_gatts_event_observer_);
    }

    /// @todo Nordic GATT client oberserver not yet implemented.

private:
    ble_gap_event_observer      nordic_gap_event_observer_;
    ble_gatts_event_observer    nordic_gatts_event_observer_;

};

} // namespace nordic
