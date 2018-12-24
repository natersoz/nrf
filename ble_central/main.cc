/**
 * @file ble_central/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

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
    logger.set_level(logger::level::debug);
    logger.set_output_stream(rtt_os);

    logger.info("--- BLE central ---");

    logger.write_data(logger::level::debug,
                      NRF_FICR->DEVICEADDR,
                      8u, false, write_data::data_prefix::address);

    constexpr uint8_t const             nordic_config_tag = 1u;
    nordic::ble_stack                   ble_stack(nordic_config_tag);

    /// @todo The connection interval needs to be thought through
    /// for a specific device.
    ble::gap::connection_parameters const connection_parameters(
        ble::gap::connection_interval_msec(100),
        ble::gap::connection_interval_msec(200),
        0u,
        ble::gap::supervision_timeout_msec(4000u));

    nordic::ble_gap_scanning        ble_scanning;
    nordic::ble_gap_operations      ble_gap_operations;
    ble_gap_connection              ble_gap_connection(ble_gap_operations,
                                                       ble_scanning,
                                                       connection_parameters);
    ble_gattc_observer              ble_gattc_observer;
    ble::profile::central           ble_central(ble_stack,
                                                ble_gap_connection,
                                                ble_gattc_observer);
    ble_gap_connection.init();
    ble_gattc_observer.init();

    unsigned int const peripheral_count = 0u;
    unsigned int const central_count    = 1u;
    ble_central.ble_stack().init(peripheral_count, central_count);
    ble_central.ble_stack().enable();

    ble::stack::version const version = ble_central.ble_stack().get_version();

    logger::instance().info(
        "BLE stack version: link layer: %u, company id: 0x%04x, vendor: 0x%x",
        version.link_layer_version,
        version.company_id,
        version.vendor_specific[0u]);

    logger::instance().info(
        "BLE softdevice %u, version: %u.%u.%u",
        static_cast<uint8_t>(version.vendor_specific[1] >> 24u),
        static_cast<uint8_t>(version.vendor_specific[1] >> 16u),
        static_cast<uint8_t>(version.vendor_specific[1] >>  8u),
        static_cast<uint8_t>(version.vendor_specific[1] >>  0u));

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

