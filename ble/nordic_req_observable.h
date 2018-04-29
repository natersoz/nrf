/**
 * @file nordic_ble_req_observable.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Publish Noridc BLE GAP events.
 */

#pragma once

#include "nordic_req_observer.h"

namespace nordic
{

class req_observable
{
public:
    static req_observable& instance();

    ~req_observable()                                   = default;
    req_observable()                                    = default;

    req_observable(req_observable const&)               = delete;
    req_observable(req_observable&&)                    = delete;
    req_observable& operator=(req_observable const &)   = delete;
    req_observable& operator=(req_observable&&)         = delete;

    void attach(req_observer& observer);
    void detach(req_observer& observer);

    /**
     * SoftDevice state request handler.
     * @param  req_event_type @see nrf_sdh_req_evt_t
     * @retval true  If ready for the SoftDevice to change state.
     * @retval false If not ready for the SoftDevice to change state.
     *                  If false is returned, the state change is aborted.
     */
    bool notify(nrf_sdh_req_evt_t req_event_type);

private:
    /**
     * @note Do not call the hook method unlink().
     * Call timer_observable_generic::detach() to keep the attached count correct.
     */
    using observer_list =
        boost::intrusive::list<
            req_observer,
            boost::intrusive::member_hook<req_observer,
                                          boost::intrusive::list_member_hook<>,
                                          &req_observer::hook_> >;

    observer_list observer_list_;
};

} // namespace nordic
