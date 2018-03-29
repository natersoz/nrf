/**
 * @file A timer_counter class based on the Nordic timer/counter peripheral.
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "nrf_cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t timer_instance_t;
typedef uint8_t cc_index_t;

typedef void (* timer_event_handler_t) (void                *context,
                                        cc_index_t          cc_index,
                                        uint32_t            cc_count);
enum timer_mode_t
{
    timer_mode_timer,
    timer_mode_counter,
};

void timer_init(timer_instance_t        timer_instance,
                enum timer_mode_t       timer_mode,
                uint8_t                 prescaler_exp,
                uint8_t                 irq_priority,
                timer_event_handler_t   handler,
                void                    *context);

void timer_deinit(timer_instance_t timer_instance);

void timer_start(timer_instance_t timer_instance);

void timer_stop(timer_instance_t timer_instance);

void timer_reset(timer_instance_t timer_instance);

void timer_cc_set(timer_instance_t  timer_instance,
                  cc_index_t        cc_index,
                  uint32_t          timer_ticks);

uint32_t timer_cc_get(timer_instance_t timer_instance, cc_index_t cc_index);

uint32_t timer_cc_get_count(timer_instance_t timer_instance);

void timer_cc_disable(timer_instance_t timer_instance, cc_index_t cc_index);

uint32_t timer_ticks_per_second(timer_instance_t timer_instance);

void timer_enable_interrupt(timer_instance_t timer_instance);


#ifdef __cplusplus
}

class timer
{
public:
    typedef uint8_t cc_index_t;

    cc_index_t const cc_count;

    virtual ~timer();

    timer()                         = delete;
    timer(timer const&)             = delete;
    timer(timer &&)                 = delete;
    timer& operator=(timer const&)  = delete;
    timer& operator=(timer&&)       = delete;

    timer(timer_instance_t  timer_instance,
          uint8_t           prescaler_exp = 4u,     // 1 MHz clock tick
          uint8_t           irq_priority  = 7u);

    void start();
    void stop();
    void reset();

    void     cc_set(cc_index_t cc_index, uint32_t timer_ticks);
    uint32_t cc_get(cc_index_t) const;
    uint32_t cc_get_count(cc_index_t cc_index) const;
    void     cc_disable(cc_index_t cc_index);

    uint32_t ticks_per_second() const;
    uint32_t usec_to_ticks(uint32_t usec) const;
    uint32_t msec_to_ticks(uint32_t msec) const;

    virtual void event_notify(cc_index_t cc_index, uint32_t cc_count) = 0;

private:
    timer_instance_t timer_instance_;
};

#endif

