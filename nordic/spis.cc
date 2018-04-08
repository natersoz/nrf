/**
 * @file spis.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Perform transfers from the Noric device acting as a SPI slave using DMA.
 */

#include "spis.h"
#include "spis_debug.h"
#include "spi_common.h"

#include "arm_utilities.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "nrfx_gpiote.h"
#include "nrfx_errors.h"
#include "nrf_log.h"
#include "logger.h"


#include "project_assert.h"

/**
 * SPI slave notes:
 *
 * Before the CPU can safely update the RXD.PTR and TXD.PTR pointers
 * it must first acquire the SPI semaphore.
 *
 * The SPI semaphore is a shared between the CPI and SPI slave hardware.
 * The semaphore is by default assigned to the CPU after the SPI slave is enabled.
 *
 * The CPU can acquire the semaphore by triggering the ACQUIRE task and then
 * receiving the ACQUIRED event.
 *
 * When the CPU has updated the RXD.PTR and TXD.PTR pointers the CPU must
 * release the semaphore before the SPI slave will be able to acquire it.
 * The CPU releases the semaphore by triggering the RELEASE task.
 *
 * The SPI slave will try to acquire the semaphore when CSN goes low.
 * If the SPI slave does not manage to acquire the semaphore at this point,
 * the transaction will be ignored. Incoming MOSI data is discarded and DEF data
 * clocked out on MISO.
 *
 * When a granted transaction is completed and CSN goes high, the SPI slave
 * will automatically release the semaphore and generate the END event.
 *
 * The END_ACQUIRE shortcut can be used to assign the SPI semaphore to the CPU
 * once the CSN deasserts (goes high). This allows the RXD, TXD buffers to be
 * read/written without being overwritten by a new SPI slave transaction.
 *
 * Higher bit rates may require GPIOs to be set as High Drive.
 */

/** States of the SPI slave transaction state machine. */
enum spis_state_t
{
    /// Initialization state.
    /// In this state the module waits for a call to @ref spi_slave_buffers_set.
    SPIS_STATE_INIT,

    /// State where the configuration of the memory buffers,
    /// which are to be used in SPI transaction, has started.
    SPIS_BUFFER_RESOURCE_REQUESTED,

    /// State where the configuration of the memory buffers,
    /// which are to be used in SPI transaction, has completed.
    SPIS_BUFFER_RESOURCE_CONFIGURED,

    /// State where SPI transaction has been completed.
    SPIS_XFER_COMPLETED
};

struct spis_control_block_t
{
    /**
     * Pointer to the structure with SPI/SPIM peripheral instance registers.
     * This must be one of:
     * {NRF_SPIM0_BASE, NRF_SPIM1_BASE, NRF_SPIM2_BASE, ...}
     * @note The instance of the registers must match the ISR found in the
     * vector table for this set of registers.
     * @example NRF_SPIM0_BASE must associate with
     *          irq_type SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn whose ISR
     *          is SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler().
     * @note NRF_SPIM3_BASE is supported only by nRF52840.
     */
    NRF_SPIS_Type *spis_registers;

    /**
     * SPI/SPIM peripheral instance IRQ number.
     * This will be one of
     * - SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn,
     * - SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn,
     * - SPIM2_SPIS2_SPI2_IRQn
     * - SPIM3_IRQn (nRF52840)
     * @see association notes in .spis_registers above.
     */
    IRQn_Type irq_type;

    /// The user supplied callback function.
    /// When the spi transfer is complete this function is called.
    spis_event_handler_t handler;

    /// @{ User supplied buffers for writing / reading
    void const *tx_buffer;
    uint32_t    tx_length;
    void       *rx_buffer;
    uint32_t    rx_length;
    /// @}

    /// The user supplied context.
    /// This is carried by the SPI interface but never modified by the SPI driver.
    void *context;

    /// The slave select pin. SPI_PIN_NOT_USED if not used.
    uint8_t ss_pin;

    enum spis_state_t volatile state;
};

