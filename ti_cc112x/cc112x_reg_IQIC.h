/**
 * @file cc112x_reg_IQIC.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * IQIC - Digital Image Channel Compensation Configuration
 * Bit  Name                    Reset   R/W
 * 7    IQIC_EN                 0x01    R/W
 *      IQ image compensation enable.
 *      When this bit is set the following must be true:
 *      fIF > 2∙RX filter BW and fIF + RX filter BW/2 <= 100 kHz
 *      (For CC1125 fIF + RX filter BW/2 <= 125 kHz)
 *      0 IQ image compensation disabled
 *      1 IQ image compensation enabled
 *
 * 6    IQIC_UPDA TE_COEFF_EN   0x01    R.W
 *      IQIC update coefficients enable
 *      0 IQIC update coefficient disabled
 *        (IQIE_I1, IQIE_I0, IQIE_Q1, and IQIE_Q0 registers are not updated)
 *      1 IQIC update coefficients enabled
 *        (IQIE_I1, IQIE_I0, IQIE_Q1, and IQIE_Q0 registers are updated)
 *
 * 5:4  IQIC_BLEN_SETTLE        0x00    R/W
 *      IQIC block length when settling.
 *      The IQIC module will do a coarse estimation of IQ imbalance coefficients during settling mode.
 *      Long block length increases settling time and improves image rejection
 *      00   8 samples
 *      01  32 samples
 *      10 128 samples
 *      11 256 samples
 *
 * 3:2  IQIC_BLEN               0x01    R/W
 *      IQIC block length. Long block length increases settling time and improves image rejection
 *      00   8 samples
 *      01  32 samples
 *      10 128 samples
 *      11 256 samples
 *
 * 1:0 IQIC_IMGCH_LEVEL_THR     0x0     R/W
 *      IQIC image channel level threshold.
 *      Image rejection will be activated when image carrier is present.
 *      The IQIC image channel level threshold is an image carrier detector.
 *      High threshold imply that image carrier must be high to enable IQIC compensation module.
 *      00 >  256
 *      01 >  512
 *      10 > 1024
 *      11 > 2048
 */

uint8_t cc112x_reg_IQIC_xxx_set(uint8_t value);
uint8_t cc112x_reg_IQIC_xxx_get(void);

