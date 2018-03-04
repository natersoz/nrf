/**
 * @file cc112x_reg_IOCFG3.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * IOCFG3 - GPIOx Pin Configuration
 * Bit  Name            Reset   R/W
 * 7    GPIO3_ATRAN     0x00    R/W
 *      Analog transfer enable
 *      0 Standard digital pad
 *      1 Pad in analog mode (digital GPIO input and output disabled)
 *
 * 6    GPIOx_INV       0x00    R/W
 * Invert output enable
 *      0 Invert output disabled
 *      1 Invert output enable
 *
 * 5:0  GPIOx_CFG       0x06    R/W
 *      Output selection
 *      Default: PKT_SYNC_RXTX
 */

uint8_t cc112x_reg_IOCFGx_ATRAN_set(uint8_t value);
uint8_t cc112x_reg_IOCFGx_ATRAN_get(void);

uint8_t cc112x_reg_IOCFGx_INV_set(uint8_t value);
uint8_t cc112x_reg_IOCFGx_INV_get(void);

uint8_t cc112x_reg_IOCFGx_CFG_set(uint8_t value);
uint8_t cc112x_reg_IOCFGx_CFG_get(void);


