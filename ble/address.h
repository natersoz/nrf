/**
 * @file address.h
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
 * Bluetooth LE Address
 *
 * BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H, page 2355
 * Figure 3.15: Identity Address Information Packet
 */
struct address
{
    static constexpr size_t const length = 6u;

    enum class type: uint8_t
    {
        public_device                   = 0x00,
        random_static                   = 0x01,
        random_private_resolvable,
        random_private_non_resolvable
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

    address(std::array<uint8_t, length> address, enum type address_type):
            octets(address), type(address_type)
    {
    }

    address(uint8_t const *address, uint8_t address_type):
        octets{address[0], address[1], address[2],
               address[3], address[4], address[5]},
        type(static_cast<enum type>(address_type))
    {
    }

    std::array<uint8_t, length>     octets;
    enum type                       type;

};

} // namespace gap
} // namespace ble
