/**
 * @file timer_observer.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Wrap a timer peripheral to proivde a simpler interface.
 * @todo @bug Need volatiles & thread safety.
 */

#include "timer_observer.h"
#include "project_assert.h"
#include <algorithm>

timer_observer::~timer_observer()
{
    if (this->observable_)
    {
        this->observable_->detach(*this);
    }
}

timer_observer::timer_observer(expiration_type  type,
                               uint32_t         expiry_ticks)
    : observable_(nullptr),
      cc_index_(cc_index_unassigned),
      expiration_type_(type),
      ticks_expiration_(expiry_ticks),
      ticks_remaining_(expiry_ticks),
      is_expired_(false)
{
}

void timer_observer::expiration_set(uint32_t ticks_expire, expiration_type type)
{
    // Since UINT32_MAX is used as the max initializaion value within
    // timer_observable::ticks_update() and checked for this value
    // in timer_observable::event_notify() to determine whether the
    // CC counter should get updated, UINT32_MAX is an invaid ticks
    // expiration value.
    ASSERT(ticks_expire < UINT32_MAX);

    this->expiration_type_  = type;
    this->ticks_expiration_ = ticks_expire;
    this->ticks_remaining_  = ticks_expire;
    this->is_expired_       = false;

    if (this->observable_)
    {
        this->observable_->observer_ticks_update(*this);
    }
}

void timer_observer::expiration_set(uint32_t ticks_expire)
{
    this->expiration_set(ticks_expire, this->expiration_type_);
}

void timer_observer::expiration_set()
{
    this->expiration_set(this->ticks_expiration_, this->expiration_type_);
}

bool timer_observer::has_expired() const
{
    return this->is_expired_;
}

bool timer_observer::one_shot_has_expired() const
{
    return this->is_expired_ &&
          (this->expiration_get_type() == expiration_type::one_shot);
}

bool timer_observer::continuous_has_expired() const
{
    return this->is_expired_ &&
          (this->expiration_get_type() == expiration_type::continuous);
}

bool timer_observer::operator==(timer_observer const& other) const
{
    // Since timer_observable is not copyable in any form, the means for
    // testing equality is whether they are the same instance in memory.
    return (this == &other);
}

int32_t timer_observer::update_tick_count(uint32_t ticks_delta)
{
    this->ticks_remaining_ -= ticks_delta;
    if (static_cast<int32_t>(this->ticks_remaining_) < timer::epsilon)
    {
        this->is_expired_ = true;
        if (this->expiration_type_ == expiration_type::continuous)
        {
            this->ticks_remaining_ += this->ticks_expiration_;
        }
        else
        {
            // Do nothing in the case of the one_shot expiration.
            // The ticks_remaing value will be ignored within the observable
            // for this case.
        }
    }

    return static_cast<int32_t>(this->ticks_remaining_);
}

void timer_observer::expiration_reset()
{
    this->ticks_remaining_ = this->ticks_expiration_;
    this->is_expired_      = false;
}

timer_observable::~timer_observable()
{
}

timer_observable::timer_observable(timer_instance_t  timer_instance,
                                   uint8_t           prescaler_exp,
                                   uint8_t           irq_priority):
    timer(timer_instance, prescaler_exp, irq_priority),
    cc_index_attach_(0u),
    logger_(logger::instance())
{
}

void timer_observable::event_notify(cc_index_t cc_index, uint32_t cc_count)
{
    this->logger_.debug("notify[%d] +++", cc_index);

    uint32_t const ticks_delta = this->ticks_update(cc_index, cc_count);

    this->logger_.debug("notify[%d]: count : %10u + delta: %8u = %10u",
                        cc_index, cc_count, ticks_delta, cc_count + ticks_delta);

    if (ticks_delta != UINT32_MAX)
    {
        this->cc_set(cc_index, cc_count + ticks_delta);
    }

    for (auto observer_iter  = this->cc_assoc_[cc_index].observer_list.begin();
              observer_iter != this->cc_assoc_[cc_index].observer_list.end();)
    {
        timer_observer &observer = *observer_iter;
        ++observer_iter;

        if (observer.is_expired_)
        {
            if (observer.expiration_get_type() ==
                timer_observer::expiration_type::continuous)
            {
                observer.is_expired_ = false;
            }

            observer.expiration_notify();
        }
    }

    this->logger_.debug("notify[%d] ---", cc_index);
}

