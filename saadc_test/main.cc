/**
 * @file saadc_test/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "saadc.h"
#include "rtc.h"
#include "timer_observer.h"
#include "leds.h"
#include "clocks.h"

#include "logger.h"
#include "segger_rtt_output_stream.h"
#include "project_assert.h"

#include "nrf_cmsis.h"

#include <cstring>
#include <iterator>

// RTC: 32,768 ticks / second
static rtc rtc_1(1u, 1u);
static segger_rtt_output_stream rtt_os;

// ADC channel convesions destination buffer;
static int16_t saadc_buffer[2u];
static uint16_t saadc_buffer_length = std::size(saadc_buffer);

static size_t conversion_count = 0u;
static uint32_t conversion_start_ticks = 0u;

// Create a continuous timer for starting SAADC conversions.
class timer_saadc: public timer_observer
{
public:
    timer_saadc(uint32_t expiry_ticks) :
        timer_observer(timer_observer::expiration_type::continuous,
                       expiry_ticks) {}

    void expiration_notify() override;
};

static timer_observable<> timer_test_observable(1u);
static timer_saadc timer_saadc(timer_test_observable.msec_to_ticks(1000u));

static void saadc_event_handler(saadc_event_t   event,
                                uint16_t        event_value,
                                void*           context)
{
    led_state_set(2u, false);
    logger &logger = logger::instance();

    switch (event)
    {
    case saadc_event_conversion_start:
        logger.debug("SAADC event: conversion start");
        break;
    case saadc_event_conversion_stop:
        logger.debug("SAADC event: conversion stop: %u samples", event_value);
        break;
    case saadc_event_conversion_complete:
        {
            uint32_t const ticks = rtc_1.get_count_extend_32();
            uint32_t const conversion_ticks = ticks - conversion_start_ticks;
            uint32_t const conversion_usec = (conversion_ticks * 1000000u) / rtc_1.ticks_per_second();
            conversion_count += 1u;

            logger.info("SAADC event: conversion complete: %u samples, ticks: %u, usec: %u",
                        event_value, conversion_ticks, conversion_usec);
            for (uint16_t index = 0u; index < event_value; ++index)
            {
                logger.info("%6d 0x%4x", saadc_buffer[index], saadc_buffer[index]);
            }
            logger.write_data(logger::level::debug,
                              saadc_buffer,
                              sizeof(saadc_buffer));
        }
        break;
    case saadc_event_limit_lower:
        {
            struct saadc_limits_t const limits = saadc_get_channel_limits(event_value);
            logger.info("SAADC event: chan: %u, lower limit %u 0x%x exceeded",
                        event_value, limits.lower, limits.lower);
        }
        break;
    case saadc_event_limit_upper:
        {
            struct saadc_limits_t const limits = saadc_get_channel_limits(event_value);
            logger.info("SAADC event: chan: %u, upper limit %u 0x%x exceeded",
                        event_value, limits.upper, limits.upper);
        }
        break;
    case saddc_event_calibration_complete:
        logger.info("SAADC event: calibration complete");
        break;
    default:
        ASSERT(0);
        break;
    }
}

void timer_saadc::expiration_notify()
{
    led_state_set(2u, true);

    struct saadc_conversion_channel_time_t const conversion = saadc_conversion_channel_time();

    logger &logger = logger::instance();
    logger.info("SAADC start: channel_count: %u / %u, time: %u usec",
                saadc_buffer_length, conversion.channel_count, conversion.time_usec);

    conversion_start_ticks = rtc_1.get_count_extend_32();
    saadc_conversion_start(saadc_buffer,
                           saadc_buffer_length,
                           saadc_event_handler);
}

int main()
{
    lfclk_enable(LFCLK_SOURCE_XO);
    rtc_1.start();
    leds_board_init();

    logger& logger = logger::instance();
    logger.set_level(logger::level::info);
    logger.set_output_stream(rtt_os);
    logger.set_rtc(rtc_1);

    logger.info("SAADC: test");
    logger.info("timer saadc: %8u ticks", timer_saadc.expiration_get_ticks());

    uint8_t const irq_priority = 7u;
    saadc_init(saadc_conversion_resolution_12_bit, nullptr, irq_priority);

    saadc_input_configure_single_ended(0u,
                                       saadc_input_AIN0,
                                       saadc_input_termination_none,
                                       saadc_gain_div_6,
                                       saadc_reference_600mV,
                                       saadc_tacq_40_usec);

    saadc_input_configure_single_ended(1u,
                                       saadc_input_AIN1,
                                       saadc_input_termination_none,
                                       saadc_gain_div_6,
                                       saadc_reference_600mV,
                                       saadc_tacq_40_usec);

    // Don't attach the tiemr observer until the SAADC is configure.
    // The conversion might start before being configured.
    timer_test_observable.attach(timer_saadc);

    while (true)
    {
        __WFE();

        led_state_toggle(0u);
        logger.flush();
    }
}

