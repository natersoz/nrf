/**
 * @file nordic_req_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Observer interface for receiving Noridc BLE GAP events.
 */

#pragma once

#include <nrf_sdh.h>
#include <boost/intrusive/list.hpp>

namespace nordic
{

class req_observable;

class req_observer
{
friend class req_observable;

public:
    virtual ~req_observer()                         = default;
    req_observer(): observable_(nullptr) {}

    req_observer(req_observer const&)               = delete;
    req_observer(req_observer&&)                    = delete;
    req_observer& operator=(req_observer const &)   = delete;
    req_observer& operator=(req_observer&&)         = delete;

    /**
     * SoftDevice state request handler.
     *
     * @param  req_event_type @see nrf_sdh_req_evt_t
     * @retval true  If ready for the SoftDevice to change state.
     * @retval false If not ready for the SoftDevice to change state.
     *                  If false is returned, the state change is aborted.
     */
    virtual bool notify(nrf_sdh_req_evt_t req_event_type) = 0;

    bool is_attached() const { return bool(this->observable_); }

private:
    boost::intrusive::list_member_hook<
            boost::intrusive::link_mode<boost::intrusive::auto_unlink>
        > hook_;

    req_observable volatile *observable_;
};

} // namespace nordic
