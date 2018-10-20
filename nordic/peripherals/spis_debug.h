/**
 * @file spis_debug.h
 */

#pragma once

#include "nrf_cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

void log_spis_registers(NRF_SPIS_Type const *spis_registers);

#ifdef __cplusplus
}
#endif

