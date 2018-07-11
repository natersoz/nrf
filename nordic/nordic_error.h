/**
 * @file ble/nordic_ble_stack.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <system_error>

/**
 * Translate Nordic errors into std::error_code.
 *
 * @param nordic_error The Nordic error codes found in the softdevice
 * header file nrf_error.h.
 *
 * @return std::error_code A std::generic_category() error_code.
 */
std::errc nordic_to_system_error(uint32_t nordic_error);

