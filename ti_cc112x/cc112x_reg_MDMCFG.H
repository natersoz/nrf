/**
 * @file cc112x_reg_MDMCFG.h
 * @copyright (c) 2015, natersoz, LLC
 */

#pragma once
#include <stdint.h>

/**
 * MDMCFG1 - General Modem Parameter Configuration Reg. 1
 * Bit  Name                    Reset   R/W
 * 7    CARRIER_SENSE_GATE       0x00    R/W
 *      When CARRIER_SENSE_GATE is 1, the demodulator will not start to look for a sync word
 *      before CARRIER_SENSE is asserted
 *      0 Search for sync word regardless of CS
 *      1 Do not start sync search before CARRIER_SENSE is asserted
 *
 * 6    FIFO_EN                 0x01    R/W
 *      FIFO enable. Specifies if data to/from modem will be passed through the FIFOs or
 *      directly to the serial pin
 *      0 Data in/out through the serial pin(s) (the FIFOs are bypassed)
 *      1 Data in/out through the FIFOs
 *
 * 5    MANCHESTER_EN           0x00    R/W
 *      Manchester mode enable.
 *      Manchester encoding/decoding is only applicable to payload data including optional CRC.
 *      Manchester encoding/decoding is not supported for 4-(G)FSK
 *      0 NRZ
 *      1 Manchester encoding/decoding
 *
 * 4    INVERT_DATA_EN          0x00    R/W
 *      Invert data enable.
 *      Invert payload data stream in RX and TX
 *      (only applicable to payload data including optional CRC)
 *      0 Invert data disabled
 *      1 Invert data enabled
 *
 * 3    COLLISION_DETECT_EN     0x00    R/W
 *      Collision detect enable.
 *      After a sync word is detected, the receiver will always receive a packet.
 *      If collision detection is enabled, the receiver will continue to search for sync.
 *      If a new sync word is found during packet reception, a collision is detected and
 *      the COLLISION_FOUND flag will be asserted
 *      0 Collision detect disabled
 *      1 Collision detect enabled
 *
 * 2:1  DVGA_GAIN               0x03    R/W
 *      Fixed DVGA gain configuration. The DVGA configuration has impact on the RSSI offset
 *      00 0 dB DVGA
 *      01 3 dB DVGA
 *      10 6 dB DVGA
 *      11 9 dB DVGA
 *
 * 0    SINGLE_ADC_EN           0x00    R/W
 *      Configure the number of active receive channels.
 *      If this bit is set the power consumption will be reduced but the sensitivity level will
 *      be reduced by ~3 dB. Image rejection will not work.
 *      0 IQ-channels
 *      1 Only I-channel
 */

uint8_t cc112x_reg_MDMCFG1_xxx_set(uint8_t value);
uint8_t cc112x_reg_MDMCFG1_xxx_get(void);



/**
 * MDMCFG0 - General Modem Parameter Configuration Reg. 0
 * Bit  Name                    Reset   R/W
 * 7    MDMCFG0_NOT_USED        0x00    R/W
 *
 * 6    TRANSP ARENT_MODE_EN    0x00    R/W
 *      Transparent mode enable
 *      0 Transparent mode disabled
 *      1 Transparent mode enabled
 *
 * 5:4  TRANSPARENT_INTFACT     0x00    R/W
 *      Transparent signal interpolation factor.
 *      The sample rate gives the jitter of the samples and the sample rate is given by
 *
 *      Sample Rate = (fxOSC * Interpolation Factor) /
 *                    (DecimationFactor * BB_CIC_DECFACT * 2)
 *
 *      Sample Rate fXOSC Interpolation Factor [Hz] DecimationFactor BB_CIC_DECFACT 2
 *      The decimation factor is given by CHAN_BW.ADC_CIC_DECFACT while the interpolation
 *      factor is given as:
 *      00 1x transparent signal interpolated one time before output (reset)
 *      01 2x transparent signal interpolated two times before output
 *      10 4x transparent signal interpolated four times before output
 *      11 Reserved
 *
 * 3    DATA_FILTER_EN          0x01    R/W
 *      Transparent data filter and extended data filter enable.
 *      Enabling transparent data filter and/or extended data filter might Improve sensitivity.
 *
 *      When TRANSPARENT_MODE_EN = 0 this bit should only be set when
 *      RX filter bandwidth/symbol rate > 10 and TOC_CFG.TOC_LIMIT = 0.
 *
 *      The table below shows the status of the transparent data filter and
 *      the extended data filter for all combinations of TRANSPARENT_MODE_EN (MSB)
 *      and DATA_FILTER_EN (LSB)
 *      00 Transparent data filter disabled and extended data filter disabled
 *      01 Transparent data filter disabled and extended data filter enabled
 *      10 Transparent data filter disabled and extended data filter disabled
 *      11 Transparent data filter  enabled and extended data filter disabled
 *
 * 2    VITERBI_EN              0x01    R/W
 *      Viterbi detection enable. Enabling viterbi detection improves the sensitivity.
 *      The latency from the antenna to the signal is available in the RXFIFO or on the
 *      GPIO is increased by 5 bits for 2-ary modulation formats and 10 bits for 4-ary
 *      modulation formats. Minimum packet length = 2 bytes when Viterbi Detection and
 *      4-(G)FSK is enabled
 *      0 Viterbi detection disabled
 *      1 Viterbi detection enabled
 *
 * 1:0  MDMCFG0_RESERVED1_0     0x01    R/W
 *      For Test purposes only. Use vaues from SmartRF Studio.
 */