void timer_observable::attach(timer_observer& observer)
{
    ASSERT(not observer.is_attached());

    observer.observable_ = this;

    this->logger_.debug("attach[%d]:+++, this: 0x%p", observer.cc_index_, &observer);

    // If the cc_index was not already assigned to the observer
    // then attempt to evenly distribute the observers across the
    // comparators attached to the timer.
    observer.cc_index_          = this->cc_index_attach_;
    observer.cc_index_          = 0u;                       /// @todo debug only
    this->cc_index_attach_     += 1u;
    if (this->cc_index_attach_ >= this->cc_count)
    {
        this->cc_index_attach_ = 0u;
    }

    this->observer_ticks_update(observer);
    this->cc_assoc_[observer.cc_index_].observer_list.push_back(observer);

    if (this->attached_count() == 1u)
    {
        this->start();
    }

    this->logger_.debug("attach[%d]:---, this: 0x%p", observer.cc_index_, &observer);
}

void timer_observable::detach(timer_observer& observer)
{
    ASSERT(observer.is_attached());

    this->cc_assoc_[observer.cc_index_].observer_list.remove(observer);
    if (this->cc_assoc_[observer.cc_index_].observer_list.size() == 0u)
    {
        this->cc_disable(observer.cc_index_);

        if (this->attached_count() == 0u)
        {
            this->stop();
        }
    }

    observer.observable_ = nullptr;
    observer.cc_index_   = timer_observer::cc_index_unassigned;
}

void timer_observable::observer_ticks_update(timer_observer& observer)
{
    // Note: The call to cc_get_count() is going to overwrite the CC value
    // we had previously stored to trigger events. That's OK since it will
    // be set to a new value when cc_set() is called.
    uint32_t const timer_count = this->cc_get_count(observer.cc_index_);
    uint32_t       ticks_delta = this->ticks_update(observer.cc_index_, timer_count);

    this->logger_.debug("oticks[%d]: count : %10u,  delta: %8u",
                        observer.cc_index_, timer_count, ticks_delta);

    observer.expiration_reset();
    ticks_delta = std::min(ticks_delta, observer.ticks_expiration_);
    this->cc_set(observer.cc_index_, timer_count + ticks_delta);

    this->logger_.debug("oticks[%d]: count : %10u + delta: %8u = %10u / %10u",
                        observer.cc_index_,
                        timer_count, ticks_delta, timer_count + ticks_delta,
                        NRF_TIMER1->CC[0]);
}

int32_t timer_observable::ticks_update(cc_index_t cc_index, uint32_t cc_count)
{
    // The number of ticks expired since the last update.
    // These shall be subtracted from each observer when update_tick_count()
    // is called.
    int32_t const ticks_delta = cc_count -
                                this->cc_assoc_[cc_index].last_ticks_count;

    this->cc_assoc_[cc_index].last_ticks_count = cc_count;

    this->logger_.debug("update[%d]: count : %10u, delta: %8u +++",
                        cc_index, cc_count, ticks_delta);

    uint32_t ticks_next_delta = UINT32_MAX;

    for (timer_observer& observer : this->cc_assoc_[cc_index].observer_list)
    {
        if (observer.one_shot_has_expired())
        {
            // Do nothing, an expired one shot does not continue.
        }
        else
        {
            int32_t const ticks_remain = observer.update_tick_count(ticks_delta);
            if (ticks_remain <= timer::epsilon)
            {
                ticks_next_delta = timer::epsilon;
            }
            else
            {
                ticks_next_delta = std::min(static_cast<uint32_t>(ticks_remain),
                                            ticks_next_delta);
            }
        }

        this->logger_.debug("remain[%d]: %10u / %10u", cc_index,
                            observer.ticks_remaining_,
                            observer.ticks_expiration_);
    }

    this->logger_.debug("update[%d]: next_delta: %8u ---", cc_index, ticks_next_delta);
    return ticks_next_delta;
}

size_t timer_observable::attached_count() const
{
    size_t count = 0u;
    for (cc_index_t cc_index = 0u; cc_index < cc_count; ++cc_index)
    {
        count += this->cc_assoc_[cc_index].observer_list.size();
    }

    return count;
}

