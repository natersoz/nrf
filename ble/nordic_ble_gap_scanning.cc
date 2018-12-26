/**
 * @file ble/nordic_ble_gap_scanning.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A set of classes for configuring and performing BLE scanning.
 */

#include "ble/nordic_ble_gap_scanning.h"
#include "ble/nordic_ble_gap_address.h"
#include "nordic_error.h"
#include "logger.h"
#include "project_assert.h"
#include <cstring>

namespace nordic
{

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

std::errc ble_gap_scanning::start()
{
    uint32_t const error_code =
        sd_ble_gap_scan_start(&this->scan_parameters_,
                              &this->nordic_response_data_);

    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("sd_ble_gap_scan_start() failed: 0x%04x '%s'",
                                 error_code, nordic_error_string(error_code));
        ASSERT(0);
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gap_scanning::stop()
{
    uint32_t const error_code = sd_ble_gap_scan_stop();
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("sd_ble_gap_scan_stop() failed: 0x%04x '%s'",
                                 error_code, nordic_error_string(error_code));
        ASSERT(0);
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gap_scanning::connect(
    ble::gap::address               const&  peer_address,
    ble::gap::connection_parameters const&  connection_parameters)
{
    nordic::ble_gap_address gap_addr(peer_address);

    ble_gap_conn_params_t const gap_conn_params = {
        .min_conn_interval  = connection_parameters.interval_min,
        .max_conn_interval  = connection_parameters.interval_max,
        .slave_latency      = connection_parameters.slave_latency,
        .conn_sup_timeout   = connection_parameters.supervision_timeout
    };

    logger::instance().debug("ble_gap_scanning::connect:");

    /// @todo nordic_config_tag should be obtained from the Nordic
    /// BLE stack implementation.
    constexpr uint8_t const nordic_config_tag = 1u;
    uint32_t const error_code = sd_ble_gap_connect(&gap_addr,
                                                   &this->scan_parameters_,
                                                   &gap_conn_params,
                                                   nordic_config_tag);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("sd_ble_gap_connect() failed: 0x%04x '%s'",
                                 error_code, nordic_error_string(error_code));
        ASSERT(0);
    }

    return nordic_to_system_error(error_code);
}

} // namespace nordic

