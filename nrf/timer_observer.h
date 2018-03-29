/**
 * @file timer_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Wrap a timer peripheral to proivde a simpler interface.
 */

#pragma once

#include "timer.h"
#include <array>
#include <boost/intrusive/list.hpp>

class timer_observable;

class timer_observer: public boost::intrusive::list_base_hook<>
{
    friend class timer_observable;

public:
    static cc_index_t const cc_index_unassigned = -1;

    /// If the ticks_remaining count is within this value the timer is expired.
    /// This is to avoid the situation where the waiting for another update
    /// call into update_tick_count() would be a worse estimate for timer
    /// expiration than expiring in the current cycle.
    static int32_t const epsilon = 10u;

    enum class expiration_type
    {
        one_shot,
        continuous
    };

    ~timer_observer();
    timer_observer()                                  = delete;
    timer_observer(timer_observer const&)             = delete;
    timer_observer(timer_observer &&)                 = delete;
    timer_observer& operator=(timer_observer const&)  = delete;
    timer_observer& operator=(timer_observer&&)       = delete;

    timer_observer(expiration_type type, uint32_t expiry_ticks);

    /**
     * When the timer_observer expiration interval completes this function
     * is called.
     */
    virtual void expiration_notify() = 0;

    bool is_attached() const { return this->is_linked(); }

    void expiration_set(uint32_t ticks_expire, expiration_type);
    void expiration_set(uint32_t ticks_expire);
    void expiration_restart();

    uint32_t        expiration_get_ticks() const { return this->ticks_expiration; }
    expiration_type expiration_get_type()  const { return this->expiry_type; }

    cc_index_t cc_index_get() const { return this->cc_index; }

    bool operator==(timer_observer const& other) const;

private:
    /// The timer comarator to assign this observer to.
    /// This value will be assiged by timer_observable when
    /// the observer is attached.
    timer::cc_index_t cc_index;

    /// Single shot or continuous.
    expiration_type expiry_type;

    /// The ticks value set for determining when the timer_observer expires.
    uint32_t ticks_expiration;

    /// The number of ticks remaining before the timer observer expires.
    uint32_t ticks_remaining;

    timer_observable *observable;

    /**
     * @note If expired then ticks remaining should be ticks_expiration.
     */
    uint32_t get_ticks_remaining() const;

    /**
     * Update the ticks_remaining value.
     *
     * @param ticks_delta The number of ticks since the last update.
     *
     * @return bool true if the timer observer has expired.
     * false if the timer is still counting down.
     */
    void update_tick_count(uint32_t ticks_delta);
};

class timer_observable : public timer
{
public:
    virtual ~timer_observable() override;

    timer_observable()                          = delete;
    timer_observable(timer_observable const&)   = delete;
    timer_observable(timer_observable&&)        = delete;

    timer_observable& operator=(timer_observable const &) = delete;
    timer_observable& operator=(timer_observable&&)       = delete;

    timer_observable(timer_instance_t  timer_instance,
                     uint8_t           prescaler_exp = 4u,
                     uint8_t           irq_priority  = 7u);

    virtual void event_notify(cc_index_t cc_index, uint32_t cc_count) override;

    void attach(timer_observer& observer);
    void detach(timer_observer& observer);

private:
    /// @note cc_index_limit may be larger than the actual number of comparators
    /// associted with a timer. Use timer::cc_count for the actual count.
    static constexpr cc_index_t const cc_index_limit = 6u;

    /// @note Do not call the hook method unlink().
    /// Always call timer_observable::detach() to keep the attached count correct.
    using timer_observer_list =
        boost::intrusive::list<timer_observer,
                               boost::intrusive::constant_time_size<true> >;
    /**
     * @struct cc_association
     * Information associated with each timer comparator.
     */
    struct cc_association
    {
        /// The observer list
        timer_observer_list observer_list;

        /// The last tick count for which all nodes within the observer_list
        /// have been updated.
        uint32_t last_ticks_count;
    };

    /// For each timer comparator a cc_association instance.
    std::array<cc_association, cc_index_limit> cc_assoc;

    /// Used distribute observers across the comparator array.
    cc_index_t cc_index_attach;

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
     */
    uint32_t ticks_update(cc_index_t cc_index, uint32_t cc_count);

    size_t attached_count() const;
};

