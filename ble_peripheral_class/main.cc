/**
 * @file ble_peripheral_class/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble_peripheral_init.h"

#include "cmsis_gcc.h"
#include "clocks.h"
#include "leds.h"
#include "buttons.h"
#include "logger.h"
#include "segger_rtt_output_stream.h"
#include "rtc_observer.h"
#include "timer_observer.h"
#include "stack_usage.h"
#include "project_assert.h"

static segger_rtt_output_stream rtt_os;
static rtc_observable<>         rtc_1(1u, 32u);

int main(void)
{
    stack_fill(0xabcd1234);
    lfclk_enable(LFCLK_SOURCE_XO);
    rtc_1.start();

    leds_board_init();
    buttons_board_init();

    logger& logger = logger::instance();
    logger.set_rtc(rtc_1);
    logger.set_level(logger::level::info);
    logger.set_output_stream(rtt_os);

    logger.info("--- BLE peripheral classes ---");

    ble::profile::peripheral& ble_peripheral = ble_peripheral_init();
    ble_peripheral.advertising().start();

    logger.info("stack: free: %5u 0x%04x, size: %5u 0x%04x",
                stack_free(), stack_free(), stack_size(), stack_size());

    for (;;)
    {
        logger.flush();
        if (rtt_os.write_pending() == 0)
        {
            __WFE();
        }
    }
}

