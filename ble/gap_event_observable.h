/**
 * @file ble/gap_event_observable.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gap_event_observer.h"

namespace ble
{
namespace gap
{

class event_observable
{
    virtual ~event_observable()                           = default;

    event_observable()                                    = default;
    event_observable(event_observable const&)             = delete;
    event_observable(event_observable &&)                 = delete;
    event_observable& operator=(event_observable const&)  = delete;
    event_observable& operator=(event_observable&&)       = delete;

    void attach(event_observer& observer)
    {
        if (not observer.hook.is_linked())
        {
            this->observer_list.push_back(observer);
        }
    }

    void attach_first(event_observer& observer)
    {
        if (not observer.hook.is_linked())
        {
            this->observer_list.push_front(observer);
        }
    }

    void detach(event_observer& observer)
    {
        if (observer.hook.is_linked())
        {
            observer.hook.unlink();
        }
    }

private:
    using list_type = boost::intrusive::list<
        event_observer,
        boost::intrusive::constant_time_size<false>,
        boost::intrusive::member_hook<
            event_observer,
            typename event_observer::list_hook_type,
            &event_observer::hook>
    >;

    list_type observer_list;
};

} // namespace gap
} // namespace ble


