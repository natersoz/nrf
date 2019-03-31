/**
 * @file spim.c
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Perform transfers from the Noric device acting as a SPI master using DMA.
 */

#include "spim.h"
#include "spim_debug.h"
#include "gpio.h"
#include "logger.h"
#include "nrf_cmsis.h"
#include "arm_utilities.h"
#include "project_assert.h"

#include <iterator>

/**
 * @struct spim_control_block_t
 * Maintain the state of the SPI master device using DMA.
 *
 * @note The register set for the SPI Masters:
 * + NRF_SPIM_Type: Uses DMA
 * + NRF_SPI_Type : Uses byte-by-byte interrupts
 * are significatly different.
 * This block is specific to NRF_SPIM_Type, using DMA.
 *
 * @todo understand and describe the tx_length, rx_length usage
 * which guards against the erratta 109 SPIM problem.
 */
struct spim_control_block_t
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
    NRF_SPIM_Type* const spim_registers;

    /**
     * SPI/SPIM peripheral instance IRQ number.
     * This will be one of
     * - SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn,
     * - SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn,
     * - SPIM2_SPIS2_SPI2_IRQn
     * - SPIM3_IRQn (nRF52840)
     * @see association notes in .spim_registers above.
     */
    IRQn_Type const irq_type;

    /// The user supplied callback function.
    /// When the spi transfer is complete this function is called.
    spim_event_handler_t handler;

    /// The user supplied context.
    /// This is carried by the SPI interface but never modified by the SPI driver.
    void *context;

    /// Transfer in progress flag.
    /// This volatile flag allows for interrupt/task resource arbitration.
    volatile bool transfer_in_progress;

    /// The slave select pin. spi_pin_not_used if not used.
    gpio_pin_t ss_pin;

    /// The over-run byte value. When the read buffer length exceeds the
    /// write buffer length the read data will be filled with 'orc'.
    uint8_t orc;
};

static void irq_handler_spim(struct spim_control_block_t* spim_control);

/// @todo Get rid of this macro hell. Can templates be used?
#if defined SPIM0_ENABLED
static struct spim_control_block_t spim_instance_0 =
{
    .spim_registers         = reinterpret_cast<NRF_SPIM_Type *>(NRF_SPIM0_BASE),
    .irq_type               = SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn,
    .handler                = nullptr,
    .context                = nullptr,
    .transfer_in_progress   = false,
    .ss_pin                 = spi_pin_not_used,
    .orc                    = 0xFFu
};
static struct spim_control_block_t* const spim_instance_ptr_0 = &spim_instance_0;

extern "C" void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void)
{
    irq_handler_spim(&spim_instance_0);
}
#else
static struct spim_control_block_t* const spim_instance_ptr_0 = nullptr;
#endif  // SPIM0_ENABLED

#if defined SPIM1_ENABLED
static struct spim_control_block_t spim_instance_1 =
{
    .spim_registers         = reinterpret_cast<NRF_SPIM_Type *>(NRF_SPIM1_BASE),
    .irq_type               = SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn,
    .handler                = nullptr,
    .context                = nullptr,
    .transfer_in_progress   = false,
    .ss_pin                 = spi_pin_not_used,
    .orc                    = 0xFFu
};
static struct spim_control_block_t* const spim_instance_ptr_1 = &spim_instance_1;

extern "C" void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void)
{
    irq_handler_spim(&spim_instance_1);
}
#else
static struct spim_control_block_t* const spim_instance_ptr_1 = nullptr;
#endif  // SPIM1_ENABLED

#if defined SPIM2_ENABLED
static struct spim_control_block_t spim_instance_2 =
{
    .spim_registers         = reinterpret_cast<NRF_SPIM_Type *>(NRF_SPIM2_BASE),
    .irq_type               = SPIM2_SPIS2_SPI2_IRQn,
    .handler                = nullptr,
    .context                = nullptr,
    .transfer_in_progress   = false,
    .ss_pin                 = spi_pin_not_used,
    .orc                    = 0xFFu
};
static struct spim_control_block_t* const spim_instance_ptr_2 = &spim_instance_2;

extern "C" void SPIM2_SPIS2_SPI2_IRQHandler(void)
{
    irq_handler_spim(&spim_instance_2);
}
#else
static struct spim_control_block_t* const spim_instance_ptr_2 = nullptr;
#endif  // SPIM2_ENABLED

#if defined SPIM3_ENABLED
static struct spim_control_block_t spim_instance_3 =
{
    .spim_registers         = reinterpret_cast<NRF_SPIM_Type *>(NRF_SPIM3_BASE),
    .irq_type               = SPIM3_IRQn,
    .handler                = nullptr,
    .context                = nullptr,
    .transfer_in_progress   = false,
    .ss_pin                 = spi_pin_not_used,
    .orc                    = 0xFFu
};
static struct spim_control_block_t* const spim_instance_ptr_3 = &spim_instance_3;

extern "C" void SPIM3_IRQHandler(void)
{
    irq_handler_spim(&spim_instance_3);
}
#else
static struct spim_control_block_t* const spim_instance_ptr_3 = nullptr;
#endif  // SPIM3_ENABLED

