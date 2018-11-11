/**
 * @file nordic_gatts.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * An adapter from the generic GATT classes to the Nordic SDK.
 */

#pragma once

#include "ble/gatt_service.h"
#include "ble/gatt_service_container.h"

#include <cstdint>

namespace nordic
{

/**
 * Add the service to the GATT sever using the Nordic softdevice.
 *
 * @param service A reference to the service to add.
 *                The service object life time is effectively forever.
 *                (As long as the BLE connect is in use and may be in use).
 * @note The service reference is not const; the handle of the GATTS
 *       characterisitics are updated with they are added.
 *
 * @return uint32_t The Nordic error code.
 * @retval NRF_SUCCESS if successful.
 */
uint32_t gatts_service_add(ble::gatt::service& service);

} // namespace nordic
