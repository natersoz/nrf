/**
 * @file ble/nordic_ble_gap_scanning.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A set of classes for configuring and performing BLE scanning.
 */

#include "ble/nordic_ble_gap_scanning.h"
#include "logger.h"
#include "project_assert.h"
#include <cstring>

namespace nordic
{

scan_parameters::scan_parameters()
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

    /**
     * Default values for scan interval and window are fast scanning.
     * See BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part C
     * Table A.1: Defined GAP timers (Sheet 5 of 6)
     *
     * T GAP: scan_fast_interval: 30-60 msec, scan_fast_window: 30 msec
     */
    this->interval  = ble::gap::scanning::interval_msec(40u);
    this->window    = ble::gap::scanning::interval_msec(30u);
    this->timeout   = ble::gap::scanning::timeout_unlimited;
}

void ble_gap_scanning::init_response_data()
{
    memset(this->response_data_, 0, sizeof(this->response_data_));
    this->nordic_response_data_.p_data = this->response_data_;
    this->nordic_response_data_.len    = sizeof(this->response_data_);
}

ble_gap_scanning::ble_gap_scanning():
    ble::gap::scanning(),
    scan_parameters_()
{
    ble_gap_scanning::init_response_data();
}

ble_gap_scanning::ble_gap_scanning(uint16_t scanning_interval,
                                   uint16_t scanning_window):
    ble::gap::scanning(),
    scan_parameters_()
{
    ble_gap_scanning::init_response_data();
}

void ble_gap_scanning::start()
{
    uint32_t const error_code =
        sd_ble_gap_scan_start(&this->scan_parameters_,
                              &this->nordic_response_data_);

    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("sd_ble_gap_scan_start() failed: 0x%04x",
                                 error_code);
        ASSERT(0);
    }
}

void ble_gap_scanning::stop()
{
    uint32_t const error_code = sd_ble_gap_scan_stop();
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("sd_ble_gap_scan_stop() failed: 0x%04x",
                                 error_code);
        ASSERT(0);
    }
}

uint16_t ble_gap_scanning::interval_get() const
{
    return this->scan_parameters_.interval;
}

void ble_gap_scanning::interval_set(uint16_t scan_interval)
{
    this->scan_parameters_.interval = scan_interval;
}

uint16_t ble_gap_scanning::window_get() const
{
    return this->scan_parameters_.window;
}

void ble_gap_scanning::window_set(uint16_t scan_window)
{
    this->scan_parameters_.window = scan_window;
}

} // namespace nordic