static struct spim_control_block_t* const spim_instances[] =
{
    spim_instance_ptr_0,
    spim_instance_ptr_1,
    spim_instance_ptr_2,
    spim_instance_ptr_3,
};

/**
 * @return bool true  if the SPIM.ENABLE register set properly for the SPIM operation.
 *              false if the SPIM.ENABLE register is set for any other mode of
 *              operation or if it is disabled.
 */
static bool spim_is_initialized(struct spim_control_block_t const *spim_control)
{
    return spim_control->spim_registers->ENABLE ==
        (SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos);
}

/**
 * @return bool true if the SPIM.ENABLE register is in use by any module
 *              {SPIM, SPIS, TWIM}.
 *              false if the SPIM.ENABLE register is not in use.
 */
static bool spim_regs_in_use(struct spim_control_block_t const *spim_control)
{
    return bool(spim_control->spim_registers->ENABLE & SPIM_ENABLE_ENABLE_Msk);
}

static struct spim_control_block_t* const spim_control_block(spi_port_t spi_port)
{
    if (spi_port < std::size(spim_instances))
    {
        return spim_instances[spi_port];
    }
    else
    {
        return nullptr;
    }
}

/**
 * See nRF52 Section 15.8.1 Peripheral Interface, Interrupt clearing
 * Clearing an interrupt may take 4 cycles. Read the register to insure the
 * interrupt is cleared before exiting the ISR.
 */
static void spim_clear_event_register(uint32_t volatile* spim_register)
{
    *spim_register = 0u;
    volatile uint32_t dummy = *spim_register;
    (void) dummy;
}

enum spi_result_t spim_init(spi_port_t                 spi_port,
                            struct spi_config_t const* spi_config)
{
    struct spim_control_block_t* const spim_control = spim_control_block(spi_port);
    ASSERT(spim_control);
    ASSERT(not spim_regs_in_use(spim_control));

    ASSERT(spi_config);
    ASSERT(spi_config->sck_pin != spi_pin_not_used);
    ASSERT(interrupt_priority_is_valid(spi_config->irq_priority));

    enum spi_result_t result_code = spi_result_success;

    spim_control->handler = nullptr;
    spim_control->context = nullptr;

    // SPI modes 0, 1 are defined as having CPOL (clock polarity) set to zero.
    // For CPOL = 0, the initial clock state is low.
    // For CPOL = 1, the initial clock state is high.
    if (spi_config->mode < spi_mode_2)
    {
        gpio_pin_clear(spi_config->sck_pin);
    }
    else
    {
        gpio_pin_set(spi_config->sck_pin);
    }

    /// @todo Add spi_config output_drive_level setting and set SPIM
    /// output pins appropriately.

    // SPI SCK:
    // See reference manual guidelines: this pin and its input buffer
    // must be connected for the SPI to work.
    gpio_configure_output(spi_config->sck_pin,
                          gpio_pull_none,
                          spi_config->output_drive);

    // SPI MOSI: output with initial value 0.
    if (spi_config->mosi_pin != spi_pin_not_used)
    {
        gpio_pin_clear(spi_config->mosi_pin);
        gpio_configure_output(spi_config->mosi_pin,
                              gpio_pull_none,
                              spi_config->output_drive);
    }

    // SPI MISO: input with internal pull-up.
    if (spi_config->miso_pin != spi_pin_not_used)
    {
        gpio_configure_input(spi_config->miso_pin, gpio_pull_up, gpio_sense_disable);
    }

    // SPI SS: output with initial value high.
    if (spi_config->ss_pin != spi_pin_not_used)
    {
        gpio_pin_set(spi_config->ss_pin);
        gpio_configure_output(spi_config->ss_pin,
                              gpio_pull_none,
                              spi_config->output_drive);
    }
    spim_control->ss_pin = spi_config->ss_pin;

    spim_control->spim_registers->PSEL.SCK  = spi_config->sck_pin;
    spim_control->spim_registers->PSEL.MOSI = spi_pin_sel(spi_config->mosi_pin);
    spim_control->spim_registers->PSEL.MISO = spi_pin_sel(spi_config->miso_pin);
    spim_control->spim_registers->FREQUENCY = spi_config->frequency;
    spim_control->spim_registers->CONFIG    = spi_configure_mode(spi_config->mode,
                                                                 spi_config->shift_order);
    spim_control->spim_registers->ORC       = spi_config->orc;

    spim_control->spim_registers->ENABLE    = (SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos);
    spim_control->spim_registers->INTENSET  = 0u;

    NVIC_SetPriority(spim_control->irq_type, spi_config->irq_priority);
    NVIC_ClearPendingIRQ(spim_control->irq_type);

    spim_control->transfer_in_progress = false;

    return result_code;
}

void spim_deinit(spi_port_t spi_port)
{
    struct spim_control_block_t* const spim_control = spim_control_block(spi_port);
    ASSERT(spim_control);
    ASSERT(spim_is_initialized(spim_control));

    spim_abort_transfer(spi_port);
    spim_control->spim_registers->ENABLE = (SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos);
}

