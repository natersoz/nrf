/**
 * @file ble/nordic_ble_gap_scanning.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A set of classes for configuring and performing BLE scanning.
 */

#pragma once

#include "ble/gap_scanning.h"
#include "ble/nordic_ble_gap_scan_parameters.h"

#include <ble_gap.h>

namespace nordic
{

/**
 * @class ble_gap_scanning
 * Implement ble::gap::scanning for the Nordic softdevice using the simplest
 * options:
 *  + No extended scan capabilities enabled,
 *  + Scan with response is not enabled.
 */
class ble_gap_scanning: public ble::gap::scanning
{
public:
    virtual ~ble_gap_scanning() override                    = default;

    ble_gap_scanning(ble_gap_scanning const&)               = delete;
    ble_gap_scanning(ble_gap_scanning &&)                   = delete;
    ble_gap_scanning& operator=(ble_gap_scanning const&)    = delete;
    ble_gap_scanning& operator=(ble_gap_scanning&&)         = delete;

    ble_gap_scanning();

    /** ctor which allows for the setting of the scan interval and window. */
    ble_gap_scanning(uint16_t scanning_interval, uint16_t scanning_window);

    /**
     * Start ble_gap_scanning.
     * The ble_gap_scanning data contained in the public member scanning_data
     * will be used when ble_gap_scanning starts.
     */
    virtual std::errc start() override;

    /** Stop ble_gap_scanning. */
    virtual std::errc stop() override;

    /**
     * Connect to a peripheral from a central.
     *
     * @param peer_address
     * @param connection_parameters
     *
     * @return std::errc
     */
    virtual std::errc connect(
        ble::gap::address               const&  peer_address,
        ble::gap::connection_parameters const&  connection_parameters) override;

private:
    nordic::ble_gap_scan_parameters scan_parameters_;
    uint8_t                         response_data_[repsonse_max_length];
    ble_data_t                      nordic_response_data_;

    void init_response_data();
};

} // namespace nordic

