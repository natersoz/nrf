/**
 * @file timer_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Wrap an TIMER peripheral into an observer and observale pattern.
 */

#pragma once

#include "timer.h"
#include "timer_observer_generic.h"
#include "timer_observable_generic.h"
#include "project_assert.h"

using timer_observer = timer_observer_generic<timer>;

/**
 * @class timer_observable
 * Inherit from timer_observable_generic so that the class timer_observable
 * can be used easily without the need to puzzle through the template parameters.
 */
template <size_t cc_index_limit = 6u>
class timer_observable : public timer_observable_generic<timer,
                                                         timer_observer,
                                                         cc_index_limit>
{
public:
    timer_observable()                                    = delete;
    timer_observable(timer_observable const&)             = delete;
    timer_observable(timer_observable&&)                  = delete;
    timer_observable& operator=(timer_observable const &) = delete;
    timer_observable& operator=(timer_observable&&)       = delete;

    virtual ~timer_observable() override = default;

    /** @see timer_init comments for more information. */
    timer_observable(timer_instance_t   timer_instance,
                   uint8_t              prescaler_exp = 4u,    // 1 MHz default clock tick
                   uint8_t              irq_priority  = 7u):
        timer_observable_generic<timer, timer_observer, cc_index_limit>(
            timer_instance, prescaler_exp, irq_priority)
    {
    }
};

