/**
 * @file ble_peripheral/ble_peripheral_init.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/profile_peripheral.h"

/**
 * Create a BLE peripheral specific to the application requirements.
 * This is a builder-ish pattern which instantiates and aggregates the
 * BLE components required for an application specific BLE peripheral.
 *
 * @return ble::profile::peripheral& The initialized BLE peripheral ready
 * for use. In this case the instance is statically allocated;
 * Its lifetime is forever.
 */
ble::profile::peripheral& ble_peripheral_init();

