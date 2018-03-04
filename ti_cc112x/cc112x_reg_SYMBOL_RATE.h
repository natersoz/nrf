/**
 * @file cc112x_reg_SYMBOLE_RATE.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * SYMBOL_RATE2 - Symbol Rate Configuration Exponent and Mantissa [19:16]
 * Bit  Name                    Reset   R/W
 * 7:4  SRATE_E                 0x04    R/W
 *      Symbol Rate, Exponent Part.
 *
 *      SRATE_E > 0:
 *      Rsymbol = fxosc * (2^20 + SRATE_M) * 2 ^ SRATE_E / 2 ^ 39 [ksps]
 *
 *      SRATE = 0
 *      Rsymbol = fxosc * SRATE_M / 2 ^ 38 [ksps]
 *
 *      Modulation format / data encoding
 *      2-(G)FSK 4-(G)FSK Manchester mode DSSS mode
 *      Data rate/symbol rate ratio
 *      1.0
 *      2.0
 *      0.5
 *      DSS mode: 1 / spreading factor
 *
 * 3:0  SRATE_M_19_16
 *      Symbol Rate Mantissa[19:16]
 *      S_RATE_M[19:16]
 */

uint8_t cc112x_reg_SRATE_xxx_set(uint8_t value);
uint8_t cc112x_reg_SRATE_xxx_get(void);

