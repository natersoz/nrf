/**
 * @file utilities/std_error.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <system_error>

inline bool is_success(std::errc error_code) {
    return static_cast<int>(error_code) == 0;
}

inline bool is_failure(std::errc error_code) {
    return not is_success(error_code);
}

static constexpr std::errc errc_success = static_cast<std::errc>(0u);
