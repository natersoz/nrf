/**
 * @file ble/gap_advertising.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A set of classes for configuring and performing BLE advertising.
 */

#pragma once

#include "ble/gap_advertising_data.h"
#include <cstddef>
#include <cstring>

namespace ble
{
namespace gap
{

/**
 * @interface advertising
 * A generic BLE advertising interface.
 *
 * Implement this interface:
 * + With vendor specific APIs and hardware
 * + To specify the profile suitable to the connection role:
 *   directed, undirected, scannable, connectable, etc.
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
    inline static constexpr uint16_t interval_msec(uint32_t interval_msec)
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
     * @param advertising_interval The advertising interval in 0.625 msec ticks.
     */
    advertising(uint16_t advertising_interval = advertising::interval_unspecified):
        advertising_data(),
        interval(advertising_interval)
    {
    }

    /**
     * Start advertising.
     * The advertising data contained in the public member advertising_data
     * will be used when advertising starts.
     */
    virtual void start() = 0;

    /** Stop advertising. */
    virtual void stop() = 0;

    /**
     * Use the tlv_encode functions to set the advertising data into
     * this data struct.
     */
    advertising_data_t advertising_data;

    /// The advertising interval in 0.625 msec units.
    uint16_t interval;
};

} // namespace gap
} // namespace ble