static void irq_handler_spis(spis_control_block_t* spis_control);

/// @todo Get rid of this macro hell. Can templates be used?
#if defined SPIS0_ENABLED
static struct spis_control_block_t spis_instance_0 =
{
    .spis_registers         = reinterpret_cast<NRF_SPIS_Type *>(NRF_SPIS0_BASE),
    .irq_type               = SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn,
    .handler                = nullptr,
    .tx_buffer              = nullptr,
    .tx_length              = 0u,
    .rx_buffer              = nullptr,
    .rx_length              = 0u,
    .context                = nullptr,
    .ss_pin                 = SPI_PIN_NOT_USED,
    .state                  = SPIS_STATE_INIT
};
static struct spis_control_block_t* const spis_instance_ptr_0 = &spis_instance_0;

extern "C" void SPIS0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void)
{
    irq_handler_spis(&spis_instance_0);
}
#else
static struct spis_control_block_t* const spis_instance_ptr_0 = nullptr;
#endif  // SPIS0_ENABLED

#if defined SPIS1_ENABLED
static struct spis_control_block_t spis_instance_1 =
{
    .spis_registers         = reinterpret_cast<NRF_SPIS_Type *>(NRF_SPIS1_BASE),
    .irq_type               = SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn,
    .handler                = nullptr,
    .tx_buffer              = nullptr,
    .tx_length              = 0u,
    .rx_buffer              = nullptr,
    .rx_length              = 0u,
    .context                = nullptr,
    .ss_pin                 = SPI_PIN_NOT_USED,
    .state                  = SPIS_STATE_INIT
};
static struct spis_control_block_t* const spis_instance_ptr_1 = &spis_instance_1;

extern "C" void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void)
{
    irq_handler_spis(&spis_instance_1);
}
#else
static struct spis_control_block_t* const spis_instance_ptr_1 = nullptr;
#endif  // SPIS1_ENABLED

#if defined SPIS2_ENABLED
static struct spis_control_block_t spis_instance_2 =
{
    .spis_registers         = reinterpret_cast<NRF_SPIS_Type *>(NRF_SPIS2_BASE),
    .irq_type               = SPIM2_SPIS2_SPI2_IRQn,
    .handler                = nullptr,
    .tx_buffer              = nullptr,
    .tx_length              = 0u,
    .rx_buffer              = nullptr,
    .rx_length              = 0u,
    .context                = nullptr,
    .ss_pin                 = SPI_PIN_NOT_USED,
    .state                  = SPIS_STATE_INIT
};
static struct spis_control_block_t* const spis_instance_ptr_2 = &spis_instance_2;

extern "C" void SPIS2_SPIS2_SPI2_IRQHandler(void)
{
    irq_handler_spis(&spis_instance_2);
}
#else
static struct spis_control_block_t* const spis_instance_ptr_2 = nullptr;
#endif  // SPIS2_ENABLED

static struct spis_control_block_t* const spis_instances[] =
{
    spis_instance_ptr_0,
    spis_instance_ptr_1,
    spis_instance_ptr_2,
};

/**
 * @return bool true  if the SPIS.ENABLE register set properly for the SPIS operation.
 *              false if the SPIS.ENABLE register is set for any other mode of
 *              operation or if it is disabled.
 */
static bool spis_is_initialized(struct spis_control_block_t const *spis_control)
{
    return spis_control->spis_registers->ENABLE ==
        (SPIS_ENABLE_ENABLE_Enabled << SPIS_ENABLE_ENABLE_Pos);
}

/**
 * @return bool true if the SPIS.ENABLE register is in use by any module
 *              {SPIM, SPIS, TWIM}.
 *              false if the SPIS.ENABLE register is not in use.
 */
static bool spis_regs_in_use(struct spis_control_block_t const *spis_control)
{
    return bool(spis_control->spis_registers->ENABLE & SPIS_ENABLE_ENABLE_Msk);
}

