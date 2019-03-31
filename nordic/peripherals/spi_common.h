/**
 * @file spi_common.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Common data types for both the SPIM and SPIS driver modules.
 */

#pragma once

#include <stdint.h>
#include "gpio.h"

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
    spi_result_success = 0,

    /// Attempt to initialize and already initialized SPIM control block.
    spi_result_invalid_state,

    /// A SPI transfer is already in progress.
    spi_result_transfer_busy,
};

/**
 * This value can be provided instead of a pin number for signals MOSI,
 * MISO, and Slave Select to specify that the given signal is not used and
 * therefore does not need to be connected to a pin.
 */
#define spi_pin_not_used  ((gpio_pin_t) -1)

enum spi_polarity_t
{
    spi_cpol_0  = (0u << 1u),
    spi_cpol_1  = (1u << 1u)
};

enum spi_phase_t
{
    spi_cpha_0  = (0u << 0u),
    spi_cpha_1  = (1u << 0u)
};

/**
 * SPI modes.
 */
enum spi_mode_t
{
    /// SCK active high, sample on leading edge of clock.
    /// CPOL = 0, CPHA = 0
    spi_mode_0 = (spi_cpol_0 | spi_cpha_0),

    ///< SCK active high, sample on trailing edge of clock.
    /// CPOL = 0, CPHA = 1
    spi_mode_1 = (spi_cpol_0 | spi_cpha_1),

    ///< SCK active low, sample on leading edge of clock.
    /// CPOL = 1, CPHA = 0
    spi_mode_2 = (spi_cpol_1 | spi_cpha_0),

    ///< SCK active low, sample on trailing edge of clock.
    /// CPOL = 1, CPHA = 1
    spi_mode_3 = (spi_cpol_1 | spi_cpha_1)
} ;

/**
 * @brief SPI bit shift ordering.
 * Normally the SPI buses shift data MSBit first.
 */
enum spi_shift_order_t
{
    /// Normal mode: Shift out MSBit first.
    spi_shift_order_msb_first = 0u,

    /// Abnormal: Shift out LSBit first.
    /// @note This is not a normal SPI implementation.
    spi_shift_order_lsb_first = 1u
};

/**
 * @brief SPI instance configuration structure; master and slave.
 */
struct spi_config_t
{
    gpio_pin_t sck_pin;
    gpio_pin_t mosi_pin;
    gpio_pin_t miso_pin;

    /// Set to spi_pin_not_used if the SS pin is controlled
    /// outside of the SPI driver.
    gpio_pin_t ss_pin;

    uint8_t irq_priority;

    ///< Over-run character.
    /// This character is used when all bytes from the TX buffer are sent,
    /// but the transfer continues due to RX.
    uint8_t orc;

    enum gpio_drive_t       output_drive;   ///< @note Not used for SPIM.
    enum gpio_pull_t        input_pull;     ///< It would make sense to do so.

    /// Set to [SPI_FREQUENCY_FREQUENCY_K125 ... SPI_FREQUENCY_FREQUENCY_M8]
    /// to set the SPI clock frequency. This value is ignored for SPIS (slave).
    uint32_t                frequency;
    enum spi_mode_t         mode;           ///< @see enum spi_mode_t
    enum spi_shift_order_t  shift_order;    ///< @see enum spi_shift_order_t
};

/**
 * Translate the pin number to a uint32_t value which indicates the SPI
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
    return (pin_no == spi_pin_not_used) ? pin_not_connected : pin_no;
}

uint32_t spi_configure_mode(enum spi_mode_t        spi_mode,
                            enum spi_shift_order_t spi_shift_order);


#ifdef __cplusplus
}
#endif

