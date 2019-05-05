/**
 * @file nordic_ble_gap_logger.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Debug logging functions for nordic::ble_gap.
 */

#pragma once

#include <ble_gap.h>
#include "logger.h"

namespace nordic
{

char const* ble_gap_addr_type_string(uint8_t addr_type);

void log_address(logger::level log_level, ble_gap_addr_t const &addr);

void log_connection_parameters(logger::level log_level,
                               ble_gap_conn_params_t const& conn_params);

void log_scan_parameters(logger::level log_level,
                         ble_gap_scan_params_t const &scan_params);

} // namespace nordic

