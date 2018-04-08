/**
 * @file spim_debug.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
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

