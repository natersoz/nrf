/**
 * @file nordic_ble_gap_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "nordic_ble_gap_observable.h"
#include "nrf_sdh_ble.h"
#include "section_macros.h"
#include "project_assert.h"

static nordic::ble_gap_observable ble_gap_observable_instance;

namespace nordic
{
    void ble_gap_observable::attach(ble_gap_observer& observer)
    {
        ASSERT(not observer.is_attached());
        observer.observable_ = this;
        this->observer_list_.push_back(observer);
    }

    void ble_gap_observable::detach(ble_gap_observer& observer)
    {
        ASSERT(observer.is_attached());
        observer.observable_ = nullptr;
        this->observer_list_.remove(observer);
    }

    void ble_gap_observable::notify(enum BLE_GAP_EVTS    gap_event_type,
                                    ble_gap_evt_t const& gap_event)
    {
        for (auto observer_iter  = this->observer_list_.begin();
                  observer_iter != this->observer_list_.end(); )
        {
            // Increment the iterator prior to using it.
            // If the client removes itself during the completion callback
            // then the iterator will be valid and can continue.
            ble_gap_observer &observer = *observer_iter;
            ++observer_iter;

            observer.notify(gap_event_type, gap_event);
        }
    }

} // namespace nordic

/**
 * Possible event types are:
 * enum BLE_COMMON_EVTS
 * enum BLE_GAP_EVTS
 * enum BLE_GATTC_EVTS
 * enum BLE_GATTS_EVTS
 * enum BLE_L2CAP_EVTS
 *
 * @note Right now the only observers being registered are BLE GAP event
 * observers (BLE_GAP_EVTS).
 *
 * @todo Add in support for the other types.
 */

static void nordic_ble_gap_event_handler(ble_evt_t const *ble_event, void *context)
{
    if ((ble_event->header.evt_id >= BLE_GATTS_EVT_BASE) &&
        (ble_event->header.evt_id <= BLE_GATTS_EVT_LAST))
    {
        nordic::ble_gap_observable *observable =
            reinterpret_cast<nordic::ble_gap_observable*>(context);

        observable->notify(
            static_cast<enum BLE_GAP_EVTS>(ble_event->header.evt_id),
            ble_event->evt.gap_evt);
    }
}

static nrf_sdh_ble_evt_observer_t sdh_ble_gap_observer
    IN_SECTION(".sdh_ble_observers") =
{
    .handler    = nordic_ble_gap_event_handler,
    .p_context  = &ble_gap_observable_instance
};

