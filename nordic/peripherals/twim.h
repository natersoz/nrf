/**
 * @file twim.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Perform transfers from the Noric device acting as a TWI master using DMA.
 */

#pragma once

#include "twi_common.h"

#ifdef __cplusplus
extern "C" {
#endif

struct twim_event_t
{
    uint32_t                type;
    struct
    {
        dma_size_t          tx_bytes;
        dma_size_t          rx_bytes;
    } xfer;
};

/**
 * @brief TWIM (master) event handler type.
 */
typedef void (* twim_event_handler_t) (struct twim_event_t const*   event,
                                       void*                        context);

/**
 * TWI Clock Frequencies.
 * @see OPS 1.4 33.8.9 FREQUENCY, Address offset: 0x524
 */
enum twim_clock_freq_t
{
    twim_clk_freq_100k = 0x0198'0000u,
    twim_clk_freq_250k = 0x0400'0000u,
    twim_clk_freq_400k = 0x0640'0000u,
};

struct twim_config_t
{
    /**
     * The TWIM driver will configure the SCL and SDA pins as inputs with
     * sense disabled. The driver level and pull-up configuration can be
     * specified in the configuration.
     *
     * When the driver is deinitialized this should also provide for good low
     * power performance.
     */
    struct twi_gpio_config_t    pin_scl;
    struct twi_gpio_config_t    pin_sda;

    enum twim_clock_freq_t      clock_freq;
    uint8_t                     irq_priority;
};

/**
 * Initialze the TWIM device driver for use.
 *
 * @param twi_port    The specific TWIM perhipheral number to use.
 * @note              The compiler flag -D TWIMn_ENABLED must be set,
 *                    where 'n' is the specific peripheral intended for use.
 * @param twi_config  The TWI configuration block.
 * @note              This is used by the driver for initialization.
 *                    It is not kept by the driver and can be discarded
 *                    after initialization.
 *
 * @return enum twi_result_t The success or failure code.
 *              The initialization never fails with a result code but will
 *              trap the error with an ASSERT().
 *              @todo remove the result codes if they are never used.
 */
enum twi_result_t twim_init(twi_port_t                  twi_port,
                            struct twim_config_t const* twim_config);

/**
 * Stop the TWIM module processing and release TWIM resources.
 * Must not be called from within an ISR.
 *
 * @param twi_port The TWIM instance.
 */
void twim_deinit(twi_port_t twi_port);

/**
 * Write data to an I2C slave device.
 *
 * @param twi_port  The TWI hardware interface to use.
 * @param address   The 8-bit slave address. The LSBit (R/_W) must be zero.
 * @param tx_buffer The data buffer to write to the slave.
 * @param tx_length The length of the data to write to the slave.
 * @param handler   The event handler callback function; must not be null.
 * @param context   An optional caller context.
 *
 * @return enum twi_result_t
 */
enum twi_result_t twim_write(twi_port_t             twi_port,
                             twi_addr_t             address,
                             void const*            tx_buffer,
                             dma_size_t             tx_length,
                             twim_event_handler_t   handler,
                             void*                  context);

/**
 * Read data from an I2C slave device.
 *
 * @param twi_port  The TWI hardware interface to use.
 * @param address   The 8-bit slave address. The LSBit (R/_W) must be one.
 * @param rx_buffer The data buffer to receive data into from the slave.
 * @param rx_length The length of the receive data buffer.
 * @param handler   The event handler callback function; must not be null.
 * @param context   An optional caller context.
 *
 * @return enum twi_result_t
 */
enum twi_result_t twim_read(twi_port_t              twi_port,
                            twi_addr_t              address,
                            void*                   rx_buffer,
                            dma_size_t              rx_length,
                            twim_event_handler_t    handler,
                            void*                   context);


/**
 * Abort a tranfer in progress.
 *
 * @param twi_port The TWIM peripheral which is being aborted.
 */
void twim_abort_transfer(twi_port_t twi_port);

#ifdef __cplusplus
}
#endif

