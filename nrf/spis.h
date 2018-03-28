/**
 * @file spis.h
 * Perform transfers from the Noric device acting as a SPI slave using DMA.
 */

#pragma once

#include "spi_common.h"
#include "nrf_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum spis_event_type_t
 * Event callback function event types.
 */
enum spis_event_type_t
{
    /// Memory buffers have been set successfully to the SPI slave device,
    /// and SPI transactions can be done.
    SPIS_EVENT_BUFFERS_SET,

    /// SPI transaction has been completed.
    SPIS_EVENT_TRANSFER,

    /// Terminate the enum
    SPIS_EVENT_END
};

/**
 * @struct spis_event_t
 * The SPIS state events.
 */
struct spis_event_t
{
    enum spis_event_type_t  type;
    uint32_t                rx_amount;  /// The MOSI number of bytes received.
    uint32_t                tx_amount;  /// The MISO number of bytes transmitted.
};

/**
 * @brief SPIS (slave) event handler type.
 */
typedef void (* spis_event_handler_t) (void *context,
                                       struct spis_event_t const* event);

enum spi_result_t spis_init(spi_port_t                  spi_port,
                            struct spi_config_t const*  spi_config);

void spis_deinit(spi_port_t spi_port);

void spis_enable_transfer(spi_port_t           spi_port,
                          void const*          tx_buffer,
                          dma_size_t           tx_length,
                          void*                rx_buffer,
                          dma_size_t           rx_length,
                          spis_event_handler_t handler,
                          void*                context);

#ifdef __cplusplus
}
#endif

