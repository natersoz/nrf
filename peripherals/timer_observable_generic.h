/**
 * @file timer_observable.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Wrap a timer peripheral into an observer pattern.
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <array>
#include <boost/intrusive/list.hpp>

#include "project_assert.h"

/**
 * @class timer_observable_generic
 * Wrap a timer peripheral with the observerable part of the observer pattern.
 * Timer observers can attach to this observable and get timer expiration
 * notifications.
 *
 * @tparam timer_type
 * cc_index_limit may be larger than the actual number of comparators
 * associted with a timer. Use timer_type::cc_alloc_count for the actual count.
 *
 * @tparam observer_type The observer type that can attach to this observable.
 * @example timer_observer, rtc_observer.
 *
 * @tparam cc_index_limit The number of comparators available for the timer
 * to use when setting up time interval event notifications.
 * This value allocates the std::array cc_assoc_ private struct and is
 * over-allocated by default to 6.
 * Within the Nordic context TIMERs:
 * - [0:1] have 4 comparators active,
 * - [2:4] have 6 comparators active.
 * Within the Nordic context RTCs:
 * - [0]   has  3 comparators active
 * - [1:2] have 4 comparators active
 * The cc_index_limit can be changed from the default to eliminate wasted RAM.
 */
template <typename timer_type, typename observer_type, size_t cc_index_limit = 6u>
class timer_observable_generic : public timer_type
{
    friend observer_type;

public:
    using cc_index_t = uint8_t;

    static cc_index_t const cc_index_unassigned = static_cast<cc_index_t>(-1);

    timer_observable_generic()                                            = delete;
    timer_observable_generic(timer_observable_generic const&)             = delete;
    timer_observable_generic(timer_observable_generic&&)                  = delete;
    timer_observable_generic& operator=(timer_observable_generic const &) = delete;
    timer_observable_generic& operator=(timer_observable_generic&&)       = delete;

    virtual ~timer_observable_generic() override {}

    /**
     * The ctor; creates a time observable.
     *
     * @param timer_instance The index into the timer peripheral to use.
     *                       This will be an integer corresponding to
     *                       TIMER0, TIMER1, ... for timer_type using class timer.
     *                       RTC0,   RTC1,   ... for timer_type using class rtc.
     * @param prescaler_exp  For Noridc TIMER usage, the prescaler exponent
     *                       A 4 bit value where 1 << (prescaler - 1) is the
     *                       prescaler divisor value.
     *                       For Nordic RTC usage the actual prescaler divisor,
     *                       12-bits wide.
     * @param irq_priority   The comparator interrupt priority value.
     */
    timer_observable_generic(timer_instance_t   timer_instance,
                             uint8_t            prescaler,
                             uint8_t            irq_priority = 7u):
        timer_type(timer_instance, prescaler, irq_priority),
        cc_index_attach_(0u)
    {
    }

    /**
     * Handle the timer comparator notificaiton.
     *
     * @param cc_index The comparator which triggered the event.
     * @param cc_count The comparator count at the time of the trigger.
     */
    virtual void event_notify(cc_index_t cc_index, uint32_t cc_count) override
    {
        uint32_t const ticks_delta = this->ticks_update(cc_index, cc_count);
        if (ticks_delta != UINT32_MAX)
        {
            this->cc_set(cc_index, cc_count + ticks_delta);
        }

        for (auto observer_iter  = this->cc_assoc_[cc_index].observer_list_.begin();
                  observer_iter != this->cc_assoc_[cc_index].observer_list_.end();)
        {
            // Increment the iterator prior to using it.
            // If the client removes itself during the completion callback the
            // iterator will be valid and can continue.
            observer_type &observer = *observer_iter;
            ++observer_iter;

            if (observer.is_expired_)
            {
                if (observer.expiration_get_type() == observer_type::expiration_type::continuous)
                {
                    observer.is_expired_ = false;
                }

                observer.expiration_notify();
            }
        }
    }

    /**
     * Attach a timer observer to the observale.
     * This enables the observer to receive timer interval expiration event
     * notifications from the observable.
     *
     * @param observer The observer to attach and enable.
     */
    void attach(observer_type& observer)
    {
        ASSERT(not observer.is_attached());
        observer.observable_ = this;

        // Attempt to evenly distribute the observers across the
        // comparators attached to the timer.
        if (observer.cc_index_get() == cc_index_unassigned)
        {
            observer.cc_index_          = this->cc_index_attach_;
            this->cc_index_attach_     += 1u;
            if (this->cc_index_attach_ >= this->cc_alloc_count)
            {
                this->cc_index_attach_ = 0u;
            }
        }

        if (profile_using_only_cc0)
        {
            observer.cc_index_ = 0u;
        }

        this->observer_ticks_update(observer);
        this->cc_assoc_[observer.cc_index_].observer_list_.push_back(observer);

        if (this->attached_count() == 1u)
        {
            this->start();
        }
    }

