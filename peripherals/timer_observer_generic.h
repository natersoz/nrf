/**
 * @file timer_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Wrap a timer peripheral to proivde a simpler interface.
 */

#pragma once

#include "timer.h"
#include "timer_observable_generic.h"
#include "project_assert.h"

#include <cstddef>
#include <cstdint>

/**
 * @class timer_observer_generic
 * This is the part of the timer class intended to be used by a timer client.
 * In the user's class override the event_notify method for timer notifications.
 *
 * @note A expiration tick count of UINT32_MAX is used within this class
 * to mean that the observer is disabled.
 */
template <typename timer_type>
class timer_observer_generic
{
    using observable_type = timer_observable_generic<timer_type, timer_observer_generic<timer_type>, 6u>;
    friend observable_type;

public:
    using cc_index_t = uint8_t;

    enum class expiration_type
    {
        one_shot,
        continuous
    };

    timer_observer_generic(timer_observer_generic const&)             = delete;
    timer_observer_generic(timer_observer_generic &&)                 = delete;
    timer_observer_generic& operator=(timer_observer_generic const&)  = delete;
    timer_observer_generic& operator=(timer_observer_generic&&)       = delete;

    virtual ~timer_observer_generic()
    {
        if (this->observable_)
        {
            this->observable_->detach(*this);
        }
    }

    timer_observer_generic():
        observable_(nullptr),
        cc_index_(observable_type::cc_index_unassigned),
        expiration_type_(expiration_type::one_shot),
        ticks_expiration_(UINT32_MAX),
        ticks_remaining_(UINT32_MAX),
        is_expired_(false)
    {
    }

    timer_observer_generic(expiration_type expiry_type, uint32_t expiry_ticks):
        observable_(nullptr),
        cc_index_(observable_type::cc_index_unassigned),
        expiration_type_(expiry_type),
        ticks_expiration_(expiry_ticks),
        ticks_remaining_(expiry_ticks),
        is_expired_(false)
    {
    }

    /**
     * When the timer_observer_generic expiration interval completes this function
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
    void expiration_set(expiration_type type, uint32_t ticks_expire)
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

    void expiration_set(expiration_type type)
    {
        this->expiration_set(type, this->ticks_expiration_);
    }

    void expiration_set(uint32_t ticks_expire)
    {
        this->expiration_set(this->expiration_type_, ticks_expire);
    }

    /** Reset a one-shot timer's remainging to its expiration time */
    void expiration_set()
    {
        this->expiration_set(this->expiration_type_, this->ticks_expiration_);
    }
    /** @} */

    /// @{ Expiration determination, also with expiration types
    bool has_expired() const { return this->is_expired_; }

    bool one_shot_has_expired() const
    {
        return this->is_expired_ &&
              (this->expiration_get_type() == expiration_type::one_shot);
    }

    bool continuous_has_expired() const
    {
        return this->is_expired_ &&
              (this->expiration_get_type() == expiration_type::continuous);
    }
    /// @}

    uint32_t        expiration_get_ticks() const { return this->ticks_expiration_; }
    expiration_type expiration_get_type()  const { return this->expiration_type_; }

    cc_index_t cc_index_get() const { return this->cc_index_; }

    observable_type const* attached_observable() const { return this->observable_; }

    /**
     * Since timer_observable is not copyable in any form, the means for
     * testing equality is whether they are the same instance in memory.
     */
    bool operator==(timer_observer_generic const& other) const { return (this == &other); }

private:
    /// The timer_observable to which this oberser is attached.
    /// If null then the observer is unattached.
    observable_type * volatile observable_;

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

    /// The ticks value set for determining when the timer observer expires.
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
    int32_t update_tick_count(uint32_t ticks_delta)
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

    void expiration_reset()
    {
        this->ticks_remaining_ = this->ticks_expiration_;
        this->is_expired_      = false;
    }
};
