/**
 * @file ble/gap_address.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <cstdint>
#include <cstddef>
#include <array>

namespace ble
{
namespace gap
{

/**
 * @class ble::gap::address
 * Encapsulate the Bluetooth Address BD_ADDR structure.
 *
 * BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H, page 2355
 * Figure 3.15: Identity Address Information Packet
 *
 * 1.2 BLUETOOTH DEVICE ADDRESSING
 * Each Bluetooth device shall be allocated a unique 48-bit Bluetooth device
 * address (BD_ADDR).
 */
struct address
{
    /// The number of octets in the BD_ADDR structure.
    static constexpr size_t const octet_length = 6u;

    /// The total length of the BD_ADDR structure which contains the 48 bits
    /// of address octets and the 8 bits of type information.
    static constexpr size_t const length = octet_length + 1u;

    enum class type: uint8_t
    {
        public_device                   = 0x00,
        random_static                   = 0x01,
        random_private_resolvable,
        random_private_non_resolvable,
        anonymous
    };

    ~address()                          = default;

    address(address const&)             = delete;
    address(address &&)                 = delete;
    address& operator=(address const&)  = delete;
    address& operator=(address&&)       = delete;

    address():
        octets({0u}),
        type(type::public_device)
    {
    }

    address(std::array<uint8_t, octet_length> const& address,
            enum type address_type)
    : octets(address), type(address_type)
    {
    }

    address(uint8_t const *address, uint8_t address_type):
        octets{address[0], address[1], address[2],
               address[3], address[4], address[5]},
        type(static_cast<enum type>(address_type))
    {
    }

    std::array<uint8_t, octet_length>   octets;
    enum type                           type;

};

} // namespace gap
} // namespace ble
