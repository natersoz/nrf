/**
 * @file cc112x_reg_ECG_CFG.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * ECG_CFG - External Clock Frequency Configuration
 * Bit  Name                    Reset   R/W
 * 7:5  ECG_CFG_NOT_USED        0x00    R
 *      Unused
 *
 * 4:0  EXT_CLOCK_FREQ          0x00    R/W
 *      External clock frequency. Controls division factor
 *      00000 64
 *      00001 62
 *      00010 60
 *      00011 58
 *      00100 56
 *      00101 54
 *      00110 52
 *      00111 50
 *      01000 48
 *      01001 46
 *      01010 44
 *      01011 42
 *      01100 40
 *      01101 38
 *      01110 36
 *      01111 34
 *      10000 32
 *      10001 30
 *      10010 28
 *      10011 26
 *      10100 24
 *      10101 22
 *      10110 20
 *      10111 18
 *      11000 16
 *      11001 14
 *      11010 12
 *      11011 10
 *      11100  8
 *      11101  6
 *      11110  4
 *      11111  3
 */
uint8_t cc112x_reg_ECG_CLK_CFG_set(uint8_t value);
uint8_t cc112x_reg_ECG_CLK_CFG_get(void);

