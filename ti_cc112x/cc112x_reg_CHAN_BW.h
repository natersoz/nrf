/**
 * @file cc112x_reg_CHAN_BW.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * IQIC - Digital Image Channel Compensation Configuration
 * Bit  Name                    Reset   R/W
 * 7    CHFILT_BYPASS           0x00    R/W
 *      Channel filter bypass enable.
 *      Bypassing the channel filter reduces the settling time at the expense of selectivity.
 *      Bypassing the channel filter is not recommended
 *      0 Channel filter enabled
 *      1 Channel filter disabled (bypassed)
 *
 * 6    ADC_CIC_DECFACT         0x00    R/W
 *      ADC_CIC_DECFACT is a table index which programs the first decimation filter
 *      and program the RX filter bandwidth.
 *      ADC_CIC_DECFACT table index:
 *      0 Decimation factor 20
 *      1 Decimation factor 32
 *
 * 6:0  BB_CIC_DECFACT          0x14    R/W
 *      BB_CIC_DECFACT configures the RX filter BW by changing decimation factor in the second
 *      decimation filter (the RX filter BW range is given for CHFILT_BYPASS = 0)
 *      Device  ADC_CIC_DECFACT     ADC_CIC_DECFACT     RX Filter BW Range [kHz]
 *      CC1120  20                  1 - 25               8.0 - 200.0
 *      CC1120  32                  1 - 16               7.8 - 125.0
 *      CC1121  20                  1 -  4              50.0 - 200.0
 *      CC1121  32                  1 -  3              41.7 - 125.0
 *      CC1125  20                  1 - 44               5.7 - 250.0
 *      CC1125  32                  1 - 44               3.6 - 156.3
 */

uint8_t cc112x_reg_CHAN_BW_xxx_set(uint8_t value);
uint8_t cc112x_reg_CHAN_BW_xxx_get(void);

