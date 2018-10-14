/**
 * @file rtc.h A Real Time Counter class based on the Nordic RTC peripheral.
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t rtc_instance_t;
typedef uint8_t rtc_cc_index_t;

typedef void (* rtc_event_handler_t) (void              *context,
                                      rtc_cc_index_t    cc_index,
                                      uint32_t          cc_count);

/**
 * Initialize the RTC module.
 * @note The clock module LFCLK is not initialized here and needs
 * to be performed separately.
 *
 * @param rtc_instance
 * @param prescaler The valid prescaler range is >= 1, <= prescaler_max;
 *                  where prescaler_max = (1u << 11u).
 *                  The RTC clock frequency is 32,768 Hz / prescaler.
 * @note  The RTC prescaler value stored in the PRESCALER register is one
 *        less than the value set here.
 *
 * @param irq_priority The timer interrupt priority.
 *                     Event notifications are handled at this irq level.
 * @param handler      The user provided timer notificaiton event handler.
 * @param context      The user supplied context; unmodified by the driver.
 */
void rtc_init(rtc_instance_t            rtc_instance,
              uint32_t                  prescaler,
              uint8_t                   irq_priority,
              rtc_event_handler_t       handler,
              void                      *context);

void rtc_deinit(rtc_instance_t rtc_instance);

void rtc_start(rtc_instance_t rtc_instance);

void rtc_stop(rtc_instance_t rtc_instance);

void rtc_reset(rtc_instance_t rtc_instance);

void rtc_cc_set(rtc_instance_t  rtc_instance,
                rtc_cc_index_t  cc_index,
                uint32_t        rtc_ticks);

uint32_t rtc_cc_get(rtc_instance_t rtc_instance, rtc_cc_index_t cc_index);

uint32_t volatile* rtc_cc_get_event(rtc_instance_t rtc_instance, rtc_cc_index_t cc_index);

uint32_t rtc_cc_get_count(rtc_instance_t rtc_instance);

uint64_t rtc_get_count_ext(rtc_instance_t rtc_instance);

void rtc_cc_disable(rtc_instance_t rtc_instance, rtc_cc_index_t cc_index);

uint32_t rtc_ticks_per_second(rtc_instance_t rtc_instance);

void rtc_enable_interrupt(rtc_instance_t rtc_instance);

#ifdef __cplusplus
}

class rtc
{
public:
    using cc_index_t = rtc_cc_index_t;

    /// The bit-wdith of the counter.
    static size_t const counter_width = 24u;

    /**
     * If the ticks_remaining count is within this value the timer is expired.
     * This is to avoid the situation where the waiting for another update
     * call into update_tick_count() would be a worse estimate for timer
     * expiration than expiring in the current cycle.
     */
    static int32_t const epsilon = 4u;

    cc_index_t const cc_alloc_count;

    virtual ~rtc();

    rtc()                       = delete;
    rtc(rtc const&)             = delete;
    rtc(rtc &&)                 = delete;
    rtc& operator=(rtc const&)  = delete;
    rtc& operator=(rtc&&)       = delete;

    rtc(rtc_instance_t  rtc_instance,
        uint8_t         prescaler     = 1u,     // 32,768 Hz clock source.
        uint8_t         irq_priority  = 7u);

    void start();
    void stop();
    void reset();

    void     cc_set(cc_index_t cc_index, uint32_t rtc_ticks);
    uint32_t cc_get(cc_index_t) const;
    uint32_t volatile* cc_get_event(cc_index_t);
    uint32_t cc_get_count(cc_index_t cc_index) const;
    uint32_t cc_get_count() const;
    uint32_t get_count_extend_32() const;
    uint64_t get_count_extend_64() const;
    void     cc_disable(cc_index_t cc_index);

    uint32_t ticks_per_second() const;
    uint32_t usec_to_ticks(uint32_t usec) const;
    uint32_t msec_to_ticks(uint32_t msec) const;

    /**
     * The rtc notificaiton virtual method for receiving callbacks when
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
    rtc_instance_t  rtc_instance_;
    uint64_t        rtc_counter_extension;
};

#endif

