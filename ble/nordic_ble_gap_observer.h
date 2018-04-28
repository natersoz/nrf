/**
 * @file nordic_ble_gap_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Observer interface for receiving Noridc BLE GAP events.
 */

#pragma once

#include "ble_gap.h"
#include <boost/intrusive/list.hpp>

namespace nordic
{

class ble_gap_observable;

class ble_gap_observer
{
friend class ble_gap_observable;

public:
    virtual ~ble_gap_observer()                             = default;
    ble_gap_observer(): observable_(nullptr) {}

    ble_gap_observer(ble_gap_observer const&)               = delete;
    ble_gap_observer(ble_gap_observer&&)                    = delete;
    ble_gap_observer& operator=(ble_gap_observer const &)   = delete;
    ble_gap_observer& operator=(ble_gap_observer&&)         = delete;

    virtual void notify(enum BLE_GAP_EVTS   gap_event_type,
                        ble_gap_evt_t const &gap_event)     = 0;

    bool is_attached() const { return bool(this->observable_); }

    /**
     * Since ble_gap_observer is not copyable in any form, the means for
     * testing equality is whether they are the same instance in memory.
     */
    bool operator==(ble_gap_observer const& other) const { return (this == &other); }

private:
    /// @todo needs volatile
    boost::intrusive::list_member_hook<> hook_;

    ble_gap_observable volatile *observable_;
};

} // namespace nordic
