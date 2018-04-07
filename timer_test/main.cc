/**
 * @file spi_master/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "timer_observer.h"
#include "timer_observer_test.h"
#include "leds.h"

#include "clocks.h"
#include "rtc.h"
#include "rtc_debug.h"
#include "nrf_gpio.h"

#include "logger.h"
#include "segger_rtt_output_stream.h"
#include "project_assert.h"

class timer_observer_1: public timer_observer_test
{
public:
    timer_observer_1(expiration_type    type,
                     uint32_t           expiration_ticks,
                     timer              &timer_reference) :
        timer_observer_test("timer_1",
                            type,
                            expiration_ticks,
                            timer_reference) {}

    void expiration_notify() override;
};

class timer_observer_2: public timer_observer_test
{
public:
    timer_observer_2(expiration_type    type,
                     uint32_t           expiration_ticks,
                     timer              &timer_reference) :
        timer_observer_test("timer_2",
                            type,
                            expiration_ticks,
                            timer_reference) {}

    void expiration_notify() override;
};

class timer_observer_3: public timer_observer_test
{
public:
    timer_observer_3(expiration_type    type,
                     uint32_t           expiration_ticks,
                     timer              &timer_reference) :
        timer_observer_test("timer_3",
                            type,
                            expiration_ticks,
                            timer_reference) {}

    void expiration_notify() override;
};

class timer_observer_4: public timer_observer_test
{
public:
    timer_observer_4(expiration_type    type,
                     uint32_t           expiration_ticks,
                     timer              &timer_reference) :
        timer_observer_test("timer_4",
                            type,
                            expiration_ticks,
                            timer_reference) {}

    void expiration_notify() override;
};

class timer_observer_5: public timer_observer_test
{
public:
    timer_observer_5(expiration_type    type,
                     uint32_t           expiration_ticks,
                     timer              &timer_reference) :
        timer_observer_test("timer_5",
                            type,
                            expiration_ticks,
                            timer_reference) {}

    void expiration_notify() override;
};

class timer_observer_6: public timer_observer_test
{
public:
    timer_observer_6(expiration_type    type,
                     uint32_t           expiration_ticks,
                     timer              &timer_reference) :
        timer_observer_test("timer_6",
                            type,
                            expiration_ticks,
                            timer_reference) {}

    void expiration_notify() override;
};


static segger_rtt_output_stream rtt_os;

// RTC: 1024 ticks / second
static rtc rtc_1(1u, 32u);

/**
 * Use the TIMER1 peripheral.
 * The -D TIMER1_ENABLED must be set.
 * A linker error will help inform the user if it is not set.
 *
 * @note When the softdevice is in use it needs exclusive access to TIMER0
 */
static timer_observable timer_test_observable(1u);
static timer timer_reference(2u);

static uint32_t const timer_tps = timer_test_observable.ticks_per_second();

// timer 1 is continuous, 1 second.
static timer_observer_1 timer_1(timer_observer::expiration_type::continuous,
                                timer_tps, timer_reference);

// timer 2 is one-shot, 200 msec.
static timer_observer_2 timer_2(timer_observer::expiration_type::one_shot,
                                timer_tps / 5u, timer_reference);

// timer 3 is continuous, fast: 500 usec.
static timer_observer_3 timer_3(timer_observer::expiration_type::continuous,
                                timer_tps / 2000u, timer_reference);

// timer 4 is one-shot, quick: 500 usec, trigger on timer 2 event.
static timer_observer_4 timer_4(timer_observer::expiration_type::one_shot,
                                timer_tps / 2000u, timer_reference);

// timer 5 is one-shot, long: 0.5 second, triggers on timer 4.
static timer_observer_5 timer_5(timer_observer::expiration_type::one_shot,
                                timer_tps / 2, timer_reference);

// timer 6 is continuous, starts on timer 4, ends on timer 5.
static timer_observer_6 timer_6(timer_observer::expiration_type::continuous,
                                timer_tps / 1000u, timer_reference);


