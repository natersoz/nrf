/**
 * @file test_service_container.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gtest/gtest.h"

#include "ble/gatt_service_container.h"
#include "ble/service/gap_service.h"
#include "ble/service/battery_service.h"
#include "ble/service/current_time_service.h"
#include "ble/gap_connection_parameters.h"

#include "ble/gatt_write_ostream.h"
#include "std_stream.h"
#include "null_stream.h"
#include "logger.h"

static io::nullout_stream os;   // Change to io::stdout_stream for debug output

TEST(ServiceContainerTest, GAP_Battery_Time)
{
    logger& logger = logger::instance();
    logger.set_output_stream(os);
    logger.set_level(logger::level::info);

    // --- GAP service
    constexpr char   const device_name[]       = "periph";
    constexpr size_t const device_name_length  = std::size(device_name) - 1u;

    ble::gap::connection_parameters const gap_connection_parameters(
        ble::gap::connection_interval_msec(100),
        ble::gap::connection_interval_msec(200),
        0u,
        ble::gap::supervision_timeout_msec(4000u));

    ble::service::device_name   device_name_characteristic(device_name, device_name_length);
    ble::service::appearance    appearance_characteristic(ble::gatt::appearance::unknown);
    ble::service::ppcp          ppcp_characteristic(gap_connection_parameters);
    ble::service::gap_service   gap_service;

    gap_service.characteristic_add(device_name_characteristic);
    gap_service.characteristic_add(appearance_characteristic);
    gap_service.characteristic_add(ppcp_characteristic);

    // --- Battery Service
    ble::service::battery_level          battery_level_characteristic;
    ble::service::battery_power_state    battery_power_characteristic;
    ble::service::battery_service        battery_service;

    battery_service.characteristic_add(battery_level_characteristic);
    battery_service.characteristic_add(battery_power_characteristic);

    // --- Current Time Service
    ble::service::current_time_service  current_time_service;

    ble::gatt::service_container service_container;
    service_container.push_back(gap_service);
    service_container.push_back(battery_service);
    service_container.push_back(current_time_service);

    ble::gatt::service_container::discovery_iterator disco_iter = service_container.discovery_begin();
    auto const discovery_end = service_container.discovery_end();
    for (; disco_iter != discovery_end; ++disco_iter)
    {
        ble::gatt::service_container::discovery_iterator::iterator_node node = *disco_iter;
        ble::gatt::service_write(os, node.service);
        os.write("\n", 1u);
        ble::gatt::attribute_write(os, node.characteristic);
        os.write("\n", 1u);
    }
}

