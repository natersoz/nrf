/**
 * @file ble_central/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

// #include "ble_central_init.h"

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

#include "ble/att.h"
#include "ble/gap_types.h"
#include "ble/gatt_uuids.h"
#include "ble/tlv_encode.h"

#include "ble/profile_central.h"
#include "ble/gap_connection.h"
#include "ble/nordic_ble_gap_operations.h"

#include "ble/nordic_ble_stack.h"
#include "ble/nordic_ble_gap_scanning.h"
#include "ble/nordic_ble_peer.h"

#include "ble_gap_connection.h"
#include "ble_gattc_observer.h"

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

    logger.info("--- BLE central ---");

    constexpr uint8_t const             nordic_config_tag = 1u;
    nordic::ble_stack                   ble_stack(nordic_config_tag);

    nordic::ble_gap_scanning            ble_scanning;
    nordic::ble_gap_operations          ble_gap_operations;
    ble_gap_connection                  ble_gap_connection(ble_gap_operations,
                                                           ble_scanning);
    ble_gattc_observer                  ble_gattc_observer;
    ble::profile::central               ble_central(ble_stack,
                                                    ble_gap_connection,
                                                    ble_gattc_observer);
    ble_gap_connection.init();
    ble_gattc_observer.init();

    ble_stack.init();
    ble_stack.enable();

    ble_peer_init();

    logger.info("stack: free: %5u 0x%04x, size: %5u 0x%04x",
                stack_free(), stack_free(), stack_size(), stack_size());

    ble_central.scanning().start();

    for (;;)
    {
        logger.flush();
        if (rtt_os.write_pending() == 0)
        {
            __WFE();
        }
    }
}

