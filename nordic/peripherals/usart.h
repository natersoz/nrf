/**
 * @file usart.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Perform transfers from the Noric device acting as a SPI master using DMA.
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t usart_port_t;

/**
 * This value can be provided instead of a pin number for signals CTS and RTS
 * pins if they are not used.
 */
#define usart_pin_not_used  ((gpio_pin_t) -1)

/**
 * @brief USART instance configuration structure.
 *
 * @see https://stackoverflow.com/a/957416/138264
 * CTS/RTS and DTR/DSR appear to have equivalent in usage.
 */
struct usart_config_t
{
    gpio_pin_t tx_pin;
    gpio_pin_t rx_pin;

    /// Set to usart_pin_not_used if not used.
    /// @note Nordic USART supports CTS/RTS as flow control.
    gpio_pin_t cts_pin;
    gpio_pin_t rts_pin;

    /** The standard baud rate 1200, 9600, 115200, up to 1,000,000. */
    uint32_t   baud_rate;

    /// The USART interrupt priority.
    uint8_t irq_priority;
};

enum usart_event_type
{
    usart_event_none = 0,

    /// The transmit write request has completed.
    /// Member value contains the number of bytes sent.
    usart_tx_complete,

    /// Data has been received and should be read.
    /// Member value contains the number of bytes received.
    usart_rx_complete,

    usart_rx_error_overrun,
    usart_rx_error_parity,
    usart_rx_error_framing,
    usart_rx_error_break,
};

struct usart_event_t
{
    enum usart_event_type   type;
    size_t                  value;
};

/**
 * @brief USART event handler type.
 */
typedef void (* usart_event_handler_t) (usart_event_t const* event, void *context);

/**
 * Initialze the USART device driver for use. The device driver will be in the
 * idle state. The enable Rx data to be processed usart_read_start() must be
 * called.
 *
 * @param usart_port    The specific USART perhipheral number to use.
 * @note                The compiler flag -D USARTn_ENABLED must be set,
 *                      where 'n' is the specific peripheral intended for use.
 * @param usart_config  The USART configuration block.
 * @note                This is used by the driver for initialization.
 *                      It is not kept by the driver and can be discarded
 *                      after initialization.
 * @param usart_event_handler
 * @param tx_buffer     The Tx buffer allocated for sending across the USART.
 * @param tx_length     The length of the Tx buffer allocation in bytes.
 * @param rx_buffer     The Rx buffer allocated for receiving data from the
 *                      USART.
 * @param rx_length     The length of the Rx buffer allocation in bytes.
 * @param context       A client supplied context.
 *
 * @note The rx_buffer is split into 2 parts so the the DMA double buffering
 * can be exploited. One half is pre-queued into the DMA handler while the
 * other hald is actively processing or ready to process Rx data. If an odd
 * value length is passed in, one byte will be wasted.
 * @note If usart_rx_complete notificaitons are required for every byte received
 * then pass in an rx_length of two. This will cause a Rx DMA notification for
 * every byte received. For Rx buffers larger than 1 byte per DMA fill it may
 * be useful, depending on the application, to call usart_read_fill() based on a
 * timer tick or some other event. This will force the Rx DMA buffers to fill,
 * and an usart_rx_complete event to be sent, before the entire buffer is
 * filled.
 */
void usart_init(usart_port_t                 usart_port,
                struct usart_config_t const* usart_config,
                usart_event_handler_t        usart_event_handler,
                void*                        tx_buffer,
                size_t                       tx_length,
                void*                        rx_buffer,
                size_t                       rx_length,
                void*                        context);

/**
 * Stop the USART module processing and release USART resources.
 * Must not be called from within an ISR.
 *
 * @param usart_port The USART instance.
 */
void usart_deinit(usart_port_t usart_port);

/**
 * Write data to the USART.
 *
 * @param usart_port The USART peripheral to send data from.
 * @param tx_buffer  The data to send out the USART Tx port.
 * @param tx_length  The number of bytes to send out the USART Tx port.
 *
 * @return The actual number of bytes written.
 */
size_t usart_write(usart_port_t usart_port,
                   void const* tx_buffer, size_t tx_length);

/**
 * Determine the number of bytes held in the USART driver waiting to be sent.
 *
 * @param usart_port   The specific USART device to query.
 *
 * @return size_t The number of bytes written to the USART driver
 *                waiting in the internal buffer yet to be sent.
 */
size_t usart_write_pending(usart_port_t usart_port);

/**
 * Determine the number of bytes which can be written to the USART driver before
 * the USART driver buffer is full.
 *
 * @param usart_port   The specific USART device to query.
 *
 * @return size_t The number of bytes which may be written into the
 *                driver before the internal buffer becomes full.
 */
size_t usart_write_avail(usart_port_t usart_port);

/**
 * Flush all data contained in the driver buffer to be written across the USART.
 *
 * @param usart_port The specific USART device to flush.
 */
void usart_write_flush(usart_port_t usart_port);

/**
 * Stop a USART Tx send in progress.
 *
 * @param usart_port The driver instance which will stop writing to the USART.
 */
void usart_write_stop(usart_port_t usart_port);

/**
 * Read data from the USART port.
 *
 * @param usart_port The USART peripheral to read data from.
 * @param rx_buffer  The client buffer to fill with received USART data.
 * @param rx_length  The size of the client buffer in bytes.
 *
 * @return The actual number of bytes read.
 */
size_t usart_read(usart_port_t usart_port, void* rx_buffer, size_t rx_length);

/**
 * Determine the number of bytes received by the USART device and are waiting
 * in the USART driver waiting to be read.
 *
 * @param usart_port The specific USART device to query.
 *
 * @return size_t The number of bytes received by USART driver
 *                waiting to be read.
 */
size_t usart_read_pending(usart_port_t usart_port);

/**
 * Determine the number of bytes which may be received by the USART device
 * prior to the driver dropping data because the internal buffer is full.
 *
 * @param usart_port The specific USART device to query.
 *
 * @return size_t The number of bytes remaining in the receive buffer before
 *                it becames full.
 */
size_t usart_read_avail(usart_port_t usart_port);

/**
 * Force the USART driver to acquire all data in transit into its internal
 * buffer so that a read() operation can be performed on all received data.
 * This call does not block; it will generate an event notification of
 * usart_rx_complete.
 */
void usart_read_fill(usart_port_t usart_port);

/**
 * Enabe the USART device driver to begin acquiring Rx data.
 *
 * @param usart_port The specific USART device to enable read acquisition.
 */
void usart_read_start(usart_port_t usart_port);

/**
 * Stop the USART Rx read process. Although the read operations are halted
 * the data buffer integrity is intact. Data can be read using the read()
 * operation and re-enabled using usart_read_start(). The data stream will
 * be received without interruption unless the Rx buffer overflows.
 *
 * @param usart_port The USART device for which reading is disabled.
 */
void usart_read_stop(usart_port_t usart_port);

#ifdef __cplusplus
}
#endif
