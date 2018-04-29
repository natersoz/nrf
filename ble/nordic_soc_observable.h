/**
 * @file nordic_ble_soc_observable.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Publish Noridc BLE GAP events.
 */

#pragma once

#include "nordic_soc_observer.h"

namespace nordic
{

class soc_observable
{
public:
    static soc_observable& instance();

    ~soc_observable()                                   = default;
    soc_observable()                                    = default;

    soc_observable(soc_observable const&)               = delete;
    soc_observable(soc_observable&&)                    = delete;
    soc_observable& operator=(soc_observable const &)   = delete;
    soc_observable& operator=(soc_observable&&)         = delete;

    void attach(soc_observer& observer);
    void detach(soc_observer& observer);

    void notify(enum NRF_SOC_EVTS soc_event_type);

private:
    /**
     * @note Do not call the hook method unlink().
     * Call timer_observable_generic::detach() to keep the attached count correct.
     */
    using observer_list =
        boost::intrusive::list<
            soc_observer,
            boost::intrusive::member_hook<soc_observer,
                                          boost::intrusive::list_member_hook<>,
                                          &soc_observer::hook_> >;

    observer_list observer_list_;
};

} // namespace nordic
