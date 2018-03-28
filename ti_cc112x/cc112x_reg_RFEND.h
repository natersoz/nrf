/**
 * @file cc112x_reg_RFEND.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * RFEND_CFG1 - RFEND Configuration Register 1
 * Bit  Name                    Reset   R/W
 * 7:6  RFEND_CFG1_UNUSED       0x00    R
 *      Unused
 *
 * 5:4  RXOFF_MODE              0x00    R/W
 *      RXOFF mode. Determines the state the radio will enter after receiving a good packet
 *      00  IDLE
 *      01  FSTXON
 *      10  TX
 *      11  RX
 *
 * 3:1  RX_TIME                 0x07    R/W
 *      Rx timeout for sunc word search in Rx
 *
 *      Rx_Timeout = MAX [1, FLOOR[EVENT0 / 2 ^ (RX_TIME+3)]] * 2 ^ (4*WOR_cfg1.WOR_RES) * 1250 / f_xosc [seconds]
 *
 *      THE Rx TIMEOUT is disabled when RX_TIME = 111b
 *      EVENT0 is found in the WOR_EVENT0_MSB AND WOR_EVENT0_LSB registers.
 *
 * 0    RX_TIME_QUAL            0x01    R/W
 *      RX TIMEOUT Qualifier
 *      0 Continue RX mode on RX timeout if sync word is found
 *      1 Continue RX mode on RX timeout if sync word has been found,
 *        or if PQT is reached or CS is asserted
 */
uint8_t cc112x_reg_RFEND_CFG1_xxx_set(uint8_t value);
uint8_t cc112x_reg_RFEND_CFG1_xxx_get(void);

/**
 * RFEND_CFG0 - RFEND Configuration Register 0
 * Bit  Name                    Reset   R/W
 * 7    RFEND_CFG0_UNUSED       0x00    R
 *      Unused
 *
 * 6    CAL_END_W AKE_UP_EN     0x00    R/W
 *      Enable additional wake-up pulses on the end of calibration.
 *      To be used together with the MCU_WAKEUP signal (MARC_STATUS_OUT will be 0)
 *      0 Disable additional wake-up pulse
 *      1 Enable additional wake-up pulse
 *
 * 5:4  TXOFF_MODE
 *      XOFF mode. Determines the state the radio will enter after transmitting a packet
 *      00 IDLE
 *      01 FSTXON
 *      10 TX
 *      11 RX
 *
 * 3    TERM_ON_BAD_P ACKET_EN
 *      Terminate on bad packet enable
 *      0 Terminate on bad packet disabled.
 *        When a bad packet is received (address, length, or CRC error)
 *        the radio stays in RX regardless of the RFEND_CFG1.RXOFF_MODE
 *      1 Terminate on bad packet enabled.
 *        RFEND_CFG1.RXOFF_MODE is ignored and the radio enters IDLE mode
 *        (or SLEEP mode if eWOR is used) when a bad packet has been received
 *
 * 2:0  ANT_DIV_RX_TERM_CFG     0x00    R/W
 *      Direct RX termination and antenna diversity configuration
 *      000 Antenna diversity and termination based on CS/PQT are disabled
 *      001 RX termination based on CS is enabled (Antenna diversity OFF)
 *      010 Single-switch antenna diversity on CS enabled. One or both antenna is CS evaluated once and RX will terminate if CS failed on both antennas
 *      011 Continuous-switch antenna diversity on CS enabled. Antennas are switched until CS is asserted or RX timeout occurs (if RX timeout is enabled)
 *      100 RX termination based on PQT is enabled (Antenna diversity OFF)
 *      101 Single-switch antenna diversity on PQT enabled. One or both antennas are PQT evaluated once and RX will terminate if PQT is not reached on any of the antennas
 *      110 Continuous-switch antenna diversity on PQT enabled. Antennas are switched until PQT is reached or RX timeout occurs (if RX timeout is enabled)
 *      111 Reserved
 */
uint8_t cc112x_reg_RFEND_CFG0_xxx_set(uint8_t value);
uint8_t cc112x_reg_RFEND_CFG0_xxx_get(void);

