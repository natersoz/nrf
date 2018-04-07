/**
 * @file timer_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Wrap a timer peripheral to proivde a simpler interface.
 */

#pragma once

#include "timer.h"
#include "logger.h"
#include <array>
#include <boost/intrusive/list.hpp>

class timer_observable;

/**
 * @class timer_observer
 * This is the part of the timer class intended to be used by a timer client.
 * In the user's class override the event_notify method for timer notifications.
 */
class timer_observer
{
    friend class timer_observable;

public:
    static cc_index_t const cc_index_unassigned = static_cast<cc_index_t>(-1);

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

    timer_observer(expiration_type type, uint32_t expiration_ticks);

    /**
     * When the timer_observer expiration interval completes this function
     * is called.
     */
    virtual void expiration_notify() = 0;

    bool is_attached() const { return bool(this->observable_); }

    /** @{
     * Set the expiration time in timer ticks and the expiration type.
     * @note The value UINT32_MAX is prohibited and will cause and ASSERT failure.
     *
     * @param ticks_expire The number of ticks to wait until the
     * expiration_notify() event is triggered.
     * @param expiration_type continuous or one-shot.
     */
    void expiration_set(uint32_t ticks_expire, expiration_type);
    void expiration_set(uint32_t ticks_expire);
    void expiration_set();
    /** @{ */

    /// @{ Expiration determination, also with expiration types
    bool has_expired() const;
    bool one_shot_has_expired() const;
    bool continuous_has_expired() const;
    /// @}

    uint32_t        expiration_get_ticks() const { return this->ticks_expiration_; }
    expiration_type expiration_get_type()  const { return this->expiration_type_; }

    cc_index_t cc_index_get() const { return this->cc_index_; }

    timer_observable const* observable() const { return this->observable_; }

    bool operator==(timer_observer const& other) const;

private:
    /// The timer_observable to which this oberser is attached.
    /// If null then the observer is unattached.
    timer_observable * volatile observable_;

    /// Alias the list hood class to something a little
    using list_hook = boost::intrusive::list_member_hook<>;

    /// @todo needs volatile
    list_hook hook_;

    /// The timer comarator to assign this observer to.
    /// This value will be assiged by timer_observable when
    /// the observer is attached.
    timer::cc_index_t volatile cc_index_;

    /// Single shot or continuous.
    expiration_type volatile expiration_type_;

    /// The ticks value set for determining when the timer_observer expires.
    uint32_t volatile ticks_expiration_;

    /// The number of ticks remaining before the timer observer expires.
    uint32_t volatile ticks_remaining_;

    /// The observer has expired, but the expiration_notify()
    /// has not yet been called.
    bool volatile is_expired_;

    /**
     * Update the ticks_remaining value.
     *
     * @param ticks_delta The number of ticks since the last update.
     *
     * @return int32_t The ticks remaining before expiration.
     * If the timer observer has expired this value will be < timer::epsilon.
     * Negative values can be returned for expirations occurring late.
     */
    int32_t update_tick_count(uint32_t ticks_delta);

    void expiration_reset();
};

class timer_observable : public timer
{
    friend class timer_observer;

public:
    virtual ~timer_observable() override;

    timer_observable()                          = delete;
    timer_observable(timer_observable const&)   = delete;
    timer_observable(timer_observable&&)        = delete;

    timer_observable& operator=(timer_observable const &) = delete;
    timer_observable& operator=(timer_observable&&)       = delete;

    /**
     * Attach a timer_observable to a Nordic TIMER peripheral by specifying
     * the timer_instance [0:4] associated with [TIMER0:TIMER4].
     * The preprpcessor definition -D TIMERn will need to be set for the
     * ISR handler to hook correctly (otherwise link-time error).
     *
     * @param timer_instance Specifies the specific peripheral to use.
     * @param prescaler_exp  Specify the HFCLK / N prescaler value.
     * @param irq_priority   The NVIC priority.
     */
    timer_observable(timer_instance_t  timer_instance,
                     uint8_t           prescaler_exp = 4u,
                     uint8_t           irq_priority  = 7u);

    virtual void event_notify(cc_index_t cc_index, uint32_t cc_count) override;

    /** @{
     * Attache and detach observers to observables.
     * When attached to observers are active.
     * To deactive an observer, detach it.
     *
     * @param observer The observer to attach to an observable.
     */
    void attach(timer_observer& observer);
    void detach(timer_observer& observer);

private:
    /// @note cc_index_limit may be larger than the actual number of comparators
    /// associted with a timer. Use timer::cc_count for the actual count.
    static constexpr cc_index_t const cc_index_limit = 6u;

    /// @note Do not call the hook method unlink().
    /// Always call timer_observable::detach() to keep the attached count correct.
    using timer_observer_list =
        boost::intrusive::list<
            timer_observer,
            boost::intrusive::member_hook<timer_observer,
                                          boost::intrusive::list_member_hook< >,
                                          &timer_observer::hook_> >;
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
        uint32_t volatile last_ticks_count;
    };

    /// For each timer comparator a cc_association instance.
    std::array<cc_association, cc_index_limit> cc_assoc_;

    /// Used distribute observers across the comparator array.
    cc_index_t cc_index_attach_;

    logger& logger_;

    /**
     * When the observable gets attached or the observable is attached and
     * the expiration is changed this function is called to get the observer
     * expiration ticks remaining count into the comparator count down.
     *
     * @param observer The observer whose expirations are being changed.
     */
    void observer_ticks_update(timer_observer& observer);

    /**
     * When an event fires or when a new observers is added this function
     * is called.
     *
     * @param cc_index The comparator to associate the event/insertion with.
     * @param cc_count The comparator count which either triggered the event
     *                 or the counter value at the time of insertion.
     *
     * @return uint32_t The number of ticks remaining within a specific
     *                  comparator index grouping of observers.
     *                  aka: The next event expiration tick count.
     */
    int32_t ticks_update(cc_index_t cc_index, uint32_t cc_count);

    /** The total number of timer observers attached. */
    size_t attached_count() const;
};

