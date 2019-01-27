/**
 * @file ble/gatt_write_ostream.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Write to an io:output_stream for use in debugging BLE GATT data structures.
 */

#pragma once

#include "ble/gatt_service.h"
#include "ble/gatt_service_container.h"
#include "ble/gatt_characteristic.h"
#include "ble/gatt_attribute.h"
#include "stream.h"

namespace ble
{
namespace gatt
{

void attribute_write(io::output_stream& os, ble::gatt::attribute const& attribute);
void service_write(io::output_stream& os, ble::gatt::service const& service);
void service_container_write(io::output_stream& os,
                             ble::gatt::service_container const& service_container);

}  // namespace gatt
}  // namespace ble
