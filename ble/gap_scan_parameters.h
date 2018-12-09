/**
 * @file ble/gap_scan_parameters.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Typically this header is included with ble/gap_scanning.h
 * It is separated out for readability.
 */

#pragma once

#include <cstdint>
#include <algorithm>

#include "project_assert.h"

namespace ble
{
namespace gap
{

struct scan_parameters
{
public:
    /// The minimum scan interval in 625 usec units: 2.5 msec.
    constexpr static const uint16_t interval_minimum = 0x0004u;
    constexpr static const uint16_t interval_maximum = 0xFFFFu;

    /**
     * Convert from milliseconds to BLE scanning interval units of 0.625 msec.
     *
     * @param interval_msec A time interval expressed in milliseconds.
     * @return uint16_t     The number of 0.625 msec ticks used by the BLE
     *                      scanning interval tick count.
     */
    static constexpr uint16_t interval_msec(uint32_t interval_msec);

    ~scan_parameters()                                  = default;

    scan_parameters(scan_parameters const&)             = default;
    scan_parameters(scan_parameters &&)                 = default;
    scan_parameters& operator=(scan_parameters const&)  = default;
    scan_parameters& operator=(scan_parameters&&)       = default;

    /**
     * Default values for scan interval and window are fast scanning.
     * See BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part C
     * Table A.1: Defined GAP timers (Sheet 5 of 6)
     *
     * T GAP: scan_fast_interval: 30-60 msec, scan_fast_window: 30 msec
     */
    scan_parameters(): interval(interval_msec(40)), window(interval_msec(30))
    {
    }

    /**
     * @param scan_interval The scan interval in 0.625 usec increments.
     * @param scan_window   The scan window   in 0.625 usec increments.
     */
    scan_parameters(uint16_t scan_interval, uint16_t scan_window):
        interval(scan_interval), window(scan_window)
    {
    }

    uint16_t const interval;    ///< 0.625 usec units
    uint16_t const window;
};

inline constexpr uint16_t scan_parameters::interval_msec(uint32_t interval_msec)
{
    interval_msec *= 1000u;
    interval_msec /=  625u;

    interval_msec = std::min(static_cast<uint32_t>(interval_maximum), interval_msec);
    interval_msec = std::max(static_cast<uint32_t>(interval_minimum), interval_msec);
    return static_cast<uint16_t>(interval_msec);
}

} // namespace gap
} // namespace ble

