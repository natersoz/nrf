/**
 * @file cc112x_reg_DCFILT_CFG.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * DCFILT_CFG - Digital DC Removal Configuration
 * Bit  Name                    Reset   R/W
 * 7    DCFILT_CFG_NOT_USED     0x00    R
 *
 * 6    DCFILT_FREEZE_COEFF     0x01    R/W
 *      DC filter override
 *      0 DC filter algorithm estimates and compensates for DC error
 *      1 Manual DC compensation through registers
 *      DCFILTOFFSET_I1, DCFILTOFFSET_I0, DCFILTOFFSET_Q1, and DCFILTOFFSET_Q0
 *
 * 5:3  DCFILT_BW_SETTLE        0x01    R/W
 *      Settling period of high pass DC filter after AGC adjustment.
 *
 *      Sample rate = f_xosc / (2 * Decimation_Factor) [Hz]
 *
 *      Decimation_Factor is 20 or 32, depending on the CHAN_BW.ADC_CIC_DECFACT setting
 *
 *      000 32 samples
 *      001 64 samples
 *      010 128 samples
 *      011 256 samples
 *      100 512 samples
 *      101 512 samples
 *      110 512 samples
 *      111 512 samples
 *
 * 2:0  DCFILT_BW               0x04    R/W
 *      Cut-off frequency (fCut-Off ) of high pass DC filter
 *
 *      fc DC_Filter = f_xosc / (Decimation_Factor * 100 * 2 ^ DCFILT_BW)  [Hz]
 *
 *      Decimation_Factor is 20 or 32, depending on the CHAN_BW.ADC_CIC_DECFACT setting
 */

uint8_t cc112x_reg_DCFILT_CFG_set(uint8_t value);
uint8_t cc112x_reg_DCFILT_CFG_get(void);

/**
 * Registers related to I/Q demodulation:
 * DCFILTOFFSET_I       DC Filter Offset I
 * DCFILTOFFSET_!       DC Filter Offset Q
 * IQIE_I               IQ Imbalance Value I
 * IQIE_Q               IQ Imbalance Value Q
 */

