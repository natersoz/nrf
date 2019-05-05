/**
 * @file spis.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Perform transfers from the Noric device acting as a SPI slave using DMA.
 */

#pragma once

#include "spi_common.h"
#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

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
                            spi_event_handler_t         handler,
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
 * @param spi_port    The SPIS device to use for transfer.
 * @param miso_buffer The data buffer to send over the MISO wire in response
 *                    to the master sending data to the slave.
 * @param miso_length The maximum number of bytes which will be sent over the
 *                    MISO wire. This value must not exceed sizeof(miso_buffer).
 *                    When the master sends more data than miso_length then the
 *                    response is filled with the 'orc' (over-run) value 0xFF.
 * @param mosi_buffer The data buffer which will receive the data over the MOSI
 *                    wire from the master.
 * @param mosi_length The maximum number of data bytes that the slave will
 *                    accept. This value is typically sizeof(rx_buffer).
 * @return bool       true  If the buffer was successfully queued for SPI slave
 *                          transfers.
 *                    false If the buffer could not be queued. This happens if
 *                    more than 2 buffers are queued at a time.
 */
bool spis_enable_transfer(spi_port_t  spi_port,
                          void const* miso_buffer, size_t miso_length,
                          void*       mosi_buffer, size_t mosi_length);

#ifdef __cplusplus
}
#endif
