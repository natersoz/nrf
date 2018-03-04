/**
 * @file cc112x_reg_WOR.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * WOR_CFG - eWOR Configuration Register 1
 * Bit  Name                    Reset   R/W
 * 7:6  WOR_RES                 0x00    R/W
 *      eWOR timer resoltuion. Controls t_Event0 and RX timeout resolution
 *
 *      t_Event0 = (1 / f_rcosc) * EVENT0 * 2 ^ (5 * WOR_ES) [seconds]
 *      and
 *      RX Timeout = MAX[1, FLOOR[EVENT0 / 2 ^ RFEND_CFG1.RXTIME+3]] *
 *                   2 ^ (4*WOR_RES * 1250 / f_xosc) [seconds]
 *
 *      00  High        Resolution
 *      01  Medium High Resolution
 *      10  Medium  Low Resolution
 *      11          Low Resolution
 *
 * 5:3  WOR_MODE                0x01    R/W
 *      eWOR Mode
 *      000 Feedback mode
 *      001 Normal Mode
 *      010 Legacy Mode
 *      011 Event1 Mask Mode
 *      100 Event0 Mask Mode
 *     >100 Reserved
 *
 * 2:0  EVENT1                  0x00    R/W
 *      Event1 Timeout
 *      t_Event1 = (1 / f_rcosc) * WOR_EVENT [seconds]
 *      EVENT1      WOR_EVENT1
 *      000          4
 *      001          6
 *      010          8
 *      011         12
 *      100         18
 *      101         24
 *      110         32
 *      111         48
 */

uint8_t cc112x_reg_WOR_CFG1_xxx_set(uint8_t value);
uint8_t cc112x_reg_WOR_CFG1_xxx_get(void);

/**
 * WOR_CFG - eWOR Configuration Register 1
 * Bit  Name                    Reset   R/W
 * 7:6  WOR_CFG_UNUSED          0x00    R
 *      Unused
 * 5    DIV_256HZ_EN            0x01    R/W
 *      Clock division enable. Enables clock division in SLEEP mode
 *      0 Clock division disabled
 *      1 Clock division enabled
 *      Setting DIV_256HZ_EN = 1 will lower the current consumption in SLEEP mode.
 *      Note that when this bit is set the radio should not be woken from SLEEP
 *      by pulling CSn low
 *
 * 4:3  EVENT2_CFG              0x00    R/W
 *      t_EVENT2 = (2 ^ WOR_EVENT2) / f_rcosc [seconds]
 *      EVENT2_CFG  WOR_EVENT2
 *      00 Disabled
 *      01  15
 *      10  18
 *      11  21
 *
 * 2:1  RC_MODE                 0x00    R/W
 *      RCOSC calibration mode. Configures when the RCOSC calibration sequence is
 *      performed. If calibration is enabled, WOR_CFG0.RC_PD must be 0.
 *      00 RCOSC calibration disabled
 *      01 RCOSC calibration disabled
 *      10 RCOSC calibration enabled
 *      11 RCOSC calibration is enabled on every 4th time the device is powered up
 *               and goes from IDLE to RX. This setting should only be used together with eWOR.
 *
 * 0    RC_PD                   0x01    R/W
 *      RCOSC power down signal
 *      0 RCOSC is running
 *      1 RCOSC is in power down
 */

uint8_t cc112x_reg_WOR_CFG0_xxx_set(uint8_t value);
uint8_t cc112x_reg_WOR_CFG0_xxx_get(void);

/**
 * EVENT0 - Event 0 Configuration
 * Bit  Name                    Reset   R/W
 * 15:0 EVENT0                  0x00    R/W
 *      Event 0 timeout
 *
 *      f_Event0 = (1 / f_rcosc) * EVENT0 * 2 ^ (5 * WOR_CFG1.WOR_RES) [seconds]
 */

/**
 * RCCAL_FINE - RC Oscillator Calibration Fine
 * Bit  Name                    Reset   R/W
 * 7    RCCAL_FINE_NOT_USED     0x00    R
 *      Unused
 *
 * 6:0  RC_FINE                 0x00    R/W
 *      32/40 kHz RCOSC calibrated fine value
 */

/**
 * RCCAL_COARSE - RC Oscillator Calibration Coarse
 * Bit  Name                    Reset   R/W
 * 7    RCCAL_COARSE_NOT_USED   0x00    R
 *      Unused
 *
 * 6:0  RC_COARSE               0x00    R/W
 *      32/40 kHz RCOSC calibrated coarse value
 */

/**
 * RCCAL_OFFSET -  RC Oscillator Calibration Clock Offset
 * Bit  Name                            Reset   R/W
 * 7    RCCAL_OFFSET_NOT_USED           0x00    R
 *      Unused
 *
 * 6:0  RCC_CLOCK_OFFSET_RESERVED4_0    0x00    R/W
 *      For test purposes only, use values from SmartRF Studio
 */

/**
 * WOR_TIME - eWOR Timer Counter Value
 * Bit  Name                    Reset   R/W
 * 7    WOR_STATUS              0x00    R
 *      eWOR timer counter value [15:0]
 */

/**
 * WOR_CAPTURE - eWOR Timer CApture Value
 * Bit  Name                    Reset   R/W
 * 7    WOR_STATUS              0x00    R
 *      eWOR timer capture value.
 *      Capture timer value on sync detect to simplify timer re- synchronization
 */

