/**
 * @file custom_uuid.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/uuid.h"

namespace ble
{
namespace service
{
namespace custom
{

enum class services: uint16_t
{
    adc_sensor      = 0x0001,
};

enum class characteristics: uint16_t
{
    adc_samples     = 0x0001,
    adc_enable      = 0x0002,
    adc_scaling     = 0x0003,
};

/// A read-only reference to the custom uuid.
extern boost::uuids::uuid const uuid_base;

/**
 * Set an existing uuid to a custom service value.
 *
 * @param uuid              The uuid to set.
 * @param service           The 16-bit custom ID for the service.
 * @return ble::att::uuid&  A reference to the uuid passed in,
 *                          merely for convenience.
 */
ble::att::uuid& uuid_service(ble::att::uuid& uuid, services service);

/**
 * Create (return by value) a uuid with a custom service value.
 *
 * @param service           The 16-bit custom ID for the service.
 * @return ble::att::uuid   The uuid set with the custom service value.
 */
ble::att::uuid uuid_service(services service);

/**
 * Set an existing uuid to a custom characteristic value.
 *
 * @param uuid              The uuid to set.
 * @param service           The 16-bit custom ID for the characteristic.
 * @return ble::att::uuid&  A reference to the uuid passed in,
 *                          merely for convenience.
 */
ble::att::uuid& uuid_characteristic(ble::att::uuid& uuid,
                                    characteristics characteristic);

/**
 * Create (return by value) a uuid with a custom characteristic value.
 *
 * @param service           The 16-bit custom ID for the characteristic.
 * @return ble::att::uuid   The uuid set with the custom characteristic value.
 */
ble::att::uuid uuid_characteristic(characteristics characteristic);

} // namespace custom
} // namespace service
} // namespace ble
