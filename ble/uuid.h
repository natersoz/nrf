/**
 * @file uuid.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <boost/uuid/uuid.hpp>
#include "charconv.h"

#include <array>
#include <cstdint>

namespace ble
{
namespace att
{

/**
 * @struct ble::uuid
 * The bluetooth base UUID:
 * 00000000-0000-1000-8000-00805F9B34FB
 *
 * When a short 16-bit UUID is sent with value 0x2A01, it fits into
 * the Bluetooth base UUID as:
 * 00002A01-0000-1000-8000-00805F9B34FB
 * @note UUIDs are generally encoded in big-endian order.
 *       That is how this class contains the UUID: big-endian.
 * @note Nordic uses little endian format within softdevice calls.
 *       There is logic to this since uint16_t uuid values are little endian
 *       on the Nordic platorm. So 128-bit is consistent with 16-bit.
 * @see  ble_types.h, comments for ble_uuid128_t.
 * @see  ble.h comments sd_ble_uuid_decode().
 * Little endian Bytes 12, 13 are compared for matching.
 * These are the least significant bytes for little endian 128-bit order.
 *
 * 0x1800 through 0x26FF are for Services
 * 0x2700 through 0x27FF are for Units
 * 0x2800 through 0x28FF are for Attribute Types
 * 0x2900 through 0x29FF are for Characteristic Descriptors
 * 0x2A00 through 0x7FFF are for Characteristic Types
 *
 * @cite Heydon, Robin. Bluetooth Low Energy: The Developer's Handbook
 * (p. 191). Pearson Education.
 * @cite Townsend, Kevin; Cuf, Carles; Akiba; Davidson, Robert.
 * Getting Started with Bluetooth Low Energy: Tools and Techniques for
 * Low-Power Networking. Chapter 4, Kindle location 1241.
 */
struct uuid: public boost::uuids::uuid
{
    ~uuid()                         = default;
    uuid();

    /** Constructor for ble::att::uuid from a boost uuid. */
    uuid(boost::uuids::uuid const &other);

    /**
     * Create a ble::att::uuid from a 16-bit or 32-bit unsigned integer.
     *
     * @example When converting a 16-bit uuid to a 128-bit ble::att::uuid
     * The 16-bit uuid 0x2A01 the result ble::att::uuid will be:
     * 00002A01-0010-0080-00805F9834FB
     * @cite Heydon, Robin "Bluetooth Low Energy: The Developer's Handbook"
     *       10.2.3. Attribute Type, p. 190
     *
     * @param uuid_32 The short uuid, either 16-bit or 32-bit will work,
     *                in native CPU format (little-endian most likely).
     */
    uuid(uint32_t uuid_32);

    /**
     * Create a ble::att::uuid from an array of 16 bytes.
     * The expected length of the uuid_bytes is not (caonnot) be checked.
     */
    uuid(uint8_t const* uuid_bytes);

    uuid(uuid const&)               = default;
    uuid(uuid &&)                   = default;
    uuid& operator=(uuid const&)    = default;
    uuid& operator=(uuid&&)         = default;

    /**
     * The Bluetooth LE Base UUID.
     * @note 16-bit uuid shortened values are encoded into
     *       bytes [2:3] in big-endian order.
     * @note 32-bit uuid shortened values are encoded into
     *       bytes [0:3] in big-endian order.
     */
    static constexpr boost::uuids::uuid const base
    {{
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,
        0x00, 0x10,
        0x00, 0x80,
        0x00, 0x80, 0x5F, 0x98, 0x34, 0xFB
    }};

    /**
     * For BLE uuids the base starts 4 bytes into the UUID.
     * @see struct ble::uuid comments.
     */
    static constexpr size_t const base_offset = sizeof(uint32_t);

    /**
     * Determine if the UUID is a BLE assigned value.
     * @retval true  The uuid is a BLE assigned value.
     * @retval false The uuid is a vendor specific value.
     */
    bool is_ble() const;

    /**
     * @return uint16_t The unsigned 16-bit value corresponding to a BLE
     * shortend uuid.
     */
    uint16_t get_u16() const;

    /**
     * @return uint32_t The unsigned 32-bit value corresponding to a BLE
     * shortend uuid.
     */
    uint32_t get_u32() const;

    /**
     * Reverse the byte order of the uuid.
     * @note Using the Nordic softdevice calls requires the use of this
     * function as Nordic parses vendor specific uuids little endian.
     *
     * @return uuid The byte reversed uuid.
     */
    uuid reverse() const;

    /**
     * Convert the UUID to a null-terminated character string.
     * The form is: "00000000-0000-1000-8000-00805F9B34FB".
     *
     * @param begin The buffer into which the conversion will take place.
     * @param end   One past the valid buffer size; i.e. begin + length.
     *
     * @return std::to_chars_result The result of the character conversion.
     */
    std::to_chars_result to_chars(char *begin, char *end) const;

    /**
     * The length required of the conversion buffer passed into to_chars()
     * in order for the conversion to succeed.
     * 16 bytes * 2 chars / byte = 32 chars.
     *  4 hyphens                =  4 chars.
     *  null terminator          1  1 char.
     */
    static constexpr size_t const conversion_length = 37u;
};

} // namespace att
} // namespace ble
