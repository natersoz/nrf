/**
 * @file spi_common.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Common data types for both the SPIM and SPIS driver modules.
 */

#include "spi_common.h"
#include "nrf_cmsis.h"
#include "project_assert.h"

// Set mode (CPOL, CPHA) and bit shift order.
uint32_t spi_configure_mode(enum spi_mode_t        spi_mode,
                            enum spi_shift_order_t spi_shift_order)
{
    uint32_t config = (spi_shift_order == spi_shift_order_msb_first)
                      ? (SPIM_CONFIG_ORDER_MsbFirst << SPIM_CONFIG_ORDER_Pos)
                      : (SPIM_CONFIG_ORDER_LsbFirst << SPIM_CONFIG_ORDER_Pos);

    switch (spi_mode)
    {
    case spi_mode_0:
        config |= (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos) |
                  (SPIM_CONFIG_CPHA_Leading    << SPIM_CONFIG_CPHA_Pos);
        break;

    case spi_mode_1:
        config |= (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos) |
                  (SPIM_CONFIG_CPHA_Trailing   << SPIM_CONFIG_CPHA_Pos);
        break;

    case spi_mode_2:
        config |= (SPIM_CONFIG_CPOL_ActiveLow  << SPIM_CONFIG_CPOL_Pos) |
                  (SPIM_CONFIG_CPHA_Leading    << SPIM_CONFIG_CPHA_Pos);
        break;

    case spi_mode_3:
        config |= (SPIM_CONFIG_CPOL_ActiveLow  << SPIM_CONFIG_CPOL_Pos) |
                  (SPIM_CONFIG_CPHA_Trailing   << SPIM_CONFIG_CPHA_Pos);
        break;

    default:
        ASSERT(0);
        break;
    }

    return config;
}

