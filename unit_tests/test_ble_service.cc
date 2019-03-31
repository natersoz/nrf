/**
 * @file test_ble_service.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gtest/gtest.h"

#include "ble/service/gap_service.h"
#include "ble/gatt_characteristic.h"
#include "ble/gatt_attribute.h"

#include "ble/service/battery_service.h"
#include "ble/service/current_time_service.h"

#include "ble/gatt_write_ostream.h"
#include "std_stream.h"
#include "null_stream.h"

static io::nullout_stream os;   // Change to io::stdout_stream for debug output

TEST(BleService, GapService)
{
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

    ASSERT_EQ(gap_service.characteristic_list.size(), 3u);

    for (auto const& node : gap_service.characteristic_list)
    {
        ble::gatt::characteristic const& characteristic =
            dynamic_cast<ble::gatt::characteristic const&>(node);

        ASSERT_EQ(characteristic.uuid.is_ble(), true);
        ASSERT_TRUE(
            (characteristic.uuid ==
             ble::att::uuid(ble::gatt::characteristic_type::device_name)) ||
            (characteristic.uuid ==
             ble::att::uuid(ble::gatt::characteristic_type::appearance)) ||
            (characteristic.uuid ==
             ble::att::uuid(ble::gatt::characteristic_type::ppcp)));
    }

    ble::gatt::service_write(os, gap_service);
    os.write("\n", 1u);
}

TEST(BleService, BatteryService)
{
    ble::service::battery_level         battery_level_characteristic;
    ble::service::battery_power_state   battery_power_characteristic;
    ble::service::battery_service       battery_service;

    battery_service.characteristic_add(battery_level_characteristic);
    battery_service.characteristic_add(battery_power_characteristic);

    ASSERT_EQ(battery_service.characteristic_list.size(), 2u);

    for (auto const& node : battery_service.characteristic_list)
    {
        ble::gatt::characteristic const& characteristic =
            dynamic_cast<ble::gatt::characteristic const&>(node);

        ASSERT_EQ(characteristic.uuid.is_ble(), true);
        ASSERT_TRUE(
            (characteristic.uuid ==
             ble::att::uuid(ble::gatt::characteristic_type::battery_level)) ||
            (characteristic.uuid ==
             ble::att::uuid(ble::gatt::characteristic_type::battery_power_state)));
    }

    ble::gatt::service_write(os, battery_service);
    os.write("\n", 1u);
}

TEST(BleService, CurrentTimeService)
{
    ble::service::current_time_service current_time_service;

    for (auto const& node : current_time_service.characteristic_list)
    {
        ble::gatt::characteristic const& characteristic =
            dynamic_cast<ble::gatt::characteristic const&>(node);

        ASSERT_EQ(characteristic.uuid.is_ble(), true);
        ASSERT_TRUE((characteristic.uuid ==
                     ble::att::uuid(ble::gatt::characteristic_type::date_time)));
    }

    ble::gatt::service_write(os, current_time_service);
    os.write("\n", 1u);
}

TEST(BleService, CustomService)
{
    ble::att::uuid const uuid_service({
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    });

    ble::att::uuid const uuid_characteristic({
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    });

    ble::gatt::properties const props(ble::gatt::properties::read   |
                                      ble::gatt::properties::write  |
                                      ble::gatt::properties::notify |
                                      0u);

    ble::gatt::service custom_service(
        uuid_service, ble::gatt::attribute_type::primary_service);

    ble::gatt::characteristic custom_characteristic(
        uuid_characteristic,
        ble::gatt::properties::broadcast |
        ble::gatt::properties::read      |
        ble::gatt::properties::write     |
        ble::gatt::properties::notify
        );

    custom_service.characteristic_add(custom_characteristic);

    for (auto const& node : custom_service.characteristic_list)
    {
        ble::gatt::characteristic const& characteristic =
            dynamic_cast<ble::gatt::characteristic const&>(node);

        ASSERT_EQ(characteristic.uuid.is_ble(), false);
        ASSERT_EQ(characteristic.uuid, uuid_characteristic);
    }

    ble::gatt::service_write(os, custom_service);
    os.write("\n", 1u);
}
