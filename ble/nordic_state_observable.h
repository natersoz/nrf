/**
 * @file nordic_ble_state_observable.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Publish Noridc BLE GAP events.
 */

#pragma once

#include "nordic_state_observer.h"

namespace nordic
{

class state_observable
{
public:
    static state_observable& instance();

    ~state_observable()                                   = default;
    state_observable()                                    = default;

    state_observable(state_observable const&)             = delete;
    state_observable(state_observable&&)                  = delete;
    state_observable& operator=(state_observable const &) = delete;
    state_observable& operator=(state_observable&&)       = delete;

    void attach(state_observer& observer);
    void detach(state_observer& observer);

    void notify(nrf_sdh_state_evt_t state_event_type);

private:
    /**
     * @note Do not call the hook method unlink().
     * Call timer_observable_generic::detach() to keep the attached count correct.
     */
    using observer_list =
        boost::intrusive::list<
            state_observer,
            boost::intrusive::member_hook<state_observer,
                                          boost::intrusive::list_member_hook<>,
                                          &state_observer::hook_> >;

    observer_list observer_list_;
};

} // namespace nordic
