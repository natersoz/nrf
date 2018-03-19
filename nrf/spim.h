/**
 * @file spim.h
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
    SPIM_FLAG_REPEATED_XFER = (1u << 3u)
};

/**
 * Initialize the SPIM control block. This is the Nordic SPI master using DMA.
 *
 * @note MISO pin has pull down enabled.
 *
 * @param spim_config   Pointer to the structure with the initial configuration.
 *
 * @param context       A user supplied context pointer.
 *                      When events are received the user supplied event handler
 *                      will receive this context within the event.
 *                      The SPIM does not modify the context.
 * @todo Move the user supplied context param to the spim_transfer() function.
 *
 * @retval NRF_SUCCESS             If initialization was successful.
 * @retval NRF_ERROR_INVALID_STATE If the driver was already initialized.
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
 * @brief Function for starting the SPI data transfer with additional option flags.
 *
 * Function enables customizing the transfer by using option flags.
 *
 * Additional options are provided using the flags parameter:
 *
 * - @ref NRF_DRV_SPI_FLAG_TX_POSTINC and
 *   @ref NRF_DRV_SPI_FLAG_RX_POSTINC
 *        Post-incrementation of buffer addresses.
 *
 * - @ref NRF_DRV_SPI_FLAG_HOLD_XFER
 *        Driver is not starting the transfer.
 *        Use this flag if the transfer is triggered externally by PPI.
 *   @ref nrf_drv_spi_start_task_get() to get the address of the start task.
 *
 * - @ref NRF_DRV_SPI_FLAG_NO_XFER_EVT_HANDLER: No interrupt nor user event
 *        handler callback after transfer completion.
 *        If used, the driver does not set the instance into busy state,
 *        so you must ensure that the next transfers are set up when SPIM
 *        is not active.
 *
 *   @ref nrf_drv_spi_end_event_get() function can be used to detect end of
 *        transfer. Used together with NRF_DRV_SPI_FLAG_REPEATED_XFER to
 *        prepare a sequence of SPI transfers without interruptions.
 *
 * - @ref NRF_DRV_SPI_FLAG_REPEATED_XFER: Prepare for repeated transfers.
 *        You can set up a number of transfers that will be triggered externally
 *        (for example by PPI). An example is a TXRX transfer with the options
 *        NRF_DRV_SPI_FLAG_RX_POSTINC.
 *
 *   @ref NRF_DRV_SPI_FLAG_NO_XFER_EVT_HANDLER, and
 *   @ref NRF_DRV_SPI_FLAG_REPEATED_XFER.
 *        After the transfer is set up, a set of transfers can be triggered by
 *        PPI that will read, for example, the same register of an external
 *        component and put it into a RAM buffer without any interrupts.
 *
 *   @ref nrf_drv_spi_end_event_get() can be used to get the address of the
 *   END event, which can be used to count the number of transfers.
 *   If NRF_DRV_SPI_FLAG_REPEATED_XFER is used, the driver does not set the
 *   instance into busy state, so you must ensure that the next
 *   transfers are set up when SPIM is not active.
 *
 * @param spi_port
 * @param xfer_desc
 * @param handler   Event handler provided by the user.
 *                  If NULL, transfers will be performed in blocking mode.
 * @param flags     Transfer options (0 for default settings).
 *
 */

/**
 * @brief SPIM (master) event handler type.
 */
typedef void (* spim_event_handler_t) (void *context);

/**
 * Function for starting the asynchronous SPI master data transfer using DMA.
 *
 * @note Data buffers to be transferred must be in RAM due to a DMA limiitation.
 *
 * @param spim_control
 * @param tx_buffer     Pointer to the transmit buffer.
 *                      NULL if there is nothing to send.
 * @param tx_length     Length of the transmit buffer in bytes.
 * @param rx_buffer     Pointer to the receive buffer.
 *                      NULL if there is nothing to receive.
 * @param rx_length     Length of the receive buffer in bytes.
 *
 * @retval NRF_SUCCESS            If the operation was successful.
 * @retval NRF_ERROR_BUSY         If a previously started transfer has not finished
 *                                yet.
 * @retval NRF_ERROR_INVALID_ADDR If the provided buffers are not placed in the Data
 *                                RAM region.
 * @todo Move the user suppplied context to this function.
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
 * @brief Function for returning the address of a SPIM start task.
 *
 * This function should be used if @ref nrf_drv_spi_xfer was called with the flag @ref NRF_DRV_SPI_FLAG_HOLD_XFER.
 * In that case, the transfer is not started by the driver, but it must be started externally by PPI.
 *
 * @param  p_instance Pointer to the driver instance structure.
 *
 * @return     Start task address.
 */
uint32_t spi_start_task_get(spi_port_t spi_port);

/**
 * @brief Function for returning the address of a END SPIM event.
 *
 * A END event can be used to detect the end of a transfer if the @ref NRF_DRV_SPI_FLAG_NO_XFER_EVT_HANDLER
 * option is used.
 *
 * @param  p_instance Pointer to the driver instance structure.
 *
 * @return     END event address.
 */
uint32_t spi_end_event_get(spi_port_t spi_port);

/**
 * @brief Function for aborting ongoing transfer.
 *
 * @param  p_instance Pointer to the driver instance structure.
 */
void spim_abort_transfer(spi_port_t spi_port);

#ifdef __cplusplus
}
#endif

