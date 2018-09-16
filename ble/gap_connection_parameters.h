/**
 * @file ble/gap_connection_parameters.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Typically this header is included with ble/gap_types.h
 * It is separated out for readability.
 */

#pragma once

namespace ble
{
namespace gap
{

/**
 * Convert from milliseconds to BLE conection interval units of 1.250 msec.
 *
 * @param interval_msec A time interval expressed in milliseconds.
 *
 * @return uint16_t The number of 1.25 msec ticks used by the BLE connection
 * interval tick count.
 */
inline constexpr uint16_t connection_interval_msec(uint32_t interval_msec)
{
    interval_msec *= 1000u;
    interval_msec /= 1250u;
    return static_cast<uint16_t>(interval_msec);
}

/**
 * Convert from milliseconds to BLE supervisory timeout tick count.
 *
 * @param interval_msec The supervisory timeout expressed in milliseconds.
 *
 * @return uint16_t The number of 10 msec ticks used by the BLE
 * supervisory timeout tick count.
 */
inline constexpr uint16_t supervision_timeout_msec(uint32_t interval_msec)
{
    interval_msec /= 10u;
    return static_cast<uint16_t>(interval_msec);
}

/**
 * @struct ble::gap::connection_parameters
 * @see Bluetooth Core Specification 5.0, Volume 3, Part C
 * 12.3 peripheral preferred connection parameters characteristic
 * Table 12.6: Format of the preferred connection parameters structured data
 *
 * This structure is used in advertising and as the attribute value within
 * the peripheral_preferred_connection_parameters characteristic (ppcp) 0x2a04.
 */
struct connection_parameters
{
    static constexpr uint16_t const unspecified_interval = 0xffffu;

    ~connection_parameters()                                        = default;

    connection_parameters(connection_parameters const&)             = default;
    connection_parameters(connection_parameters &&)                 = default;
    connection_parameters& operator=(connection_parameters const&)  = default;
    connection_parameters& operator=(connection_parameters&&)       = default;

    connection_parameters():
        interval_min(unspecified_interval),
        interval_max(unspecified_interval),
        slave_latency(0u),
        supervision_timeout(unspecified_interval)
    {
    }

    connection_parameters(uint16_t conn_interval_min,
                          uint16_t conn_interval_max,
                          uint16_t latency,
                          uint16_t conn_supervision_timeout):
        interval_min(conn_interval_min),
        interval_max(conn_interval_max),
        slave_latency(latency),
        supervision_timeout(conn_supervision_timeout)
    {
    }

    /** @{ Connection interval, minimum and maximum.
     * Units: 1.25 msec. Range: [0x0006:0x0C80]. 0xFFFF indicates unspecified.
     * Values outside the range (except 0xFFFF) are reserved for future use.
     */
    uint16_t interval_min;
    uint16_t interval_max;
    /** @} */

    /**
     * Core specification Volume 6, Part B, 4.5.1 Connection Events.
     * The number of connection events the peripheral is allowed to not respond
     * before timing out. Range: [0x0000:0x01F3],
     * Slave latency max =
     *      connection_supervision_timeout / (connection_interval * 2) - 1
     */
    uint16_t slave_latency;

    /**
     * Defines the connection supervisor timeout. Units: 10ms.
     * Range: 0xFFFF indicates no specific value requested.
     * Range: [0x000A:0x0C80]; 100 ms to 32 seconds.
     */
    uint16_t supervision_timeout;
};

// This is a sanity check for the struct connection_parameters.
// There are 4 uint16_t data elements in the class so this should check ok.
static_assert(sizeof(connection_parameters) == sizeof(uint16_t) * 4u);

} // namespace gap
} // namespace ble

