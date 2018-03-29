/**
 * @file timer_observer.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Wrap a timer peripheral to proivde a simpler interface.
 */

#include "timer_observer.h"
#include "project_assert.h"
#include <algorithm>

/**
 * @note Since timer_observer inherits from boost::intrusive::list and
 * auto-unlink is enabled then the list hook dtor will unlink the node from
 * the list for us. Do not do it here (static_asserts, which is nice).
 * @see http://www.boost.org/doc/libs/1_64_0/doc/html/intrusive/auto_unlink_hooks.html
 */
timer_observer::~timer_observer()
{
    if (this->observable)
    {
        this->observable->detach(*this);
    }
}

timer_observer::timer_observer(expiration_type  type,
                               uint32_t         expiry_ticks)
    : cc_index(cc_index_unassigned),
      expiry_type(type),
      ticks_expiration(expiry_ticks),
      ticks_remaining(expiry_ticks),
      observable(nullptr)
{
}

void timer_observer::expiration_set(uint32_t ticks_expire, expiration_type type)
{
    this->ticks_expiration = ticks_expire;
    this->ticks_remaining  = ticks_expire;
    this->expiry_type      = type;
}

void timer_observer::expiration_set(uint32_t ticks_expire)
{
    this->ticks_expiration = ticks_expire;
    this->ticks_remaining  = ticks_expire;
}

void timer_observer::expiration_restart()
{
    this->ticks_remaining = this->ticks_expiration;
}

bool timer_observer::operator==(timer_observer const& other) const
{
    // Since timer_observable is not copyable in any form, the means for
    // testing equality is whether they are the same instance in memory.
    return (this == &other);
}

uint32_t timer_observer::get_ticks_remaining() const
{
    if ((this->expiry_type == expiration_type::one_shot) &&
        (this->ticks_remaining == 0u))
    {
        return UINT32_MAX;
    }

    return this->ticks_remaining;
}

void timer_observer::update_tick_count(uint32_t ticks_delta)
{
    if ((this->expiry_type == expiration_type::one_shot) &&
        (this->ticks_remaining == 0u))
    {
        // This one shot timer has already expired but has not
        // been removed from the queue.
        return;
    }

    int32_t const ticks_remain = this->ticks_remaining - ticks_delta;
    if (ticks_remain < epsilon)
    {
        if (this->expiry_type == expiration_type::continuous)
        {
            this->ticks_remaining = ticks_remain + this->ticks_expiration;
        }
        else
        {
            this->ticks_remaining = 0u;
        }

        this->expiration_notify();
    }
}

timer_observable::~timer_observable()
{
}

timer_observable::timer_observable(timer_instance_t  timer_instance,
                                   uint8_t           prescaler_exp,
                                   uint8_t           irq_priority):
    timer(timer_instance, prescaler_exp, irq_priority),
    cc_index_attach(0u)
{
}

void timer_observable::event_notify(cc_index_t cc_index, uint32_t cc_count)
{
    uint32_t const ticks_delta = this->ticks_update(cc_index, cc_count);
    this->cc_set(cc_index, cc_count + ticks_delta);
}

void timer_observable::attach(timer_observer& observer)
{
    ASSERT(not observer.is_linked());

    observer.observable = this;

    // If the cc_index was not already assigned to the observer
    // then attempt to evenly distribute the observers across the
    // comparators attached to the timer.
    observer.cc_index = this->cc_index_attach;
    this->cc_index_attach += 1u;
    if (this->cc_index_attach >= this->cc_count)
    {
        this->cc_index_attach = 0u;
    }

    // Note: The call to cc_get_count() is going to overwrite the CC value
    // we had previously stored to trigger events. That's OK since it will
    // be set to a new value when cc_set() is called.
    uint32_t timer_count = this->cc_get_count(observer.cc_index);
    uint32_t ticks_delta = this->ticks_update(observer.cc_index, timer_count);

    ticks_delta = std::min(ticks_delta, observer.ticks_expiration);
    this->cc_set(observer.cc_index, timer_count + ticks_delta);

    observer.ticks_remaining = observer.ticks_expiration;
    cc_assoc[observer.cc_index].observer_list.push_back(observer);

    if (this->attached_count() == 1u)
    {
        this->start();
    }
}

void timer_observable::detach(timer_observer& observer)
{
    ASSERT(observer.is_linked());

    observer.observable = nullptr;
    observer.cc_index   = timer_observer::cc_index_unassigned;

    cc_assoc[observer.cc_index].observer_list.remove(observer);
    if (cc_assoc[observer.cc_index].observer_list.size() == 0u)
    {
        this->cc_disable(observer.cc_index);

        if (this->attached_count() == 0u)
        {
            this->stop();
        }
    }
}

uint32_t timer_observable::ticks_update(cc_index_t cc_index, uint32_t cc_count)
{
    uint32_t const ticks_delta = cc_count - cc_assoc[cc_index].last_ticks_count;
    this->cc_assoc[cc_index].last_ticks_count = cc_count;

    uint32_t ticks_delta_next = UINT32_MAX;

    for (auto observer_iter = cc_assoc[cc_index].observer_list.begin();
         observer_iter != cc_assoc[cc_index].observer_list.end();
        )
    {
        timer_observer &observer = *observer_iter;
        ++observer_iter;

        // Note 1: Update ticks count before calculating ticks_remaining.
        // Note 2: Calling into the observer means that the observer might
        // detach at this point.
        // The (odd) iteration of the for loop is handles this gracefully.
        observer.update_tick_count(ticks_delta);
        ticks_delta_next = std::min(ticks_delta_next,
                                    observer.get_ticks_remaining());
    }

    return ticks_delta_next;
}

size_t timer_observable::attached_count() const
{
    size_t count = 0u;
    for (cc_index_t cc_index = 0u; cc_index < cc_count; ++cc_index)
    {
        count += cc_assoc[cc_index].observer_list.size();
    }

    return count;
}

