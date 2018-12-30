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

    /**
     * @enum type
     * The bluetooth BD_ADDR type.
     *
     * @see * BLUETOOTH SPECIFICATION Version 5.0 | Vol 6, Part B page 2556
     * 1.3 DEVICE ADDRESS
     */
    enum class type: uint8_t
    {
        /// Bluetooth core specification, 1.3.1 Public Device Address
        public_device                   = 0x00,

        /// Bluetooth core specification, 1.3.2.1 Static Device Address
        random_static                   = 0x01,

        /// Bluetooth core specification,
        /// 1.3.2.2 Private Device Address Generation
        /// 1.3.2.3 Private Device Address Resolution
        random_private_resolvable       = 0x02,

        /// Bluetooth core specification,
        /// 1.3.2.2 Private Device Address Generation
        /// 1.3.2.3 Private Device Address Resolution
        random_private_non_resolvable   = 0x03,
    };

    ~address()                          = default;

    address()                           = delete;
    address(address const&)             = delete;
    address(address &&)                 = delete;
    address& operator=(address const&)  = delete;
    address& operator=(address&&)       = delete;

    address(std::array<uint8_t, octet_length> const& address, type addr_type)
    :   type(addr_type),
        octets{address[0], address[1], address[2],
               address[3], address[4], msbyte(address[5])}
    {
    }

    address(uint8_t const *address, uint8_t addr_type):
        type(static_cast<enum type>(addr_type)),
        octets{address[0], address[1], address[2],
               address[3], address[4], msbyte(address[5])}
    {
    }

    enum type                         const type;
    std::array<uint8_t, octet_length> const octets;

private:
    /**
     * Modify the MSByte of the address octects based on the address type.
     * @param bd_addr_type The bluetooth address type.
     */
    uint8_t msbyte(uint8_t octet)
    {
        switch (this->type)
        {
        case type::public_device:
            // Public device address. Address octects are unmodified.
            return octet;

        case type::random_static:
            // 1.3.2.1 Static Device Address: The upper 2 MSbits must be '11'.
            octet |= 0xC0u;
            return octet;

        case type::random_private_resolvable:
            // 1.3.2.2 Private Device Address: The upper 2 MSbits must be '01'.
            octet &= 0x3Fu;
            octet |= 0x40u;
            return octet;

        case type::random_private_non_resolvable:
            // 1.3.2.2 Private Device Address: The upper 2 MSbits must be '00'.
            octet &= 0x3Fu;
            return octet;

        default:
            return octet;
        }
    }
};

} // namespace gap
} // namespace ble
