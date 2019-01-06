/**
 * @file nordic_error.h
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

/**
 * Return an error string corresponding to the Nordic error code.
 *
 * @param nordic_error The nordic error code.
 *
 * @return char const* The null terminated string literal corresponding to
 *                     the parameter nordic_error.
 */
char const* nordic_error_string(uint32_t nordic_error);

