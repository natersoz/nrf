/**
 * @file cc112x_reg_EXT_CTRL.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * EXT_CTRL - External Control Configuration
 * Bit  Name                        Reset   R/W
 * 7:3  CFM_DA  EXT_CTRL_NOT_USED   0x00    R
 *      Unused
 *
 * 2    PIN_CTRL_EN                 0x00    R/W
 *      Pin control enable. Pin control reuses the SPI interface pins to execute SRX, STX, SPWD, and IDLE strobes
 *      0 Pin control disabled
 *      1 Pin control enabled
 *
 * 1    EXT_32_40K_CLOCK_EN         0x00    R/W
 *      External 32/40k clock enable
 *      0 External 32/40k clock disabled
 *      1 External 32/40k clock enabled.
 *      IOCFG3.GPIO3_CFG must be set to HIGHZ (EXT_32_40K_CLOCK)
 *
 * 0    BURST_ADDR_INCR_EN          0x00    R/W
 *      Burst address increment enable
 *      0 Burst address increment disabled
 *      (i.e. consecutive writes to the same address location in burst mode)
 *      1 Burst address increment  enabled
 *      (i.e. the address is incremented during burst access)
 */
uint8_t cc112x_reg_EXT_CTRL_set(uint8_t value);
uint8_t cc112x_reg_EXT_CTRL_get(void);

