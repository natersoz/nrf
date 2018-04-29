/**
 * @file nordic_state_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Observer interface for receiving Noridc BLE GAP events.
 */

#pragma once

#include "nrf_sdh.h"
#include <boost/intrusive/list.hpp>

namespace nordic
{

class state_observable;

class state_observer
{
friend class state_observable;

public:
    virtual ~state_observer()                         = default;
    state_observer(): observable_(nullptr) {}

    state_observer(state_observer const&)               = delete;
    state_observer(state_observer&&)                    = delete;
    state_observer& operator=(state_observer const &)   = delete;
    state_observer& operator=(state_observer&&)         = delete;

    virtual void notify(nrf_sdh_state_evt_t state_event_type) = 0;

    bool is_attached() const { return bool(this->observable_); }

    /**
     * Since state_observer is not copyable in any form, the means for
     * testing equality is whether they are the same instance in memory.
     */
    bool operator==(state_observer const& other) const { return (this == &other); }

private:
    /// @todo needs volatile
    boost::intrusive::list_member_hook<> hook_;

    state_observable volatile *observable_;
};

} // namespace nordic
