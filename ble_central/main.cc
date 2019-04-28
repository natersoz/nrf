/**
 * @file ble_central/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

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

#include "ble/att.h"
#include "ble/gap_connection.h"
#include "ble/gap_event_logger.h"
#include "ble/gap_types.h"
#include "ble/gatt_enum_types.h"
#include "ble/gattc_service_builder.h"
#include "ble/gatt_service.h"
#include "ble/gatt_characteristic.h"
#include "ble/gatt_descriptors.h"
#include "ble/nordic_ble_gap_operations.h"
#include "ble/profile_central.h"

#include "ble/nordic_ble_event_observable.h"
#include "ble/nordic_ble_gap_operations.h"
#include "ble/nordic_ble_gap_scanning.h"
#include "ble/nordic_ble_gattc_operations.h"
#include "ble/nordic_ble_peer.h"
#include "ble/nordic_ble_stack.h"

#include "ble_gap_connection.h"
#include "ble_gattc_observer.h"

#include <cmsis_gcc.h>
#include "nrf_cmsis.h"

static char rtt_os_buffer[4096u];

static std::array<ble::gatt::service,         16u>  services_list;
static std::array<ble::gatt::characteristic,  32u>  characteristics_list;
static std::array<ble::gatt::descriptor_base, 32u>  descriptors_list;

static void free_lists_alloc(ble::gattc::service_builder &service_builder)
{
    for (auto& node : services_list)
    {
        service_builder.free_list.services.push_back(node);
    }

    for (auto& node : characteristics_list)
    {
        service_builder.free_list.characteristics.push_back(node);
    }

    for (auto& node : descriptors_list)
    {
        service_builder.free_list.descriptors.push_back(node);
    }
}

int main(void)
{
    stack_fill(0xabcd1234);
    lfclk_enable(LFCLK_SOURCE_XO);

    rtc_observable<> rtc_1(1u, 32u);
    rtc_1.start();

    rtt_output_stream rtt_os(rtt_os_buffer, sizeof(rtt_os_buffer));
    logger& logger = logger::instance();
    logger.set_rtc(rtc_1);
    logger.set_level(logger::level::info);
    logger.set_output_stream(rtt_os);

    segger_rtt_enable();

    leds_board_init();
    buttons_board_init();

    logger.info("--- BLE central ---");

    logger.write_data(logger::level::debug,
                      NRF_FICR->DEVICEADDR,
                      8u, false, io::data_prefix::address);

    constexpr uint8_t const             nordic_config_tag = 1u;
    nordic::ble_stack                   ble_stack(nordic_config_tag);

    /// @todo The connection interval needs to be thought through
    /// for a specific device.
    ble::gap::connection_parameters const connection_parameters(
        ble::gap::connection_interval_msec(100),
        ble::gap::connection_interval_msec(200),
        0u,
        ble::gap::supervision_timeout_msec(4000u));

    /// @todo This also is an arbitrary MTU size just to test the MTU
    /// exchange operation.
    /// Chose something more applicable to the specific client application.
    ble::att::length_t const mtu_size = 240u;

    nordic::ble_gap_scanning                gap_scanning;
    nordic::ble_gap_operations              gap_operations;
    ble_gap_connection                      gap_connection(gap_operations,
                                                           gap_scanning,
                                                           connection_parameters,
                                                           mtu_size);

    ble_gattc_observer                      gattc_observer;
    nordic::ble_gattc_operations            gattc_operations;
    nordic::ble_gattc_discovery_operations  gattc_service_discovery;
    ble::gattc::service_builder             gattc_service_builder(gattc_service_discovery);
    free_lists_alloc(gattc_service_builder);

    ble::profile::central                   ble_central(ble_stack,
                                                        gap_connection,
                                                        gattc_observer,
                                                        gattc_operations,
                                                        gattc_service_builder);

    nordic::ble_observables& nordic_observables = nordic::ble_observables::instance();

    ble::gap::event_logger                  gap_event_logger(logger::level::info);
    nordic::ble_gap_event_observer          nordic_gap_event_logger(gap_event_logger);
    nordic::ble_gap_event_observer          nordic_gap_event_observer(gap_connection);
    nordic::ble_gattc_event_observer        nordic_gattc_event_observer(gattc_observer);
    nordic::ble_gattc_discovery_observer    nordic_gattc_discovery_observer(gattc_service_builder);

    nordic_observables.gap_event_observable.attach_first(nordic_gap_event_logger);
    nordic_observables.gap_event_observable.attach(nordic_gap_event_observer);
    nordic_observables.gattc_event_observable.attach(nordic_gattc_event_observer);
    nordic_observables.gattc_discovery_observable.attach(nordic_gattc_discovery_observer);

    unsigned int const peripheral_count = 0u;
    unsigned int const central_count    = 1u;
    ble_central.ble_stack().init(peripheral_count, central_count);
    ble_central.ble_stack().enable();

    ble::stack::version const version = ble_central.ble_stack().get_version();

    logger::instance().info("version: %s, git hash: %02x%02x%02x%02x",
                            version_info.version,
                            version_info.git_hash[0u],
                            version_info.git_hash[1u],
                            version_info.git_hash[2u],
                            version_info.git_hash[3u]);

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

    logger.info("alloc: services: %u 0x%04x, characteristics: %u 0x%04x, descriptors: %u 0x%04x",
                std::size(services_list),        sizeof(services_list),
                std::size(characteristics_list), sizeof(characteristics_list),
                std::size(descriptors_list),     sizeof(descriptors_list));

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

