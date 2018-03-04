/**
 * @file cc112x_reg_CFM_DATA_CFG.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * CFM_DATA_CFG - Custom Frequency Modulation Configuration
 * Bit  Name                        Reset   R/W
 * 7    CFM_DA T A_CFG_NOT_USED     0x00    R
 *      Unused
 *
 * 6:5  SYMBOL_MAP_CFG              0x00    R/W
 *      Symbol map configuration.
 *      Configures the modulated symbol mapping definition from data bit to modulated symbols.
 *
 *      For 2'ary modulation schemes the symbol mapping definition is as follows:
 *      Data bit    00              01              10              11
 *             0    -Dev[A_min]     +Dev[A_max]     +Dev[A_max]     +Dev[A_max]
 *             1    +Dev[A_max]     -Dev[A_min]     -Dev[A_min]     -Dev[A_min]
 *
 *      OOK/ASK: AMAX = Maximum amplitude, AMIN = Minimum amplitude
 *
 *      For 4'ary modulation schemes the symbol mapping definition is as follows:
 *      Data bit    00              01              10              11
 *            00    -Dev/3          -Dev            +Dev/3          +Dev
 *            01    -Dev            -Dev/3          +Dev            +Dev/3
 *            10    +Dev/3          +Dev            -Dev/3          -Dev
 *            11    +Dev            +Dev/3          -Dev            -Dev/3
 *
 * 4:1  CFM_DA T A_CFG_RESERVED4_1  0x00    R/W
 *      For test purposes only, use values from SmartRF Studio
 *
 * 0    CFM_DA T A_EN               0x00    R/W
 *      Custom frequency modulation enable
 *      0 CFM mode disabled
 *      1 CFM mode enabled (write frequency word directly)
 */
uint8_t cc112x_reg_CFM_DATA_CFG_set(uint8_t value);
uint8_t cc112x_reg_CFM_DATA_CFG_get(void);

