/**
 * @file nordic_gatts.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * An adapter from the generic GATT classes to the Nordic SDK.
 */

#pragma once

#include "ble/gatt_service.h"
#include <cstdint>

namespace nordic
{

/**
 * Add the service to the GATT sever using the Nordic softdevice.
 *
 *
 *
 * @param service
 *
 * @return uint32_t The Nordic error code.
 * @retval NRF_SUCCESS if successful.
 */
uint32_t gatts_service_add(ble::gatt::service& service);

} // namespace nordic
