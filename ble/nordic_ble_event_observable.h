/**
 * @file nordic_ble_event_observable.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A template class for publishing Noridc BLE events.
 * The event type is parameterized in templates and instantiated by type in
 * nordic_ble_event_observable.cc.
 *
 * BLE event types that are published by the Nordic softdevice:
 *   enum BLE_COMMON_EVTS
 *   enum BLE_GAP_EVTS
 *   enum BLE_GATTC_EVTS
 *   enum BLE_GATTS_EVTS
 *   enum BLE_L2CAP_EVTS        @todo not yet implemeneted
 */

#pragma once

#include "nordic_ble_event_observer.h"
#include "project_assert.h"
#include <cstdint>

namespace nordic
{

template <typename observer_type>
class ble_event_observable
{
public:
    ~ble_event_observable()                                       = default;
    ble_event_observable()                                        = default;

    ble_event_observable(ble_event_observable const&)             = delete;
    ble_event_observable(ble_event_observable&&)                  = delete;
    ble_event_observable& operator=(ble_event_observable const &) = delete;
    ble_event_observable& operator=(ble_event_observable&&)       = delete;

    void attach(observer_type& observer)
    {
        ASSERT(not observer.is_attached());
        observer.observable_ = this;
        this->observer_list_.push_back(observer);
    }

    void detach(observer_type& observer)
    {
        ASSERT(observer.is_attached());
        observer.observable_ = nullptr;
        this->observer_list_.remove(observer);
    }

    void notify(typename observer_type::event_enum_t         event_type,
                typename observer_type::event_data_t const&  event_data);

private:
    /**
     * @note Do not call the hook method unlink().
     * Call timer_observable_generic::detach() to keep the attached count correct.
     */
    using observer_list =
        boost::intrusive::list<
            observer_type,
            boost::intrusive::member_hook<observer_type,
                                          boost::intrusive::list_member_hook<>,
                                          &observer_type::hook_> >;

    observer_list observer_list_;
};

struct ble_observables
{
    static ble_observables& instance();

    ~ble_observables()                                  = default;
    ble_observables()                                   = default;

    ble_observables(ble_observables const&)             = delete;
    ble_observables(ble_observables&&)                  = delete;
    ble_observables& operator=(ble_observables const &) = delete;
    ble_observables& operator=(ble_observables&&)       = delete;

    ble_event_observable<ble_common_event_observer>     common_event_observable;
    ble_event_observable<ble_gap_event_observer>        gap_event_observable;
    ble_event_observable<ble_gattc_event_observer>      gattc_event_observable;
    ble_event_observable<ble_gattc_discovery_observer>  gattc_discovery_observable;
    ble_event_observable<ble_gatts_event_observer>      gatts_event_observable;
};

/**
 * Specialized Nordic hack to aquire 128-bit UUIDs which have not been
 * pre-registered with the softdevice.
 *
 * @param connection_handle
 * @param gatt_handle
 *
 * @return uint32_t
 */
uint32_t gattc_uuid128_acquire(uint16_t connection_handle, uint16_t gatt_handle);

} // namespace nordic
