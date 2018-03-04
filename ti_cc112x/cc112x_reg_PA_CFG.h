/**
 * @file cc112x_reg_PA_CFG.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * PA_CFG2 - Power Amplifier Configuration Reg. 2
 * Bit  Name                    Reset   R/W
 * 7    PA_CFG2_UNUSED          0x00    R
 *      Unused
 *
 * 6    PA_CFG2_RESERVED6       0x01
 *      For test purposes only, use values from SmartRF Studio
 *
 * 5:0  PA_POWER_RAMP           0x3f    R/W
 *      Output Power ramp target level
 *
 *      Output Power = [ (PA_POWER_RAMP + 1) / 2 ] - 18 [dBm]
 *
 *      PA_POWER_RAMP >= 0x03 for the equation to be valid.
 *      {0x00, 0x01, 0x02} are special power levels
 */
uint8_t cc112x_reg_PA_CFG2_xxx_set(uint8_t value);
uint8_t cc112x_reg_PA_CFG2_xxx_Get(void);

/**
 * PA_CFG1 - Power Amplifier Configuration Reg. 1
 * Bit  Name                    Reset   R/W
 * 7:5  FIRST_IPL               0x02    R/W
 *      First intermediate power level.
 *      The first intermediate power level can be programmed within
 *      the power level range 0 - 7/16 in steps of 1/16
 *
 * 4:2  SECOND_IPL              0x05    R/W
 *      SECOND intermediate power level.
 *      Second intermediate power level.
 *      The second intermediate power level can be programmed within
 *      the power level range 8/16 - 15/16 in steps of 1/16
 *
 * 1:0  RAMP_SHAPE              0x02    R/W
 * PA ramp time and ASK/OOK shape length. Note that only certain values of PA_CFG0.UPSAMPLER_P complies with the different ASK/OOK shape lengths
 *      00 3/8 symbol ramp time and 1/32 symbol ASK/OOK shape length
 *             (legal UPSAMPLER_P values: 100b, 101b, and 110b)
 *      01 3/2 symbol ramp time and 1/16 symbol ASK/OOK shape length
 *             (legal UPSAMPLER_P values: 011b, 100b, 101b, and 110b)
 *      10 3   symbol ramp time and 1/8 symbol ASK/OOK shape length
 *             (legal UPSAMPLER_P values: 010b, 011b, 100b, 101b, and 110b)
 *      11 6   symbol ramp time and 1/4 symbol ASK/OOK shape length
 *             (legal UPSAMPLER_P values: 010b , 010b, 011b, 100b, 101b, and 110b )
 */
uint8_t cc112x_reg_PA_CFG1_xxx_set(uint8_t value);
uint8_t cc112x_reg_PA_CFG1_xxx_Get(void);

/**
 * PA_CFG0 - Power Amplifier Configuration Reg. 0
 * Bit  Name                    Reset   R/W
 * 7    PA_CFG0_UNUSED          0x00    R
 *      Unused
 *
 * 4:2  ASK_DEPTH               0x0f    R/W
 *      ASK/OOK depth
 *      A_max = [ (PA_POWER_RAMP + 1) / 2 ] - 18 [dBm]
 *      A_min = [ (PA_POWER_RAMP + 1) / 2 ] - 18 - 2 * ASK_DEPTH [dBm]
 *
 *      PA_POWER_RAMP must be >= 0x03 and
 *      PA_POWER_RAMP - 4 * ASK_DEPTH >= 0x00.
 *
 *      For OOK with maximum depth the PA_POWER_RAMP - 4 * ASK_DEPTH must always be all zero;
 *      hence for ASK/OOK with maximum depth the maximum ASK/OOK output power is 12.5 dBm.
 *
 * 1:0  UPSAMPLER_P             0x04    R/W
 *      UPSAMPLER_P configures the variable upsampling factor P
 *      for the TX upsampler. The total upsampling factor = 16 * P.
 *      The upsampler factor P must satisfy the following:
 *
 *      Symbol_Rate * 16 * P < f_xosc / 4 , Where P should be as large as possible.
 *      The upsampler reduces repetitive spectrum at 16·symbol rate
 *
 *      000 TX upsampler factor P = 1 (bypassed)
 *      001 TX upsampler factor P = 2
 *      010 TX upsampler factor P = 4
 *      011 TX upsample factor P = 8
 *      100 TX upsampler Factor P = 16
 *      101 TX upsampler Factor P = 32
 *      110 TX upsampler Factor P = 64
 *      111 Not used
 */
uint8_t cc112x_reg_PA_CFG0_xxx_set(uint8_t value);
uint8_t cc112x_reg_PA_CFG0_xxx_Get(void);

