/**
 * @file nordic_ble_event_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/common_event_observer.h"      // BLE event observer interfaces
#include "ble/gap_event_observer.h"
#include "ble/gattc_event_observer.h"
#include "ble/gattc_discovery_observer.h"
#include "ble/gatts_event_observer.h"
#include "ble/profile_connectable.h"

#include <ble.h>                            // Nordic BLE softdevice headers
#include <ble_gap.h>
#include <ble_gattc.h>
#include <ble_gatts.h>

#include <cstdint>

namespace nordic
{
void ble_common_event_notify(ble::common::event_observer&   observer,
                             enum BLE_COMMON_EVTS           event_type,
                             ble_common_evt_t const&        event_data);

void ble_gap_event_notify(ble::gap::event_observer&         observer,
                          enum BLE_GAP_EVTS                 gap_event_type,
                          ble_gap_evt_t const&              gap_event_data);

void ble_gatts_event_notify(ble::gatts::event_observer&     observer,
                            enum BLE_GATTS_EVTS             gatts_event_type,
                            ble_gatts_evt_t const&          gatts_event_data);

void ble_gattc_event_notify(ble::gattc::event_observer&     observer,
                            enum BLE_GATTC_EVTS             event_type,
                            ble_gattc_evt_t const&          event_data);

void ble_discovery_response(ble::gattc::discovery_observer& observer,
                            enum BLE_GATTC_EVTS             event_type,
                            ble_gattc_evt_t const&          event_data);

/**
 * Enable the Nordic softdevice events to be observed by the
 * ble::profile::connectable class.
 *
 * @param ble_connectable The ble::profile::connectable reference to receive
 *                        BLE events.
 */
void register_ble_connectable(ble::profile::connectable& ble_connectable);

/**
 * Disable the Nordic softdevice events from being observed by the
 * ble::profile::connectable class.
 *
 * @param ble_connectable The ble::profile::connectable reference to disable
 *                        receiving BLE events.
 */
void deregister_ble_connectable(ble::profile::connectable& ble_connectable);

/**
 * Specialized hack to aquire 128-bit UUIDs which have not been
 * pre-registered with the Nordic softdevice.
 *
 * @param connection_handle The connection handle to re-request the GATT handle
 *                          from in order to resolve it.
 * @param gatt_handle       The attribute handle for which a 128-bit UUID was
 *                          reported by the Nordic softdevice as 'unknown'.
 *
 * @return uint32_t         NRF_SUCCESS if successful.
 */
uint32_t gattc_uuid128_acquire(uint16_t connection_handle, uint16_t gatt_handle);

} // namespace nordic
