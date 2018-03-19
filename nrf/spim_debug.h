/**
 * @file spim_debug.h
 */

#pragma once

#include "nrf_cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

void log_spim_registers(NRF_SPIM_Type const *spim_registers);

#ifdef __cplusplus
}
#endif

