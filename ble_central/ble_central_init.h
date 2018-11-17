/**
 * @file ble_central/ble_central_init.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/profile_central.h"

/**
 * Create a BLE central specific to the application requirements.
 *
 * @return ble::profile::central& The initialized BLE central ready
 * for use. In this case the instance is statically allocated;
 * Its lifetime is forever.
 */
ble::profile::central& ble_central_init();

