/**
 * @file spis.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Perform transfers from the Noric device acting as a SPI slave using DMA.
 */

#include "spis.h"
#include "spi_common.h"
#include "spis_debug.h"
#include "gpio.h"
#include "gpio_te.h"

#include "nordic_critical_section.h"

#include "nrf_cmsis.h"
#include "arm_utilities.h"

#include "logger.h"
#include "project_assert.h"

#include <iterator>

constexpr static uint32_t const spis_interrupt_mask = SPIS_INTENSET_ACQUIRED_Msk |
                                                      SPIS_INTENSET_END_Msk      |
                                                      0u;

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
    NRF_SPIS_Type* const spis_registers;

    /**
     * SPI/SPIM peripheral instance IRQ number.
     * This will be one of
     * - SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn,
     * - SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn,
     * - SPIM2_SPIS2_SPI2_IRQn
     * - SPIM3_IRQn (nRF52840)
     * @see association notes in .spis_registers above.
     */
    IRQn_Type const irq_type;

    /// Does the firmware own the SPI semaphore?
    /// true: The firmware owns it. false: The SPI peripheral owns it.
    /// When the firmware owns it RXD, TDX registers may be written.
    bool spis_semaphore_owned;

    /// Indicates that the client has set the TX, Rx buffers into the driver
    /// block via enable_transfer() and the data is ready to be set into the
    /// SPIS peripheral TXD, RXD locations once the semaphore is acquired.
    /// Once transferred into TXD, RXD, this flag is set false.
    bool data_is_ready;

    /// @{ User supplied buffers for writing / reading
    void const *tx_buffer;
    uint32_t    tx_length;
    void       *rx_buffer;
    uint32_t    rx_length;
    /// @}

    /// Used to work around DMA anomaly 109.
    /// @see spis_init_dma_anomaly_109().
    gpio_te_channel_t gpio_te_channel;

    /// The user supplied callback function.
    /// When the spi transfer is complete this function is called.
    spis_event_handler_t handler;

    /// The user supplied context.
    /// This is carried by the SPI interface but never modified by the SPI driver.
    void *context;

    /// The slave select pin. Must not be set to spi_pin_not_used.
    gpio_pin_t ss_pin;
};

static void irq_handler_spis(spis_control_block_t* spis_control);
static gpio_te_channel_t spis_init_dma_anomaly_109(gpio_pin_t spis_ss_pin);
static void spis_deinit_dma_anomaly_109(gpio_te_channel_t gpio_te_channel);

/// @todo Get rid of this macro hell. Can templates be used?
#if defined SPIS0_ENABLED
static struct spis_control_block_t spis_instance_0 =
{
    .spis_registers         = reinterpret_cast<NRF_SPIS_Type *>(NRF_SPIS0_BASE),
    .irq_type               = SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn,
    .spis_semaphore_owned   = false,
    .data_is_ready          = false,
    .tx_buffer              = nullptr,
    .tx_length              = 0u,
    .rx_buffer              = nullptr,
    .rx_length              = 0u,
    .gpio_te_channel        = gpio_te_channel_invalid,
    .handler                = nullptr,
    .context                = nullptr,
    .ss_pin                 = spi_pin_not_used,
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
    .spis_semaphore_owned   = false,
    .data_is_ready          = false,
    .tx_buffer              = nullptr,
    .tx_length              = 0u,
    .rx_buffer              = nullptr,
    .rx_length              = 0u,
    .gpio_te_channel        = gpio_te_channel_invalid,
    .handler                = nullptr,
    .context                = nullptr,
    .ss_pin                 = spi_pin_not_used,
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
    .spis_semaphore_owned   = false,
    .data_is_ready          = false,
    .tx_buffer              = nullptr,
    .tx_length              = 0u,
    .rx_buffer              = nullptr,
    .rx_length              = 0u,
    .gpio_te_channel        = gpio_te_channel_invalid,
    .handler                = nullptr,
    .context                = nullptr,
    .ss_pin                 = spi_pin_not_used,
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
    if (spi_port < std::size(spis_instances))
    {
        return spis_instances[spi_port];
    }
    else
    {
        return nullptr;
    }
}

static void spis_clear_event_register(uint32_t volatile* spis_register)
{
    *spis_register = 0u;
    volatile uint32_t dummy = *spis_register;
    (void) dummy;
}

