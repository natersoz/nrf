/**
 * @file test_rtc_stubs.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "rtc.h"

// Fill the logger::log_time() requirement.
uint32_t rtc::ticks_per_second() const { return 0u; }
uint64_t rtc::get_count_extend_64() const { return 0u; }
