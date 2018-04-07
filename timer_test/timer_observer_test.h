/**
 * @file timer_observer_test.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Test for timer observer latency class.
 */

#pragma once

#include "timer_observer.h"
#include "rtc.h"
#include "logger.h"
#include "bit_manip.h"
#include <cmath>

class timer_observer_test: public timer_observer
{
public:
    timer_observer_test(char const      *timer_name,
                        expiration_type type,
                        uint32_t        expiration_ticks,
                        timer           &timer_reference)
        : timer_observer(type, expiration_ticks),
          name_(timer_name),
          timer_reference_(timer_reference),
          notification_count_(0u),
          last_notification_ticks_(0u)
    {
    }

    /**
     * When a timer is attached this funciton should be called to
     * set the base notification ticks. Especially important for
     * one shot timers since they don't accumulate this value.
     *
     * @param counter_ticks The reference timer tick value.
     */
    void ticks_start_set(uint32_t counter_ticks)
    {
        this->last_notification_ticks_ = counter_ticks;
    }

    void update_stats()
    {
        uint32_t const ref_ticks_count = this->timer_reference_.cc_get_count(0u);

        int32_t const delta_ref = ref_ticks_count - this->last_notification_ticks_;
        int32_t const error     = this->expiration_get_ticks() - delta_ref;

        logger &logger = logger::instance();
        logger.debug("%s[%3d]: ticks: %10d - %10d, delta_ref: %10d, error: %10d",
                    this->name_,
                    this->cc_index_get(),
                    ref_ticks_count,
                    this->last_notification_ticks_,
                    delta_ref,
                    error);

        error_stats.min  = std::min(error, error_stats.min);
        error_stats.max  = std::max(error, error_stats.max);
        error_stats.avg += error;

        this->notification_count_     += 1u;
        this->last_notification_ticks_ = ref_ticks_count;
    }

    void log_stats()
    {
        if (this->notification_count_ > 0u)
        {
            int32_t const average =
                this->error_stats.avg / static_cast<int32_t>(this->notification_count_);

            logger &logger = logger::instance();
            logger.info("%s[%3d]: min: %8d, max: %8d, avg: %10d, n: %10u",
                        this->name_,
                        this->cc_index_get(),
                        this->error_stats.min,
                        this->error_stats.max,
                        average,
                        this->notification_count_
                        );
        }
    }

private:
    struct timer_error
    {
        timer_error(): min(0), max(0), avg(0) {}
        int32_t min;
        int32_t max;
        int32_t avg;
    };

    struct timer_error  error_stats;
    char const* const   name_;
    timer               &timer_reference_;
    uint32_t volatile   notification_count_;
    uint32_t volatile   last_notification_ticks_;
};

