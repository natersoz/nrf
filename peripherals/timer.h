/**
 * @file A timer_counter class based on the Nordic timer/counter peripheral.
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t timer_instance_t;
typedef uint8_t timer_cc_index_t;

typedef void (* timer_event_handler_t) (void                *context,
                                        timer_cc_index_t    cc_index,
                                        uint32_t            cc_count);
enum timer_mode_t
{
    timer_mode_timer,
    timer_mode_counter,
};

/**
 * Initialize the TIMER module. The TIMER module is driven by HFCLK at 16 MHz.
 *
 * @param timer_instance
 * @param prescaler_exp This is the prescaler 'exponent' value.
 *                      The prescaler divisor N = (1 << prescaler_exp).
 * Therefore a prescaler_exp value of 0 provides N = 1.
 * Since HFCLK is 16 MHz, if prescaler_exp = 1, timer clock F = 16 MHz.
 * Since HFCLK is 16 MHz, if prescaler_exp = 4, timer clock F =  1 MHz.
 *
 * @param irq_priority The timer interrupt priority.
 *                     Event notifications are handled at this irq level.
 * @param handler      The user provided timer notificaiton event handler.
 * @param context      The user supplied context; unmodified by the driver.
 */
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
                  timer_cc_index_t  cc_index,
                  uint32_t          timer_ticks);

uint32_t timer_cc_get(timer_instance_t timer_instance, timer_cc_index_t cc_index);

uint32_t volatile* timer_cc_get_event(timer_instance_t timer_instance, timer_cc_index_t cc_index);

uint32_t timer_cc_get_count(timer_instance_t timer_instance);

void timer_cc_disable(timer_instance_t timer_instance, timer_cc_index_t cc_index);

uint32_t timer_ticks_per_second(timer_instance_t timer_instance);

void timer_enable_interrupt(timer_instance_t timer_instance);


#ifdef __cplusplus
}

class timer
{
public:
    using cc_index_t = timer_cc_index_t;

    /// The bit-wdith of the counter.
    static size_t const counter_width = 32u;

    /**
     * If the ticks_remaining count is within this value the timer is expired.
     * This is to avoid the situation where the waiting for another update
     * call into update_tick_count() would be a worse estimate for timer
     * expiration than expiring in the current cycle.
     *
     * Using the timer_test application 6 timers are hooked up to a single
     * comparator. An epsilon value of 500u was most accurate and large enough
     * to not missing interrupts.
     */
    static int32_t const epsilon = 500u;

    cc_index_t const cc_alloc_count;

    virtual ~timer();

    timer()                         = delete;
    timer(timer const&)             = delete;
    timer(timer &&)                 = delete;
    timer& operator=(timer const&)  = delete;
    timer& operator=(timer&&)       = delete;

    timer(timer_instance_t  timer_instance,
          uint8_t           prescaler_exp = 4u,     // 1 MHz default clock tick
          uint8_t           irq_priority  = 7u);

    void start();
    void stop();
    void reset();

    void     cc_set(cc_index_t cc_index, uint32_t timer_ticks);
    uint32_t cc_get(cc_index_t) const;
    uint32_t volatile* cc_get_event(cc_index_t);
    uint32_t cc_get_count(cc_index_t cc_index) const;
    void     cc_disable(cc_index_t cc_index);

    uint32_t ticks_per_second() const;
    uint32_t usec_to_ticks(uint32_t usec) const;
    uint32_t msec_to_ticks(uint32_t msec) const;

    /**
     * The timer notificaiton virtual method for receiving callbacks when
     * timer comaprator events happen. The default case is to do nothing.
     * Doing nothing allows for a simple polling timer within inheritance.
     *
     * @param cc_index The comparator index which triggered the event callback.
     * @param cc_count The comparator value at the time of the callback.
     */
    virtual void event_notify(cc_index_t cc_index, uint32_t cc_count)
    {
        (void) cc_index;
        (void) cc_count;
    };

private:
    timer_instance_t timer_instance_;
};

#endif

