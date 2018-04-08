/**
 * @file spi_common.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Common data types for both the SPIM and SPIS driver modules.
 */

#pragma once

#include <stdint.h>
#include "nrf_cmsis.h"
#include "nrf_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

// The NRF52840 allows for 16-bit DMA length transfers.
// The NRF52810 allows for 10-bite DMA length transfers,
//     downgraded to 8-bit here.
// The NRF52832 allows for 8-bit DMA length transfers.
#if defined (NRF52840_XXAA)
    typedef uint16_t dma_size_t;
#else
    typedef uint8_t dma_size_t;
#endif

typedef uint8_t spi_port_t;

enum spi_result_t
{
    SPI_RESULT_SUCCESS = 0,

    /// Attempt to initialize and already initialized SPIM control block.
    SPI_RESULT_INVALID_STATE,

    /// A SPI transfer is already in progress.
    SPI_RESULT_TRANSFER_BUSY,

};

/**
 * This value can be provided instead of a pin number for signals MOSI,
 * MISO, and Slave Select to specify that the given signal is not used and
 * therefore does not need to be connected to a pin.
 */
#define SPI_PIN_NOT_USED  ((uint8_t) 0xFFu)

enum spi_polarity_t
{
    SPI_CPOL_0  = (0u << 1u),
    SPI_CPOL_1  = (1u << 1u)
};

enum spi_phase_t
{
    SPI_CPHA_0  = (0u << 0u),
    SPI_CPHA_1  = (1u << 0u)
};

/**
 * SPI modes.
 */
enum spi_mode_t
{
    /// SCK active high, sample on leading edge of clock.
    /// CPOL = 0, CPHA = 0
    SPI_MODE_0 = (SPI_CPOL_0 | SPI_CPHA_0),

    ///< SCK active high, sample on trailing edge of clock.
    /// CPOL = 0, CPHA = 1
    SPI_MODE_1 = (SPI_CPOL_0 | SPI_CPHA_1),

    ///< SCK active low, sample on leading edge of clock.
    /// CPOL = 1, CPHA = 0
    SPI_MODE_2 = (SPI_CPOL_1 | SPI_CPHA_0),

    ///< SCK active low, sample on trailing edge of clock.
    /// CPOL = 1, CPHA = 1
    SPI_MODE_3 = (SPI_CPOL_1 | SPI_CPHA_1)
} ;

/**
 * @brief SPI bit shift ordering.
 * Normally the SPI buses shift data MSBit first.
 */
enum spi_shift_order_t
{
    /// Normal mode: Shift out MSBit first.
    SPI_SHIFT_ORDER_MSB_FIRST = SPI_CONFIG_ORDER_MsbFirst,

    /// Abnormal: Shift out LSBit first.
    SPI_SHIFT_ORDER_LSB_FIRST = SPI_CONFIG_ORDER_LsbFirst
};

/**
 * @brief SPI instance configuration structure; master and slave.
 */
struct spi_config_t
{
    uint8_t sck_pin;
    uint8_t mosi_pin;
    uint8_t miso_pin;

    /// Set to NRF_DRV_SPI_PIN_NOT_USED if the SS pin is controlled
    /// outside of the SPI driver.
    uint8_t ss_pin;
    uint8_t irq_priority;

    ///< Over-run character.
    /// This character is used when all bytes from the TX buffer are sent,
    /// but the transfer continues due to RX.
    uint8_t orc;

    nrf_gpio_pin_drive_t    output_drive;   ///< @note Not used for SPIM.
    nrf_gpio_pin_pull_t     input_pull;     ///< It would make sense to do so.

    /// Set to [SPI_FREQUENCY_FREQUENCY_K125 ... SPI_FREQUENCY_FREQUENCY_M8]
    /// to set the SPI clock frequency. This value is ignored for SPIS (slave).
    uint32_t                frequency;
    enum spi_mode_t         mode;           ///< @see enum spi_mode_t
    enum spi_shift_order_t  shift_order;    ///< @see enum spi_shift_order_t
};

/**
 * Translate the pin number to a uint32_t value which indicated the SPI
 * pin to use in the SPIM_PSEL_Type struct.
 * @note This used to deal with unused MOSI and MISO pin values.
 *
 * @param pin_no The assigned pin number.
 *
 * @return uint32_t The 32-bit equivalent value.
 */
static inline uint32_t spi_pin_sel(uint8_t pin_no)
{
    uint32_t const pin_not_connected = UINT32_MAX;
    return (pin_no == SPI_PIN_NOT_USED) ? pin_not_connected : pin_no;
}

// Set mode (CPOL, CPHA) and bit shift order.
static inline uint32_t spi_configure_mode(enum spi_mode_t        spi_mode,
                                          enum spi_shift_order_t spi_shift_order)
{
    uint32_t config = (spi_shift_order == SPI_SHIFT_ORDER_MSB_FIRST)
                      ? SPIM_CONFIG_ORDER_MsbFirst
                      : SPIM_CONFIG_ORDER_LsbFirst;

    switch (spi_mode)
    {
    case SPI_MODE_0:
        config |= (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos) |
                  (SPIM_CONFIG_CPHA_Leading    << SPIM_CONFIG_CPHA_Pos);
        break;

    case SPI_MODE_1:
        config |= (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos) |
                  (SPIM_CONFIG_CPHA_Trailing   << SPIM_CONFIG_CPHA_Pos);
        break;

    case SPI_MODE_2:
        config |= (SPIM_CONFIG_CPOL_ActiveLow  << SPIM_CONFIG_CPOL_Pos) |
                  (SPIM_CONFIG_CPHA_Leading    << SPIM_CONFIG_CPHA_Pos);
        break;

    case SPI_MODE_3:
        config |= (SPIM_CONFIG_CPOL_ActiveLow  << SPIM_CONFIG_CPOL_Pos) |
                  (SPIM_CONFIG_CPHA_Trailing   << SPIM_CONFIG_CPHA_Pos);
        break;

    default:
//        ASSERT(0);        /// @todo do assert here
        break;
    }

    return config;
}

#ifdef __cplusplus
}
#endif

