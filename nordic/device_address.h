/**
 * @file nrf_cmsis.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <cstdint>
#include "nrf_cmsis.h"

namespace nordic
{

inline void const* get_device_address()
{
    return const_cast<void const*>(
        reinterpret_cast<void volatile const*>(NRF_FICR->DEVICEADDR)
        );
}


} // namespace nordic

