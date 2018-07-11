/**
 * @file ble_peripheral_class/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "app_timer.h"
#include "clocks.h"
#include "leds.h"
#include "buttons.h"
#include "logger.h"
#include "segger_rtt_output_stream.h"
#include "rtc_observer.h"
#include "project_assert.h"

#include "ble/advertising.h"
#include "ble/att.h"
#include "ble/gap_event_observer.h"
#include "ble/gap_types.h"
#include "ble/gatt_uuids.h"
#include "ble/nordic_ble_stack.h"
#include "ble/nordic_gatts.h"
#include "ble/service/battery_service.h"
#include "ble/service/current_time_service.h"
#include "ble/service/gap_service.h"
#include "ble/service/gatt_service.h"
#include "ble/tlv_encode.h"
#include "nordic/device_address.h"

#include "ble/nordic_ble_stack.h"
#include "ble/nordic_ble_peer.h"

static segger_rtt_output_stream rtt_os;

static rtc_observable<> rtc_1(1u, 32u);

static uint16_t const advertising_interval = ble::advertising::interval_msec(100u);
static ble::advertising advertising(advertising_interval);

static constexpr char const device_name_str[]  = "periph_class";
static constexpr char const device_short_str[] = "cls";
static constexpr ble::att::length_t const device_name_length  = sizeof(device_name_str)  - 1u;
static constexpr ble::att::length_t const device_short_length = sizeof(device_short_str) - 1u;

static size_t set_advertising_data(ble::advertising_data_t &data)
{
    ble::gatt::services const services_16[] = {
        ble::gatt::services::device_information,
        ble::gatt::services::battery_service,
        ble::gatt::services::current_time_service
    };

    size_t const services_16_count = sizeof(services_16) / sizeof(services_16[0]);

    size_t length = 0u;

    length += ble::tlv_encode(data,
                              ble::gap_type::flags,
                              ble::le_general_discovery);

    length += ble::tlv_encode(data,
                              ble::gap_type::local_name_short,
                              device_short_str,
                              device_short_length);

    length += ble::tlv_encode_address(data,
                                      false,
                                      nordic::get_device_address());

    length += ble::tlv_encode(data,
                              ble::gap_type::uuid_service_16_incomplete,
                              services_16,
                              services_16_count);
    return length;
}

int main(void)
{
    lfclk_enable(LFCLK_SOURCE_XO);
    app_timer_init(rtc_1);
    rtc_1.start();

    leds_board_init();
    buttons_board_init();

    logger& logger = logger::instance();
    logger.set_rtc(rtc_1);
    logger.set_level(logger::level::debug);
    logger.set_output_stream(rtt_os);

    logger.info("--- BLE peripheral classes ---");

    ble::nordic::stack ble_stack(rtc_1);
    ble_stack.init();
    ble_stack.enable();

    ble_peer_init();

    /// @todo "periph_class" is common with advertising device name.
    ble::service::device_name device_name_characteristic(device_name_str, device_name_length);
    ble::service::appearance  appearance_characteristic(ble::gatt::appearance::unknown);

    /// @todo Connection interval needs to be thought through for a specific device.
    ble::gap::connection_parameters const connection_parameters(
        ble::gap::connection_interval_msec(100),
        ble::gap::connection_interval_msec(200),
        0u,
        ble::gap::supervision_timeout_msec(4000u));

    ble::service::ppcp          ppcp(connection_parameters);
    ble::service::gap_service   gap_service;
    gap_service.characteristic_add(device_name_characteristic);
    gap_service.characteristic_add(appearance_characteristic);
    gap_service.characteristic_add(ppcp);

    ble::service::gatt_service  gatt_service;

    ble::service::battery_service     battery_service;
    ble::service::battery_level       battery_level_characteristic;
    ble::service::battery_power_state battery_power_characteristic;
    battery_service.characteristic_add(battery_level_characteristic);
    battery_service.characteristic_add(battery_power_characteristic);

    ble::service::current_time_service current_time_service;
    current_time_service.current_time.greg_date.year    = 2018u;
    current_time_service.current_time.greg_date.month   =   12u;
    current_time_service.current_time.greg_date.day     =   13u;
    current_time_service.current_time.greg_date.hours   =   17u;
    current_time_service.current_time.greg_date.minutes =   29u;
    current_time_service.current_time.greg_date.seconds =   52u;

    nordic::gatts_service_add(gap_service);
    nordic::gatts_service_add(gatt_service);
    nordic::gatts_service_add(battery_service);
    nordic::gatts_service_add(current_time_service);

    set_advertising_data(advertising.advertising_data);
    advertising.start();

    for (;;)
    {
        rtt_os.flush();
        if (rtt_os.write_pending() == 0)
        {
            __WFE();
        }
    }
}

