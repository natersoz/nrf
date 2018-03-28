/**
 * @file cc112x_reg_AGC.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * AGC Reference Level Configuration
 * Bit  Name                    Reset   R/W
 * 7:0  AGC_REFERNCE[7:0]       0x36    R/W
 *
 *      AGC reference level.
 *      The AGC reference level must be higher than the minimum SNR to the demodulator.
 *      The AGC reduces the analog front end gain when the magnitude output from
 *      channel filter > AGC reference level.
 *      An optimum AGC reference level is given by several conditions, but a rule of thumb
 *      is to use the formula:
 *
 *      AGC _ REFERENCE 10 log10 (RX Filter BW) 106 RSSI Offset
 *
 *      RX filter BW 10 kHz AGC_REFERENCE
 *       10 kHz      0x24 MDMCFG1.DVGAIN = 11, RSSI offset ~ -102 dB
 *       20 kHz      0x27 MDMCFG1.DVGAIN = 11, RSSI offset ~ -102 dB
 *       50 kHz      0x2b MDMCFG1.DVGAIN = 11, RSSI offset ~ -102 dB
 *      200 kHz      0x2b MDMCFG1.DVGAIN = 11, RSSI offset ~ -102 dB
 *      For zero-IF configuration, AGC hysteresis > 3 dB,
 *      or modem format which needs SNR > 15 dB,
 *      a higher AGC reference value is needed
 */

/**
 * AGC_CS_THR - Carrier Sense Threshold Configuration
 * Bit  Name                    Reset   R/W
 * 7:0  AGC_CS_THRESHOLD        0x00    R/W
 *      AGC gain adjustment.
 *      AGC carrier sense threshold. Two's complement number with 1 dB resolution.
 */

/**
 * AGC_CS_THR - RSSI Offset Configuration
 * Bit  Name                    Reset   R/W
 * 7:0  GAIN_ADJUSTMENT         0x00    R/W
 *      AGC gain adjustment.
 *      This register is used to adjust RSSI[11:0] to the actual carrier input signal level
 *      to compensate for interpolation gains (two's complement with 1 dB resolution)
 */

/**
 * AGC_CFG3 - Automatic Gain Control Configuration Reg. 3
 * Bit  Name                    Reset   R/W
 * 7    RSSI_STEP_THR           0x01    R/W
 *      AGC has a built in function to signal if there has been a step in the RSSI value
 *      0 RSSI threshold is 3 dB
 *      1 RSSI threshold is 6 dB
 *
 * 6:5  AGC_ASK_BW              0x00    R/W
 *      Controls the bandwidth of the data filter in ASK/OOK mode.
 *      The -3 dB cut-off frequency (fCut-Off) is given below:
 *
 *      CHAN_BW.CHFILT_BYPASS = 0
 *      fcutoff = 4 * ASK BW ScaleFactor * RXFilterBW [Hz]
 *
 *      CHAN_BW.CHFILT_BYPASS = 1
 *      fcutoff = ASK BW ScaleFactor * RXFilterBW [Hz]
 *
 *      A rule of thumb is to set fCut-Off >= 5 * symbol rate
 *      00 ASK BW scale factor = 0.28
 *      01 ASK BW scale factor = 0.18
 *      10 ASK BW scale factor = 0.15
 *      11 ASK BW scale factor = 0.14
 *
 * 4:0  AGC_MIN_GAIN            0x11    R/W
 *      AGC minimum gain.
 *      Limits the AGC minimum gain compared to the preset gain table range.
 *      AGC_MIN_GAIN can have a value in the range
 *      0 to 17 when AGC_CFG2.FE_PERFORMANCE_MODE = 00b or 01b
 *      0 to 13 when AGC_CFG2.FE_PERFORMANCE_MODE = 10b
 */

