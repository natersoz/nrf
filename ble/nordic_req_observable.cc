/**
 * @file nordic_ble_req_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "nordic_req_observable.h"
#include "nrf_sdh.h"
#include "section_macros.h"
#include "project_assert.h"

static nordic::req_observable req_observable_instance;

nordic::req_observable& nordic::req_observable::instance()
{
    return req_observable_instance;
}

namespace nordic
{
    void req_observable::attach(req_observer& observer)
    {
        ASSERT(not observer.is_attached());
        observer.observable_ = this;
        this->observer_list_.push_back(observer);
    }

    void req_observable::detach(req_observer& observer)
    {
        ASSERT(observer.is_attached());
        observer.observable_ = nullptr;
        observer.hook_.unlink();
    }

    bool req_observable::notify(nrf_sdh_req_evt_t req_event_type)
    {
        for (auto observer_iter  = this->observer_list_.begin();
                  observer_iter != this->observer_list_.end(); )
        {
            // Increment the iterator prior to using it.
            // If the client removes itself during the completion callback
            // then the iterator will be valid and can continue.
            req_observer &observer = *observer_iter;
            ++observer_iter;

            bool const ready = observer.notify(req_event_type);
            if (not ready)
            {
                // If no registered observers are ready for the softdevice
                // state change then we cannot proceed.
                return false;
            }
        }

        return true;
    }

} // namespace nordic

static bool nordic_req_event_handler(nrf_sdh_req_evt_t req_event_id, void *context)
{
    auto *observable = reinterpret_cast<nordic::req_observable*>(context);
    return observable->notify(req_event_id);
}

static nrf_sdh_req_observer_t sdh_req_observer
    IN_SECTION(".sdh_req_observers") =
{
    .handler    = nordic_req_event_handler,
    .p_context  = &req_observable_instance
};

