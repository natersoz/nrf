/**
 * @file usart_test/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "usart.h"

#include "rtc.h"
#include "timer_observer.h"
#include "leds.h"
#include "clocks.h"
#include "gpio_te.h"
#include "nrf_cmsis.h"

#include "logger.h"
#include "write_data.h"
#include "rtt_output_stream.h"
#include "segger_rtt.h"
#include "project_assert.h"

#include <cstring>

static usart_port_t usart_port = 0u;

class usart_test_timer: public timer_observer
{
public:
    usart_test_timer(expiration_type type, uint32_t expiry_ticks) :
        timer_observer(type, expiry_ticks) {}

    void expiration_notify() override;
};

void usart_test_timer::expiration_notify()
{
    usart_read_fill(usart_port);
}

// RTC: 1024 ticks / second
static rtc  rtc_1(1u, 32u);
static char rtt_os_buffer[4096u];

static uint8_t usart_tx_buffer[2048u];
static uint8_t usart_rx_buffer[  32u];

static const char test_data[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3', '4', '5',
    '6', '7', '8', '9', 'A', 'B', 'C', 'D',
    'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1',
    '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '0', '1', '1', '2', '2', '3', '3',
    '4', '4', '5', '5', '6', '6', '7', '7',
    '8', '8', '9', '9', 'A', 'A', 'B', 'B',
    'C', 'C', 'D', 'D', 'E', 'E', 'F', 'F',
    'G', 'G', 'H', 'H', 'I', 'I', 'J', 'J',
    'K', 'K', 'L', 'L', 'M', 'M', 'N', 'N',
    'O', 'O', 'P', 'P', 'F', 'E', 'D', 'C',
    'B', 'A', '9', '8', '7', '6', '5', '4',
    '3', '2', '1', '0',
};

static void usart_event_handler(usart_event_t const* event, void* context)
{
    logger& logger = logger::instance();

    switch (event->type)
    {
    case usart_tx_complete:
        logger.debug("tx_complete: %4u", event->value);
        break;
    case usart_rx_complete:
        {
            size_t const n_read = usart_read(usart_port,
                                             usart_rx_buffer,
                                             sizeof(usart_rx_buffer));
            logger.info("rx_read: %4u, event rx: %4u", n_read, event->value);
            logger.write_data(logger::level::info,
                              usart_rx_buffer,
                              n_read,
                              true);
        }
        break;
    case usart_rx_error_overrun:
        logger.warn("usart_rx_error_overrun");
        break;
    case usart_rx_error_parity:
        logger.warn("usart_rx_error_parity");
        break;
    case usart_rx_error_framing:
        logger.warn("usart_rx_error_framing");
        break;
    case usart_rx_error_break:
        logger.warn("usart_rx_error_break");
        break;
    default:
        logger.error("unhandled event: %4u", event->type);
        break;
    }
}

int main()
{
    lfclk_enable(LFCLK_SOURCE_XO);
    rtc_1.start();
    leds_board_init();

    rtt_output_stream rtt_os(rtt_os_buffer, sizeof(rtt_os_buffer));
    logger& logger = logger::instance();
    logger.set_level(logger::level::info);
    logger.set_output_stream(rtt_os);
    logger.set_rtc(rtc_1);
    segger_rtt_enable();

    led_state_set(0u, false);
    led_state_set(1u, false);
    led_state_set(2u, false);
    led_state_set(3u, false);

    timer_observable<> timer_test_observable(1u);
    usart_test_timer usart_timer(usart_test_timer::expiration_type::continuous,
                                 timer_test_observable.msec_to_ticks(1000u));

    timer_test_observable.attach(usart_timer);

    logger.info("----- usart test -----");

    /*
     * FTDI Cable pinout:
     * Pin      Color   Function        USART   nrf52 Pin
     * 1        Black   Ground
     * 2        Brown   CTS             -> RTS  P0.25
     * 3        Red     VCC (+5V)       ->      P0.24
     * 4        Orange  TXD             -> RXD  P0.23
     * 5        Yellow  RXD             -> TXD  P0.22
     * 6        Green   RTS             -> CTS  P0.20
     */
    struct usart_config_t const usart_config = {
        .tx_pin         = 22u,
        .rx_pin         = 23u,
        .cts_pin        = 20u,
        .rts_pin        = 25u,
        .baud_rate      = 115'200u,
        .irq_priority   = 7u,
    };

    usart_init(usart_port,
               &usart_config,
               usart_event_handler,
               usart_tx_buffer,
               sizeof(usart_tx_buffer),
               usart_rx_buffer,
               sizeof(usart_rx_buffer),
               nullptr);

    usart_read_start(usart_port);

    usart_write(usart_port, "\r\n\r\n", 4);
    usart_write(usart_port, test_data, sizeof(test_data));
    usart_write(usart_port, test_data, sizeof(test_data));
    usart_write(usart_port, test_data, sizeof(test_data));
    usart_write(usart_port, test_data, sizeof(test_data));
    usart_write(usart_port, test_data, sizeof(test_data));
    usart_write(usart_port, test_data, sizeof(test_data));
    usart_write(usart_port, test_data, sizeof(test_data));
    usart_write(usart_port, "\r\n\r\n", 4);

    while (true)
    {
        led_state_set(0u, false);
        __WFE();
//        led_state_set(0u, true);

        logger.flush();
    }
}
