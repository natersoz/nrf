/**
 * @file cc112x_reg_FS.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * SETTLING_CFG - Frequency Synthesizer Calibration and Settling Configuration
 * Bit  Name                    Reset   R/W
 * 7:5  SETTLING_CFG_NOT_USED   0X00    R
 *      Unused
 *
 * 4:3  FS_AUTOCAL              0x01    R/W
 *      00 Never (manually calibrate using SCAL strobe)
 *      01 When going from IDLE to RX or TX (or FSTXON)
 *      10 When going from RX or TX back to IDLE automatically
 *      11 Every 4th time when going from RX or TX to IDLE automatically
 *
 * 2:1  LOCK_TIME               0x01    R/W
 *      Sets the time for the frequency synthesizer to settle to lock state.
 *      The table shows settling after calibration and settling when switching between
 *      TX and RX. Use values from SmartRF Studio
 *      00   50/20 usec
 *      01   75/20 usec
 *      10  100/40 usec
 *      11  150/60 usec
 *
 * 0    FSREG_TIMe              0x01    R/W
 *      0   30 usec
 *      1   60 usec
 */

uint8_t cc112x_reg_SETTLING_CFG_xxx_set(uint8_t value);
uint8_t cc112x_reg_SETTLING_CFG_xxx_get(void);

/**
 * FS_CFG - Frequency Synthesizer Calibration and Settling Configuration
 * Bit  Name                    Reset   R/W
 * 7:5  FS_CFG_NOT_USED         0X00    R
 *      Unused
 *
 * 4    FS_LOCK_EN              0x00    R/W
 *      0 Out of lock detector disabled
 *      0 Out of lock detector enabled
 *
 * 3:0  FSD_BANDSELECT          0x02    R/W
 *      Band select setting for LO divider
 *      0010 820.0 - 960.0 MHz Band (LO Divider = 4)
 *      0100 410.0 - 480.0 MHz Band (LO Divider = 8)
 *      0100 273.0 - 320.0 MHz Band (LO Divider = 8)
 *      0100 205.0 - 240.0 MHz Band (LO Divider = 16)
 *      1010 164.0 - 192.0 MHz Band (LO Divider = 16)
 *      1011 136.7 - 160.0 MHz Band (LO Divider = 16)
 */

uint8_t cc112x_reg_SETTLING_CFG_xxx_set(uint8_t value);
uint8_t cc112x_reg_SETTLING_CFG_xxx_get(void);

/**
 * FREQOFF_CFG - Frequency Offset Correction Configuration
 * Bit  Name                    Reset   R/W
 * 7:6  FREQOFF_CFG_NOT_USED    0X00    R
 *      Unused
 *
 * 5    FOC_EN                  0x01    R/W
 *      Frequency offset correction enable
 *      0 Frequency offset correction disabled
 *      1 Frequency offset correction enabled
 *
 * 4:3  FOC_CFG                 0x00    R/W
 *      Frequency offset correction configuration.
 *      FOC_CFG ≠ 0 enables a narrower RX filter BW than FOC_CFG = 0 but needs longer settle time.
 *      When FOC in FS is enabled, the device automatically switch to 'FOC after channel filter'
 *      when a sync word is detected.
 *      00 FOC after channel filter (typical 0 - 1 preamble bytes for settling)
 *      01 FOC in FS enabled. Loop gain factor is 1/128 (typical 2 - 4 preamble bytes for settling)
 *      10 FOC in FS enabled. Loop gain factor is 1/256 (typical 2 - 4 preamble bytes for settling)
 *      11 FOC in FS enabled. Loop gain factor is 1/512 (typical 2 - 4 preamble bytes for settling)
 *
 * 2    FOC_LIMIT               0x00    R/W
 *      FOC limit.
 *      This is the maximum frequency offset correction in the frequency synthesizer.
 *      Only valid when FOC_CFG =!= 0
 *      0 RX filter bandwidth / 4
 *      1 RX filter bandwidth / 8
 *
 * 3:0  FOC_KI_FACTOR           0x02    R/W
 *      Frequency offset correction.
 *      MDMCFG0.TRANSPARENT_MODE_EN | FOC_KI_FACTOR
 *
 *      000 Frequency offset compensation disabled after sync detected (typical setting for short packets)
 *      001 Frequency offset compensation during packet reception with loop gain factor = 1/32 (fast loop)
 *      010 Frequency offset compensation during packet reception with loop gain factor = 1/64
 *      011 Frequency offset compensation during packet reception with loop gain factor = 1/128 (slow loop)
 *      100 Frequency offset compensation with loop gain factor 1/128 (fast loop)
 *      101 Frequency offset compensation with loop gain factor 1/256
 *      110 Frequency offset compensation with loop gain factor 1/512
 *      111 Frequency offset compensation with loop gain factor 1/1024 (slow loop)
 */
uint8_t cc112x_reg_FREQOFF_CFG_xxx_set(uint8_t value);
uint8_t cc112x_reg_FREQOFF_CFG_xxx_get(void);

/**
 * FREQOFF - Frequency Offset
 * Bit  Name                    Reset   R/W
 * 15:0 FREQ_OFF                0x00    R/W
 *      Frequency offset.
 *      Updated by user or SAFC strobe.
 *      The value is in two's complement format
 */

/**
 * FREQ - Frequency Configuration
 * Bit  Name                    Reset   R/W
 * 23:0 FREQ                    0x00    R/W
 *      Frequency.
 *
 *      f_RF = f_vco / LO_Divider [Hz]
 *
 *      where
 *
 *      f_vco = [ (FREQ / 2 ^ 16) + (FREQOFF / 2^18) ] * f_xosc [Hz]
 *
 *      The LO_Divider = FS_CFG.FSD.BANDSELECT
 */

