/**
 * @file ble/nordic_ble_att.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/att.h"
#include "ble/uuid.h"
#include "ble/gatt_declaration.h"

#include <ble_gatt.h>

#include <cstdint>

namespace nordic
{

/**
 * Convert the Nordic ble_uuid_t type to ble::att::uuid type.
 * @note The softdevice function sd_ble_uuid_vs_add() must have previously been
 * called in order for this function to work properly.
 *
 * @param uuid The Nordic uuid abreviated type.
 *
 * @return ble::att::uuid The generic uuid type.
 */
ble::att::uuid to_att_uuid(ble_uuid_t const& uuid);

/**
 * Convert a Nordic ble_uuid_t uuid type to the ble::att::uuid type.
 *
 * @note In Nordic's little-endian based API UUID bytes [12:15] correspond
 * to ble::att::uuid bytes [3:0] and that the byte order storage is reversed:
 *     ble::att::uuid[0] -> Nordic UUID[15]
 *     ble::att::uuid[1] -> Nordic UUID[14]
 *     ble::att::uuid[2] -> Nordic UUID[13]
 *     ble::att::uuid[3] -> Nordic UUID[12]
 * @see ble::att::uuid constructor for uint32_t values.
 *
 * @example When converting a 16-bit uuid to a 128-bit ble::att::uuid
 * The 16-bit uuid 0x2A01 the result ble::att::uuid will be:
 * 00002A01-0010-0080-00805F9834FB
 * @cite Heydon, Robin "Bluetooth Low Energy: The Developer's Handbook"
 *       10.2.3. Attribute Type, p. 190
 */
ble::att::uuid to_att_uuid(ble_uuid128_t const& uuid);

ble_uuid128_t from_att_uuid_128(ble::att::uuid const& uuid);
ble_uuid_t    from_att_uuid_16 (ble::att::uuid const& uuid);

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
