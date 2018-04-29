/**
 * @file nordic_ble_soc_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "nordic_soc_observable.h"
#include "nrf_sdh_soc.h"
#include "section_macros.h"
#include "project_assert.h"

static nordic::soc_observable soc_observable_instance;

nordic::soc_observable& nordic::soc_observable::instance()
{
    return soc_observable_instance;
}

namespace nordic
{
    void soc_observable::attach(soc_observer& observer)
    {
        ASSERT(not observer.is_attached());
        observer.observable_ = this;
        this->observer_list_.push_back(observer);
    }

    void soc_observable::detach(soc_observer& observer)
    {
        ASSERT(observer.is_attached());
        observer.observable_ = nullptr;
        this->observer_list_.remove(observer);
    }

    void soc_observable::notify(enum NRF_SOC_EVTS soc_event_type)
    {
        for (auto observer_iter  = this->observer_list_.begin();
                  observer_iter != this->observer_list_.end(); )
        {
            // Increment the iterator prior to using it.
            // If the client removes itself during the completion callback
            // then the iterator will be valid and can continue.
            soc_observer &observer = *observer_iter;
            ++observer_iter;

            observer.notify(soc_event_type);
        }
    }

} // namespace nordic

static void nordic_soc_event_handler(uint32_t soc_event_id, void *context)
{
    if (soc_event_id < NRF_EVT_NUMBER_OF_EVTS)
    {
        auto *observable = reinterpret_cast<nordic::soc_observable*>(context);
        observable->notify(static_cast<enum NRF_SOC_EVTS>(soc_event_id));
    }
}

static nrf_sdh_soc_evt_observer_t sdh_soc_observer
    IN_SECTION(".sdh_soc_observers") =
{
    .handler    = nordic_soc_event_handler,
    .p_context  = &soc_observable_instance
};