static struct spis_control_block_t* const spis_control_block(spi_port_t spi_port)
{
    /// @todo use std::end() here
    if (spi_port < sizeof(spis_instances) / sizeof(spis_instances[0]))
    {
        return spis_instances[spi_port];
    }
    else
    {
        return nullptr;
    }
}

/**
 * An event handler that gets called when the GPIO pin assigned to the SPIS
 * slave select (aka csn) gets asserted low. Nothing happens.
 *
 * @param pin The pin that was triggered.
 * @param action The transition polarity.
 */
static void csn_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    (void) pin;
    (void) action;

    logger &logger = logger::instance();
    logger.debug("csn_event_handler: pin: %u, action: %u", pin, action);
}

enum spi_result_t spis_init(spi_port_t                 spi_port,
                            struct spi_config_t const* spi_config)
{
    struct spis_control_block_t* const spis_control = spis_control_block(spi_port);
    ASSERT(spis_control);
    ASSERT(not spis_regs_in_use(spis_control));

    ASSERT(spi_config);
    ASSERT(spi_config->ss_pin  != SPI_PIN_NOT_USED);
    ASSERT(spi_config->sck_pin != SPI_PIN_NOT_USED);
    ASSERT(interrupt_priority_is_valid(spi_config->irq_priority));

    spis_control->handler = nullptr;
    spis_control->context = nullptr;

    logger &logger = logger::instance();
    logger.debug("spis_init: pins: ss: %u, sck: %u, mosi: %u, miso: %u",
                spi_config->ss_pin, spi_config->sck_pin,
                spi_config->miso_pin, spi_config->mosi_pin);

    // Note: In the NRF52 Product Specification (v1.4) SPIS states that the
    // SPIS pins must all be set as inputs prior to the enabling the peripheral.
    // See Table 71: GPIO configuration before enabling peripheral.
    // When the transaction is triggered via the SS assertion low then the SPIS
    // block controls the pin direction to output for MISO.
    nrf_gpio_cfg(spi_config->ss_pin,
                 NRF_GPIO_PIN_DIR_INPUT,
                 NRF_GPIO_PIN_INPUT_CONNECT,
                 spi_config->input_pull,
                 NRF_GPIO_PIN_S0S1,
                 NRF_GPIO_PIN_NOSENSE);

    nrf_gpio_cfg(spi_config->sck_pin,
                 NRF_GPIO_PIN_DIR_INPUT,
                 NRF_GPIO_PIN_INPUT_CONNECT,
                 NRF_GPIO_PIN_NOPULL,
                 NRF_GPIO_PIN_S0S1,
                 NRF_GPIO_PIN_NOSENSE);

    if (spi_config->miso_pin != SPI_PIN_NOT_USED)
    {
        nrf_gpio_cfg(spi_config->miso_pin,
                    NRF_GPIO_PIN_DIR_INPUT,
                    NRF_GPIO_PIN_INPUT_CONNECT,
                    NRF_GPIO_PIN_NOPULL,
                    spi_config->output_drive,
                    NRF_GPIO_PIN_NOSENSE);
    }

    if (spi_config->mosi_pin != SPI_PIN_NOT_USED)
    {
        nrf_gpio_cfg(spi_config->mosi_pin,
                     NRF_GPIO_PIN_DIR_INPUT,
                     NRF_GPIO_PIN_INPUT_CONNECT,
                     NRF_GPIO_PIN_NOPULL,
                     NRF_GPIO_PIN_S0S1,
                     NRF_GPIO_PIN_NOSENSE);
    }

    spis_control->spis_registers->PSEL.CSN  = spi_config->ss_pin;
    spis_control->spis_registers->PSEL.SCK  = spi_config->sck_pin;
    spis_control->spis_registers->PSEL.MISO = spi_pin_sel(spi_config->miso_pin);
    spis_control->spis_registers->PSEL.MOSI = spi_pin_sel(spi_config->mosi_pin);
    spis_control->spis_registers->CONFIG    = spi_configure_mode(spi_config->mode,
                                                                 spi_config->shift_order);

