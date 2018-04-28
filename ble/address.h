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

class address
{
public:
    static constexpr size_t const length = 6u;

    enum class type
    {
        public_device,
        random_static,
        random_private_resolvable,
        random_private_non_resolvable
    };

    std::array<uint8_t, length> address_;

    ~address()                          = default;

    address(address const&)             = delete;
    address(address &&)                 = delete;
    address& operator=(address const&)  = delete;
    address& operator=(address&&)       = delete;

    address():
        address_({0u})
    {
    }
};

} // namespace ble