enum spi_result_t spim_transfer(spi_port_t              spi_port,
                                void const*             tx_buffer,
                                dma_size_t              tx_length,
                                void*                   rx_buffer,
                                dma_size_t              rx_length,
                                spim_event_handler_t    handler,
                                void*                   context,
                                uint32_t                flags)

{
    struct spim_control_block_t* const spim_control = spim_control_block(spi_port);
    ASSERT(spim_control);
    ASSERT(spim_is_initialized(spim_control));

    // If buffer pointers are not null then their length must not be zero
    // and they must be RAM based.
    if (tx_buffer != nullptr)
    {
        ASSERT(tx_length > 0u);
        ASSERT(is_valid_ram(tx_buffer, tx_length));
    }

    if (rx_buffer != nullptr)
    {
        ASSERT(rx_length > 0u);
        ASSERT(is_valid_ram(rx_buffer, rx_length));
    }

    enum spi_result_t result = spi_result_success;

    if (spim_control->transfer_in_progress)
    {
        result = spi_result_transfer_busy;
        logger &logger = logger.instance();
        logger.error("%s, error: %d", __func__, result);
        return result;
    }

    spim_control->transfer_in_progress = true;
    spim_control->handler              = handler;
    spim_control->context              = context;

    if (spim_control->ss_pin != spi_pin_not_used)
    {
        gpio_pin_clear(spim_control->ss_pin);
    }

    spim_control->spim_registers->TXD.PTR    = reinterpret_cast<uint32_t>(tx_buffer);
    spim_control->spim_registers->TXD.MAXCNT = tx_length;

    spim_control->spim_registers->RXD.PTR    = reinterpret_cast<uint32_t>(rx_buffer);
    spim_control->spim_registers->RXD.MAXCNT = rx_length;

    spim_clear_event_register(&spim_control->spim_registers->EVENTS_END);

    /// @todo Understand how the DMA list operations work.
    spim_control->spim_registers->TXD.LIST = (SPIM_FLAG_TX_POSTINC & flags) ? 1u : 0u;
    spim_control->spim_registers->RXD.LIST = (SPIM_FLAG_RX_POSTINC & flags) ? 1u : 0u;

    /// @todo This conditional is the notion of setting up without actually
    /// starting the transfer. Determine whether this is useful or not.
    /// Supporting this case seems related to somehow being triggered by an
    /// event. In this case we have to be concerned with erratta
    /// SPIM_NRF52_ANOMALY_109_WORKAROUND.
    if (not bool(flags & SPIM_FLAG_HOLD_XFER))
    {
        // Start the DMA transfer
        spim_control->spim_registers->TASKS_START = 1u;
    }

    if (spim_control->handler)
    {
        spim_control->spim_registers->INTENSET = SPIM_INTENSET_END_Msk;

        NVIC_ClearPendingIRQ(spim_control->irq_type);
        NVIC_EnableIRQ(spim_control->irq_type);
    }
    else
    {
        spim_control->spim_registers->INTENCLR = SPIM_INTENSET_END_Msk;
        while (spim_control->spim_registers->EVENTS_END == 0u)
        {
            /// @todo need timeout with error handling.
        }
        spim_clear_event_register(&spim_control->spim_registers->EVENTS_END);
        spim_control->transfer_in_progress = false;

        if (spim_control->ss_pin != spi_pin_not_used)
        {
            gpio_pin_set(spim_control->ss_pin);
        }
    }

    // log_spim_registers(spim_control->spim_registers);

    return result;
}

void spim_abort_transfer(spi_port_t spi_port)
{
    ASSERT(not interrupt_context_check());

    struct spim_control_block_t* const spim_control = spim_control_block(spi_port);
    ASSERT(spim_control);

    uint32_t const disable_all = UINT32_MAX;

    NVIC_DisableIRQ(spim_control->irq_type);
    spim_control->spim_registers->INTENCLR = disable_all;

    if (spim_control->transfer_in_progress)
    {
        spim_control->spim_registers->TASKS_STOP = 1u;

        while (not spim_control->spim_registers->EVENTS_STOPPED)
        {
            // Block while pending SPI transactions complete.
        }
        spim_control->transfer_in_progress = false;
    }

    spim_clear_event_register(&spim_control->spim_registers->EVENTS_END);
}

/** Called from the SPIM interrupt completion ISR. */
static void finish_transfer(struct spim_control_block_t* const spim_control)
{
    // If Slave Select signal is used, this is the time to deactivate it.
    if (spim_control->ss_pin != spi_pin_not_used)
    {
        gpio_pin_set(spim_control->ss_pin);
    }

    // Clearing this flag before calling the handler we allow subsequent
    // transfers to be started directly from the handler function.
    spim_control->transfer_in_progress = false;

    if (spim_control->handler)
    {
        spim_control->handler(spim_control->context);
    }
}

static void irq_handler_spim(struct spim_control_block_t* const spim_control)
{
    if (spim_control->spim_registers->EVENTS_END)
    {
        spim_clear_event_register(&spim_control->spim_registers->EVENTS_END);
        finish_transfer(spim_control);
    }
}

