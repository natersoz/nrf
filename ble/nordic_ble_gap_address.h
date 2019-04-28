/**
 * @file ble/nordic_ble_gap_address.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/gap_address.h"
#include <ble_gap.h>

namespace nordic
{

/**
 * @struct ble_gap_address
 * A class which allows for easy conversion from ble::gap::address to
 * Nordic typedef struct ble_gap_addr_t BLE address representation.
 */
struct ble_gap_address: public ble_gap_addr_t
{
    /// Convert a generic ble::gap::address::type to a Nordic address type.
    static constexpr uint8_t address_type(enum ble::gap::address::type addr_type);

    ~ble_gap_address()                                  = default;

    ble_gap_address(ble_gap_address const&)             = default;
    ble_gap_address(ble_gap_address &&)                 = default;
    ble_gap_address& operator=(ble_gap_address const&)  = default;
    ble_gap_address& operator=(ble_gap_address&&)       = default;

    /** Create Nordic address from a generic ble::gap::address. */
    ble_gap_address(ble::gap::address const& address);
};

} // namespace nordic


