/**
 * @file ble/nordic_ble_gap_scan_parameters.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_gap_scan_parameters.h"
#include <cstring>

namespace nordic
{

void ble_gap_scan_parameters::init(ble::gap::scan_parameters const& scan_params)
{
    // Setting all fiels to zero provides a good starting point.
    // Specifically the channel mask is cleared, enabling scanning on
    // all appropriate channels.
    memset(this, 0, sizeof(*this));

    this->extended               = false;
    this->active                 = false;
    this->report_incomplete_evts = false;
    this->filter_policy          = BLE_GAP_SCAN_FP_ACCEPT_ALL;
    this->scan_phys              = BLE_GAP_PHY_AUTO;

    this->interval  = scan_params.interval;
    this->window    = scan_params.window;
    this->timeout   = timeout_unlimited;
}

} // namespace nordic

