/**
 * @file spim.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Perform transfers from the Noric device acting as a SPI master using DMA.
 */

#pragma once

#include "spi_common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum spim_flags_t
{
    /// TX buffer address incremented after transfer.
    /// Used for chaining successive Tx DMA buffers.
    SPIM_FLAG_TX_POSTINC = (1u << 0u),

    /// RX buffer address incremented after transfer.
    /// Used for chaining successive Rx DMA buffers.
    SPIM_FLAG_RX_POSTINC = (1u << 1u),

    /// Set up the transfer but do not start it.
    SPIM_FLAG_HOLD_XFER = (1u << 2u),

    /// Flag indicating that the transfer will be executed multiple times.
    /// @note @todo This value is not used.
    SPIM_FLAG_REPEATED_XFER = (1u << 3u)
};

/**
 * Initialze the SPIM device driver for use.
 *
 * @param spi_port    The specific SPIM perhipheral number to use.
 * @note              The compiler flag -D SPIMn_ENABLED must be set,
 *                    where 'n' is the specific peripheral intended for use.
 * @param spim_config The SPI configuration block.
 * @note              This is used by the driver for initialization.
 *                    It is not kept by the driver and can be discarded
 *                    after initialization.
 *
 * @return enum spi_result_t The success or failure code.
 *              The initialization never fails with a result code but will
 *              trap the error with an ASSERT().
 *              @todo remove the result codes if they are never used.
 */
enum spi_result_t spim_init(spi_port_t                  spi_port,
                            struct spi_config_t const*  spim_config);

/**
 * Stop the SPIM module processing and release SPIM resources.
 * Must not be called from within an ISR.
 *
 * @param spi_port The SPIM instance.
 */
void spim_deinit(spi_port_t spi_port);

/**
 * @brief SPIM (master) event handler type.
 */
typedef void (* spim_event_handler_t) (void *context);

/**
 * Start the SPIM data transfer using DMA.
 * Nordic DMA requires that the buffers being transferred be in RAM.
 *
 * @param spi_port  The SPIM peripheral to perform the data transfer.
 * @param tx_buffer
 * @param tx_length
 * @param rx_buffer
 * @param rx_length
 * @param handler
 * @param context
 * @param flags
 *
 * @return enum spi_result_t
 */
enum spi_result_t spim_transfer(spi_port_t              spi_port,
                                void const*             tx_buffer,
                                dma_size_t              tx_length,
                                void*                   rx_buffer,
                                dma_size_t              rx_length,
                                spim_event_handler_t    handler,
                                void*                   context,
                                uint32_t                flags);

/**
 * Abort a tranfer in progress.
 *
 * @param spi_port The SPIM peripheral which is being aborted.
 */
void spim_abort_transfer(spi_port_t spi_port);

#ifdef __cplusplus
}
#endif

