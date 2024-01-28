/**
 * @file ble_peripheral/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble_peripheral_init.h"

#include "cmsis_gcc.h"
#include "clocks.h"
#include "leds.h"
#include "buttons.h"
#include "logger.h"
#include "segger_rtt.h"
#include "rtt_output_stream.h"
#include "rtc_observer.h"
#include "timer_observer.h"
#include "stack_usage.h"
#include "version_info.h"
#include "project_assert.h"

// The RTT output stream buffer allocation.
static char rtt_os_buffer[4096u];

int main(void)
{
    lfclk_enable(LFCLK_SOURCE_XO);

    rtc_observable<> rtc_1(1u, 32u);
    rtc_1.start();

    rtt_output_stream rtt_os(rtt_os_buffer, sizeof(rtt_os_buffer));
    logger& logger = logger::instance();
    logger.set_rtc(rtc_1);
    logger.set_level(logger::level::debug);
    logger.set_output_stream(rtt_os);

    segger_rtt_enable();

    leds_board_init();
    buttons_board_init();

    logger.info("--- BLE peripheral ---");

    logger.info("version: %s, git hash: %02x%02x%02x%02x",
                version_info.version,
                version_info.git_hash[0u],
                version_info.git_hash[1u],
                version_info.git_hash[2u],
                version_info.git_hash[3u]);

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
