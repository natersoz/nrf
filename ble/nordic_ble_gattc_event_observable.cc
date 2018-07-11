/**
 * @file nordic_ble_gattc_event_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Provide attachment and notifications for the ble::gap::event_observer class
 * into the Nordic BLE observables.
 */

#include "ble/nordic_ble_event_observable.h"
#include "ble/nordic_ble_event_observer.h"
#include "ble/gattc_event_observer.h"

namespace nordic
{

template<>
void ble_event_observable<ble_gattc_event_observer>::notify(
    ble_gattc_event_observer::event_enum_t event_type,
    ble_gattc_event_observer::event_data_t const&  event_data)
{
    for (auto observer_iter  = this->observer_list_.begin();
              observer_iter != this->observer_list_.end(); )
    {
        // Increment the iterator prior to using it.
        // If the client removes itself during the completion callback
        // then the iterator will be valid and can continue.
        auto &observer = *observer_iter;
        ++observer_iter;

        observer.notify(event_type, event_data);
    }
}

} // namespace nordic
