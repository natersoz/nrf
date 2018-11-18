/**
 * @file ble/gap_scanning.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A set of classes for configuring and performing BLE scanning.
 */

#pragma once

#include "ble/att.h"

#include <algorithm>
#include <cstddef>
#include <cstring>

namespace ble
{
namespace gap
{

/**
 * @interface scanning
 * A generic BLE scanning interface.
 * Does not support extended data scanning nor scanning on extended channels.
 * @todo inherit this interface to support extended scanning.
 *
 * Implement this interface:
 * + With vendor specific APIs and hardware
 * + To specify the profile suitable to the connection role:
 *   directed, undirected, scannable, connectable, etc.
 */
class scanning
{
public:
    static constexpr att::length_t const repsonse_max_length = 31u;

    /// The minimum scan interval in 625 usec units: 2.5 msec.
    constexpr static const uint16_t interval_minimum = 0x0004u;
    constexpr static const uint16_t interval_maximum = 0xFFFFu;

    /// Timeout values are in 10 msec ticks.
    constexpr static const uint16_t timeout_minimum   = 0x0001u;
    constexpr static const uint16_t timeout_unlimited = 0x0000u;

    /**
     * Convert from milliseconds to BLE scanning interval units of 0.625 msec.
     *
     * @param interval_msec A time interval expressed in milliseconds.
     * @return uint16_t     The number of 0.625 msec ticks used by the BLE
     *                      scanning interval tick count.
     */
    static constexpr uint16_t interval_msec(uint32_t interval_msec);

    virtual ~scanning()                   = default;

    scanning()                            = default;
    scanning(scanning const&)             = delete;
    scanning(scanning &&)                 = delete;
    scanning& operator=(scanning const&)  = delete;
    scanning& operator=(scanning&&)       = delete;

    /**
     * Start scanning.
     * The scanning data contained in the public member scanning_data
     * will be used when scanning starts.
     */
    virtual void start() = 0;

    /** Stop scanning. */
    virtual void stop() = 0;

    virtual uint16_t interval_get() const = 0;
    virtual void     interval_set(uint16_t scan_interval) = 0;

    virtual uint16_t window_get() const = 0;
    virtual void     window_set(uint16_t scan_window) = 0;
};

inline constexpr uint16_t scanning::interval_msec(uint32_t interval_msec)
{
    interval_msec *= 1000u;
    interval_msec /=  625u;

    interval_msec = std::min(static_cast<uint32_t>(interval_maximum),
                             interval_msec);

    return std::max(interval_minimum, static_cast<uint16_t>(interval_msec));
}

} // namespace gap
} // namespace ble

