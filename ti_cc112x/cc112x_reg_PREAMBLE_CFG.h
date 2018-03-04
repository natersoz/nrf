/**
 * @file cc112x_reg_PREAMBLE_CFG.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * PREAMBLE_CFG1 - Preamble Length Configuration Reg. 1
 * Bit  Name                    Reset   R/W
 * 7:6  PREAMBLE_CFG1_NOT_USED  0x00    R
 * 5:2  NUM_PREAMBLE            0x05    Sets the number of preamble bits to be transmitted
 *      0000 No preamble
 *      0001 0.5 byte
 *      0010 1   byte
 *      0011 1.5 bytes
 *      0100 2   bytes
 *      0101 3   bytes
 *      0110 4   bytes
 *      0111 5   bytes
 *      1000 6   bytes
 *      1001 7   bytes
 *      1010 8   bytes
 *      1011 12  bytes
 *      1100 24  bytes
 *      1101 30  bytes
 *      1110 Reserved
 *      1111 Reserved
 * 1:0  PREAMBLE_WORD           0x00    R/W
 *      Preamble byte configuration.
 *      PREAMBLE_WORD determines how a preamble byte looks like.
 *      Note that when 4'ary modulation is used the preamble uses 2'are modulation
 *      (the symbol rate is kept the same)
 *      00 10101010 (0xAA)
 *      01 01010101 (0x55)
 *      10 00110011 (0x33)
 *      11 11001100 (0xCC)
 */

uint8_t cc112x_reg_PREAMBLE_CFG1_xxx_set(uint8_t value);
uint8_t cc112x_reg_PREAMBLE_CFG1_xxx_get(void);

/**
 * PREAMBLE_CFG0 - Preamble Length Configuration Reg. 0
 * Bit  Name                    Reset   R/W
 * 7:6  PREAMBLE_CFG0_NOT_USED  0x00    R
 * 5    PQT_EN                  0x01    R/W
 *      Preamble detection enable
 *      0 Preamble detection disabled
 *      1 Preamble detection enabled
 * 4    PQT_VALID_TIMEOUT       0x00    R/W
 *      PQT start-up timer.
 *      PQT_VALID_TIMEOUT sets the number of symbols that must be received
 *      before PQT_VALID is asserted
 *      0 16 symbols
 *      1 43 symbols
 * 3:0  PQT                     0x0a    R/W
 *      Soft decision PQT.
 *      A preamble is detected when the calculated preamble qualifier value
 *      (PQT_SYNC_ERR.PQT_ERROR) is less than PQT. A low threshold value means
 *      a strict preamble qualifier (preamble must be of high quality to be
 *      accepted) while a high threshold value will accept preamble of a poorer
 *      quality (increased probability of detecting ‘false’ preamble).
 */

uint8_t cc112x_reg_PREAMBLE_CFG0_xxx_set(uint8_t value);
uint8_t cc112x_reg_PREAMBLE_CFG0_xxx_get(void);

