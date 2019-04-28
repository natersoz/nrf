/**
 * @file saadc_test/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "saadc.h"
#include "temperature_sensor.h"
#include "rtc.h"
#include "timer.h"
#include "timer_observer.h"
#include "leds.h"
#include "clocks.h"

#include "logger.h"
#include "rtt_output_stream.h"
#include "segger_rtt.h"
#include "project_assert.h"

#include "nrf_cmsis.h"

#include <cstring>
#include <iterator>

static rtc   rtc_1(1u);
static timer timer_1(1u);
static char  rtt_os_buffer[4096u];

// ADC channel convesions destination buffer;
static int16_t saadc_buffer[2u][2u];
static uint8_t saadc_buffer_index = 0u;
static uint16_t saadc_buffer_length = std::size(saadc_buffer[0u]);

static size_t conversion_count = 0u;
static uint32_t conversion_start_ticks = 0u;

// Create a continuous timer for starting measurement conversions.
// Both SAADC and temperature.
class measurement_timer: public timer_observer
{
public:
    explicit measurement_timer(uint32_t expiry_ticks) :
        timer_observer(timer_observer::expiration_type::continuous,
                       expiry_ticks) {}
private:
    void expiration_notify() override;
};

static timer_observable<> timer_test_observable(1u);
static measurement_timer measurement_timer(timer_test_observable.msec_to_ticks(1000u));

static void saadc_event_handler(saadc_event_type_t              event_type,
                                union saadc_event_info_t const* event_info,
                                void*                           context)
{
    logger &logger = logger::instance();

    switch (event_type)
    {
    case saadc_event_conversion_started:
        {
            uint8_t const prev_buffer_index = saadc_buffer_index;
            saadc_buffer_index ^= 1u;
            saadc_queue_conversion_buffer(saadc_buffer[saadc_buffer_index], saadc_buffer_length);
            logger.debug("SAADC event: conversion started, index: %u -> %u, buffer queued: 0x%p",
                         prev_buffer_index, saadc_buffer_index, saadc_buffer[saadc_buffer_index]);
        }
        break;
    case saadc_event_conversion_stop:
        logger.info("SAADC event: conversion stop: samples: 0x%p, %u",
                    event_info->conversion.data, event_info->conversion.length);
        break;
    case saadc_event_conversion_complete:
        {
            uint32_t const ticks = rtc_1.get_count_extend_32();
            uint32_t const conversion_ticks = ticks - conversion_start_ticks;
            uint32_t const conversion_usec = (conversion_ticks * 1000000u) / rtc_1.ticks_per_second();
            conversion_count += 1u;

            logger.info("SAADC event: conversion complete: samples: 0x%p, %u, ticks: %u, usec: %u",
                        event_info->conversion.data, event_info->conversion.length,
                        conversion_ticks, conversion_usec);
            for (int16_t index = 0u; index < event_info->conversion.length; ++index)
            {
                logger.info("%6d 0x%4x", event_info->conversion.data[index],
                                         event_info->conversion.data[index]);
            }
            logger.write_data(logger::level::debug,
                              event_info->conversion.data,
                              event_info->conversion.length * sizeof(int16_t),
                              false,
                              io::data_prefix::address);
        }
        break;
    case saadc_event_limit_lower:
        {
            saadc_input_channel_t const input_channel = event_info->limits_exceeded.input_channel;
            struct saadc_limits_t const limits = saadc_get_channel_limits(input_channel);
            logger.info("SAADC event: chan: %d, lower limit %u 0x%x exceeded",
                        input_channel, limits.lower, limits.lower);
        }
        break;
    case saadc_event_limit_upper:
        {
            saadc_input_channel_t const input_channel = event_info->limits_exceeded.input_channel;
            struct saadc_limits_t const limits = saadc_get_channel_limits(input_channel);
            logger.info("SAADC event: chan: %d, upper limit %u 0x%x exceeded",
                        input_channel, limits.upper, limits.upper);
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

static void temperature_measurement_handler(int32_t temperature_Cx4, void* context)
{
    logger &logger = logger::instance();
    logger.info("Temperature: %d C", temperature_Cx4 / 4);
}

void measurement_timer::expiration_notify()
{
    struct saadc_conversion_info_t const conversion = saadc_conversion_info();

    logger &logger = logger::instance();
    logger.info("SAADC start: channel_count: %u / %u, time: %u usec",
                saadc_buffer_length, conversion.channel_count, conversion.time_usec);

    conversion_start_ticks = rtc_1.get_count_extend_32();

    bool const started = temperature_sensor_take_measurement(
        temperature_measurement_handler, nullptr);

    logger.info("temperature started: %u", started);
}

int main()
{
    lfclk_enable(LFCLK_SOURCE_XO);
    rtc_1.start();

    leds_board_init();
    led_state_set(0u, true);

    rtt_output_stream rtt_os(rtt_os_buffer, sizeof(rtt_os_buffer));
    logger& logger = logger::instance();
    logger.set_level(logger::level::info);
    logger.set_output_stream(rtt_os);
    logger.set_rtc(rtc_1);

    segger_rtt_enable();


    logger.info("---------- SAADC test ----------");
    logger.debug("timer: %8u ticks", measurement_timer.expiration_get_ticks());

    uint8_t const irq_priority = 7u;
    saadc_init(saadc_conversion_resolution_12_bit,
               saadc_event_handler,
               nullptr,
               irq_priority);

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

    /* Attach exclusively so that the events triggered by the timer comparator
     * (CC) register is only used by us; it won't generate more events than
     * being asked for here.
     *
     * Don't attach the timer observer until the SAADC is configured.
     * The conversion might start before being configured.
     */
    timer_observable<>::cc_index_t const cc_index =
        timer_test_observable.attach_exclusive(measurement_timer);

    logger.debug("timer exclusive index: %u", cc_index);
    ASSERT(cc_index != timer_observable<>::cc_index_unassigned);

    uint32_t volatile* saadc_trigger_event = timer_1.cc_get_event(cc_index);
    saadc_conversion_start(saadc_buffer[saadc_buffer_index],
                           saadc_buffer_length,
                           saadc_trigger_event);

    while (true)
    {
        led_state_set(0u, false);   // Turn off the LED when sleeping.
        __WFE();
        led_state_set(0u, true);
        logger.flush();
    }
}

