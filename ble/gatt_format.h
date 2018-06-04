/**
 * @file gatt_format.h
 */

#pragma once
#include <cstdint>

namespace ble
{
namespace gatt
{

/**
 * @enum class format
 *
 * @see Bluetooth Core Specification 5.0, Volume 3, Part G
 * 3.3.3.5.2 Format, Table 3.16: Characteristic Format types
 */
enum class format
{
    rfu                     = 0x00,     ///< Reserved for Future Use
    boolean                 = 0x01,     ///< unsigned 1-bit; 0 = false, 1 = true
    two_bit                 = 0x02,     ///< unsigned 2-bit integer
    nibble                  = 0x03,     ///< unsigned 4-bit integer
    uint8                   = 0x04,     ///< unsigned 8-bit integer
    uint12                  = 0x05,     ///< unsigned 12-bit integer
    uint16                  = 0x06,     ///< unsigned 16-bit integer
    uint24                  = 0x07,     ///< unsigned 24-bit integer
    uint32                  = 0x08,     ///< unsigned 32-bit integer
    uint48                  = 0x09,     ///< unsigned 48-bit integer
    uint64                  = 0x0A,     ///< unsigned 64-bit integer
    uint128                 = 0x0B,     ///< unsigned 128-bit integer
    sint8                   = 0x0C,     ///< signed 8-bit integer
    sint12                  = 0x0D,     ///< signed 12-bit integer
    sint16                  = 0x0E,     ///< signed 16-bit integer
    sint24                  = 0x0F,     ///< signed 24-bit integer
    sint32                  = 0x10,     ///< signed 32-bit integer
    sint48                  = 0x11,     ///< signed 48-bit integer
    sint64                  = 0x12,     ///< signed 64-bit integer
    sint128                 = 0x13,     ///< signed 128-bit integer
    float32                 = 0x14,     ///< IEEE-754 32-bit floating point
    float64                 = 0x15,     ///< IEEE-754 64-bit floating point
    SFLOAT                  = 0x16,     ///< IEEE-11073 16-bit SFLOAT
    FLOAT                   = 0x17,     ///< IEEE-11073 32-bit FLOAT
    duint16                 = 0x18,     ///< IEEE-20601 format
    utf8s                   = 0x19,     ///< UTF-8 string
    utf16s                  = 0x1A,     ///< UTF-16 string
    opaque_struct           = 0x1B,     ///< Opaque structure
}; // enum class format

} // namespace gatt
} // namespace ble


