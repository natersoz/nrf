/**
 * @file rtc_debug.h
 */

#pragma once

#include "nrf_cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

void log_rtc_registers(NRF_RTC_Type const *rtc_registers);

#ifdef __cplusplus
}
#endif