    /**
     * Detach a timer from the observable thereby disabling timer interval
     * expiration event notifications.
     *
     * @param observer The observer to detach.
     */
    void detach(observer_type& observer)
    {
        ASSERT(observer.is_attached());

        this->cc_assoc_[observer.cc_index_].observer_list_.remove(observer);
        if (this->cc_assoc_[observer.cc_index_].observer_list_.size() == 0u)
        {
            this->cc_disable(observer.cc_index_);

            if (this->attached_count() == 0u)
            {
                this->stop();
            }
        }

        observer.observable_ = nullptr;
    }

private:
    /**
     * Debug/testing only: set to true to force all observers to be
     * assigned to CC[0]. This creates a 'more congested' situation for
     * profiling the performance related to notification processing.
     */
    static constexpr bool const profile_using_only_cc0 = false;

    /// @note Do not call the hook method unlink().
    /// Always call timer_observable_generic::detach() to keep the attached
    /// count correct.
    using observer_list =
        boost::intrusive::list<
            observer_type,
            boost::intrusive::member_hook<observer_type,
                                          boost::intrusive::list_member_hook< >,
                                          &observer_type::hook_> >;
    /**
     * @struct cc_association
     * Information associated with each timer comparator.
     */
    struct cc_association
    {
        /// Each comparator is allocated a list of observers.
        observer_list observer_list_;

        /// The last tick count for which all nodes within the observer_list_
        /// have been updated.
        uint32_t last_ticks_count_;
    };

    /// For each timer comparator a cc_association instance.
    std::array<cc_association, cc_index_limit> cc_assoc_;

    /// Used distribute observers across the comparator array.
    cc_index_t cc_index_attach_;

    /**
     * When the observable gets attached or the observable is attached and
     * the expiration is changed this function is called to get the observer
     * expiration ticks remaining count into the comparator count down.
     *
     * @param observer The observer whose expirations are being changed.
     */
    void observer_ticks_update(observer_type& observer)
    {
        // Note: The call to cc_get_count() is going to overwrite the CC value
        // we had previously stored to trigger events. That's OK since it will
        // be set to a new value when cc_set() is called.
        uint32_t const timer_count = this->cc_get_count(observer.cc_index_);
        uint32_t       ticks_delta = this->ticks_update(observer.cc_index_, timer_count);

        observer.expiration_reset();
        ticks_delta = (ticks_delta < observer.ticks_expiration_)?
                       ticks_delta : observer.ticks_expiration_;
        this->cc_set(observer.cc_index_, timer_count + ticks_delta);
    }

    /**
     * When an event fires or when a new observers is added this function
     * is called.
     *
     * @param cc_index The comparator to associate the event/insertion with.
     * @param cc_count The comparator count which either triggered the event
     *                 or the counter value at the time of insertion.
     *
     * @return uint32_t The counter value increase from the current value
     *                  for which the next event should be triggered.
     *                  i.e. The next CC counter comparator value.
     */
    uint32_t ticks_update(cc_index_t cc_index, uint32_t cc_count)
    {
        uint32_t const counter_mask = (timer_type::counter_width < 32u) ?
            ((1u << timer_type::counter_width) - 1u) : UINT32_MAX ;

        // The number of ticks expired since the last update.
        // These shall be subtracted from each observer when update_tick_count()
        // is called.
        uint32_t const ticks_delta = (cc_count -
            this->cc_assoc_[cc_index].last_ticks_count_) & counter_mask;

        this->cc_assoc_[cc_index].last_ticks_count_ = cc_count;

        uint32_t ticks_next_delta = UINT32_MAX;
        for (observer_type& observer : this->cc_assoc_[cc_index].observer_list_)
        {
            if (observer.one_shot_has_expired())
            {
                // Do nothing, an expired one shot does not continue.
            }
            else
            {
                int32_t const ticks_remain = observer.update_tick_count(ticks_delta);
                if (ticks_remain <= timer_type::epsilon)
                {
                    ticks_next_delta = timer_type::epsilon;
                }
                else
                {
                    ticks_next_delta = std::min(static_cast<uint32_t>(ticks_remain),
                                                ticks_next_delta);
                }
            }
        }

        return ticks_next_delta;
    }

    /** @return size_t The total number of timer observers attached. */
    size_t attached_count() const
    {
        size_t count = 0u;
        for (cc_index_t cc_index = 0u; cc_index < this->cc_alloc_count; ++cc_index)
        {
            count += this->cc_assoc_[cc_index].observer_list_.size();
        }

        return count;
    }
};

