/**
 * @file nordic_ble_state_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "nordic_state_observable.h"
#include "nrf_sdh.h"
#include "section_macros.h"
#include "project_assert.h"

static nordic::state_observable state_observable_instance;

nordic::state_observable& nordic::state_observable::instance()
{
    return state_observable_instance;
}

namespace nordic
{
    void state_observable::attach(state_observer& observer)
    {
        ASSERT(not observer.is_attached());
        observer.observable_ = this;
        this->observer_list_.push_back(observer);
    }

    void state_observable::detach(state_observer& observer)
    {
        ASSERT(observer.is_attached());
        observer.observable_ = nullptr;
        this->observer_list_.remove(observer);
    }

    void state_observable::notify(nrf_sdh_state_evt_t state_event_type)
    {
        for (auto observer_iter  = this->observer_list_.begin();
                  observer_iter != this->observer_list_.end(); )
        {
            // Increment the iterator prior to using it.
            // If the client removes itself during the completion callback
            // then the iterator will be valid and can continue.
            state_observer &observer = *observer_iter;
            ++observer_iter;

            observer.notify(state_event_type);
        }
    }

} // namespace nordic

static void nordic_state_event_handler(nrf_sdh_state_evt_t state_event_id, void *context)
{
    auto *observable = reinterpret_cast<nordic::state_observable*>(context);
    observable->notify(state_event_id);
}

static nrf_sdh_state_observer_t sdh_state_observer
    IN_SECTION(".sdh_state_observers") =
{
    .handler    = nordic_state_event_handler,
    .p_context  = &state_observable_instance
};

