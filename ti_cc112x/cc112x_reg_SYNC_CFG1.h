/**
 * @file cc112x_reg_SYNC_CFG.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * SYNC_CFG1 - Sync Word Detection Configuration Reg. 1
 * Bit  Name            Reset   R/W
 * 7    Reserved        0x00    R/W
 *      Test purposes only, use values from SmartRF Studio
 *
 * 6    PQT_GATING_EN   0x00    R/W
 *      PQT gating enable.
 *      When PQT gating is enabled the demodulator will not start
 *      to look for a sync word before a preamble is detected
 *      (i.e. PQT_REACHED is asserted).
 *      The preamble detector must be enabled for this feature to work
 *      (PREAMBLE_CFG0.PQT_EN = 1)
 *
 * 5    Reserved
 *      Test purposes only, use values from SmartRF Studio
 *
 * 4:0  SYNC_THR        0x0A    R/W
 *      Soft decision sync word threshold.
 *      A sync word is accepted when the calculated sync word qualifier
 *      value (PQT_SYNC_ERR.SYNC_ERROR) is less than SYNC_THR/2).
 *      A low threshold value means a strict sync word qualifier
 *      (sync word must be of high quality to be accepted)
 *      while a high threshold value will accept sync word of a poorer
 *      quality (increased probability of detecting ?false? sync words)
 */


/**
 * SYNC_CFG0 - Sync Word Detection Configuration Reg. 0
 * Bit  Name                    Reset   R/W
 * 7:5  SYNC_CFG0_NOT_USED      0x00    R/W
 *
 * 4:2  SYNC_MODE               0x05    R/W
 *      Sync word configuration.
 *      When SYNC_MODE = 0, all samples (noise or data) received after
 *      RX mode is entered will either be put in the RX FIFO or output
 *      on a GPIO configured as SERIAL_RX. Note that when 4'ary modulation
 *      is used the sync word uses 2'ary modulation
 *      (the symbol rate is kept the same)
 *          000 No sync word
 *          001  11 bits [SYNC15_8[2:0]:SYNC7_0]
 *          010  16 bits [SYNC15_8: SYNC7_0]
 *          011  18 bits [SYNC23_16[1:0]: SYNC15_8: SYNC7_0]
 *          100  24 bits [SYNC23_16: SYNC15_8: SYNC7_0]
 *          101  32 bits [SYNC31_24: SYNC23_16: SYNC15_8: SYNC7_0]
 *          110 16H bits [SYNC31_24: SYNC23_16]
 *          111 16D bits (DualSync search). When this setting is used in TX mode
 *                       [SYNC15_8:SYNC7_0] is transmitted
 *
 * 1:0  SYNC_NUM_ERROR          0x03    R/W
 *      Bit check on sync word.
 *      When SYNC_NUM_ERROR != 11b the sync word will only be accepted when
 *      the programmable conditions below are true. The bit error qualifier
 *      can be useful if the sync word in use has weak correlation properties
 *          00 0 bit error in last received sync byte (normally SYNC0)
 *          01 < 2 bit error in last received sync byte (normally SYNC0)
 *          10 Reserved
 *          11 Bit error qualifier disabled. No check on bit errors
 *
 *
 */

uint8_t cc112x_reg_IOCFGx_ATRAN_set(uint8_t value);
uint8_t cc112x_reg_IOCFGx_ATRAN_get(void);

uint8_t cc112x_reg_IOCFGx_INV_set(uint8_t value);
uint8_t cc112x_reg_IOCFGx_INV_get(void);

uint8_t cc112x_reg_IOCFGx_CFG_set(uint8_t value);
uint8_t cc112x_reg_IOCFGx_CFG_get(void);

