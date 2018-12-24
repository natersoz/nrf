/**
 * @file nrf_cmsis.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gap_address.h"
#include "nrf_cmsis.h"

namespace nordic
{

/**
 * DEVICEADDR[0:1] contains FIPS compliant randomly generated 64 bits of data.
 * Since this data is randomly generated it does not matter which bytes are
 * MSB/LSB; it is just random data. The should containe a '1' indicating the
 * address data is randomly generated.
 *
 * @return ble::gap::address The BD_ADDR address type from the Nordic Factory
 *                           programmed data.
 */
inline ble::gap::address get_device_address()
{
    uint32_t const* device_addr32 =
        const_cast<uint32_t const*>(NRF_FICR->DEVICEADDR);

    return ble::gap::address(reinterpret_cast<uint8_t const*>(device_addr32),
                             NRF_FICR->DEVICEADDRTYPE);
}

} // namespace nordic

