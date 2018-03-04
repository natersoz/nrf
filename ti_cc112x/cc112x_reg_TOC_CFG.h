/**
 * @file cc112x_reg_TOC_CFG.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * TOC_CFG - Timing Offset Correction Configuration
 * Bit  Name                    Reset   R/W
 * 7:6  TOC_LIMIT               0x00    R/W
 *      Timing offset correction limit.
 *      TOC_LIMIT specifies maximum symbol rate offset the receiver is able to handle.
 *      TOC_LIMIT != 0 requires 2 - 4 bytes preamble for symbol rate offset compensation.
 *      00 < 0.2 %
 *      01 < 2 %
 *      10 Reserved
 *      11 < 12 % (MDMCFG1.CARRIER_SENSE_GATE must be set)
 *
 * 4:3  TOC_PRE_SYNC_BLOCKLEN   0x01    R/W
 *      When TOC_LIMIT = 0 the receiver uses a block based time offset error calculation algorithm
 *      where the block length is configurable through register TOC_CFG. Before a sync word is found
 *      (SYNC_EVENT is asserted) the TOC_PRE_SYNC_BLOCKLEN sets the actual block length used for
 *      the time offset algorithm.
 *      000 8 symbols integration window
 *      001 16 symbols integration window
 *      010 32 symbols integration window
 *      011 64 symbols integration window
 *      100 128 symbols integration window
 *      101 256 symbols integration window
 *      110 Reserved
 *      111 Reserved
 *
 *      If TOC_LIMIT != 0: Symbol by symbol timing error proportional scale factor
 *      000 Proportional scale factor = 8/16
 *      001 Proportional scale factor = 6/16
 *      010 Proportional scale factor = 2/16
 *      011 Proportional scale factor = 1/16
 *      1xx Proportional scale factor = 1/16 after sync found
 *
 * 2:0  TOC_POST_SYNC_BLOCKLEN  0x03    R/W
 *      When TOC_LIMIT = 0 the receiver uses a block based time offset error calculation algorithm
 *      where the block length is configurable through register TOC_CFG. After a sync word is found
 *      (SYNC_EVENT is asserted) the TOC_POST_SYNC_BLOCKLEN sets the actual block length used for the
 *      time offset algorithm
 *      000 8 symbols integration window
 *      001 16 symbols integration window
 *      010 32 symbols integration window
 *      011 64 symbols integration window
 *      100 128 symbols integration window
 *      101 256 symbols integration window
 *      110 Reserved
 *      111 Reserved
 *      If TOC_LIMIT != 0: Symbol by symbol timing error proportional scale factor
 *      000 Integral scale factor = 8/32
 *      001 Integral scale factor = 6/32
 *      010 Integral scale factor = 2/32
 *      011 Integral scale factor = 1/32
 *      1xx Integral scale factor = 1/32 after sync found
 */

uint8_t cc112x_reg_TOC_CFG_xxx_set(uint8_t value);
uint8_t cc112x_reg_TOC_CFG_xxx_get(void);
