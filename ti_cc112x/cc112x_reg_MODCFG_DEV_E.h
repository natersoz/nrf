/**
 * @file cc112x_reg_MODCFG_DEV_E.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * MODCFG_DEV_E - Modulation Format and Frequency Deviation Configuration
 * Bit  Name            Reset   R/W
 * 7:6  MODEM_MODE      0x00    R/W
 *      00 Normal mode
 *      01 DSSS repeat mode.
 *         Requires that SYNC_CFG0.SYNC_MODE = 1 or 010b.
 *         TXmode:  PKT_CFG2.PKT_FORMAT = 0
 *         RXmode:  PKT_CFG2.PKT_FORMAT = 1
 *                  MDMCFG1.FIFO_EN = 0
 *                  MDMCFG0.TRANSPARENT_MODE_EN = 0
 *                  In RX mode, data is only available on GPIO
 *                  by configuring IOCFGx.GPIOx_CFG = 18
 *      10 DSSS PN mode. Both FIFO mode and synchronous serial mode
 *                  are supported (PKT_CFG2.PKT_FORMAT = 0 or 1)
 *      11 Reserved
 * 5:3  MOD_FORMAT      0x00    R/W
 *      Modulation format
 *      000 2-FSK
 *      001 2-GFSK
 *      010 Reserved
 *      011 ASK/OOK
 *      100 4-FSK
 *      101 4-GFSK
 *      110 SC-MSK unshaped (CC1125, TX only). For CC1120, CC1121, and CC1175
 *              this setting is reserved
 *      111 SC-MSK   shaped (CC1125, TX only). For CC1120, CC1121, and CC1175
 *              this setting is reserved
 * 2:0  DEV_E           0x00    R/W
 *      Frequency deviation (exponent part). See DEVIATION_M
 */

uint8_t cc112x_reg_MODCFG_DEV_E_set(uint8_t value);
uint8_t cc112x_reg_MODCFG_DEV_E_get(void);


