/**
 * @file nordic_ble_common_event_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_event_observable.h"
#include "ble/nordic_ble_event_observer.h"

namespace nordic
{

template<>
void ble_event_observable<ble_common_event_observer>::notify(
    ble_common_event_observer::event_enum_t event_type,
    ble_common_event_observer::event_data_t const&  event_data)
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