    spis_control->spis_registers->RXD.PTR    = 0u;
    spis_control->spis_registers->RXD.MAXCNT = 0u;

    spis_control->spis_registers->TXD.PTR    = 0u;
    spis_control->spis_registers->TXD.MAXCNT = 0u;

    /// @note DEF is set the same as ORC.
    spis_control->spis_registers->ORC        = spi_config->orc;
    spis_control->spis_registers->DEF        = spi_config->orc;

    // Clear the transfer completion event.
    spis_control->spis_registers->EVENTS_END = 0u;
    {
        volatile uint32_t dummy = spis_control->spis_registers->EVENTS_END;
        (void) dummy;
    }

    // Clear the semaphore hand-off (peripheral to firmware) event.
    spis_control->spis_registers->EVENTS_ACQUIRED = 0u;
    {
        volatile uint32_t dummy = spis_control->spis_registers->EVENTS_ACQUIRED;
        (void) dummy;
    }

    // Enable END_ACQUIRE shortcut.
    spis_control->spis_registers->SHORTS |= SPIS_SHORTS_END_ACQUIRE_Msk;

    spis_control->state = SPIS_STATE_INIT;

    (void) nrfx_gpiote_init();
    static nrfx_gpiote_in_config_t const csn_gpiote_config =
    {
        .sense          = NRF_GPIOTE_POLARITY_HITOLO,
        .pull           = NRF_GPIO_PIN_NOPULL,      /// @todo should this be the same as ss_pin pull value?
        .is_watcher     = false,
        .hi_accuracy    = true,
    };

    ret_code_t const gpiote_err_code = nrfx_gpiote_in_init(spi_config->ss_pin,
                                                           &csn_gpiote_config,
                                                           csn_event_handler);
    if (gpiote_err_code != NRFX_SUCCESS)
    {
        logger.error("%s: nrf_drv_gpiote_in_init failed: 0x%x", __func__, gpiote_err_code);
        ASSERT(0);
    }

    spis_control->spis_registers->ENABLE = (SPIS_ENABLE_ENABLE_Enabled << SPIS_ENABLE_ENABLE_Pos);

    nrfx_gpiote_in_event_enable(spi_config->ss_pin, true);

    spis_control->spis_registers->INTENSET = (SPIS_INTENSET_ACQUIRED_Msk |
                                              SPIS_INTENSET_END_Msk      |
                                              0u);

    NVIC_SetPriority(spis_control->irq_type, spi_config->irq_priority);
    NVIC_ClearPendingIRQ(spis_control->irq_type);
    NVIC_EnableIRQ(spis_control->irq_type);

    return SPI_RESULT_SUCCESS;
}

void spis_deinit(spi_port_t spi_port)
{
    struct spis_control_block_t* const spis_control = spis_control_block(spi_port);
    ASSERT(spis_control);
    ASSERT(spis_is_initialized(spis_control));

    spis_control->spis_registers->ENABLE = (SPIS_ENABLE_ENABLE_Disabled << SPIS_ENABLE_ENABLE_Pos);

    NVIC_DisableIRQ(spis_control->irq_type);
    uint32_t const disable_all = UINT32_MAX;
    spis_control->spis_registers->INTENCLR = disable_all;
}

