/**
 * @file ble_peripheral/ble_peripheral_init.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "logger.h"

#include "ble/att.h"
#include "ble/gap_types.h"
#include "ble/gatt_enum_types.h"
#include "ble/ltv_encode.h"

#include "ble/gap_connection.h"
#include "ble/gap_event_logger.h"
#include "ble/profile_peripheral.h"
#include "ble/service/gap_service.h"
#include "ble/service/gatt_service.h"
#include "ble/service/device_information_service.h"
#include "ble/service/battery_service.h"
#include "ble/service/current_time_service.h"
#include "ble/service/nordic_saadc_sensor_service.h"

#include "ble/nordic_ble_event_observable.h"
#include "ble/nordic_ble_event_observer.h"
#include "ble/nordic_ble_gap_advertising.h"
#include "ble/nordic_ble_gap_operations.h"
#include "ble/nordic_ble_gatts.h"
#include "ble/nordic_ble_gatts_operations.h"
#include "ble/nordic_ble_peer.h"
#include "ble/nordic_ble_stack.h"
#include "nordic/device_address.h"

#include "ble_gap_connection.h"
#include "ble_gatts_observer.h"
#include "nordic_saadc_sensor_acquisition.h"

#include "nrf_cmsis.h"

static constexpr uint16_t const advertising_interval =
    ble::gap::advertising::interval_msec(100u);
static nordic::ble_gap_advertising gap_advertising(advertising_interval);

static constexpr char   const device_name[]       = "periph";
static constexpr size_t const device_name_length  = std::size(device_name) - 1u;
static constexpr char   const short_name[]        = "periph";
static constexpr size_t const short_name_length   = std::size(short_name) - 1u;

static size_t set_advertising_data(ble::gap::advertising_data &data)
{
    ble::gatt::service_type const services_16[] = {
        ble::gatt::service_type::device_information,
        ble::gatt::service_type::battery_service,
        ble::gatt::service_type::current_time_service,
    };

    size_t length = 0u;

    constexpr uint8_t const le_discovery =
        static_cast<uint8_t>(ble::gap::le_general_discovery);
    length += ble::gap::ltv_encode(data,
                                   ble::gap::type::flags,
                                   le_discovery);

    length += ble::gap::ltv_encode(data,
                                   ble::gap::type::local_name_short,
                                   short_name, short_name_length);

    length += ble::gap::ltv_encode_address(data,
                                           nordic::get_device_address());

    length += ble::gap::ltv_encode(data,
                                   ble::gap::type::uuid_service_16_incomplete,
                                   services_16,
                                   std::size(services_16));
    return length;
}

/// @todo The connection interval needs to be thought through for a specific device.
static ble::gap::connection_parameters const gap_connection_parameters(
    ble::gap::connection_interval_msec(100),
    ble::gap::connection_interval_msec(200),
    0u,
    ble::gap::supervision_timeout_msec(4000u));

constexpr uint8_t const                     nordic_config_tag = 1u;
static nordic::ble_stack                    ble_stack(nordic_config_tag);

static nordic::ble_gap_operations           gap_operations;
static ble_gap_connection                   gap_connection(
                                                gap_operations,
                                                gap_advertising,
                                                gap_connection_parameters);

static ble_gatts_observer                   gatts_observer;
static nordic::ble_gatts_operations         gatts_operations;
static ble::profile::peripheral             ble_peripheral(ble_stack,
                                                           gap_connection,
                                                           gatts_observer,
                                                           gatts_operations);

static ble::gap::event_logger               gap_event_logger(logger::level::info);
static nordic::ble_gap_event_observer       nordic_gap_event_logger(gap_event_logger);
static nordic::ble_gap_event_observer       nordic_gap_event_observer(gap_connection);
static nordic::ble_gatts_event_observer     nordic_gatts_event_observer(gatts_observer);

// GAP service: 0x1800
//   device name: uuid = 0x2a00
//   appearance : uuid = 0x2a01
//   ppcp       : uuid = 0x2a04
static ble::service::device_name            device_name_characteristic(
                                                device_name,
                                                device_name_length);
static ble::service::appearance             appearance_characteristic(
                                                ble::gatt::appearance::unknown);
static ble::service::ppcp                   ppcp(gap_connection_parameters);
static ble::service::gap_service            gap_service;

static ble::service::device_information_service
                                            device_information_service;
static ble::service::serial_number_string<sizeof(NRF_FICR->DEVICEADDR) * 2u>
    nordic_serial_number_characteristic(
        const_cast<uint32_t const*>(NRF_FICR->DEVICEADDR),
        sizeof(NRF_FICR->DEVICEADDR));

// Note that using the Nordic softdevice the GATT service does not have
// any effect. It is here for completeness.
// In another silicon vendor this will have meaning.
// See comments in nordic::gatts_service_add().
static ble::service::gatt_service           gatt_service;

// ----- Battery Service
static ble::service::battery_service        battery_service;
static ble::service::battery_level          battery_level_characteristic;
static ble::service::battery_power_state    battery_power_characteristic;

// ----- Current Time Service
static ble::service::current_time_service   current_time_service;

// ----- Custom ADC Sensor Service
static nordic::saadc_sensor_service         adc_sensor_service;
static nordic::saadc_samples_characteristic adc_samples_characteristic;
static nordic::saadc_enable_characteristic  adc_enable_characteristic;

static timer_observable<>                   timer_1_observable(1u);
static nordic::saadc_sensor_acquisition     adc_sensor_acq(
                                                adc_samples_characteristic,
                                                timer_1_observable);

ble::profile::peripheral& ble_peripheral_init()
{
    unsigned int const peripheral_count = 1u;
    unsigned int const central_count    = 0u;
    ble_peripheral.ble_stack().init(peripheral_count, central_count);
    ble_peripheral.ble_stack().enable();
    ble::stack::version const version = ble_peripheral.ble_stack().get_version();

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

    nordic::ble_observables& nordic_observables = nordic::ble_observables::instance();

    nordic_observables.gap_event_observable.attach_first(nordic_gap_event_logger);
    nordic_observables.gap_event_observable.attach(nordic_gap_event_observer);
    nordic_observables.gatts_event_observable.attach(nordic_gatts_event_observer);

    ble_peer_init();

    // ----- GAP service
    gap_service.characteristic_add(device_name_characteristic);
    gap_service.characteristic_add(appearance_characteristic);
    gap_service.characteristic_add(ppcp);

    // ----- Deivce Information Service
    device_information_service.characteristic_add(nordic_serial_number_characteristic);

    // ----- Battery Service
    battery_service.characteristic_add(battery_level_characteristic);
    battery_service.characteristic_add(battery_power_characteristic);

    // ----- Custom ADC Sensor Service
    adc_sensor_service.characteristic_add(adc_samples_characteristic);
    adc_sensor_service.characteristic_add(adc_enable_characteristic);
    adc_samples_characteristic.set_adc_sensor_acq(adc_sensor_acq);

    adc_sensor_acq.init();

    // ----- Add the services to the peripheral.
    ble_peripheral.service_add(gap_service);
    ble_peripheral.service_add(gatt_service);
    ble_peripheral.service_add(device_information_service);
    ble_peripheral.service_add(battery_service);
    ble_peripheral.service_add(current_time_service);
    ble_peripheral.service_add(adc_sensor_service);

    set_advertising_data(ble_peripheral.advertising().data);

    return ble_peripheral;
}
