/**
 * @file gatt_enum_types_strings.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include "gatt_enum_types.h"

namespace ble
{
namespace gatt
{

char const* to_string(ble::gatt::attribute_type         attribute_type);
char const* to_string(ble::gatt::descriptor_type        descriptor_type);
char const* to_string(ble::gatt::service_type           service_type);
char const* to_string(ble::gatt::characteristic_type    characteristic_type);
char const* to_string(ble::gatt::units_type             units_type);

} // namespace gatt
} // namespace ble
