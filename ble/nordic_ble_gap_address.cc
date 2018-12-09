/**
 * @file ble/nordic_ble_gap_address.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_gap_address.h"
#include <cstring>

namespace nordic
{

constexpr uint8_t ble_gap_address::address_type(enum ble::gap::address::type addr_type)
{
    switch (addr_type)
    {
    case ble::gap::address::type::public_device:
        return BLE_GAP_ADDR_TYPE_PUBLIC;

    case ble::gap::address::type::random_static:
        return BLE_GAP_ADDR_TYPE_RANDOM_STATIC;

    case ble::gap::address::type::random_private_resolvable:
        return BLE_GAP_ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE;

    case ble::gap::address::type::random_private_non_resolvable:
        return BLE_GAP_ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE;

    case ble::gap::address::type::anonymous:
        return BLE_GAP_ADDR_TYPE_ANONYMOUS;

    default:
        return BLE_GAP_ADDR_TYPE_ANONYMOUS;
    }
}

ble_gap_address::ble_gap_address(ble::gap::address const& address)
{
    this->addr_id_peer  = 1u;
    this->addr_type     = address_type(address.type);
    memcpy(&this->addr, address.octets.data(), ble::gap::address::length);
}

} // namespace nordic


