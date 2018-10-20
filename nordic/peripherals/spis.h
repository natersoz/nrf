/**
 * @file spis.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Perform transfers from the Noric device acting as a SPI slave using DMA.
 */

#pragma once

#include "spi_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum spis_event_type_t
 * Event callback function event types.
 */
enum spis_event_type_t
{
    /// The SPIS driver is ready and waiting for a call to enable_transfer().
    /// Note that the client does not need to wait for this event.
    /// It is an indication that the client needs to 'keep up'.
    spis_event_data_ready,

    /// SPI transaction has been completed.
    spis_event_transfer_complete,
};

/**
 * @struct spis_event_t
 * The SPIS state events.
 */
struct spis_event_t
{
    enum spis_event_type_t  type;
    uint32_t                rx_length;  /// The MOSI number of bytes received.
    uint32_t                tx_length;  /// The MISO number of bytes transmitted.
};

/**
 * SPIS (slave) event handler type.
 *
 * @param context is a user supplied value that is supplied to the transfer
 * setup call spis_enable_transfer(), held by the SPIS driver, and passed back
 * with SPIS completion event callbacks. It is not miodified by the SPIS driver.
 * @param event The SPIS completion event @see struct spis_event_t.
 */
typedef void (* spis_event_handler_t) (void *context,
                                       struct spis_event_t const* event);

/**
 * Initialize the SPIS driver for operation.
 *
 * @param spi_port   An integer indicating which SPI peripheral device to
 *                   bring into service. Note that the copiler flag
 *                   -D SPISn_ENABLED must also be set, where 'n' is this
 *                   same SPI peripheral number.
 * @param spi_config The SPI initialization block @see struct spi_config_t
 * @note             This is used by the driver for initialization.
 *                   It is not kept by the driver and can be discarded
 *                   after initialization.
 * @param handler    The event completion handler.
 *                   When SPIS events occur the client is notified via this callback.
 * @param context    A user supplied context @see spis_event_handler_t.
 *
 * @return enum spi_result_t The result code indicating success or failure.
 */
enum spi_result_t spis_init(spi_port_t                  spi_port,
                            struct spi_config_t const*  spi_config,
                            spis_event_handler_t        handler,
                            void*                       context);

/**
 * Release the SPIS peripheral from service.
 *
 * @param spi_port The SPIS perhipher number to release.
 */
void spis_deinit(spi_port_t spi_port);

/**
 * Prepare the SPIS peripheral for a data transfer.
 *
 * @param spi_port   The SPIS device to use for transfer.
 * @param tx_buffer  The data buffer to send over the MISO wire in response
 *                   to the master sending data to the slave.
 * @param tx_length  The maximum number of bytes which will be sent over the
 *                   MISO wire. This value must not exceed sizeof(tx_buffer).
 *                   When the master sends more data than tx_length then the
 *                   response is filled with the 'ocr' (over-run) value.
 * @param rx_buffer  The data buffer which will receive the data over the MOSI
 *                   wire from the master.
 * @param rx_length  The maximum number of data bytes that the slave will accept.
 *                   This value is typically sizeof(rx_buffer).
 */
void spis_enable_transfer(spi_port_t  spi_port,
                          void const* tx_buffer, dma_size_t tx_length,
                          void*       rx_buffer, dma_size_t rx_length);

#ifdef __cplusplus
}
#endif