/**
 * FS_DIG1 - Frequency Synthesizer Digital Reg 1
 * Bit  Name                    Reset   R/W
 * 7:4  FS_DIG1_NOT_USED        0x00    R
 *      Unused
 *
 * 7:4  FS_DIG1_RESERVED3_0     0x08    R/W
 *      For test purposes only, use values from SmartRF Studio
 */

/**
 * FS_DIG0 - Frequency Synthesizer Digital Reg 0
 * Bit  Name                    Reset   R/W
 * 7:4  FS_DIG1_RESERVED        0x00    R/W
 *      For test purposes only, use values from SmartRF Studio
 *
 * 3:2  RX_LPF_BW               0x02    R/W
 *      FS loop bandwidth in RX
 *      00 101.6 kHz
 *      01 131.7 kHz
 *      10 150 kHz
 *      11 170.8 kHz
 *
 * 1:0 TX_LPF_BW                0x02    R/W
 *      FS loop bandwidth in TX
 *      00 101.6 kHz
 *      01 131.7 kHz
 *      10 150 kHz
 *      11 170.8 kHz
 */

enum FS_LOOP_BW
{
    FS_LOOP_BW_101600000HZ      =   = 0,
    FS_LOOP_BW_131700000_HZ         = 1,
    FS_LOOP_BW____150000_HZ         = 2,
    FS_LOOP_BW____170800_HZ         = 3,
};

/**
 * FS_CAL3 -  Frequency Synthesizer Calibration Reg 3
 * Bit  Name                    Reset   R/W
 * 7:5  FS_CAL3_NOT_USED        0x00    R
 *      Unused
 *
 * 4    KVCO_HIGH_RES_CFG       0x08    R/W
 *      KVCO high resolution enable
 *      0 High resolution disabled (normal resolution mode)
 *      1 High resolution enabled (increased charge pump calibration, but will extend the calibration time)
 *
 * 3:0  FS_CAL3_RESERVED3_0     0x00    R/W
 *      For test purposes only, use values from SmartRF Studio
 */

/**
 * FS_CAL2 -  Frequency Synthesizer Calibration Reg 2
 * Bit  Name                    Reset   R/W
 * 7:6  FS_CAL2_NOT_USED        0x00    R
 *      Unused
 *
 * 5:0  VCDAC_START             0x200   R/W
 *      VCDAC start value. Use value from SmartRF Studio
 */

/**
 * FS_CAL1 -  Frequency Synthesizer Calibration Reg 1
 * Bit  Name                    Reset   R/W
 * 7:0  FS_CAL1_RESERVED7_0     0x00    R/W
 *      For test purposes only, use values from SmartRF Studio
 */

/**
 * FS_CAL0 -  Frequency Synthesizer Calibration Reg 0
 * Bit  Name                    Reset   R/W
 * 7:6  FS_CAL0_NOT_USED        0x00    R
 *      Unused
 *
 * 3:2  LOCK_CFG                0x00   R/W
 *      Out of lock detector average time
 *      00 Average the measurement over 512 cycles
 *      01 Average the measurement over 1024 cycles
 *      10 Average the measurement over 256 cycles
 *      11 Infinite average
 *
 * 1:0  FS_CAL0_RESERVED1_0     0x00    R/W
 *      For test purposes only, use values from SmartRF Studio
 */

/**
 * FS_CHP -  Frequency Synthesizer Charge Pump Configuration
 * Bit  Name                    Reset   R/W
 * 7:6  FS_VHP_NOT_USED         0x00    R
 *      Unused
 *
 * 5:0  CHP_CAL_CURR            0x28    R/W
 *      Charge pump current and calibration. Use values from SmartRF Studio
 */

/**
 * FS_DIVTWO - Frequency Synthesizer Divide by 2
 * Bit  Name                    Reset   R/W
 * 7:2  FS_DIVTWO_NOT_USED      0x00    R
 *      Unused
 * 1:0  FS_DIVTWO_RESERVED1_0   0x00    R/W
 *      For test purposes only, use values from SmartRF Studio
 */

/**
 * FS_VCO2 - FS Voltage Controlled Oscillator Configuration Reg. 2
 * Bit  Name                    Reset   R/W
 * 7    FS_VCO2_NOT_USED        0x00    R
 *      Unused
 *
 * 1:0  FSD_VCO_CAL_CAPARR      0x00    R/W
 *      VCO cap-array configuration set during calibration
 *
 */

/**
 * FS_VCO1 - FS Voltage Controlled Oscillator Configuration Reg. 1
 * Bit  Name                    Reset   R/W
 * 7:2  FSD_VCDAC               0x00    R/W
 *      VCO VCDAC configuration.
 *      Used in open-loop CAL mode.
 *      Note that avdd is the internal VCO regulated voltage
 *      000000 VCDAC out = min 160 mV
 *      111111 VCDAC out = max avdd - 160 mV
 *
 * 1:0  FS_VCO1_RESERVED1_0     0x00    R/W
 *      For test purposes only, use values from SmartRF Studio
 *
 */

/**
 * FS_DSM
 * FS_DVCO
 * FS_LBI
 * FS_PFD
 * FS_PRE
 * FS_REG_DIV_CML
 * FS_SPARE
 * FS_VCO3
 * FS_VCO0
 * GBIAS
 * For test purposes only, use values from SmartRF Studio
 *
 */


