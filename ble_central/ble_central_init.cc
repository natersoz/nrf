/**
 * @file ble_central_class/ble_central_init.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "logger.h"

#include "ble/att.h"
#include "ble/gap_types.h"
#include "ble/gatt_uuids.h"
#include "ble/tlv_encode.h"

#include "ble/profile_central.h"
#include "ble/gap_connection.h"
#include "ble/service/gap_service.h"
#include "ble/service/gatt_service.h"

#include "ble/nordic_ble_stack.h"
#include "ble/nordic_ble_peer.h"
// #include "ble/nordic_ble_gattc_operations.h"

#include "ble_gap_connection.h"

/**
 * @todo List of implementations yet to be done:
 * + ble_gap_connection specific to central
 * + ble::profile::central
 *
 */

constexpr uint8_t const                     nordic_config_tag = 1u;
static nordic::ble_stack                    ble_stack(nordic_config_tag);

static nordic::ble_gap_operations           ble_gap_operations;
static ble_gap_connection                   ble_gap_connection(
                                                ble_gap_operations);

static ble_gatts_observer                   ble_gatts_observer;
static nordic::ble_gatts_operations         gatts_operations;
static ble::profile::central                ble_central(ble_stack,
                                                        ble_gap_connection,
                                                        ble_gatts_observer,
                                                        gatts_operations);
// GAP service: 0x1800
//   device name: uuid = 0x2a00
//   appearance : uuid = 0x2a01
//   ppcp       : uuid = 0x2a04
static ble::service::device_name            device_name_characteristic(
                                                device_name,
                                                device_name_length);
static ble::service::appearance             appearance_characteristic(
                                                ble::gatt::appearance::unknown);
static ble::service::ppcp                   ppcp(connection_parameters);
static ble::service::gap_service            gap_service;

// Note that using the Nordic softdevice the GATT service does not have
// any effect. It is here for completeness.
// In another silicon vendor this will have meaning.
// See comments in nordic::gatts_service_add().
static ble::service::gatt_service           gatt_service;


ble::profile::central& ble_central_init()
{
    ble_central.ble_stack().init();
    ble_central.ble_stack().enable();

    /// @todo consider having each gap, gatts, gattc interface expose a
    /// post ctor init() method so that these init() calls can be made from
    /// the interface level; like so: ble_central.connection().init().
    ble_gap_connection.init();
    ble_gatts_observer.init();

    ble_peer_init();

    // ----- GAP service
    gap_service.characteristic_add(device_name_characteristic);
    gap_service.characteristic_add(appearance_characteristic);
    gap_service.characteristic_add(ppcp);

    // ----- Battery Service
    battery_service.characteristic_add(battery_level_characteristic);
    battery_service.characteristic_add(battery_power_characteristic);

    // ----- Custom ADC Sensor Service
    adc_sensor_service.characteristic_add(adc_samples_characteristic);
    adc_sensor_service.characteristic_add(adc_enable_characteristic);
    adc_samples_characteristic.set_adc_sensor_acq(adc_sensor_acq);

    adc_sensor_acq.init();

    // ----- Add the services to the central.
    ble_central.service_add(gap_service);
    ble_central.service_add(gatt_service);
    ble_central.service_add(battery_service);
    ble_central.service_add(adc_sensor_service);

    set_advertising_data(ble_central.advertising().advertising_data);

    return ble_central;
}



