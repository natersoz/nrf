/**
 * @file nordic_soc_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Observer interface for receiving Noridc BLE GAP events.
 */

#pragma once

#include <nrf_soc.h>
#include <boost/intrusive/list.hpp>

namespace nordic
{

class soc_observable;

class soc_observer
{
friend class soc_observable;

public:
    virtual ~soc_observer()                         = default;
    soc_observer(): observable_(nullptr) {}

    soc_observer(soc_observer const&)               = delete;
    soc_observer(soc_observer&&)                    = delete;
    soc_observer& operator=(soc_observer const &)   = delete;
    soc_observer& operator=(soc_observer&&)         = delete;

    virtual void notify(enum NRF_SOC_EVTS soc_event_type) = 0;

    bool is_attached() const { return bool(this->observable_); }

private:
    boost::intrusive::list_member_hook<
            boost::intrusive::link_mode<boost::intrusive::auto_unlink>
        > hook_;

    soc_observable volatile *observable_;
};

} // namespace nordic