/**
 * AGC_CFG2 - Automatic Gain Control Configuration Reg. 2
 * Bit  Name                    Reset   R/W
 * 7    START_PREVIOUS_GAIN_EN  0x00    R/W
 *      0 Receiver starts with maximum gain value
 *      1 Receiver starts from previous gain value
 *
 * 6:5  FE_PERFORMANCE_MODE     0x01    R/W
 *      Controls which gain tables to be applied
 *      00 Optimized linearity mode
 *      01 Normal operation mode
 *      10 Low power mode with reduced gain range 11 Reserved
 *
 * 4:0  AGC_MAX_GAIN            0x00    R/W
 *      AGC maximum gain.
 *      Limits the AGC maximum gain compared to the preset gain table range.
 *      AGC_MAX_GAIN can have a value in the range
 *      0 to 17 when FE_PERFORMANCE_MODE = 00b or 01b
 *      0 to 13 when FE_PERFORMANCE_MODE = 10b
 */

/**
 * AGC_CFG1 - Automatic Gain Control Configuration Reg. 1
 * Bit  Name                    Reset   R/W
 * 7:5  AGC_SYNC_ BEHAVIOUR     0x05    R/W
 *      AGC behavior after sync word detection
 *      000 No AGC gain freeze. Keep computing/updating RSSI
 *      001 AGC gain freeze. Keep computing/updating RSSI
 *      010 No AGC gain freeze.
 *              Keep computing/updating RSSI (AGC slow mode enabled)
 *      011 Freeze both AGC gain and RSSI
 *      100 No AGC gain freeze. Keep computing/updating RSSI
 *      101 Freeze both AGC gain and RSSI
 *      110 No AGC gain freeze. Keep computing/updating RSSI (AGC slow mode enabled)
 *      111 Freeze both AGC gain and RSSI
 *
 * 4:2  AGC_WIN_SIZE            0x02    R/W
 *      AGC sampling frequency, which is programmed to be 4 times the desired RX filter BW.
 *      000       8 samples
 *      001      16 samples
 *      010      32 samples
 *      011      64 samples
 *      100     128 samples
 *      101     256 samples
 *      110     Reserved
 *      111     Reserved
 *
 * 1:0  AGC_SETTLE_WAIT         0x02    R/W
 *      Sets the wait time between AGC gain adjustments.
 *      00  24 samples
 *      01  32 samples
 *      10  40 samples
 *      11  48 samples
 */

/**
 * AGC_CFG0 - Automatic Gain Control Configuration Reg. 0
 * Bit  Name                    Reset   R/W
 * 7:5  AGC_HYST_LEVEL          0x03    R/W
 *      AGC hysteresis level. The difference between the desired signal level and the
 *      actual signal level must be larger than AGC hysteresis level before the AGC
 *      changes the front end gain.
 *      00   2 dB
 *      01   4 dB
 *      10   7 dB
 *      11  10 dB
 *
 * 5:4  AGC_SLEWRATE_LIMIT      0x00    R/W
 *      AGC slew rate limit. Limits the maximum front end gain adjustment
 *      00  60 dB
 *      01  30 dB
 *      10  18 dB
 *      11   9 dB
 *
 * 3:2  RSSI_VALID_COUNT        0x00    R/W
 *      Gives the number of new input samples to the moving average filter
 *      (internal RSSI estimates) that are required before the next update
 *      of the RSSI value. The RSSI_VALID signal will be asserted from the
 *      first RSSI update. RSSI_VALID is available on a GPIO or can be read
 *      from the RSSI0 register
 *      00  2
 *      01  3
 *      10  5
 *      11  9
 *
 * 1:0  AGC_ASK_DECAY           0x03    R/W
 *      The OOK/ASK receiver uses a max peak magnitude (logic 1) tracker and
 *      low peak magnitude (logic 0) tracker to estimate ASK_THRESHOLD (decision level)
 *      as the average of the max and min value. The max peak magnitude value is also
 *      used by the AGC to set the gain. AGC_ASK_DECAY controls the max peak magnitude
 *      decay steps in OOK/ASK mode and defines the number of samples required for the
 *      max peak level to be reduced to 10% when receiving logic 0’s after receiving a logic 1.
 *
 *      SampleRate = f xosc / (2 * Decimation Factor * CHAN_BW.BB_CIC_DECFACT) [Hz]
 *      The decimation factor is given by CHAN_BW.ADC_CIC_DECFACT
 *      00   600 samples
 *      01  1200 samples
 *      10  2500 samples
 *      11  5000 samples
 */
uint8_t cc112x_reg_AGC_CFG0_xxx_set(uint8_t value);
uint8_t cc112x_reg_AGC_CFG0_xxx_get(void);


