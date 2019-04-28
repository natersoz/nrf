/**
 * @file gpio_te_test/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gpio_te.h"
#include "ppi.h"
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

// Create a continuous timer for starting SAADC conversions.
class timer_gpio_te: public timer_observer
{
public:
    explicit timer_gpio_te(uint32_t expiry_ticks) :
        timer_observer(timer_observer::expiration_type::continuous,
                       expiry_ticks) {}

    void expiration_notify() override;
};

static timer_observable<> timer_test_observable(1u);
static timer_gpio_te timer_gpio_te(timer_test_observable.msec_to_ticks(500u));

static void gpio_te_port_event_handler(uint32_t latched, void* context)
{
    logger &logger = logger::instance();
    logger.debug("GPIO PORT event");
}

void gpio_te_pin_event_handler(gpio_te_channel_t gpio_te_channel,
                               void*             context)
{
    logger &logger = logger::instance();
    logger.debug("gpio_te_pin_event_handler: channel :%u, context: %p",
                 gpio_te_channel, context);
}

void timer_gpio_te::expiration_notify()
{
    logger &logger = logger::instance();
    logger.debug("timer_gpio_te expired");
}

int main()
{
    lfclk_enable(LFCLK_SOURCE_XO);
    rtc_1.start();

    leds_board_init();
    led_state_set(0u, true);

    rtt_output_stream rtt_os(rtt_os_buffer, sizeof(rtt_os_buffer));
    logger& logger = logger::instance();
    logger.set_level(logger::level::debug);
    logger.set_output_stream(rtt_os);
    logger.set_rtc(rtc_1);
    logger.set_output_stream(rtt_os);

    logger.info("---------- GPIO TE test ----------");

    uint8_t const irq_priority = 7u;
    gpio_te_init(irq_priority);

    /* Attach exclusively so that the events triggered by the timer comparator
     * (CC) register is only used by us; it won't generate more events than
     * being asked for here.
     */
    timer_observable<>::cc_index_t const cc_index =
        timer_test_observable.attach_exclusive(timer_gpio_te);

    logger.debug("timer exclusive index: %u", cc_index);
    ASSERT(cc_index != timer_observable<>::cc_index_unassigned);

    uint32_t volatile* gpio_te_trigger_event = timer_1.cc_get_event(cc_index);

    // This pin configuration is compatible with the the spi_test.
    // The SPIM and SPIS clock pins are tied together through pins 11, 6.
    // Use that same configuration to have the task pin drive the event pin.
    // Note that pin 18 can be used to toggle LED 1.
    uint16_t const gpio_task_pin  = 11u;
    uint16_t const gpio_event_pin =  6u;

    // Allocate a GPIO TE channel of TASKS for use with the pin.
    // We will use TASKS_OUT, since it can be configured to toggle.
    gpio_te_channel_t gpio_task_channel =
        gpio_te_allocate_channel_task(gpio_task_pin,
                                      gpio_te_polarity_toggle,
                                      gpio_te_output_init_high);

    ASSERT(gpio_task_channel != gpio_te_channel_invalid);

    // Once the GPIO TE channel has been allocated we can take its TASKS_OUT
    // and assign it to a PPI channel. This allows us to connect it to the
    // timer CC generated event.
    ppi_channel_t const ppi_channel = ppi_channel_allocate(
        gpio_te_channel_get_task_out(gpio_task_channel),
        gpio_te_trigger_event,
        nullptr);

    ASSERT(ppi_channel != ppi_channel_invalid);

    ppi_channel_enable(ppi_channel);

    // Allocate a GPIO TE channel EVENTS_IN.
    // When a rising edge is detected an event will be generated.
    // In this case the event will trigger an interrupt.
    // The gpio_event_pin (6) gets its input changed by gpio_task_pin (11)
    // because they are connected together with a jumper wire.
    gpio_te_channel_t gpio_event_channel =
        gpio_te_allocate_channel_event(gpio_event_pin,
                                       gpio_te_polarity_rising,
                                       nullptr,
                                       gpio_te_pin_event_handler,
                                       nullptr);

    gpio_te_channel_enable_task(gpio_task_channel);
    gpio_te_channel_enable_event(gpio_event_channel);

    // This is for testing the PORT event - not yet implemented.
    bool const latch_detect_mode = false;
    gpio_te_port_enable(gpio_te_port_event_handler, nullptr, latch_detect_mode);

    while (true)
    {
        led_state_set(0u, false);   // Turn off the LED when sleeping.
        __WFE();
        led_state_set(0u, true);
        logger.flush();
    }
}

