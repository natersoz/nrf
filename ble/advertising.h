/**
 * @file ble/advertising.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A set of classes for configuring and performing BLE advertising.
 */

#pragma once

#include "ble/advertising_data.h"
#include <cstddef>
#include <cstring>
#include <vector>

namespace ble
{

/**
 * @class advertising
 * A generic BLE advertising class, undirected and connectable.
 */
class advertising
{
public:
    /// Used to specify that the advertising interval can be anything.
    static uint16_t const interval_unspecified = 0xFFFFu;

    /**
     * Convert from milliseconds to BLE advertising interval units of 0.625 msec.
     *
     * @param interval_msec A time interval expressed in milliseconds.
     *
     * @return uint16_t The number of 0.625 msec ticks used by the BLE advertising
     * interval tick count.
     */
    inline static uint16_t interval_msec(uint32_t interval_msec)
    {
        interval_msec *= 1000u;
        interval_msec /=  625u;
        return static_cast<uint16_t>(interval_msec);
    }

    virtual ~advertising()                      = default;
    advertising()                               = delete;

    advertising(advertising const&)             = delete;
    advertising(advertising &&)                 = delete;
    advertising& operator=(advertising const&)  = delete;
    advertising& operator=(advertising&&)       = delete;

    /**
     * Create the advertising class with a specific intreval based on
     * 0.625 msec tick counts.
     *
     * @param interval The advertising interval in 0.625 msec ticks.
     */
    advertising(uint16_t interval = advertising::interval_unspecified);

    void start();
    void stop();

    /**
     * Use the tlv_encode functions to set the advertising data into
     * this data struct.
     */
    advertising_data_t advertising_data;

    /// The advertising interval in 0.625 msec units.
    uint16_t interval;
};

} // namespace ble

