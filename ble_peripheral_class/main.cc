/**
 * @file ble_peripheral_class/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include <cstdint>
#include <cstring>

#include "app_timer.h"
#include "clocks.h"
#include "leds.h"
#include "buttons.h"
#include "logger.h"
#include "segger_rtt_output_stream.h"
#include "rtc_observer.h"
#include "project_assert.h"

#include "ble/att.h"
#include "ble/gap_types.h"
#include "ble/gatt_uuids.h"
#include "ble/tlv_encode.h"

#include "ble/peripheral.h"
#include "ble/gap_connection.h"
#include "ble/service/battery_service.h"
#include "ble/service/current_time_service.h"
#include "ble/service/gap_service.h"
#include "ble/service/gatt_service.h"

#include "ble/nordic_ble_stack.h"
#include "ble/nordic_ble_gap_advertising.h"
#include "ble/nordic_ble_peer.h"
#include "ble/nordic_ble_gatts.h"

#include "nordic/device_address.h"

#include "ble_gap_connection.h"
#include "ble_gatts_observer.h"

static segger_rtt_output_stream rtt_os;

static rtc_observable<> rtc_1(1u, 32u);

static constexpr uint16_t const advertising_interval = ble::gap::advertising::interval_msec(100u);
static nordic::gap_advertising ble_advertising(advertising_interval);

static constexpr char const device_name[]       = "periph_class";
static constexpr char const device_name_short[] = "cls";

static size_t set_advertising_data(ble::gap::advertising_data_t &data)
{
    ble::gatt::services const services_16[] = {
        ble::gatt::services::device_information,
        ble::gatt::services::battery_service,
        ble::gatt::services::current_time_service
    };

    size_t length = 0u;

    length += ble::tlv_encode(data,
                              ble::gap_type::flags,
                              ble::le_general_discovery);

    length += ble::tlv_encode(data,
                              ble::gap_type::local_name_short,
                              device_name_short,
                              std::size(device_name_short) - 1u);

    length += ble::tlv_encode_address(data,
                                      false,
                                      nordic::get_device_address());

    length += ble::tlv_encode(data,
                              ble::gap_type::uuid_service_16_incomplete,
                              services_16,
                              std::size(services_16));
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

    /// @todo Connection interval needs to be thought through for a specific device.
    ble::gap::connection_parameters const connection_parameters(
        ble::gap::connection_interval_msec(100),
        ble::gap::connection_interval_msec(200),
        0u,
        ble::gap::supervision_timeout_msec(4000u));

    uint8_t const connection_configuration_tag = 1u;
    nordic::ble_stack ble_stack(connection_configuration_tag);

    // Implementations specific to the ble_peripheral_class.
    nordic::ble_gap_request_response ble_gap_request_response;
    ble_gap_connection ble_gap_connection(ble_gap_request_response,
                                          ble_advertising,
                                          connection_parameters);
    ble_gatts_observer ble_gatts_observer;

    ble::peripheral ble_peripheral(ble_stack,
                                   ble_gap_connection,
                                   ble_gatts_observer);

    ble_peripheral.ble_stack().init();
    ble_peripheral.ble_stack().enable();

    ble_peer_init();

    // gap service: 0x1800
    //   device name: uuid = 0x2a00
    //   appearance : uuid = 0x2a01
    //   ppcp       : uuid = 0x2a04
    ble::service::device_name device_name_characteristic(device_name, std::size(device_name) - 1u);
    ble::service::appearance  appearance_characteristic(ble::gatt::appearance::unknown);
    ble::service::ppcp        ppcp(connection_parameters);
    ble::service::gap_service gap_service;
    gap_service.characteristic_add(device_name_characteristic);
    gap_service.characteristic_add(appearance_characteristic);
    gap_service.characteristic_add(ppcp);

    // Note that using the Nordic softdevice the GATT service  does not have
    // any effect. It is here for completeness.
    // In another silicon vendor this will have meaning.
    // See comments in nordic::gatts_service_add().
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

    set_advertising_data(ble_peripheral.connection().advertising().advertising_data);
    ble_peripheral.connection().advertising().start();

    /// @todo this is for debug, remove once characteristics work properly.
    battery_level_characteristic.write_battery_percentage(88u);

    for (;;)
    {
        rtt_os.flush();
        if (rtt_os.write_pending() == 0)
        {
            __WFE();
        }
    }
}

