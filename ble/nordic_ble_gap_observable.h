/**
 * @file nordic_ble_gap_observable.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Publish Noridc BLE GAP events.
 */

#pragma once

#include "nordic_ble_gap_observer.h"

namespace nordic
{

class ble_gap_observable
{
public:
    ~ble_gap_observable()                                       = default;
    ble_gap_observable()                                        = default;

    ble_gap_observable(ble_gap_observable const&)               = delete;
    ble_gap_observable(ble_gap_observable&&)                    = delete;
    ble_gap_observable& operator=(ble_gap_observable const &)   = delete;
    ble_gap_observable& operator=(ble_gap_observable&&)         = delete;

    void attach(ble_gap_observer& observer);
    void detach(ble_gap_observer& observer);

    void notify(enum BLE_GAP_EVTS    gap_event_type,
                ble_gap_evt_t const& gap_event);

private:
    /**
     * @note Do not call the hook method unlink().
     * Call timer_observable_generic::detach() to keep the attached count correct.
     */
    using observer_list =
        boost::intrusive::list<
            ble_gap_observer,
            boost::intrusive::member_hook<ble_gap_observer,
                                          boost::intrusive::list_member_hook<>,
                                          &ble_gap_observer::hook_> >;

    observer_list observer_list_;
};

} // namespace nordic
