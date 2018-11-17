/**
 * @file ble/nordic_ble_att.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/att.h"
#include "ble/uuid.h"
#include "ble/gatt_declaration.h"

#include "ble_gatt.h"       // Nordic softdevice

#include <cstdint>

namespace nordic
{

/**
 * Convert a generic ble::att::error_code which is based on
 * Bluetooth Core Specification 5.0m Part F
 * Section 3.4.1.1 Error Response, Table 3.3: Error Codes
 * into a Nordic error code; which contains offsets.
 *
 * @param error_code @see ble::att::error_code
 * @return uint16_t The Nordic equivalent error code.
 */
uint16_t from_att_error_code(ble::att::error_code error_code);

/// Convert a Nordic error code into a BLE Core ble::att::error_code.
ble::att::error_code to_att_error_code(uint16_t nordic_error_code);

/// Convert a Nordic ble_uuid_t uuid type to the ble::att::uuid type.
ble::att::uuid to_att_uuid(ble_uuid_t const& uuid);

/// Convert a Nordic ble_gatt_char_props_t struct to ble::gatt::properties.
ble::gatt::properties to_att_properties(ble_gatt_char_props_t     props,
                                        ble_gatt_char_ext_props_t props_ext);

ble::gatt::properties to_att_properties(ble_gatt_char_props_t props);

/**
 * Convert from Nordic BLE_GATT_WRITE_OPS GATT Write operation type to
 * ble::att::op_ocode type
 */
ble::att::op_code to_att_write_op_code(uint8_t nordic_gatt_write_op);

} // namespace nordic
