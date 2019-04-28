/**
 * @file ble/nordic_ble_event_strings.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <ble.h>
#include <ble_gap.h>
#include <ble_gatts.h>
#include <ble_gattc.h>
#include <ble_l2cap.h>
#include <nrf_soc.h>

#include <cstdint>

namespace nordic
{

char const* ble_event_string(enum BLE_COMMON_EVTS ble_event);

char const* ble_gap_event_string(enum BLE_GAP_EVTS ble_gap_event);

char const* ble_gatts_event_string(enum BLE_GATTS_EVTS ble_gatts_event);

char const* ble_gattc_event_string(enum BLE_GATTC_EVTS ble_gattc_event);

char const* ble_l2cap_event_string(enum BLE_L2CAP_EVTS ble_l2cap_event);

char const* nrf_soc_event_string(enum NRF_SOC_EVTS ble_l2cap_event);

char const* event_string(uint16_t nordic_event);

} // namespace nordic