void timer_observer_1::expiration_notify()
{
    this->update_stats();
    led_state_toggle(1u);
    logger &logger = logger::instance();
    logger.debug("obsv_1[%d]: this: 0x%p", this->cc_index_get(), this);

    timer_2.ticks_start_set(timer_reference.cc_get_count(0u));
    timer_test_observable.attach(timer_2);
}

void timer_observer_2::expiration_notify()
{
    this->update_stats();
    led_state_toggle(2u);
    logger &logger = logger::instance();
    logger.debug("obsv_2[%d]: this: 0x%p", this->cc_index_get(), this);

    timer_4.ticks_start_set(timer_reference.cc_get_count(0u));
    timer_test_observable.attach(timer_4);
    timer_test_observable.detach(timer_2);
}

void timer_observer_3::expiration_notify()
{
    this->update_stats();
    led_state_toggle(3u);
    logger &logger = logger::instance();
    logger.debug("obsv_3[%d]: this: 0x%p", this->cc_index_get(), this);
}

void timer_observer_4::expiration_notify()
{
    this->update_stats();
    led_state_toggle(2u);
    logger &logger = logger::instance();
    logger.debug("obsv_4[%d]: this: 0x%p", this->cc_index_get(), this);

    timer_6.ticks_start_set(timer_reference.cc_get_count(0u));
    timer_test_observable.attach(timer_6);
    timer_5.ticks_start_set(timer_reference.cc_get_count(0u));
    timer_test_observable.attach(timer_5);
    timer_test_observable.detach(timer_4);
}

void timer_observer_5::expiration_notify()
{
    this->update_stats();
//    led_state_toggle(2u);
    logger &logger = logger::instance();
    logger.debug("obsv_5[%d]: this: 0x%p", this->cc_index_get(), this);

    timer_test_observable.detach(timer_6);
    timer_test_observable.detach(timer_5);
}

void timer_observer_6::expiration_notify()
{
    this->update_stats();
//    led_state_toggle(2u);
    logger &logger = logger::instance();
    logger.debug("obsv_6[%d]: this: 0x%p", this->cc_index_get(), this);
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

    logger.info("--- Timer Test ---");
    logger.info("timer ticks/second: %u", timer_test_observable.ticks_per_second());
    logger.info("timer_1: %8u ticks, mode: %u, this: 0x%p", timer_1.expiration_get_ticks(), timer_1.expiration_get_type(), &timer_1);
    logger.info("timer_2: %8u ticks, mode: %u, this: 0x%p", timer_2.expiration_get_ticks(), timer_2.expiration_get_type(), &timer_2);
    logger.info("timer_3: %8u ticks, mode: %u, this: 0x%p", timer_3.expiration_get_ticks(), timer_3.expiration_get_type(), &timer_3);
    logger.info("timer_4: %8u ticks, mode: %u, this: 0x%p", timer_4.expiration_get_ticks(), timer_4.expiration_get_type(), &timer_4);
    logger.info("timer_5: %8u ticks, mode: %u, this: 0x%p", timer_5.expiration_get_ticks(), timer_5.expiration_get_type(), &timer_5);
    logger.info("timer_6: %8u ticks, mode: %u, this: 0x%p", timer_6.expiration_get_ticks(), timer_6.expiration_get_type(), &timer_6);

    timer_reference.start();
    timer_1.ticks_start_set(timer_reference.cc_get_count(0u));
    timer_test_observable.attach(timer_1);
    timer_3.ticks_start_set(timer_reference.cc_get_count(0u));
    timer_test_observable.attach(timer_3);

    uint32_t rtc_count_last = rtc_1.get_count_extend_32();

    while (true)
    {
        led_state_set(0u, false);

        __WFE();

        led_state_set(0u, true);

        uint32_t const rtc_count = rtc_1.get_count_extend_32();
        if (rtc_count - rtc_count_last >= rtc_1.ticks_per_second())
        {
            rtc_count_last = rtc_count;
            timer_1.log_stats();
            timer_2.log_stats();
            timer_3.log_stats();
            timer_4.log_stats();
            timer_5.log_stats();
            timer_6.log_stats();
        }
        logger.flush();
    }
}

