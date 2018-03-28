/**
 * @file cc112x_status.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "bit_manip.h"

/**
 * Table 2: Status Byte Summary
 * Bit  Name
 * 7    CHIP_RDYn
 *      Stays high until power and crystal have stabilized.
 *      Should always be low when using the SPI interface.
 * 6:4  STATE
 *      Indicates the current main state machine mode
 *      @see enum CC112X_STATE
 * 3:0  Reserved
 */

/**
 * @enum CC112X_STATE
 * Table 2: Status Byte Summary, STATE[2:0]
 */
enum CC112X_STATE
{
    CC112X_STATE_IDLE               = 0,
    CC112X_STATE_RX                 = 1,
    CC112X_STATE_TX                 = 2,
    CC112X_STATE_FSTXON             = 3,
    CC112X_STATE_CALIBRATE          = 4,
    CC112X_STATE_SETTLING           = 5,
    CC112X_STATE_RX_FIFO_ERROR      = 6,
    CC112X_STATE_TX_FIFO_ERROR      = 7,
};

inline enum cc112X_STATE cc112x_status_get_state(uint8_t status)
{
    uint8_t state = bit_manip::value_set(status, 6u, 4u);
    return (enum CC112X_STATE) status;
}

inline bool cc112x_status_get_ready(uint8_t status)
{
    uint8_t chip_not_rdy = bit_manip::value_set(status, 7u, 7u);
    return !chip_not_rdy;
}

