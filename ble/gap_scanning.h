/**
 * @file ble/gap_scanning.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A set of classes for configuring and performing BLE scanning.
 */

#pragma once

#include "ble/att.h"
#include "ble/gap_address.h"
#include "ble/gap_scan_parameters.h"
#include "ble/gap_connection_parameters.h"

#include <system_error>

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

    virtual ~scanning()                   = default;

    scanning()                            = default;
    scanning(scanning const&)             = delete;
    scanning(scanning &&)                 = delete;
    scanning& operator=(scanning const&)  = delete;
    scanning& operator=(scanning&&)       = delete;

    scanning(ble::gap::scan_parameters const &scan_params):
        scan_parameters(scan_params)
    {
    }

    /**
     * Start scanning.
     * The scan_parameters member variable will be used to determine the scan
     * rate and window.
     */
    virtual std::errc start() = 0;

    /** Stop scanning. */
    virtual std::errc stop() = 0;

    /**
     * Central connections are establshed within the ble::gap::scanning module
     * since the process of connecting involves scanning for a peripheral which
     * matches the central criteria.
     *
     * @todo At present the central needs to know the peripheral's BLE address.
     *
     * @param peer_address The peripheral's BLE address.
     * @param connection_parameters Once the connection is established the
     *        central's preferred connection parameters.
     *
     * @return std::errc The resulting error code; zero for success.
     */
    virtual std::errc connect(
        ble::gap::address               const&  peer_address,
        ble::gap::connection_parameters const&  connection_parameters) = 0;

    ble::gap::scan_parameters scan_parameters;
};

} // namespace gap
} // namespace ble

