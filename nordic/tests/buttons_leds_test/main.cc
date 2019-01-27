/**
 * @file buttons_leds_test/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "leds.h"
#include "buttons.h"
#include "gpio_te.h"
#include "gpio.h"
#include "rtc.h"
#include "rtc_observer.h"
#include "clocks.h"

#include "logger.h"
#include "rtt_output_stream.h"
#include "segger_rtt.h"
#include "project_assert.h"

#include "nrf_cmsis.h"

#include <cstring>
#include <iterator>

// The LED module must be initialized prior to calling rtc1_observer ctor.
class rtc1_observer: public rtc_observer
{
public:
    rtc1_observer(uint32_t expiry_ticks) :
        rtc_observer(rtc_observer::expiration_type::continuous,
                     expiry_ticks),
        led_index_on(0u)
        {
            led_state_set(led_index_on, true);
        }

    void expiration_notify() override;

private:
    led_index_t led_index_on;

    led_index_t led_increment(led_index_t led_index)
    {
        led_index += 1u;
        if (led_index >= led_count)
        {
            led_index = 0u;
        }
        return led_index;
    }
};

void rtc1_observer::expiration_notify()
{
    led_state_set(led_index_on, false);
    led_index_on = led_increment(led_index_on);

    logger::instance().debug("LED %u on", led_index_on);
    led_state_set(led_index_on, true);
}

static void gpio_port_event_handler(uint32_t latched, void* context)
{
    logger &logger = logger::instance();
    logger.info("GPIO PORT, latched 0x%08x", latched);
    logger.info("Button: %u, %u, %u, %u",
                 button_state_get(0u),
                 button_state_get(1u),
                 button_state_get(2u),
                 button_state_get(3u));
}

static rtc                  rtc_1(1u);
static rtc_observable<>     rtc1_observable(1u);
static char                 rtt_os_buffer[4096u];

int main()
{
    lfclk_enable(LFCLK_SOURCE_XO);
    rtc_1.start();

    leds_board_init();
    buttons_board_init();

    rtt_output_stream rtt_os(rtt_os_buffer, sizeof(rtt_os_buffer));
    logger& logger = logger::instance();
    logger.set_level(logger::level::info);
    logger.set_output_stream(rtt_os);
    logger.set_rtc(rtc_1);
    logger.set_output_stream(rtt_os);

    logger.info("---------- Buttons, LEDs test ----------");

    uint8_t gpio_irq_priority = 7u;
    gpio_te_init(gpio_irq_priority);

    // Note: ldetect of true will yield an EVENTS_PORT interrupt storm.
    // Do not use until that gets figured out.
    bool const ldetect = false;
    gpio_te_port_enable(gpio_port_event_handler, nullptr, ldetect);

    uint32_t const timer_ticks_per_second = rtc1_observable.ticks_per_second();
    uint32_t const timer_expiration = timer_ticks_per_second / 2u;

    rtc1_observer rtc1_observer(timer_expiration);
    rtc1_observable.attach(rtc1_observer);

    while (true)
    {
        __WFE();
        logger.flush();
    }
}