enum spi_result_t spis_init(spi_port_t                  spi_port,
                            struct spi_config_t const*  spi_config,
                            spis_event_handler_t        handler,
                            void*                       context)
{
    struct spis_control_block_t* const spis_control = spis_control_block(spi_port);
    ASSERT(spis_control);
    ASSERT(handler);

    ASSERT(not spis_regs_in_use(spis_control));

    ASSERT(spi_config);
    ASSERT(spi_config->ss_pin  != spi_pin_not_used);
    ASSERT(spi_config->sck_pin != spi_pin_not_used);
    ASSERT(interrupt_priority_is_valid(spi_config->irq_priority));

    spis_control->handler = handler;
    spis_control->context = context;

    // This uses up one of the GPIO TE channels.
    // It should be enabled unless a determination is made that it does not apply.
    spis_control->gpio_te_channel = spis_init_dma_anomaly_109(spi_config->ss_pin);

    logger &logger = logger::instance();
    logger.debug("spis_init: pins: ss: %u, sck: %u, mosi: %u, miso: %u",
                spi_config->ss_pin,   spi_config->sck_pin,
                spi_config->miso_pin, spi_config->mosi_pin);

    // Note: In the NRF52 Product Specification (v1.4) SPIS states that the
    // SPIS pins must all be set as inputs prior to the enabling the peripheral.
    // See Table 71: GPIO configuration before enabling peripheral.
    // When the transaction is triggered via the SS assertion low then the SPIS
    // block controls the pin direction to output for MISO.
    // Only the SS pin gets a pull up/down configuration setting.
    gpio_configure_input(spi_config->ss_pin,
                         spi_config->input_pull,
                         gpio_sense_disable);

    gpio_configure_input(spi_config->sck_pin,
                         gpio_pull_none,
                         gpio_sense_disable);

    if (spi_config->miso_pin != spi_pin_not_used)
    {
        // See comments above: MISO pin set to input;
        // SPIS peripheral takes control on SS assertion/semaphore acquisition.
        bool const gpio_input_connect = true;
        gpio_configure(spi_config->miso_pin,
                       gpio_direction_in,
                       gpio_input_connect,
                       gpio_pull_none,
                       spi_config->output_drive,
                       gpio_sense_disable);
    }

    if (spi_config->mosi_pin != spi_pin_not_used)
    {
        gpio_configure_input(spi_config->mosi_pin,
                             gpio_pull_none,
                             gpio_sense_disable);
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
    spis_clear_event_register(&spis_control->spis_registers->EVENTS_END);

    // Clear the semaphore hand-off (peripheral to firmware) event.
    spis_clear_event_register(&spis_control->spis_registers->EVENTS_ACQUIRED);

    // Enable END_ACQUIRE shortcut.
    spis_control->spis_registers->SHORTS |= SPIS_SHORTS_END_ACQUIRE_Msk;

    gpio_configure_input(spi_config->ss_pin,
                         spi_config->input_pull,
                         gpio_sense_disable);
    // Enable the SPIS peripheral.
    spis_control->spis_registers->ENABLE = (SPIS_ENABLE_ENABLE_Enabled << SPIS_ENABLE_ENABLE_Pos);

    // When the SPIS is first enabled the semaphore is owned by firmware.
    spis_control->spis_semaphore_owned = true;

    spis_control->spis_registers->INTENSET = spis_interrupt_mask;

    NVIC_SetPriority(spis_control->irq_type, spi_config->irq_priority);
    NVIC_ClearPendingIRQ(spis_control->irq_type);
    NVIC_EnableIRQ(spis_control->irq_type);

    return spi_result_success;
}

void spis_deinit(spi_port_t spi_port)
{
    struct spis_control_block_t* const spis_control = spis_control_block(spi_port);
    ASSERT(spis_control);
    ASSERT(spis_is_initialized(spis_control));

    spis_control->spis_registers->ENABLE = (SPIS_ENABLE_ENABLE_Disabled << SPIS_ENABLE_ENABLE_Pos);

    NVIC_DisableIRQ(spis_control->irq_type);
    spis_control->spis_registers->INTENCLR = spis_interrupt_mask;

    if (spis_control->gpio_te_channel != gpio_te_channel_invalid)
    {
        spis_deinit_dma_anomaly_109(spis_control->gpio_te_channel);
    }
}

static void spis_arm_transfer(struct spis_control_block_t* spis_control)
{
    spis_control->spis_registers->TXD.PTR    = reinterpret_cast<uintptr_t>(spis_control->tx_buffer);
    spis_control->spis_registers->TXD.MAXCNT = spis_control->tx_length;

    spis_control->spis_registers->RXD.PTR    = reinterpret_cast<uintptr_t>(spis_control->rx_buffer);
    spis_control->spis_registers->RXD.MAXCNT = spis_control->rx_length;

    // Release the SPI slave semaphore from CPU ownership.
    spis_control->spis_semaphore_owned          = false;
    spis_control->data_is_ready                 = false;
    spis_control->spis_registers->TASKS_RELEASE = 1u;
}

void spis_enable_transfer(spi_port_t  spi_port,
                          void const* tx_buffer, dma_size_t tx_length,
                          void*       rx_buffer, dma_size_t rx_length)
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

    logger &logger = logger::instance();
    logger.debug("spis_enable, sem_owned: %u", spis_control->spis_semaphore_owned);

    uint8_t critical_nested = 0u;
    app_util_critical_region_enter(&critical_nested);

    spis_control->tx_buffer     = tx_buffer;
    spis_control->rx_buffer     = rx_buffer;
    spis_control->tx_length     = tx_length;
    spis_control->rx_length     = rx_length;
    spis_control->data_is_ready = true;

    if (spis_control->spis_semaphore_owned)
    {
        // If we own the semaphore it means that the SPIS peripheral is waiting
        // for the software to load the Rx, Tx buffers.
        // Load the Rx, Tx buffers and release the semaphore.
        spis_arm_transfer(spis_control);
    }
    else
    {
        // Request ownership of the spis semaphore.
        // When the EVENTS_ACQUIRED gets triggered in the ISR,
        // the firmware owns the semaphore.
        spis_control->spis_registers->TASKS_ACQUIRE = 1u;
    }

    // Note: Either TASKS_RELEASE (releasing  semaphore) or
    //              TASKS_ACQUIRE (requesting semaphore)
    // was set based on whether the semaphore was owned.

    app_util_critical_region_exit(critical_nested);
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
        // The CPU acquires the semaphore when the ACQUIRED event is received.
        spis_clear_event_register(&spis_control->spis_registers->EVENTS_ACQUIRED);
        spis_control->spis_semaphore_owned = true;

        logger.debug("spis_irq: EVENTS_ACQUIRED, data ready: %u", spis_control->data_is_ready);

        if (spis_control->data_is_ready)
        {
            /**
             * @note If the call to spis_enable_transfer() is made from an
             * interrupt of higher priorty that the SPIS interrupt then a
             * critical section needs to guard this call.
             */
            spis_arm_transfer(spis_control);
        }
        else
        {
            // Notify the user we are ready when they are.
            struct spis_event_t const event = {
                .type = spis_event_data_ready,
                .rx_length = 0u,
                .tx_length = 0u
            };
            spis_control->handler(spis_control->context, &event);
        }
    }

    // Check for SPI transaction complete event.
    if (spis_control->spis_registers->EVENTS_END)
    {
        logger.debug("spis_irq: EVENTS_END");

        // The SPI data transfer has completed.
        spis_clear_event_register(&spis_control->spis_registers->EVENTS_END);

        struct spis_event_t const event = {
            .type = spis_event_transfer_complete,
            .rx_length = spis_control->spis_registers->RXD.AMOUNT,
            .tx_length = spis_control->spis_registers->TXD.AMOUNT
        };

        spis_control->handler(spis_control->context, &event);
    }
}

