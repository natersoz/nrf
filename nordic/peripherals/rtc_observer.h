/**
 * @file rtc_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Wrap an RTC peripheral into an observer and observale pattern.
 */

#pragma once

#include "rtc.h"
#include "timer_observer_generic.h"
#include "timer_observable_generic.h"
#include "project_assert.h"

using rtc_observer = timer_observer_generic<rtc>;

/**
 * @class rtc_observable
 * Inherit from timer_observable_generic so that the class rtc_observable
 * can be used easily without the need to puzzle through the template parameters.
 */
template <size_t cc_index_limit = 6u>
class rtc_observable : public timer_observable_generic<rtc,
                                                       rtc_observer,
                                                       cc_index_limit>
{
public:
    rtc_observable()                                  = delete;
    rtc_observable(rtc_observable const&)             = delete;
    rtc_observable(rtc_observable&&)                  = delete;
    rtc_observable& operator=(rtc_observable const &) = delete;
    rtc_observable& operator=(rtc_observable&&)       = delete;

    virtual ~rtc_observable() override = default;

    /** @see rtc_init comments for more information. */
    rtc_observable(timer_instance_t  rtc_instance,
                   uint8_t           prescaler    = 32u,    // 1024 Hz default clock tick
                   uint8_t           irq_priority =  7u):
        timer_observable_generic<rtc, rtc_observer, cc_index_limit>(
            rtc_instance, prescaler, irq_priority)
    {
    }
};
