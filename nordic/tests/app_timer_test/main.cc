/**
 * @file app_timer_test/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "app_timer.h"
#include "leds.h"

#include "clocks.h"
#include "rtc.h"
#include "rtc_debug.h"

#include "logger.h"
#include "rtt_output_stream.h"
#include "segger_rtt.h"
#include "project_assert.h"

APP_TIMER_DEF(timer_1);
APP_TIMER_DEF(timer_2);
APP_TIMER_DEF(timer_3);

static uint32_t timer_expiration_count_1 = 0u;
static uint32_t timer_expiration_count_2 = 0u;
static uint32_t timer_expiration_count_3 = 0u;

static char rtt_os_buffer[4096u];

void timer_expiration_notify_1(void *context)
{
    uint32_t *count = reinterpret_cast<uint32_t *>(context);
    *count += 1u;

    app_timer_start(timer_2, APP_TIMER_TICKS( 200u), &timer_expiration_count_2);
    app_timer_start(timer_3, APP_TIMER_TICKS(  10u), &timer_expiration_count_3);

    logger &logger = logger::instance();
    logger.info("notify_1: %10u", *count);
}

void timer_expiration_notify_2(void *context)
{
    uint32_t *count = reinterpret_cast<uint32_t *>(context);
    *count += 1u;

    app_timer_stop(timer_3);

    logger &logger = logger::instance();
    logger.info("notify_2: %10u", *count);
}

void timer_expiration_notify_3(void *context)
{
    uint32_t *count = reinterpret_cast<uint32_t *>(context);
    *count += 1u;

    logger &logger = logger::instance();
    logger.info("notify_3: %10u", *count);
}

rtc_observable<> rtc_1(1u, 1u);

int main()
{
    lfclk_enable(LFCLK_SOURCE_XO);
    rtc_1.start();
    leds_board_init();

    rtt_output_stream rtt_os(rtt_os_buffer, sizeof(rtt_os_buffer));
    logger& logger = logger::instance();
    logger.set_level(logger::level::debug);
    logger.set_output_stream(rtt_os);
    logger.set_rtc(rtc_1);

    segger_rtt_enable();

    app_timer_init(rtc_1);

    app_timer_create(&timer_1, APP_TIMER_MODE_REPEATED,    timer_expiration_notify_1);
    app_timer_create(&timer_2, APP_TIMER_MODE_SINGLE_SHOT, timer_expiration_notify_2);
    app_timer_create(&timer_3, APP_TIMER_MODE_REPEATED,    timer_expiration_notify_3);

    logger.info("--- App Timer Test ---");
    logger.info("rtc ticks/second: %u", rtc_1.ticks_per_second());

    app_timer_start(timer_1, APP_TIMER_TICKS(1000u), &timer_expiration_count_1);

    while (true)
    {
        logger.flush();
        led_state_set(0u, false);
        __WFE();
        led_state_set(0u, true);
    }
}