static void gpio_te_pin_event_handler(gpio_te_channel_t gpio_te_channel,
                                      void*             context)
{
    // This function does nothing other than provide the work around for
    // DMA anomaly 109. Provide debug output to check if the workaround is
    // enabled and working.
    logger::instance().debug("anomaly 109 event");
}

/**
 * Enable the DMA anomaly workaround
 *
 * @see http://infocenter.nordicsemi.com/pdf/nRF52_PAN_109_add_v1.1.pdf
 * System enters IDLE and stops the 64 MHz clock at the same time as the
 * peripheral that is using DMA is started. This results in the wrong data being
 * sent to the external device.
 *
 * @param spis_ss_pin       The SPIS SS pin
 * @return gpio_te_channel  The GPIO TE channel allocated to the workaround.
 */
static gpio_te_channel_t spis_init_dma_anomaly_109(gpio_pin_t spis_ss_pin)
{
    if (not gpio_te_is_initialized())
    {
        uint8_t const irq_priority = 7u;
        gpio_te_init(irq_priority);
    }

    // This prevents the system from entering idle (WFE) at the same time
    // that the DMA is started.
    gpio_te_channel_t const gpio_te_channel =
        gpio_te_allocate_channel_event(spis_ss_pin,
                                       gpio_te_polarity_falling,
                                       nullptr,
                                       gpio_te_pin_event_handler,
                                       nullptr);

    gpio_te_channel_enable_event(gpio_te_channel);

    return gpio_te_channel;
}

/**
 * Release the GPIO TE channel allocated for the DMA anomaly workaround.
 * @param gpio_te_channel The GPIO TE channel to release.
 */
static void spis_deinit_dma_anomaly_109(gpio_te_channel_t gpio_te_channel)
{
    gpio_te_channel_release(gpio_te_channel);
}
