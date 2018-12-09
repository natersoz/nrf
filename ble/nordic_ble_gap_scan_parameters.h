/**
 * @file ble/nordic_ble_gap_scan_parameters.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gap_scan_parameters.h"
#include "ble_gap.h"

namespace nordic
{

class ble_gap_scan_parameters: public ble_gap_scan_params_t
{
public:
    /// Timeout values are in 10 msec ticks.
    /// @note Timeouts are a Noridc invention, not BLE.
    constexpr static const uint16_t timeout_minimum   = 0x0001u;
    constexpr static const uint16_t timeout_unlimited = 0x0000u;

    ~ble_gap_scan_parameters()                                          = default;

    ble_gap_scan_parameters(ble_gap_scan_parameters const&)             = default;
    ble_gap_scan_parameters(ble_gap_scan_parameters &&)                 = default;
    ble_gap_scan_parameters& operator=(ble_gap_scan_parameters const&)  = default;
    ble_gap_scan_parameters& operator=(ble_gap_scan_parameters&&)       = default;

    /** Use the default values for ble::gap::scan_parameters (fast scanning). */
    ble_gap_scan_parameters() {
        ble::gap::scan_parameters const scan_params;
        this->init(scan_params);
    }

    /** User specified values for ble::gap::ble_gap_scan_parameters. */
    ble_gap_scan_parameters(ble::gap::scan_parameters const& scan_params) {
        this->init(scan_params);
    }

private:
    void init(ble::gap::scan_parameters const& scan_params);
};

} // namespace nordic