void spis_enable_transfer(spi_port_t            spi_port,
                          void const*           tx_buffer,
                          dma_size_t            tx_length,
                          void*                 rx_buffer,
                          dma_size_t            rx_length,
                          spis_event_handler_t  handler,
                          void*                 context)
{
    struct spis_control_block_t* const spis_control = spis_control_block(spi_port);
    ASSERT(spis_control);
    ASSERT(spis_is_initialized(spis_control));
    ASSERT(tx_buffer);
    ASSERT(tx_length > 0u);
    ASSERT(is_valid_ram(tx_buffer, tx_length));     /// @todo these RAM verifications
    ASSERT(rx_buffer);                              /// should also test that it is
    ASSERT(rx_length > 0u);                         /// not in stack range.
    ASSERT(is_valid_ram(rx_buffer, rx_length));
    ASSERT(handler);

    logger &logger = logger::instance();
    logger.debug("spis_enable: state: %u", spis_control->state);

    switch (spis_control->state)
    {
    case SPIS_STATE_INIT:
    case SPIS_XFER_COMPLETED:
    case SPIS_BUFFER_RESOURCE_CONFIGURED:
        spis_control->tx_buffer = tx_buffer;
        spis_control->rx_buffer = rx_buffer;
        spis_control->tx_length = tx_length;
        spis_control->rx_length = rx_length;
        spis_control->state     = SPIS_BUFFER_RESOURCE_REQUESTED;
        spis_control->handler   = handler;
        spis_control->context   = context;

        // CPU requests ownership of the SPI slave semaphore.
        // This will trigger the ACQURIED event.
        spis_control->spis_registers->TASKS_ACQUIRE = 1u;
        break;

    case SPIS_BUFFER_RESOURCE_REQUESTED:
        // Client is requesting to set buffers while the SPI slave semaphore is
        // already acquired by the CPU resulting in a double request.
        ASSERT(0);
        break;

    default:
        // @note: execution of this code path would imply internal error in the design.
        ASSERT(0);
        break;
    }
}

/**
 * @note as multiple events can be pending for processing,
 * the correct event processing order is:
 * - SPI semaphore   acquired event.
 * - SPI transaction complete event.
 */
static void irq_handler_spis(spis_control_block_t* spis_control)
{
    logger &logger = logger::instance();

    if (spis_control->spis_registers->EVENTS_ACQUIRED)
    {
        logger.debug("spis_irq: EVENTS_ACQUIRED, state: %u", spis_control->state);
        if (spis_control->state == SPIS_BUFFER_RESOURCE_REQUESTED)
        {
            // The SPI slave semaphore is now owned by the CPU.
            spis_control->spis_registers->EVENTS_ACQUIRED = 0u;
            volatile uint32_t dummy = spis_control->spis_registers->EVENTS_ACQUIRED;
            (void) dummy;

            spis_control->spis_registers->TXD.PTR    = reinterpret_cast<uint32_t>(spis_control->tx_buffer);
            spis_control->spis_registers->TXD.MAXCNT = spis_control->tx_length;

            spis_control->spis_registers->RXD.PTR    = reinterpret_cast<uint32_t>(spis_control->rx_buffer);
            spis_control->spis_registers->RXD.MAXCNT = spis_control->rx_length;

            // Release the SPI slave semaphore from CPU ownership.
            spis_control->spis_registers->TASKS_RELEASE = 1u;
            spis_control->state = SPIS_BUFFER_RESOURCE_CONFIGURED;

            // See notes for enum value SPIS_EVENT_BUFFERS_SET regarding
            // the usefulness of this event.
            struct spis_event_t const event = {
                .type = SPIS_EVENT_BUFFERS_SET,
                .rx_length = 0u,
                .tx_length = 0u
            };
            spis_control->handler(spis_control->context, &event);
        }
    }

    // Check for SPI transaction complete event.
    if (spis_control->spis_registers->EVENTS_END)
    {
        logger.debug("spis_irq: EVENTS_END, state: %u", spis_control->state);

        if (spis_control->state == SPIS_BUFFER_RESOURCE_CONFIGURED)
        {
            // The SPI data transfer has completed.
            spis_control->spis_registers->EVENTS_END = 0u;
            volatile uint32_t dummy = spis_control->spis_registers->EVENTS_END;
            (void) dummy;

            struct spis_event_t const event = {
                .type = SPIS_EVENT_TRANSFER,
                .rx_length = spis_control->spis_registers->RXD.AMOUNT,
                .tx_length = spis_control->spis_registers->TXD.AMOUNT
            };

            spis_control->state = SPIS_XFER_COMPLETED;
            spis_control->handler(spis_control->context, &event);
        }
    }
}

