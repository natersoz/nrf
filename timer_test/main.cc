/**
 * @file spi_master/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "timer_observer.h"
#include "leds.h"

#include "clocks.h"
#include "rtc.h"
#include "rtc_debug.h"
#include "nrf_gpio.h"

#include "logger.h"
#include "segger_rtt_output_stream.h"
#include "project_assert.h"

#include <string.h>

class timer_observer_1: public timer_observer
{
public:
    timer_observer_1(expiration_type type, uint32_t expiry_ticks) :
        timer_observer(type, expiry_ticks) {}

    void expiration_notify() override;
};

class timer_observer_2: public timer_observer
{
public:
    timer_observer_2(expiration_type type, uint32_t expiry_ticks) :
        timer_observer(type, expiry_ticks) {}

    void expiration_notify() override;
};

class timer_observer_3: public timer_observer
{
public:
    timer_observer_3(expiration_type type, uint32_t expiry_ticks) :
        timer_observer(type, expiry_ticks) {}

    void expiration_notify() override;
};

class timer_observer_4: public timer_observer
{
public:
    timer_observer_4(expiration_type type, uint32_t expiry_ticks) :
        timer_observer(type, expiry_ticks) {}

    void expiration_notify() override;
};

class timer_observer_5: public timer_observer
{
public:
    timer_observer_5(expiration_type type, uint32_t expiry_ticks) :
        timer_observer(type, expiry_ticks) {}

    void expiration_notify() override;
};


static segger_rtt_output_stream rtt_os;

/**
 * Use the TIMER1 peripheral.
 * The -D TIMER1_ENABLED must be set.
 * A linker error will help inform the user if it is not set.
 *
 * @note When the softdevice is in use it needs exclusive access to TIMER0
 */
static timer_observable timer_test_observable(1u);

// timer 1 is continuous.
static timer_observer_1 timer_1(timer_observer::expiration_type::continuous,
                                timer_test_observable.ticks_per_second());

// timer 2 is one-shot.
static timer_observer_2 timer_2(timer_observer::expiration_type::one_shot,
                                timer_test_observable.ticks_per_second() / 5u);

// timer 3 is continuous, fast.
static timer_observer_3 timer_3(timer_observer::expiration_type::continuous,
                                timer_test_observable.ticks_per_second() / 4u);

// timer 4 is one-shot, quick, trigger on timer 2 event.
static timer_observer_4 timer_4(timer_observer::expiration_type::continuous,
                                timer_test_observable.ticks_per_second() / 20u);

// timer 5
//static timer_observer_5 timer_5(timer_observer::expiration_type::continuous,
//                                timer_test_observable.ticks_per_second());


void timer_observer_1::expiration_notify()
{
    uint32_t const count = this->observable_->cc_get_count(2u); /// @todo debug only
    led_state_toggle(1u);
    logger &logger = logger::instance();
    logger.info("obsv_1[%d]: notify: %10u, this: 0x%p", this->cc_index_get(), count, this);

    timer_test_observable.attach(timer_2);
}

void timer_observer_2::expiration_notify()
{
    uint32_t const count = this->observable_->cc_get_count(2u);  /// @todo debug only
    led_state_toggle(2u);
    logger &logger = logger::instance();
    logger.info("obsv_2[%d]: notify: %10u, this: 0x%p", this->cc_index_get(), count, this);

    timer_test_observable.attach(timer_4);
    timer_test_observable.detach(timer_2);
}

void timer_observer_3::expiration_notify()
{
    uint32_t const count = this->observable_->cc_get_count(2u);      /// @todo debug only
    led_state_toggle(3u);
    logger &logger = logger::instance();
    logger.info("obsv_3[%d]: notify: %10u, this: 0x%p", this->cc_index_get(), count, this);
}

void timer_observer_4::expiration_notify()
{
    uint32_t const count = this->observable_->cc_get_count(2u);      /// @todo debug only
    led_state_toggle(2u);
    logger &logger = logger::instance();
    logger.info("obsv_4[%d]: notify: %10u, this: 0x%p", this->cc_index_get(), count, this);

    timer_test_observable.detach(timer_4);
}

// Init RTC 1 to have 1024 ticks/second (32,768 / 32).
// Given the 24 bit counter, this will roll over (1 << 24)/1024 = 16,384 seconds.
// - a little over 4.5 hours.
static rtc rtc_1(1u, 32u);

int main()
{
    lfclk_enable(LFCLK_SOURCE_XO);
    rtc_1.start();
    leds_board_init();

    logger& logger = logger::instance();
    logger.set_level(logger::level::debug);
    logger.set_output_stream(rtt_os);
    logger.set_rtc(rtc_1);

    logger.info("--- Timer Test ---");
    logger.info("timer_1: %8u ticks, mode: %u, this: 0x%p", timer_1.expiration_get_ticks(), timer_1.expiration_get_type(), &timer_1);
    logger.info("timer_2: %8u ticks, mode: %u, this: 0x%p", timer_2.expiration_get_ticks(), timer_2.expiration_get_type(), &timer_2);
    logger.info("timer_3: %8u ticks, mode: %u, this: 0x%p", timer_3.expiration_get_ticks(), timer_3.expiration_get_type(), &timer_3);
    logger.info("timer_4: %8u ticks, mode: %u, this: 0x%p", timer_4.expiration_get_ticks(), timer_4.expiration_get_type(), &timer_4);

    timer_test_observable.attach(timer_1);
    timer_test_observable.attach(timer_3);

    while (true)
    {
        led_state_set(0u, false);

        __WFE();

        led_state_set(0u, true);
        logger.flush();
    }
}
