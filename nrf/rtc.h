/**
 * @file rtc.h A Real Time Counter class based on the Nordic RTC peripheral.
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "nrf_cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t rtc_instance_t;
typedef uint8_t cc_index_t;

typedef void (* rtc_event_handler_t) (void          *context,
                                      cc_index_t    cc_index,
                                      uint32_t      cc_count);

/**
 *
 *
 *
 * @param rtc_instance
 * @param prescaler The valid prescaler range is >= 1, <= prescaler_max;
 *                  where prescaler_max = (1u << 11u).
 *                  The RTC clock frequency is 32,768 Hz / prescaler.
 * @note  The RTC prescaler value stored in the PRESCALER register is one
 *        less than the value set here.
 * @param irq_priority
 * @param handler
 * @param context
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
                cc_index_t      cc_index,
                uint32_t        rtc_ticks);

uint32_t rtc_cc_get(rtc_instance_t rtc_instance, cc_index_t cc_index);

uint32_t rtc_cc_get_count(rtc_instance_t rtc_instance);

uint64_t rtc_get_count_ext(rtc_instance_t rtc_instance);

void rtc_cc_disable(rtc_instance_t rtc_instance, cc_index_t cc_index);

uint32_t rtc_ticks_per_second(rtc_instance_t rtc_instance);

void rtc_enable_interrupt(rtc_instance_t rtc_instance);


#ifdef __cplusplus
}

class rtc
{
public:
    using uint8_t = cc_index_t;

    /// The bit-wdith of the counter.
    static size_t const counter_width = 24u;

    cc_index_t const cc_count;

    virtual ~rtc();

    rtc()                       = delete;
    rtc(rtc const&)             = delete;
    rtc(rtc &&)                 = delete;
    rtc& operator=(rtc const&)  = delete;
    rtc& operator=(rtc&&)       = delete;

    rtc(rtc_instance_t  rtc_instance,
        uint8_t         prescaler     = 0u,     // 32,768 Hz clock source.
        uint8_t         irq_priority  = 7u);

    void start();
    void stop();
    void reset();

    void     cc_set(cc_index_t cc_index, uint32_t rtc_ticks);
    uint32_t cc_get(cc_index_t) const;
    uint32_t cc_get_count(cc_index_t cc_index) const;
    uint32_t cc_get_count() const;
    uint32_t get_count_extend_32() const;
    uint64_t get_count_extend_64() const;
    void     cc_disable(cc_index_t cc_index);

    uint32_t ticks_per_second() const;
    uint32_t usec_to_ticks(uint32_t usec) const;
    uint32_t msec_to_ticks(uint32_t msec) const;

    virtual void event_notify(cc_index_t cc_index, uint32_t cc_count) {};

private:
    rtc_instance_t  rtc_instance_;
    uint64_t        rtc_counter_extension;
};

#endif

