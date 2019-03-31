/**
 * @file twis.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Perform transfers from the Noric device acting as a TWI master using DMA.
 */

#pragma once

#include "twi_common.h"

#ifdef __cplusplus
extern "C" {
#endif

struct twis_event_t
{
    uint32_t                type;
    struct
    {
        dma_size_t          tx_bytes;
        dma_size_t          rx_bytes;
    } xfer;
    uint8_t                 addr_index;
};

/**
 * @brief TWIS (slave) event handler type.
 */
typedef void (* twis_event_handler_t) (struct twis_event_t const*   event,
                                       void*                        context);

#define twis_addr_count  (2u)

struct twis_config_t
{
    struct twi_gpio_config_t    pin_scl;
    struct twi_gpio_config_t    pin_sda;

    twi_addr_t                  slave_addr[twis_addr_count];

    uint8_t                     irq_priority;
    uint8_t                     orc;
};

/**
 * Initialze the TWIS device driver for use.
 *
 * @param twi_port    The specific TWIS perhipheral number to use.
 * @note              The compiler flag -D TWISn_ENABLED must be set,
 *                    where 'n' is the specific peripheral intended for use.
 * @param twis_config The TWI configuration block.
 * @note              This is used by the driver for initialization.
 *                    It is not kept by the driver and can be discarded
 *                    after initialization.
 *
 * @return enum twi_result_t The success or failure code.
 *              The initialization never fails with a result code but will
 *              trap the error with an ASSERT().
 *              @todo remove the result codes if they are never used.
 */
enum twi_result_t twis_init(twi_port_t                  twi_port,
                            struct twis_config_t const* twis_config,
                            twis_event_handler_t        twis_handler,
                            void*                       twis_context);

/**
 * Stop the TWIS module processing and release TWIS resources.
 * Must not be called from within an ISR.
 *
 * @param twi_port The TWIS instance.
 */
void twis_deinit(twi_port_t twi_port);

/**
 * When arming the TWI slave for reading, pass in the const buffer to be read.
 * Since data is transferred from slave to master the slave buffer is Tx.
 *
 * @param twi_port         The TWI port index.
 * @param tx_buffer        The data buffer to be sent; read from.
 * @param tx_buffer_length The length of the data which can be sent.
 *
 * @return enum twi_result_t
 */
enum twi_result_t twis_enable_read(twi_port_t       twi_port,
                                   void const*      tx_buffer,
                                   dma_size_t       tx_buffer_length);

/**
 * When arming the TWI slave for writing, pass in the buffer to be written.
 * Since data is transferred from slave to master the slave buffer is Rx.
 *
 * @param twi_port         The TWI port index.
 * @param rx_buffer        The data buffer to be received; written into.
 * @param rx_buffer_length The length of the data which can be written into.
 *
 * @return enum twi_result_t
 */
enum twi_result_t twis_enable_write(twi_port_t      twi_port,
                                    void*           rx_buffer,
                                    dma_size_t      rx_buffer_length);

/**
 * Abort a tranfer in progress.
 *
 * @param twi_port The TWIS peripheral which is being aborted.
 */
void twis_abort_transfer(twi_port_t twi_port);

#ifdef __cplusplus
}
#endif

